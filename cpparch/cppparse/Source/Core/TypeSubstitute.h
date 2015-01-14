
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
const SimpleType& substitute(const SimpleType& dependent, const InstantiationContext& context);
inline const SimpleType* substitute(const SimpleType* dependent, const InstantiationContext& context)
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


inline UniqueTypeWrapper getSubstitutedType(const Type& type, const InstantiationContext& context)
{
	SYMBOLS_ASSERT(type.dependentIndex != INDEX_INVALID);
	SYMBOLS_ASSERT(context.enclosingType != 0);
	SYMBOLS_ASSERT(!isDependent(*context.enclosingType));
	SYMBOLS_ASSERT(type.dependentIndex < context.enclosingType->substitutedTypes.size());
	return context.enclosingType->substitutedTypes[type.dependentIndex];
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



struct QualifiedDeclaration
{
	const SimpleType* enclosing;
	DeclarationInstanceRef declaration;
	QualifiedDeclaration(const SimpleType* enclosing, DeclarationInstanceRef declaration)
		: enclosing(enclosing), declaration(declaration)
	{
	}
};

inline QualifiedDeclaration substituteClassMember(UniqueTypeWrapper qualifying, Name name, const InstantiationContext& context)
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

	return QualifiedDeclaration(enclosing, result);
}

inline QualifiedDeclaration getUsingMember(const Declaration& declaration)
{
	SYMBOLS_ASSERT(isUsing(declaration));
	SYMBOLS_ASSERT(!isDependent(declaration.usingBase));
	const SimpleType* enclosing = declaration.usingBase == gUniqueTypeNull ? 0 : &getSimpleType(declaration.usingBase.value);
	return QualifiedDeclaration(enclosing, *declaration.usingMember);
}

inline QualifiedDeclaration getUsingMember(const Declaration& declaration, const SimpleType* enclosing, const InstantiationContext& context)
{
	SYMBOLS_ASSERT(isUsing(declaration));
	const SimpleType* idEnclosing = enclosing != 0 ? enclosing : context.enclosingType;
	return isDependent(declaration.usingBase) // if the name is dependent
		? substituteClassMember(declaration.usingBase, declaration.getName().value, setEnclosingTypeSafe(context, idEnclosing)) // substitute it
		: getUsingMember(declaration);
}

inline QualifiedDeclaration resolveQualifiedDeclaration(QualifiedDeclaration qualified, const InstantiationContext& context)
{
	const Declaration& declaration = *qualified.declaration;
	if(!isUsing(declaration)) // if the member name was not introduced by a using declaration
	{
		return qualified; // nothing to do
	}

	// the member name was introduced by a using declaration
	QualifiedDeclaration substituted = getUsingMember(declaration, qualified.enclosing, context);
	return resolveQualifiedDeclaration(substituted, context); // the result may also be a (possibly depedendent) using-declaration
}

inline QualifiedDeclaration resolveQualifiedDeclaration(QualifiedDeclaration qualified)
{
	const Declaration& declaration = *qualified.declaration;
	if(!isUsing(declaration)) // if the member name was not introduced by a using declaration
	{
		return qualified; // nothing to do
	}	

	// the member name was introduced by a using declaration
	SYMBOLS_ASSERT(!isDependent(declaration.usingBase));
	return resolveQualifiedDeclaration(getUsingMember(declaration));
}

inline UniqueTypeWrapper substituteTemplateParameter(const Declaration& declaration, const InstantiationContext& context)
{
	size_t index = declaration.templateParameter;
	SYMBOLS_ASSERT(index != INDEX_INVALID);
	const SimpleType* enclosingType = findEnclosingTemplate(context.enclosingType, declaration.scope);
	SYMBOLS_ASSERT(enclosingType != 0);
	SYMBOLS_ASSERT(!enclosingType->declaration->isSpecialization || enclosingType->instantiated); // a specialization must be instantiated (or in the process of instantiating)
	const TemplateArgumentsInstance& templateArguments = enclosingType->declaration->isSpecialization
		? enclosingType->deducedArguments : enclosingType->templateArguments;
	SYMBOLS_ASSERT(index < templateArguments.size());
	return templateArguments[index];
}

inline const ExpressionWrapper& getSubstitutedExpression(const PersistentExpression& expression, const SimpleType* enclosingType)
{
	if(!isDependentExpression(expression))
	{
		return expression;
	}
	SYMBOLS_ASSERT(expression.dependentIndex != INDEX_INVALID);
	SYMBOLS_ASSERT(expression.dependentIndex < enclosingType->substitutedExpressions.size());
	return enclosingType->substitutedExpressions[expression.dependentIndex];
}

inline const ExpressionWrapper& getSubstitutedExpression(const PersistentExpression& expression, const InstantiationContext& context)
{
	const SimpleType* enclosingType = !isDependent(*context.enclosingType) ? context.enclosingType : context.enclosingType->enclosing;
	return getSubstitutedExpression(expression, enclosingType);
}

inline unsigned char getTemplateDepth(const SimpleType& instance)
{
	SYMBOLS_ASSERT(instance.declaration->templateParamScope != 0);
	return unsigned char(instance.declaration->templateParamScope->templateDepth - 1);
}

inline unsigned char findEnclosingTemplateDepth(const SimpleType* enclosingType)
{
	for(const SimpleType* i = enclosingType; i != 0; i = (*i).enclosing)
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
inline bool canSubstitute(const SimpleType* enclosingType, Dependent dependent)
{
	return canSubstitute(findEnclosingTemplateDepth(enclosingType), dependent);
}

// returns true if we depend only upon the enclosing template
inline bool canEvaluate(const SimpleType* enclosingType, Dependent dependent)
{
	return canEvaluate(findEnclosingTemplateDepth(enclosingType), dependent);
}

#endif
