
#ifndef INCLUDED_CPPPARSE_AST_UNIQUETYPE_H
#define INCLUDED_CPPPARSE_AST_UNIQUETYPE_H

#include "Common/IndirectSet.h"
#include "Parse/Grammar.h"
#include <typeinfo>

// ----------------------------------------------------------------------------
// unique types
// Representation of a declarator, with type-elements linked in 'normal' order.
// e.g. int(*)[] == pointer to array of == DeclaratorPointerType -> DeclaratorArrayType
// Note that this is the reverse of the order that the declarator is parsed in.
// This means a given unique type sub-sequence need only be stored once.
// This allows fast comparison of types and simplifies printing of declarators.

struct TypeElementVisitor
{
#if 0
	virtual void visit(const struct Namespace&) = 0;
#endif
	virtual void visit(const struct DependentType&) = 0;
	virtual void visit(const struct DependentTypename&) = 0;
	virtual void visit(const struct DependentNonType&) = 0;
	virtual void visit(const struct TemplateTemplateArgument&) = 0;
	virtual void visit(const struct NonType&) = 0;
	virtual void visit(const struct SimpleType&) = 0;
	virtual void visit(const struct PointerType&) = 0;
	virtual void visit(const struct ReferenceType&) = 0;
	virtual void visit(const struct ArrayType&) = 0;
	virtual void visit(const struct MemberPointerType&) = 0;
	virtual void visit(const struct FunctionType&) = 0;
};

struct TypeElement
{
	UniqueType next;

	TypeElement()
	{
	}
	virtual ~TypeElement()
	{
	}
	virtual void accept(TypeElementVisitor& visitor) const = 0;
	virtual bool operator<(const TypeElement& other) const = 0;
};

struct TypeElementEmpty : TypeElement
{
	TypeElementEmpty()
	{
		next = 0;
	}
	virtual void accept(TypeElementVisitor& visitor) const
	{
		throw SymbolsError();
	}
	virtual bool operator<(const TypeElement& other) const
	{
		throw SymbolsError();
	}
};

extern const TypeElementEmpty gTypeElementEmpty;


template<typename T>
struct TypeElementGeneric : TypeElement
{
	T value;
	TypeElementGeneric(const T& value)
		: value(value)
	{
	}
	void accept(TypeElementVisitor& visitor) const
	{
		visitor.visit(value);
	}
	bool operator<(const TypeElementGeneric& other) const
	{
		return value < other.value;
	}
	bool operator<(const TypeElement& other) const
	{
		return next != other.next
			? next < other.next
			: abstractLess(*this, other);
	}
};

const UniqueType UNIQUETYPE_NULL = &gTypeElementEmpty;


typedef IndirectSet<UniqueType> UniqueTypes;

extern UniqueTypes gBuiltInTypes;

template<typename T>
inline UniqueType pushBuiltInType(UniqueType type, const T& value)
{
	TypeElementGeneric<T> node(value);
	node.next = type;
	return *gBuiltInTypes.insert(node);
}

template<typename T>
inline UniqueType pushUniqueType(UniqueTypes& types, UniqueType type, const T& value)
{
	TypeElementGeneric<T> node(value);
	node.next = type;
	{
		UniqueTypes::iterator i = gBuiltInTypes.find(node);
		if(i != gBuiltInTypes.end())
		{
			return *i;
		}
	}
	return *types.insert(node);
}

extern UniqueTypes gUniqueTypes;

template<typename T>
inline void pushUniqueType(UniqueType& type, const T& value)
{
	type = pushUniqueType(gUniqueTypes, type, value);
}

inline void popUniqueType(UniqueType& type)
{
	SYMBOLS_ASSERT(type.getBits() != 0);
	type = type->next;
}

struct UniqueTypeWrapper
{
	UniqueType value;

	UniqueTypeWrapper()
		: value(&gTypeElementEmpty)
	{
	}
	explicit UniqueTypeWrapper(UniqueType value)
		: value(value)
	{
	}
	template<typename T>
	void push_front(const T& t)
	{
		pushUniqueType(value, t);
	}
	void pop_front()
	{
		SYMBOLS_ASSERT(value != 0);
		SYMBOLS_ASSERT(value != UNIQUETYPE_NULL);
		popUniqueType(value);
	}
	void swap(UniqueTypeWrapper& other)
	{
		std::swap(value, other.value);
	}
	bool empty() const
	{
		return value == UNIQUETYPE_NULL;
	}
	bool isSimple() const
	{
		return typeid(*value) == typeid(TypeElementGeneric<SimpleType>);
	}
#if 0
	bool isNamespace() const
	{
		return typeid(*value) == typeid(TypeElementGeneric<struct Namespace>);
	}
#endif
	bool isPointer() const
	{
		return typeid(*value) == typeid(TypeElementGeneric<PointerType>);
	}
	bool isReference() const
	{
		return typeid(*value) == typeid(TypeElementGeneric<ReferenceType>);
	}
	bool isArray() const
	{
		return typeid(*value) == typeid(TypeElementGeneric<ArrayType>);
	}
	bool isMemberPointer() const
	{
		return typeid(*value) == typeid(TypeElementGeneric<MemberPointerType>);
	}
	bool isFunction() const
	{
		return typeid(*value) == typeid(TypeElementGeneric<FunctionType>);
	}
	bool isDependentNonType() const
	{
		return typeid(*value) == typeid(TypeElementGeneric<DependentNonType>);
	}
	bool isDependentType() const
	{
		return typeid(*value) == typeid(TypeElementGeneric<DependentType>);
	}
	bool isDependent() const
	{
		return isDependentType()
			|| typeid(*value) == typeid(TypeElementGeneric<DependentTypename>)
			|| isDependentNonType();
	}
	bool isNonType() const
	{
		return typeid(*value) == typeid(TypeElementGeneric<NonType>);
	}
	bool isTemplateTemplateArgument() const
	{
		return typeid(*value) == typeid(TypeElementGeneric<TemplateTemplateArgument>);
	}
	bool isSimplePointer() const
	{
		return isPointer()
			&& UniqueTypeWrapper(value->next).isSimple();
	}
	bool isSimpleReference() const
	{
		return isReference()
			&& UniqueTypeWrapper(value->next).isSimple();
	}
	bool isSimpleArray() const
	{
		return isArray()
			&& UniqueTypeWrapper(value->next).isSimple();
	}
	bool isFunctionPointer() const
	{
		return isPointer()
			&& UniqueTypeWrapper(value->next).isFunction();
	}
};

template<typename T>
inline UniqueTypeWrapper pushType(UniqueTypeWrapper type, const T& t)
{
	pushUniqueType(type.value, t);
	return type;
}

template<typename T>
inline UniqueTypeWrapper pushBuiltInType(UniqueTypeWrapper type, const T& value)
{
	return UniqueTypeWrapper(pushBuiltInType(type.value, value));
}

inline UniqueTypeWrapper popType(UniqueTypeWrapper type)
{
	type.pop_front();
	return type;
}

inline UniqueTypeWrapper qualifyType(UniqueTypeWrapper type, CvQualifiers qualifiers)
{
	type.value.setQualifiers(qualifiers);
	return type;
}

inline bool operator==(UniqueTypeWrapper l, UniqueTypeWrapper r)
{
	return l.value == r.value;
}

inline bool operator!=(UniqueTypeWrapper l, UniqueTypeWrapper r)
{
	return !operator==(l, r);
}

inline bool operator<(UniqueTypeWrapper l, UniqueTypeWrapper r)
{
	return l.value < r.value;
}

inline bool isGreaterCvQualification(CvQualifiers l, CvQualifiers r)
{
	return l.isConst + l.isVolatile > r.isConst + r.isVolatile;
}

inline bool isGreaterCvQualification(UniqueTypeWrapper to, UniqueTypeWrapper from)
{
	return isGreaterCvQualification(to.value.getQualifiers(), from.value.getQualifiers());
}

inline bool isEqualCvQualification(UniqueTypeWrapper to, UniqueTypeWrapper from)
{
	return to.value.getQualifiers() == from.value.getQualifiers();
}

#endif
