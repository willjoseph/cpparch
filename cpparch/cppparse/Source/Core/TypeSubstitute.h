
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
const Instance& substitute(const Instance& dependent, const InstantiationContext& context);
inline const Instance* substitute(const Instance* dependent, const InstantiationContext& context)
{
	if(dependent == 0)
	{
		return 0;
	}
	return &substitute(*dependent, context);
}
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
	const Instance* qualifying;
	MemberNotFoundError(Location source, Name name, const Instance* qualifying)
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


inline UniqueTypeWrapper getSubstitutedType(const Type& type, const InstantiationContext& context)
{
	SYMBOLS_ASSERT(type.dependentIndex != INDEX_INVALID);
	SYMBOLS_ASSERT(context.enclosingInstance != 0);
	SYMBOLS_ASSERT(!isDependent(*context.enclosingInstance));
	SYMBOLS_ASSERT(type.dependentIndex < context.enclosingInstance->substitutedTypes.size());
	return context.enclosingInstance->substitutedTypes[type.dependentIndex];
}

inline UniqueTypeWrapper getUniqueType(const TypeId& type, const InstantiationContext& context, bool allowDependent = false);
inline UniqueTypeWrapper getUniqueType(const Type& type, const InstantiationContext& context, bool allowDependent = false);

template<typename T>
inline UniqueTypeWrapper getUniqueTypeImpl(const T& type, const InstantiationContext& context, bool allowDependent, bool allowSubstitution = false)
{
	UniqueTypeWrapper result = getUniqueType(type);
	if(type.isDependent
		&& !allowDependent)
	{
		if(type.dependentIndex == INDEX_INVALID)
		{
			//SYMBOLS_ASSERT(allowSubstitution); // TODO: only occurs for template default-argument
			UniqueTypeWrapper substituted = substitute(result, context);
			SYMBOLS_ASSERT(!isDependent(substituted));
			return substituted;
		}

		UniqueTypeWrapper substituted = getSubstitutedType(type, context);
		SYMBOLS_ASSERT(substituted == substitute(result, context));
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



struct ResolvedDeclaration
{
	const Instance* enclosingInstance;
	DeclarationInstanceRef declaration;
	ResolvedDeclaration(const Instance* enclosingInstance, DeclarationInstanceRef declaration)
		: enclosingInstance(enclosingInstance), declaration(declaration)
	{
	}
};

inline ResolvedDeclaration substituteClassMember(UniqueTypeWrapper qualifying, Name name, const InstantiationContext& context)
{
	// evaluate the qualifying/enclosing/declaration referred to by the using declaration
	qualifying = substitute(qualifying, context);
	SYMBOLS_ASSERT(qualifying != gUniqueTypeNull);
	SYMBOLS_ASSERT(qualifying.isSimple());
	const Instance* enclosing = &getInstance(qualifying.value);

	instantiateClass(*enclosing, context);
	Identifier id;
	id.value = name;
	std::size_t visibility = enclosing->instantiating ? getPointOfInstantiation(*context.enclosingInstance) : VISIBILITY_ALL;
	LookupResultRef result = findDeclaration(*enclosing, id, LookupFilter(IsAny(visibility)));

	Declaration* declaration = result;

	if(result == DeclarationPtr(0)) // if the name was not found within the qualifying class
	{
		// [temp.deduct]
		// - Attempting to use a type in the qualifier portion of a qualified name that names a type when that
		//   type does not contain the specified member
		throw MemberNotFoundError(context.source, name, enclosing);
	}

	return ResolvedDeclaration(enclosing, result);
}

inline ResolvedDeclaration getUsingMember(const Declaration& declaration)
{
	SYMBOLS_ASSERT(isUsing(declaration));
	SYMBOLS_ASSERT(!isDependent(declaration.usingBase));
	const Instance* memberEnclosing = declaration.usingBase == gUniqueTypeNull ? 0 : &getInstance(declaration.usingBase.value);
	return ResolvedDeclaration(memberEnclosing, *declaration.usingMember);
}

inline ResolvedDeclaration getUsingMember(const Declaration& declaration, const Instance* enclosingInstance, const InstantiationContext& context)
{
	SYMBOLS_ASSERT(isUsing(declaration));
	const Instance* memberEnclosing = enclosingInstance != 0 ? enclosingInstance : context.enclosingInstance;
	return isDependent(declaration.usingBase) // if the name is dependent
		? substituteClassMember(declaration.usingBase, declaration.getName().value, setEnclosingInstanceSafe(context, memberEnclosing)) // substitute it
		: getUsingMember(declaration);
}

inline ResolvedDeclaration resolveUsingDeclaration(ResolvedDeclaration resolved, const InstantiationContext& context)
{
	const Declaration& declaration = *resolved.declaration;
	if(!isUsing(declaration)) // if the member name was not introduced by a using declaration
	{
		return resolved; // nothing to do
	}

	// the member name was introduced by a using declaration
	ResolvedDeclaration substituted = getUsingMember(declaration, resolved.enclosingInstance, context);
	return resolveUsingDeclaration(substituted, context); // the result may also be a (possibly depedendent) using-declaration
}

inline ResolvedDeclaration resolveUsingDeclaration(ResolvedDeclaration resolved)
{
	const Declaration& declaration = *resolved.declaration;
	if(!isUsing(declaration)) // if the member name was not introduced by a using declaration
	{
		return resolved; // nothing to do
	}	

	// the member name was introduced by a using declaration
	SYMBOLS_ASSERT(!isDependent(declaration.usingBase));
	return resolveUsingDeclaration(getUsingMember(declaration));
}

inline UniqueTypeWrapper substituteTemplateParameter(const Declaration& declaration, const InstantiationContext& context)
{
	size_t index = declaration.templateParameter;
	SYMBOLS_ASSERT(index != INDEX_INVALID);
	const Instance* enclosingInstance = findEnclosingTemplate(context.enclosingInstance, declaration.scope);
	SYMBOLS_ASSERT(enclosingInstance != 0);
	SYMBOLS_ASSERT(!enclosingInstance->declaration->isSpecialization || enclosingInstance->instantiated); // a specialization must be instantiated (or in the process of instantiating)
	const TemplateArgumentsInstance& templateArguments = enclosingInstance->declaration->isSpecialization
		? enclosingInstance->deducedArguments : enclosingInstance->templateArguments;
	SYMBOLS_ASSERT(index < templateArguments.size());
	return templateArguments[index];
}

inline const ExpressionWrapper& getSubstitutedExpression(const PersistentExpression& expression, const Instance* enclosingInstance)
{
	if(!isDependentExpression(expression))
	{
		return expression;
	}
	SYMBOLS_ASSERT(expression.dependentIndex != INDEX_INVALID);
	SYMBOLS_ASSERT(expression.dependentIndex < enclosingInstance->substitutedExpressions.size());
	return enclosingInstance->substitutedExpressions[expression.dependentIndex];
}

inline const ExpressionWrapper& getSubstitutedExpression(const PersistentExpression& expression, const InstantiationContext& context)
{
	const Instance* enclosingInstance = !isDependent(*context.enclosingInstance) ? context.enclosingInstance : context.enclosingInstance->enclosing;
	return getSubstitutedExpression(expression, enclosingInstance);
}

inline unsigned char getTemplateDepth(const Instance& instance)
{
	SYMBOLS_ASSERT(instance.declaration->templateParamScope != 0);
	return unsigned char(instance.declaration->templateParamScope->templateDepth - 1);
}

inline unsigned char findEnclosingTemplateDepth(const Instance* enclosingInstance)
{
	for(const Instance* i = enclosingInstance; i != 0; i = (*i).enclosing)
	{
		if((*i).declaration->templateParamScope != 0)
		{
			return getTemplateDepth(*i);
		}
	}
	return 255;
}


// returns true if we depend upon the enclosing template
inline bool canSubstitute(unsigned char depth, Dependent dependent)
{
	SYMBOLS_ASSERT(depth != 255);
	SYMBOLS_ASSERT(dependent.any());
	return depth >= dependent.minDepth;
}

// returns true if we depend only upon the enclosing template
inline bool canEvaluate(unsigned char depth, Dependent dependent)
{
	SYMBOLS_ASSERT(depth != 255);
	SYMBOLS_ASSERT(dependent.any());
	return depth >= dependent.maxDepth;
}

// returns true if we depend upon the enclosing template
inline bool canSubstitute2(unsigned char depth, Dependent dependent)
{
	SYMBOLS_ASSERT(depth != 255);
	SYMBOLS_ASSERT(dependent.any());
	return depth == dependent.maxDepth;
}

// returns true if we depend upon the enclosing template
inline bool canSubstitute(const Instance* enclosingInstance, Dependent dependent)
{
	return canSubstitute(findEnclosingTemplateDepth(enclosingInstance), dependent);
}

// returns true if we depend only upon the enclosing template
inline bool canEvaluate(const Instance* enclosingInstance, Dependent dependent)
{
	return canEvaluate(findEnclosingTemplateDepth(enclosingInstance), dependent);
}

#endif
