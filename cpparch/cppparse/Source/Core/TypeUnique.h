
#ifndef INCLUDED_CPPPARSE_CORE_TYPEUNIQUE_H
#define INCLUDED_CPPPARSE_CORE_TYPEUNIQUE_H

#include "Ast/Type.h"
#include "TypeSubstitute.h"
#include "ExpressionEvaluate.h"

inline UniqueTypeWrapper makeUniqueQualifying(const Qualifying& qualifying, const InstantiationContext& context, bool allowDependent = false)
{
	return qualifying.empty()
		|| isNamespace(*qualifying.back().declaration)
		? gUniqueTypeNull
		: getUniqueType(qualifying.back(), context, allowDependent);
}


inline const SimpleType* makeUniqueEnclosing(const Qualifying& qualifying, const InstantiationContext& context, UniqueTypeWrapper& unique)
{
	if(!qualifying.empty())
	{
		if(isNamespace(*qualifying.back().declaration))
		{
			return 0; // name is qualified by a namespace, therefore cannot be enclosed by a class
		}
		bool allowDependent = qualifying.back().isDependent;
		unique = getUniqueType(qualifying.back(), context, allowDependent);
		if(allowDependent // if this is a dependent type
			&& !unique.isSimple())
		{
			return 0;
		}
		const SimpleType& type = getSimpleType(unique.value);
		if(allowDependent // if this is a dependent type
			&& !isEnclosingType(context.enclosingType, &type)) // which not the current instantiation
		{
			return 0;
		}
		// [temp.inst] A class template is implicitly instantiated ... if the completeness of the class-type affects the semantics of the program.
		instantiateClass(type, context, allowDependent);
		return &type;
	}
	return context.enclosingType;
}

inline const SimpleType* makeUniqueEnclosing(const Qualifying& qualifying, const InstantiationContext& context)
{
	UniqueTypeWrapper tmp;
	return makeUniqueEnclosing(qualifying, context, tmp);
}


inline UniqueTypeWrapper makeUniqueTemplateArgument(const TemplateArgument& argument, const InstantiationContext& context, bool allowDependent, bool allowSubstitution = false)
{
	SYMBOLS_ASSERT(argument.type.declaration != 0);
	extern Declaration gNonType;
	if(argument.type.declaration == &gNonType)
	{
		SYMBOLS_ASSERT(argument.expression.isUnique); // TODO: report non-fatal error: expected integral-constant-expression
		if(allowDependent && argument.expression.isValueDependent)
		{
			return pushType(gUniqueTypeNull, DependentNonType(argument.expression));
		}
		ExpressionValue result = evaluateExpression(argument.expression, context);
		SYMBOLS_ASSERT(result.isConstant);
		return pushType(gUniqueTypeNull, NonType(result.value));
	}

	return getUniqueTypeImpl(argument.type, context, allowDependent && argument.type.isDependent, allowSubstitution);
}


inline void makeUniqueTemplateParameters(const TemplateParameters& templateParams, TemplateArgumentsInstance& arguments, const InstantiationContext& context)
{
	arguments.reserve(std::distance(templateParams.begin(), templateParams.end()));
	for(Types::const_iterator i = templateParams.begin(); i != templateParams.end(); ++i)
	{
		const Type& argument = (*i);
		UniqueTypeWrapper result;
		extern Declaration gParam;
		if(argument.declaration->type.declaration == &gParam)
		{
			result = getUniqueType(argument);
			SYMBOLS_ASSERT(result.value != UNIQUETYPE_NULL);
		}
		else
		{
			UniqueTypeWrapper type = getUniqueType(argument.declaration->type);

			bool isTypeDependent = isDependent(type);
			SubstitutedExpression substituted(
				isTypeDependent ? gNullExpressionType : ExpressionType(substitute(type, context), false), // non-lvalue
				EXPRESSIONRESULT_INVALID,
				Dependent(unsigned char(argument.declaration->scope->templateDepth - 1)),
				true, isTypeDependent, true);
			ExpressionWrapper expression = ExpressionWrapper(makeUniqueExpression(NonTypeTemplateParameter(argument.declaration, type)), substituted);
			expression.isUnique = true;
			result = pushType(gUniqueTypeNull, DependentNonType(expression));
		}
		arguments.push_back(result);
	}
	SYMBOLS_ASSERT(arguments.size() == arguments.capacity());
}

inline bool isDependentTemplateArgument(const TemplateArgument& argument)
{
	return argument.type.declaration == &gNonType
		? argument.expression.isTypeDependent || argument.expression.isValueDependent
		: argument.type.isDependent;
}


inline void makeUniqueTemplateArguments(const TemplateArguments& arguments, TemplateArgumentsInstance& templateArguments, const InstantiationContext& context)
{
	templateArguments.reserve(std::distance(arguments.begin(), arguments.end()));
	for(TemplateArguments::const_iterator i = arguments.begin(); i != arguments.end(); ++i)
	{
		const TemplateArgument& argument = *i;
		SYMBOLS_ASSERT(argument.type.declaration != 0); // TODO: non-fatal error: not enough template arguments!
		bool allowDependent = isDependentTemplateArgument(argument);
		UniqueTypeWrapper result = makeUniqueTemplateArgument(argument, context, allowDependent);
		templateArguments.push_back(result);
	}
}

inline bool isDependentQualifying(const Qualifying& qualifying)
{
	if(qualifying.empty()
		|| isNamespace(*qualifying.back().declaration))
	{
		return false;
	}
	return qualifying.back().isDependent;
}


// unqualified object name: int, Object,
// qualified object name: Qualifying::Object
// unqualified typedef: Typedef, TemplateParam
// qualified typedef: Qualifying::Type
// /p type
// /p enclosingType The enclosing template, required when uniquing a template-argument: e.g. Enclosing<int>::Type
//			Note: if 'type' is a class-template template default argument, 'enclosingType' will be the class-template, which does not require instantiation!
inline UniqueTypeWrapper makeUniqueType(const Type& type, const InstantiationContext& context)
{
	if(type.expression) // decltype(expression)
	{
		if(type.expression.isTypeDependent)
		{
			return pushType(gUniqueTypeNull, DependentDecltype(type.expression));
		}
		return typeOfDecltypeSpecifier(type.expression, context);
	}
	// the type in which template-arguments are looked up: returns qualifying type if specified, else returns enclosingType
	UniqueTypeWrapper qualifying;
	const SimpleType* enclosing = makeUniqueEnclosing(type.qualifying, context, qualifying);
	Declaration* declaration = type.declaration;
	extern Declaration gDependentType;
	extern Declaration gDependentTemplate;
	extern Declaration gDependentNested;
	extern Declaration gDependentNestedTemplate;
	if(declaration == &gDependentType // T::Type
		|| declaration == &gDependentTemplate // T::Type<>
		|| declaration == &gDependentNested // T::Type::
		|| declaration == &gDependentNestedTemplate) // T::Type<>::
	{
		// this is a type-name (or template-id) preceded by a dependent nested-name-specifier
		bool isNested = declaration == &gDependentNested || declaration == &gDependentNestedTemplate;
		SYMBOLS_ASSERT(type.id != IdentifierPtr(0));
		TemplateArgumentsInstance templateArguments;
		makeUniqueTemplateArguments(type.templateArguments, templateArguments, context);
		return pushType(gUniqueTypeNull, DependentTypename(type.id->value, qualifying, templateArguments, isNested, declaration->isTemplate));
	}
	size_t index = declaration->templateParameter;
	if(index != INDEX_INVALID)
	{
		SYMBOLS_ASSERT(type.qualifying.empty());
		// Find the template-specialisation it belongs to:
		const SimpleType* parameterEnclosing = findEnclosingType(enclosing, declaration->scope);
		if(parameterEnclosing != 0
			&& !isDependent(*parameterEnclosing)) // if the enclosing type is not dependent
		{
			SYMBOLS_ASSERT(!parameterEnclosing->declaration->isSpecialization || parameterEnclosing->instantiated); // a specialization must be instantiated (or in the process of instantiating)
			const TemplateArgumentsInstance& arguments = parameterEnclosing->declaration->isSpecialization
				? parameterEnclosing->deducedArguments
				: parameterEnclosing->templateArguments;

			SYMBOLS_ASSERT(index < arguments.size());
			UniqueTypeWrapper result = arguments[index];
			SYMBOLS_ASSERT(result != gUniqueTypeNull); // fails for non-type template-argument
			return result;
		}

		TemplateArgumentsInstance templateArguments; // for template-template-parameter
		makeUniqueTemplateArguments(type.templateArguments, templateArguments, context);
		std::size_t templateParameterCount = declaration->isTemplate ? std::distance(declaration->templateParams.begin(), declaration->templateParams.end()) : 0;
		return UniqueTypeWrapper(pushUniqueType(gUniqueTypes, UNIQUETYPE_NULL, DependentType(declaration, templateArguments, templateParameterCount)));
	}

	// TODO: resolve using-declaration during name lookup instead?
	while(isUsing(*declaration)) // occurs when this declaration names a template that is introduced via a using-declaration
	{
		SYMBOLS_ASSERT(declaration->usingMember != &gDependentTypeInstance);
		declaration = *declaration->usingMember;
		SYMBOLS_ASSERT(declaration->templateParameter == INDEX_INVALID);
	}

	const SimpleType* memberEnclosing = isMember(*declaration) // if the declaration is a class member
		? findEnclosingType(enclosing, declaration->scope) // it must be a member of (a base of) the qualifying class: find which one.
		: 0; // the declaration is not a class member and cannot be found through qualified name lookup

	if(isTypedef(*declaration))
	{
		bool allowDependent = declaration->type.isDependent
			&& (memberEnclosing == 0 || memberEnclosing->isLocal || isDependent(*memberEnclosing)); // if a member of a non-dependent non-local type, declaration cannot be dependent
		UniqueTypeWrapper result = getUniqueType(declaration->type, setEnclosingType(context, memberEnclosing), allowDependent);
		return result;
	}

	if(declaration->isTemplate
		&& type.isImplicitTemplateId // if no template argument list was specified
		&& !type.isInjectedClassName) // and the type is not the name of an enclosing class
	{
		// this is a template-name
		return UniqueTypeWrapper(pushUniqueType(gUniqueTypes, UNIQUETYPE_NULL, TemplateTemplateArgument(declaration, memberEnclosing)));
	}

	SimpleType tmp(declaration, memberEnclosing);
	SYMBOLS_ASSERT(declaration->type.declaration != &gArithmetic || tmp.enclosing == 0); // arithmetic types should not have an enclosing template!
	if(declaration->isTemplate)
	{
		SYMBOLS_ASSERT(isClass(*declaration));
		tmp.declaration = tmp.primary = findPrimaryTemplate(declaration);

		// [temp.local]
		// Like normal (non-template) classes, class templates have an injected-class-name. The injected-class-
		// name can be used as a template-name or a type-name. When it is used with a template-argument-list,
		// as a template-argument for a template template-parameter, or as the final identifier in the elaborated-type-specifier
		// of a friend class template declaration, it refers to the class template itself. Otherwise, it is equivalent
		// to the template-name followed by the template-parameters of the class template enclosed in <>.
		// 
		// Within the scope of a class template specialization or partial specialization, when the injected-class-name is
		// used as a type-name, it is equivalent to the template-name followed by the template-arguments of the class
		// template specialization or partial specialization enclosed in <>.

		// when the name of a class template is used without arguments, substitute the parameters (in case of an enclosing explicit/partial-specialization, substitute the arguments)
		bool isEnclosingSpecialization = type.isInjectedClassName && isSpecialization(*type.declaration);
		const TemplateArguments& defaults = tmp.declaration->templateParams.defaults;
		SYMBOLS_ASSERT(!defaults.empty());
		if(type.isImplicitTemplateId // if no template argument list was specified
			&& !isEnclosingSpecialization) // and the type is not the name of an enclosing class-template explicit/partial-specialization
		{
			// substitute the primary template's template parameters.
			makeUniqueTemplateParameters(tmp.declaration->templateParams, tmp.templateArguments, context);
		}
		else
		{
			tmp.templateArguments.reserve(std::distance(defaults.begin(), defaults.end()));
			const TemplateArguments& arguments = isEnclosingSpecialization
				? type.declaration->templateArguments
				: type.templateArguments;

			bool dependent = false;
			TemplateArguments::const_iterator d = defaults.begin();
			for(TemplateArguments::const_iterator a = arguments.begin(); a != arguments.end(); ++a, ++d)
			{
				const TemplateArgument& argument = *a;
				bool allowDependent = isDependentTemplateArgument(argument);
				dependent |= allowDependent;
				UniqueTypeWrapper result = makeUniqueTemplateArgument(argument, context, allowDependent);
				tmp.templateArguments.push_back(result);
			}

			if(!dependent)
			{
				for(; d != defaults.end(); ++d)
				{
					const TemplateArgument& argument = *d;
					SYMBOLS_ASSERT(argument.type.declaration != 0); // TODO: non-fatal error: not enough template arguments!
					// resolve dependent template-parameter-defaults in context of template class
					UniqueTypeWrapper result = makeUniqueTemplateArgument(argument, setEnclosingType(context, &tmp), false, true);
					tmp.templateArguments.push_back(result);
				}

				SYMBOLS_ASSERT(!tmp.templateArguments.empty()); // non-dependent templates must have at least one argument
			}
		}
	}
	SYMBOLS_ASSERT(tmp.bases.empty());
	static size_t uniqueId = 0;
	tmp.uniqueId = ++uniqueId;
	return makeUniqueSimpleType(tmp);
}


inline std::size_t evaluateArraySize(const ExpressionWrapper& expression, const InstantiationContext& context)
{
	if(expression == 0) // []
	{
		return 0;
	}
	ExpressionValue result = evaluateExpression(expression, context);
	SYMBOLS_ASSERT(result.isConstant);
	return result.value.value;
}

struct TypeSequenceMakeUnique : TypeSequenceVisitor
{
	UniqueType& type;
	const InstantiationContext context;
	TypeSequenceMakeUnique(UniqueType& type, const InstantiationContext& context)
		: type(type), context(context)
	{
	}
	void visit(const DeclaratorPointerType& element)
	{
		pushUniqueType(type, PointerType());
		type.setQualifiers(element.qualifiers);
	}
	void visit(const DeclaratorReferenceType& element)
	{
		pushUniqueType(type, ReferenceType());
	}
	void visit(const DeclaratorArrayType& element)
	{
		for(ArrayRank::const_reverse_iterator i = element.rank.rbegin(); i != element.rank.rend(); ++i)
		{
			const ExpressionWrapper& expression = *i;
			if(expression.isValueDependent)
			{
				SYMBOLS_ASSERT(expression.isUnique);
				pushUniqueType(type, DependentArrayType(expression));
			}
			else
			{
				pushUniqueType(type, ArrayType(evaluateArraySize(expression, context)));
			}
		}
	}
	void visit(const DeclaratorMemberPointerType& element)
	{
		bool allowDependent = element.type.isDependent;
		UniqueTypeWrapper tmp = getUniqueType(element.type, context, allowDependent);
		pushUniqueType(type, MemberPointerType(tmp));
		type.setQualifiers(element.qualifiers);
	}
	void visit(const DeclaratorFunctionType& element)
	{
		FunctionType result;
		result.isEllipsis = element.parameters.isEllipsis;
		result.parameterTypes.reserve(element.parameters.size());
		for(Parameters::const_iterator i = element.parameters.begin(); i != element.parameters.end(); ++i)
		{
			bool allowDependent = (*i).declaration->type.isDependent;
			result.parameterTypes.push_back(getUniqueType((*i).declaration->type, context, allowDependent));
		}
		pushUniqueType(type, result);
		type.setQualifiers(element.qualifiers);
	}
};

inline UniqueTypeWrapper makeUniqueType(const TypeId& type, const InstantiationContext& context)
{
	UniqueTypeWrapper result = makeUniqueType(*static_cast<const Type*>(&type), context);
	result.value.addQualifiers(type.qualifiers);
	TypeSequenceMakeUnique visitor(result.value, context);
	type.typeSequence.accept(visitor);
	return result;
}


#endif
