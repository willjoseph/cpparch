
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
		expression = makeConstantExpression(parseNumericLiteral(symbol));
	}
	SEMA_POLICY(cpp::string_literal, SemaPolicyIdentity)
	void action(cpp::string_literal* symbol)
	{
		// [expr.prim.general] A string literal is an lvalue
		expression = makeConstantExpression(IntegralConstantExpression(ExpressionType(getStringLiteralType(symbol), true), IntegralConstant()));
	}
};



struct SemaPrimaryExpression : public SemaBase
{
	SEMA_BOILERPLATE;

	ExpressionWrapper expression;
	IdentifierPtr id; // only valid when the expression is a (parenthesised) id-expression
	TemplateArguments arguments; // only valid when the expression is a (qualified) template-id
	Dependent typeDependent;
	Dependent valueDependent;
	bool isUndeclared;
	SemaPrimaryExpression(const SemaState& state)
		: SemaBase(state), id(0), arguments(context)
		, isUndeclared(false)
	{
	}
	SEMA_POLICY(cpp::literal, SemaPolicyPush<struct SemaLiteral>)
	void action(cpp::literal* symbol, const SemaLiteral& walker)
	{
		expression = walker.expression;
		SEMANTIC_ASSERT(!expression.type.empty());
	}
	SEMA_POLICY(cpp::id_expression, SemaPolicyPushChecked<struct SemaIdExpression>)
	bool action(cpp::id_expression* symbol, SemaIdExpression& walker)
	{
		if(!walker.commit())
		{
			return reportIdentifierMismatch(symbol, *walker.id, walker.declaration, "object-name");
		}
		// [expr.prim.general]
		// An identifier is an id-expression provided it has been suitably declared (Clause 7). [....]
		// The type of the expression is the type of the identifier.
		// The result is the entity denoted by the identifier. The result is an lvalue if the entity is a function, variable,
		// or data member and a prvalue otherwise.
		id = walker.id;
		arguments = walker.arguments;
		LookupResultRef declaration = walker.declaration;
		addDependent(typeDependent, walker.typeDependent);
		addDependent(valueDependent, walker.valueDependent);
		expression = walker.expression;
		isUndeclared = walker.isUndeclared;

		SEMANTIC_ASSERT(memberClass == 0); // assert that the id-expression is not part of a class-member-access
		return true;
	}
	SEMA_POLICY(cpp::primary_expression_parenthesis, SemaPolicyPush<struct SemaExpression>)
	void action(cpp::primary_expression_parenthesis* symbol, const SemaExpressionResult& walker)
	{
		expression = walker.expression;
		expression.isParenthesised = true;
		id = walker.id;
		addDependent(typeDependent, walker.typeDependent);
		addDependent(valueDependent, walker.valueDependent);
	}
	SEMA_POLICY(cpp::primary_expression_builtin, SemaPolicyIdentity)
	void action(cpp::primary_expression_builtin* symbol)
	{
		SEMANTIC_ASSERT(enclosingType != 0);
		ExpressionType type = ExpressionType(pushType(typeOfEnclosingClass(getInstantiationContext()), PointerType()), false); // non lvalue
		/* 14.6.2.2-2
		'this' is type-dependent if the class type of the enclosing member function is dependent
		*/
		addDependent(typeDependent, enclosingDependent);
		expression = makeExpression(ExplicitTypeExpression(type), isDependentOld(typeDependent));
		setExpressionType(symbol, expression.type);
	}
};

#endif
