
#ifndef INCLUDED_CPPPARSE_SEMA_SEMAPRIMARYEXPRESSION_H
#define INCLUDED_CPPPARSE_SEMA_SEMAPRIMARYEXPRESSION_H

#include "SemaCommon.h"
#include "SemaIdExpression.h"
#include "Core/Literal.h"


struct SemaLiteral : public SemaBase
{
	SEMA_BOILERPLATE;

	ExpressionWrapper expression;
	SemaLiteral(const SemaState& state)
		: SemaBase(state)
	{
	}
	SEMA_POLICY(cpp::numeric_literal, SemaPolicyIdentity)
	void action(cpp::numeric_literal* symbol)
	{
		if(symbol->id == cpp::numeric_literal::UNKNOWN) // workaround for boost::wave issue: T_PP_NUMBER exists in final token stream
		{
			symbol->id = isFloatingLiteral(symbol->value.value.c_str()) ? cpp::numeric_literal::FLOATING : cpp::numeric_literal::INTEGER;
		}
		expression = makeUniqueExpression(parseNumericLiteral(symbol));
	}
	SEMA_POLICY(cpp::string_literal, SemaPolicyIdentity)
	void action(cpp::string_literal* symbol)
	{
		expression = makeUniqueExpression(IntegralConstantExpression(getStringLiteralType(symbol), IntegralConstant()));
	}
};


inline bool isIntegralConstant(UniqueTypeWrapper type)
{
	return type.isSimple()
		&& type.value.getQualifiers().isConst
		&& (isIntegral(type)
		|| isEnumeration(type));
}


struct SemaPrimaryExpression : public SemaBase
{
	SEMA_BOILERPLATE;

	UniqueTypeId type;
	ExpressionWrapper expression;
	IdentifierPtr id; // only valid when the expression is a (parenthesised) id-expression
	TemplateArguments arguments; // only valid when the expression is a (qualified) template-id
	const SimpleType* idEnclosing; // may be valid when the above id-expression is a qualified-id
	Dependent typeDependent;
	Dependent valueDependent;
	bool isUndeclared;
	SemaPrimaryExpression(const SemaState& state)
		: SemaBase(state), id(0), arguments(context), idEnclosing(0), isUndeclared(false)
	{
	}
	SEMA_POLICY(cpp::literal, SemaPolicyPush<struct SemaLiteral>)
	void action(cpp::literal* symbol, const SemaLiteral& walker)
	{
		expression = walker.expression;
		type = typeOfExpression(expression, getInstantiationContext());
		SEMANTIC_ASSERT(!type.empty());
	}
	SEMA_POLICY(cpp::id_expression, SemaPolicyPushChecked<struct SemaIdExpression>)
	bool action(cpp::id_expression* symbol, SemaIdExpression& walker)
	{
		if(!walker.commit())
		{
			return reportIdentifierMismatch(symbol, *walker.id, walker.declaration, "object-name");
		}
		id = walker.id;
		arguments = walker.arguments;
		type = gUniqueTypeNull;
		LookupResultRef declaration = walker.declaration;
		addDependent(typeDependent, walker.typeDependent);
		addDependent(valueDependent, walker.valueDependent);
		expression = walker.expression;
		isUndeclared = walker.isUndeclared;

		SEMANTIC_ASSERT(memberClass == 0); // assert that the id-expression is not part of a class-member-access

		if(isUndeclared)
		{
			type = gUniqueTypeNull;
			idEnclosing = 0;
		}
		else if(expression.p != 0
			&& !isDependent(typeDependent))
		{
			UniqueTypeWrapper qualifyingType = makeUniqueQualifying(walker.qualifying, getInstantiationContext());
			const SimpleType* qualifyingClass = qualifyingType == gUniqueTypeNull ? 0 : &getSimpleType(qualifyingType.value);
			type = typeOfIdExpression(qualifyingClass, declaration, getInstantiationContext());
			idEnclosing = isSpecialMember(*declaration) ? 0 : getIdExpressionClass(qualifyingClass, declaration, enclosingType);

			if(!type.isFunction()) // if the id-expression refers to a function, overload resolution depends on the parameter types; defer evaluation of type
			{
				// [expr.const]
				// An integral constant-expression can involve only ... enumerators, const variables or static
				// data members of integral or enumeration types initialized with constant expressions, non-type template
				// parameters of integral or enumeration types
				expression.isConstant |= (isIntegralConstant(type) && declaration->initializer.isConstant); // TODO: determining whether the expression is constant depends on the type of the expression!
			}
		}
		return true;
	}
	SEMA_POLICY(cpp::primary_expression_parenthesis, SemaPolicyPush<struct SemaExpression>)
	void action(cpp::primary_expression_parenthesis* symbol, const SemaExpressionResult& walker)
	{
		type = walker.type;
		expression = walker.expression;
		id = walker.id;
		addDependent(typeDependent, walker.typeDependent);
		addDependent(valueDependent, walker.valueDependent);
	}
	SEMA_POLICY(cpp::primary_expression_builtin, SemaPolicyIdentity)
	void action(cpp::primary_expression_builtin* symbol)
	{
		SEMANTIC_ASSERT(enclosingType != 0);
		// TODO: cv-qualifiers: change enclosingType to a UniqueType<SimpleType>
		type = UniqueTypeWrapper(pushUniqueType(gUniqueTypes, makeUniqueSimpleType(*enclosingType).value, PointerType()));
		/* 14.6.2.2-2
		'this' is type-dependent if the class type of the enclosing member function is dependent
		*/
		addDependent(typeDependent, enclosingDependent);
		expression = makeExpression(ExplicitTypeExpression(type), false, isDependent(typeDependent));
		setExpressionType(symbol, type);
	}
};

#endif
