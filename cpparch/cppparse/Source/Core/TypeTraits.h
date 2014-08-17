
#ifndef INCLUDED_CPPPARSE_CORE_TYPETRAITS_H
#define INCLUDED_CPPPARSE_CORE_TYPETRAITS_H

#include "TypeInstantiate.h"
#include "Fundamental.h"
#include "OverloadResolve.h"


template<typename T>
inline Name getTypeTraitName(T* symbol)
{
	return symbol->trait->value.value;
}


inline bool isUnion(const SimpleType& classType)
{
	return classType.declaration->isUnion;
}

inline bool isUnion(UniqueTypeWrapper type, const InstantiationContext& context)
{
	return type.isSimple() && isUnion(getSimpleType(type.value));
}

inline bool isClass(UniqueTypeWrapper type, const InstantiationContext& context)
{
	return isClass(type);
}

inline bool isEnum(UniqueTypeWrapper type, const InstantiationContext& context)
{
	return isEnum(type);
}


inline bool hasNothrowConstructor(UniqueTypeWrapper type, const InstantiationContext& context)
{
	return false; // TODO: safe but sub-optimal
}

inline bool hasNothrowCopy(UniqueTypeWrapper type, const InstantiationContext& context)
{
	return false; // TODO: safe but sub-optimal
}

inline bool hasTrivialAssign(UniqueTypeWrapper type, const InstantiationContext& context)
{
	return false; // TODO: safe but sub-optimal
}

inline bool hasTrivialConstructor(UniqueTypeWrapper type, const InstantiationContext& context)
{
	return false; // TODO: safe but sub-optimal
}

inline bool hasTrivialCopy(UniqueTypeWrapper type, const InstantiationContext& context)
{
	return false; // TODO: safe but sub-optimal
}

// returns true if type has no destructor and no members or bases with non-trivial destructors
inline bool hasTrivialDestructor(UniqueTypeWrapper type, const InstantiationContext& context)
{
	return false; // TODO: safe but sub-optimal
}


inline bool hasVirtualDestructor(const SimpleType& classType, const InstantiationContext& context)
{
	SYMBOLS_ASSERT(isClass(classType));
	SYMBOLS_ASSERT(isComplete(classType));
	instantiateClass(classType, context); // requires a complete type
	// [class.dtor]
	// A destructor can be declared virtual (10.3) or pure virtual (10.4); if any objects of that class or any
	// derived class are created in the program, the destructor shall be defined.If a class has a base class with a
	// virtual destructor, its destructor(whether user- or implicitly-declared) is virtual.
	return classType.hasVirtualDestructor;
}

// return true if class (or any base thereof) has a virtual destructor.
inline bool hasVirtualDestructor(UniqueTypeWrapper type, const InstantiationContext& context)
{
	if(!isClass(type))
	{
		return false;
	}
	const SimpleType& classType = getSimpleType(type.value);
	return hasVirtualDestructor(classType, context);
}

inline bool isEmpty(const SimpleType& classType, const InstantiationContext& context)
{
	SYMBOLS_ASSERT(isClass(classType));
	SYMBOLS_ASSERT(isComplete(classType));
	instantiateClass(classType, context); // requires a complete type
	if(isUnion(classType))
	{
		return false; // union cannot be empty
	}
	// TODO: if any member is not a zero-sized bitfield, return false
	return classType.isEmpty;
}

// returns true if type is a non-union class with no members (other than bitfield size zero), no virtual functions, no virtual base classes, no non-empty base classes
inline bool isEmpty(UniqueTypeWrapper type, const InstantiationContext& context)
{
	if(!isClass(type))
	{
		return false;
	}
	const SimpleType& classType = getSimpleType(type.value);
	return isEmpty(classType, context);
}

inline bool isPod(const SimpleType& classType, const InstantiationContext& context)
{
	SYMBOLS_ASSERT(isClass(classType));
	SYMBOLS_ASSERT(isComplete(classType));
	instantiateClass(classType, context); // requires a complete type
	// TODO: check for members which are non-POD!
	if(isUnion(classType))
	{
		return true;
	}
	return classType.isPod;
}

// returns true if type is a class or union with no non-pod members, no base classes, no virtual functions, no constructor or destructor
inline bool isPod(UniqueTypeWrapper type, const InstantiationContext& context)
{
	if(!isClass(type))
	{
		return false; // TODO: MSVC returns false for non-class non-union types, GCC returns true
	}
	const SimpleType& classType = getSimpleType(type.value);
	return isPod(classType, context);
}

inline bool isPolymorphic(const SimpleType& classType, const InstantiationContext& context)
{
	SYMBOLS_ASSERT(isClass(classType));
	SYMBOLS_ASSERT(isComplete(classType));
	instantiateClass(classType, context); // requires a complete type
	return classType.isPolymorphic;
}

// return true if class (or any base thereof) has at least one virtual function.
inline bool isPolymorphic(UniqueTypeWrapper type, const InstantiationContext& context)
{
	if(!isClass(type))
	{
		return false;
	}
	const SimpleType& classType = getSimpleType(type.value);
	return isPolymorphic(classType, context);
}

inline bool isAbstract(const SimpleType& classType, const InstantiationContext& context)
{
	SYMBOLS_ASSERT(isClass(classType));
	SYMBOLS_ASSERT(isComplete(classType));
	instantiateClass(classType, context); // requires a complete type
	return classType.isAbstract;
}

// return true if class (or any base thereof) has at least one pure virtual function.
inline bool isAbstract(UniqueTypeWrapper type, const InstantiationContext& context)
{
	if(!isClass(type))
	{
		return false;
	}
	const SimpleType& classType = getSimpleType(type.value);
	return isAbstract(classType, context);
}

inline bool isConvertibleTo(UniqueTypeWrapper from, UniqueTypeWrapper to, const InstantiationContext& context)
{
	return getIcsRank(to, from, context) != ICSRANK_INVALID;
}


inline UnaryTypeTraitsOp getUnaryTypeTraitsOp(cpp::typetraits_unary* symbol)
{
	switch(symbol->id)
	{
	case cpp::typetraits_unary::HAS_NOTHROW_CONSTRUCTOR: return hasNothrowConstructor;
	case cpp::typetraits_unary::HAS_NOTHROW_COPY: return hasNothrowCopy;
	case cpp::typetraits_unary::HAS_TRIVIAL_ASSIGN: return hasTrivialAssign;
	case cpp::typetraits_unary::HAS_TRIVIAL_CONSTRUCTOR: return hasTrivialConstructor;
	case cpp::typetraits_unary::HAS_TRIVIAL_COPY: return hasTrivialCopy;
	case cpp::typetraits_unary::HAS_TRIVIAL_DESTRUCTOR: return hasTrivialDestructor;
	case cpp::typetraits_unary::HAS_VIRTUAL_DESTRUCTOR: return hasVirtualDestructor;
	case cpp::typetraits_unary::IS_ABSTRACT: return isAbstract;
	case cpp::typetraits_unary::IS_CLASS: return isClass;
	case cpp::typetraits_unary::IS_EMPTY: return isEnum;
	case cpp::typetraits_unary::IS_ENUM: return isEmpty;
	case cpp::typetraits_unary::IS_POD: return isPod;
	case cpp::typetraits_unary::IS_POLYMORPHIC: return isPolymorphic;
	case cpp::typetraits_unary::IS_UNION: return isUnion;
	default: break;
	}
	throw SymbolsError();
}

inline BinaryTypeTraitsOp getBinaryTypeTraitsOp(cpp::typetraits_binary* symbol)
{
	switch(symbol->id)
	{
	case cpp::typetraits_binary::IS_BASE_OF: return isBaseOf;
	case cpp::typetraits_binary::IS_CONVERTIBLE_TO: return isConvertibleTo;
	default: break;
	}
	throw SymbolsError();
}

#endif
