
#ifndef INCLUDED_CPPPARSE_CORE_TYPESUBSTITUTE_H
#define INCLUDED_CPPPARSE_CORE_TYPESUBSTITUTE_H

#include "Ast/Type.h"
#include "Ast/Print.h"
#include "TypeInstantiate.h"


UniqueTypeWrapper substituteImpl(UniqueTypeWrapper dependent, const InstantiationContext& context);
inline UniqueTypeWrapper substitute(UniqueTypeWrapper dependent, const InstantiationContext& context)
{
	if(!isDependent(dependent)) // if the type to be substituted is not dependent
	{
		return dependent; // no substitution required
	}
	return substituteImpl(dependent, context);
}
void substitute(UniqueTypeArray& substituted, const UniqueTypeArray& dependent, const InstantiationContext& context);
// ----------------------------------------------------------------------------

struct TypeError
{
	virtual void report() = 0;
};


struct TypeErrorBase : TypeError
{
	Location source;
	TypeErrorBase(Location source) : source(source)
	{
	}
	void report()
	{
		printPosition(source);
	}
};

struct MemberNotFoundError : TypeErrorBase
{
	Name name;
	const SimpleType* qualifying;
	MemberNotFoundError(Location source, Name name, const SimpleType* qualifying)
		: TypeErrorBase(source), name(name), qualifying(qualifying)
	{
		SYMBOLS_ASSERT(qualifying != 0);
	}
	void report()
	{
		TypeErrorBase::report();
		std::cout << "member '" << name.c_str() << "' not found in ";
#if 1
		if(qualifying->instantiating)
		{
			std::cout << "(partially instantiated) ";
		}
#endif
		printType(*qualifying);
		std::cout << std::endl;
	}
};

struct MemberIsNotTypeError : TypeErrorBase
{
	Name name;
	UniqueTypeWrapper qualifying;
	MemberIsNotTypeError(Location source, Name name, UniqueTypeWrapper qualifying)
		: TypeErrorBase(source), name(name), qualifying(qualifying)
	{
	}
	void report()
	{
		TypeErrorBase::report();
		std::cout << "member '" << name.c_str() << "' is not a type in ";
		printType(qualifying);
		std::cout << std::endl;
	}
};

struct ExpectedTemplateTemplateArgumentError : TypeErrorBase
{
	UniqueTypeWrapper type;
	ExpectedTemplateTemplateArgumentError(Location source, UniqueTypeWrapper type)
		: TypeErrorBase(source), type(type)
	{
	}
	void report()
	{
		TypeErrorBase::report();
		std::cout << "expected template template argument: ";
		printType(type);
		std::cout << std::endl;
	}
};

struct MismatchedTemplateTemplateArgumentError : TypeErrorBase
{
	UniqueTypeWrapper type;
	MismatchedTemplateTemplateArgumentError(Location source, UniqueTypeWrapper type)
		: TypeErrorBase(source), type(type)
	{
	}
	void report()
	{
		TypeErrorBase::report();
		std::cout << "mismatched template template argument: ";
		printType(type);
		std::cout << std::endl;
	}
};

struct QualifyingIsNotClassError : TypeErrorBase
{
	UniqueTypeWrapper qualifying;
	QualifyingIsNotClassError(Location source, UniqueTypeWrapper qualifying)
		: TypeErrorBase(source), qualifying(qualifying)
	{
	}
	void report()
	{
		TypeErrorBase::report();
		std::cout << "qualifying type is not a class: ";
		printType(qualifying);
		std::cout << std::endl;
	}
};

struct PointerToReferenceError : TypeErrorBase
{
	PointerToReferenceError(Location source)
		: TypeErrorBase(source)
	{
	}
	void report()
	{
		TypeErrorBase::report();
		std::cout << "cannot create pointer to reference" << std::endl;
	}
};

struct ReferenceToReferenceError : TypeErrorBase
{
	ReferenceToReferenceError(Location source)
		: TypeErrorBase(source)
	{
	}
	void report()
	{
		TypeErrorBase::report();
		std::cout << "cannot create reference to reference" << std::endl;
	}
};

struct InvalidArrayError : TypeErrorBase
{
	InvalidArrayError(Location source)
		: TypeErrorBase(source)
	{
	}
	void report()
	{
		TypeErrorBase::report();
		std::cout << "cannot create array of type void, function or reference" << std::endl;
	}
};

struct VoidParameterError : TypeErrorBase
{
	VoidParameterError(Location source)
		: TypeErrorBase(source)
	{
	}
	void report()
	{
		TypeErrorBase::report();
		std::cout << "cannot create function with void parameter" << std::endl;
	}
};

struct TooFewTemplateArgumentsError : TypeErrorBase
{
	TooFewTemplateArgumentsError(Location source)
		: TypeErrorBase(source)
	{
	}
	void report()
	{
		TypeErrorBase::report();
		std::cout << "too few template arguments" << std::endl;
	}
};



inline UniqueTypeWrapper getUniqueType(const TypeId& type, const InstantiationContext& context, bool allowDependent = false);
inline UniqueTypeWrapper getUniqueType(const Type& type, const InstantiationContext& context, bool allowDependent = false);

template<typename T>
inline UniqueTypeWrapper getUniqueTypeImpl(const T& type, const InstantiationContext& context, bool allowDependent)
{
	UniqueTypeWrapper result = getUniqueType(type);
	if(type.isDependent
		&& !allowDependent)
	{
		UniqueTypeWrapper substituted = substitute(result, context);
		SYMBOLS_ASSERT(!isDependent(substituted));
		return substituted;
	}
	return result;
}

inline UniqueTypeWrapper getUniqueType(const TypeId& type, const InstantiationContext& context, bool allowDependent)
{
	return getUniqueTypeImpl(type, context, allowDependent);
}

inline UniqueTypeWrapper getUniqueType(const Type& type, const InstantiationContext& context, bool allowDependent)
{
	return getUniqueTypeImpl(type, context, allowDependent);
}



struct ClassMember
{
	const SimpleType* enclosing;
	DeclarationInstanceRef declaration;
	ClassMember(const SimpleType* enclosing, DeclarationInstanceRef declaration)
		: enclosing(enclosing), declaration(declaration)
	{
	}
};

inline ClassMember substituteClassMember(UniqueTypeWrapper qualifying, Name name, const InstantiationContext& context)
{
	// evaluate the qualifying/enclosing/declaration referred to by the using declaration
	qualifying = substitute(qualifying, context);
	SYMBOLS_ASSERT(qualifying != gUniqueTypeNull);
	SYMBOLS_ASSERT(qualifying.isSimple());
	const SimpleType* enclosing = &getSimpleType(qualifying.value);

	instantiateClass(*enclosing, context);
	Identifier id;
	id.value = name;
	std::size_t visibility = enclosing->instantiating ? getPointOfInstantiation(*context.enclosingType) : VISIBILITY_ALL;
	LookupResultRef result = findDeclaration(*enclosing, id, LookupFilter(IsAny(visibility)));

	Declaration* declaration = result;

	if(result == DeclarationPtr(0)) // if the name was not found within the qualifying class
	{
		// [temp.deduct]
		// - Attempting to use a type in the qualifier portion of a qualified name that names a type when that
		//   type does not contain the specified member
		throw MemberNotFoundError(context.source, name, enclosing);
	}

	return ClassMember(enclosing, result);
}

inline ClassMember getUsingMember(const Declaration& declaration)
{
	SYMBOLS_ASSERT(isUsing(declaration));
	SYMBOLS_ASSERT(!isDependent(declaration.usingBase));
	const SimpleType* enclosing = declaration.usingBase == gUniqueTypeNull ? 0 : &getSimpleType(declaration.usingBase.value);
	return ClassMember(enclosing, *declaration.usingMember);
}

inline ClassMember evaluateClassMember(ClassMember member, const InstantiationContext& context)
{
	const Declaration& declaration = *member.declaration;
	if(!isUsing(declaration)) // if the member name was not introduced by a using declaration
	{
		return member; // nothing to do
	}

	// the member name was introduced by a using declaration
	ClassMember substituted = isDependent(declaration.usingBase) // if the name is dependent
		? substituteClassMember(declaration.usingBase, declaration.getName().value, context) // substitute it
		: getUsingMember(declaration);

	return evaluateClassMember(substituted, context); // the result may also be a (possibly depedendent) using-declaration
}

inline ClassMember evaluateClassMember(ClassMember member)
{
	const Declaration& declaration = *member.declaration;
	if(!isUsing(declaration)) // if the member name was not introduced by a using declaration
	{
		return member; // nothing to do
	}	

	// the member name was introduced by a using declaration
	SYMBOLS_ASSERT(!isDependent(declaration.usingBase));
	return evaluateClassMember(getUsingMember(declaration));
}

#endif
