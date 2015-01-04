
#ifndef INCLUDED_CPPPARSE_SEMA_SEMAEXPRESSION_H
#define INCLUDED_CPPPARSE_SEMA_SEMAEXPRESSION_H

#include "SemaCommon.h"
#include "SemaPostfixExpression.h"
#include "Common/Util.h"

struct SemaExplicitTypeExpression : public SemaBase, SemaExplicitTypeExpressionResult
{
	SEMA_BOILERPLATE;

	SemaExplicitTypeExpression(const SemaState& state)
		: SemaBase(state), SemaExplicitTypeExpressionResult(context)
	{
	}
	SEMA_POLICY(cpp::simple_type_specifier, SemaPolicyPush<struct SemaTypeSpecifier>)
	void action(cpp::simple_type_specifier* symbol, const SemaTypeSpecifierResult& walker)
	{
		type = walker.type;
		if(type.declaration == 0)
		{
			type = getFundamentalType(walker.fundamental);
		}
		makeUniqueTypeSafe(type);
	}
	SEMA_POLICY(cpp::typename_specifier, SemaPolicyPush<struct SemaTypenameSpecifier>)
	void action(cpp::typename_specifier* symbol, const SemaTypenameSpecifierResult& walker)
	{
		type = walker.type;
		makeUniqueTypeSafe(type);
	}
	SEMA_POLICY(cpp::type_id, SemaPolicyPushCommit<struct SemaTypeId>)
	void action(cpp::type_id* symbol, const SemaTypeIdResult& walker)
	{
		walker.committed.test();
		type = walker.type;
	}
	SEMA_POLICY(cpp::assignment_expression, SemaPolicyPush<struct SemaExpression>)
	void action(cpp::assignment_expression* symbol, const SemaExpressionResult& walker)
	{
		arguments.push_back(walker.expression);
		isDependent |= isDependentExpression(walker.expression);
	}
	SEMA_POLICY(cpp::cast_expression, SemaPolicyPush<struct SemaExpression>)
	void action(cpp::cast_expression* symbol, const SemaExpressionResult& walker)
	{
		arguments.push_back(walker.expression);
		isDependent |= isDependentExpression(walker.expression);
	}
};

struct SemaNewExpression : public SemaBase, SemaExplicitTypeExpressionResult
{
	SEMA_BOILERPLATE;
	ExpressionWrapper newArray;

	SemaNewExpression(const SemaState& state)
		: SemaBase(state), SemaExplicitTypeExpressionResult(context)
	{
	}
	SEMA_POLICY(cpp::new_type, SemaPolicyPush<struct SemaNewType>)
	void action(cpp::new_type* symbol, const SemaNewTypeResult& walker)
	{
		type = walker.type;
		newArray = walker.newArray;
		makeUniqueTypeSafe(type);
	}
	SEMA_POLICY(cpp::assignment_expression, SemaPolicyPush<struct SemaExpression>)
	void action(cpp::assignment_expression* symbol, const SemaExpressionResult& walker)
	{
		arguments.push_back(walker.expression);
		isDependent |= isDependentExpression(walker.expression);
	}
};

struct SemaSizeofTypeExpression : public SemaBase
{
	SEMA_BOILERPLATE;

	ExpressionWrapper expression;
	SemaSizeofTypeExpression(const SemaState& state)
		: SemaBase(state)
	{
	}
	SEMA_POLICY(cpp::type_id, SemaPolicyPushCommit<struct SemaTypeId>)
	void action(cpp::type_id* symbol, const SemaTypeIdResult& walker)
	{
		walker.committed.test();
		UniqueTypeId type = getUniqueType(walker.type);
		expression = makeExpression(SizeofTypeExpression(type));
		setExpressionType(symbol, expression.type);
	}
};

struct SemaConditionalExpression : public SemaBase
{
	SEMA_BOILERPLATE;

	ExpressionWrapper left;
	ExpressionWrapper right;
	SemaConditionalExpression(const SemaState& state)
		: SemaBase(state)
	{
	}
	SEMA_POLICY(cpp::expression, SemaPolicyPush<struct SemaExpression>)
	void action(cpp::expression* symbol, const SemaExpressionResult& walker)
	{
		left = walker.expression;
	}
	SEMA_POLICY(cpp::assignment_expression, SemaPolicyPush<struct SemaExpression>)
	void action(cpp::assignment_expression* symbol, const SemaExpressionResult& walker)
	{
		right = walker.expression;
	}
};

// a comma-separated list of assignment_expression
struct SemaExpressionList : public SemaBase, SemaArgumentListResult
{
	SEMA_BOILERPLATE;

	SemaExpressionList(const SemaState& state)
		: SemaBase(state)
	{
	}

	SEMA_POLICY(cpp::assignment_expression, SemaPolicyPushSrc<struct SemaExpression>)
	void action(cpp::assignment_expression* symbol, const SemaExpressionResult& walker)
	{
		arguments.push_back(walker.expression);
		isDependent |= isDependentExpression(walker.expression);
		// [expr.comma] The type and value of the result are the type and value of the right operand
		setExpressionType(symbol, walker.expression.type);
	}
};

struct SemaExpression : public SemaBase, SemaExpressionResult
{
	SEMA_BOILERPLATE;

	bool isAddressOf;
	SemaExpression(const SemaState& state)
		: SemaBase(state), isAddressOf(false)
	{
	}

	// this path handles the right-hand side of a binary expression
	// it is assumed that 'type' already contains the type of the left-hand side
	template<BuiltInBinaryTypeOp typeOp, typename T>
	void walkBinaryExpression(T*& symbol, const SemaExpressionResult& walker)
	{
		id = walker.id;
		// TODO: SEMANTIC_ASSERT(type.declaration != 0 && walker.type.declaration != 0);
		BinaryIceOp iceOp = getBinaryIceOp(symbol);
		ExpressionWrapper leftExpression = expression;
		expression = makeExpression(BinaryExpression(getOverloadedOperatorId(symbol), iceOp, typeOfBinaryExpression<typeOp>, expression, walker.expression));
		ExpressionTypeHelper<T>::set(symbol, expression.type);
	}
	template<typename T>
	void walkBinaryArithmeticExpression(T* symbol, const SemaExpressionResult& walker)
	{
		walkBinaryExpression<binaryOperatorArithmeticType>(symbol, walker);
	}
	template<typename T>
	void walkBinaryAdditiveExpression(T* symbol, const SemaExpressionResult& walker)
	{
		walkBinaryExpression<binaryOperatorAdditiveType>(symbol, walker);
	}
	template<typename T>
	void walkBinaryIntegralExpression(T* symbol, const SemaExpressionResult& walker)
	{
		walkBinaryExpression<binaryOperatorIntegralType>(symbol, walker);
	}
	template<typename T>
	void walkBinaryBooleanExpression(T* symbol, const SemaExpressionResult& walker)
	{
		walkBinaryExpression<binaryOperatorBoolean>(symbol, walker);
	}
	SEMA_POLICY(cpp::assignment_expression_suffix, SemaPolicyPushSrc<struct SemaExpression>)
	void action(cpp::assignment_expression_suffix* symbol, const SemaExpressionResult& walker)
	{
		// 5.1.7 Assignment operators
		// the type of an assignment expression is that of its left operand
		walkBinaryExpression<binaryOperatorAssignment>(symbol, walker);
	}
	SEMA_POLICY(cpp::conditional_expression_suffix, SemaPolicyPush<struct SemaConditionalExpression>)
	void action(cpp::conditional_expression_suffix* symbol, const SemaConditionalExpression& walker)
	{
		expression = makeExpression(TernaryExpression(conditional, expression, walker.left, walker.right));
	}
	SEMA_POLICY(cpp::logical_or_expression_default, SemaPolicyPushSrc<struct SemaExpression>)
	void action(cpp::logical_or_expression_default* symbol, const SemaExpressionResult& walker)
	{
		walkBinaryIntegralExpression(symbol, walker);
	}
	SEMA_POLICY(cpp::logical_and_expression_default, SemaPolicyPushSrc<struct SemaExpression>)
	void action(cpp::logical_and_expression_default* symbol, const SemaExpressionResult& walker)
	{
		walkBinaryBooleanExpression(symbol, walker);
	}
	SEMA_POLICY(cpp::inclusive_or_expression_default, SemaPolicyPushSrc<struct SemaExpression>)
	void action(cpp::inclusive_or_expression_default* symbol, const SemaExpressionResult& walker)
	{
		walkBinaryIntegralExpression(symbol, walker);
	}
	SEMA_POLICY(cpp::exclusive_or_expression_default, SemaPolicyPushSrc<struct SemaExpression>)
	void action(cpp::exclusive_or_expression_default* symbol, const SemaExpressionResult& walker)
	{
		walkBinaryIntegralExpression(symbol, walker);
	}
	SEMA_POLICY(cpp::and_expression_default, SemaPolicyPushSrc<struct SemaExpression>)
	void action(cpp::and_expression_default* symbol, const SemaExpressionResult& walker)
	{
		walkBinaryIntegralExpression(symbol, walker);
	}
	SEMA_POLICY(cpp::equality_expression_default, SemaPolicyPushSrc<struct SemaExpression>)
	void action(cpp::equality_expression_default* symbol, const SemaExpressionResult& walker)
	{
		walkBinaryBooleanExpression(symbol, walker);
	}
	SEMA_POLICY(cpp::relational_expression_default, SemaPolicyPushSrc<struct SemaExpression>)
	void action(cpp::relational_expression_default* symbol, const SemaExpressionResult& walker)
	{
		walkBinaryBooleanExpression(symbol, walker);
	}
	SEMA_POLICY(cpp::shift_expression_default, SemaPolicyPushSrc<struct SemaExpression>)
	void action(cpp::shift_expression_default* symbol, const SemaExpressionResult& walker)
	{
		walkBinaryIntegralExpression(symbol, walker);
	}
	SEMA_POLICY(cpp::additive_expression_default, SemaPolicyPushSrc<struct SemaExpression>)
	void action(cpp::additive_expression_default* symbol, const SemaExpressionResult& walker)
	{
		walkBinaryAdditiveExpression(symbol, walker);
	}
	SEMA_POLICY(cpp::multiplicative_expression_default, SemaPolicyPushSrc<struct SemaExpression>)
	void action(cpp::multiplicative_expression_default* symbol, const SemaExpressionResult& walker)
	{
		walkBinaryArithmeticExpression(symbol, walker);
	}
	SEMA_POLICY(cpp::pm_expression_default, SemaPolicyPushSrc<struct SemaExpression>)
	void action(cpp::pm_expression_default* symbol, const SemaExpressionResult& walker)
	{
		walkBinaryExpression<binaryOperatorMemberPointer>(symbol, walker);
		id = 0; // not a parenthesised id-expression, expression is not 'call to named function' [over.call.func]
	}
	SEMA_POLICY(cpp::assignment_expression, SemaPolicyPushSrc<struct SemaExpression>)
	void action(cpp::assignment_expression* symbol, const SemaExpressionResult& walker) // expression_list, assignment_expression_suffix
	{
		expression = walker.expression;
		id = walker.id;
		setExpressionType(symbol, expression.type);
	}
	SEMA_POLICY(cpp::expression_list, SemaPolicyPushSrc<struct SemaExpressionList>)
	void action(cpp::expression_list* symbol, const SemaExpressionList& walker) // a comma-separated list of assignment_expression
	{
		id = 0;
		if(walker.arguments.size() == 1)
		{
			expression = walker.arguments.front();
		}
		else
		{
			expression = makeExpression(ExpressionList(walker.arguments));
		}
		setExpressionType(symbol, expression.type);
	}
	SEMA_POLICY(cpp::unary_operator, SemaPolicyIdentity)
	void action(cpp::unary_operator* symbol)
	{
		isAddressOf = symbol->id == cpp::unary_operator::AND;
	}
	SEMA_POLICY(cpp::postfix_expression, SemaPolicyPushSrcChecked<struct SemaPostfixExpression>)
	bool action(cpp::postfix_expression* symbol, const SemaPostfixExpression& walker)
	{
		if(walker.isUndeclared)
		{
			return reportIdentifierMismatch(symbol, *id, &gUndeclared, "object-name");
		}
		id = walker.id;
		expression = walker.expression;
		bool isPointerToMember = isAddressOf && expression.isQualifiedNonStaticMemberName;
		if(!isPointerToMember) // if the expression does not form a pointer to member
		{
			expression = makeTransformedIdExpression(expression);
		}
		setExpressionType(symbol, expression.type);
		return true;
	}
	SEMA_POLICY(cpp::unary_expression_op, SemaPolicyIdentity)
	void action(cpp::unary_expression_op* symbol)
	{
		id = 0; // not a parenthesised id-expression, expression is not 'call to named function' [over.call.func]

		UnaryIceOp iceOp = getUnaryIceOp(symbol);
		expression = makeExpression(UnaryExpression(getOverloadedOperatorId(symbol->op), iceOp, expression));

		setExpressionType(symbol, expression.type);
	}
	/* 14.6.2.2-3
	Expressions of the following forms are type-dependent only if the type specified by the type-id, simple-type-specifier
	or new-type-id is dependent, even if any subexpression is type-dependent:
	- postfix-expression-construct
	- new-expression
	- postfix-expression-cast
	- cast-expression
	*/
	/* temp.dep.constexpr
	Expressions of the following form are value-dependent if either the type-id or simple-type-specifier is dependent or the
	expression or cast-expression is value-dependent:
	simple-type-specifier ( expression-listopt )
	static_cast < type-id > ( expression )
	const_cast < type-id > ( expression )
	reinterpret_cast < type-id > ( expression )
	( type-id ) cast-expression
	*/
	SEMA_POLICY(cpp::new_expression_placement, SemaPolicyPushSrc<struct SemaNewExpression>)
	void action(cpp::new_expression_placement* symbol, const SemaNewExpression& walker)
	{
		ExpressionType type = ExpressionType(getUniqueType(walker.type), false); // non lvalue
		expression = makeExpression(NewExpression(type, walker.newArray, walker.arguments));
		setExpressionType(symbol, expression.type);
	}
	SEMA_POLICY(cpp::new_expression_default, SemaPolicyPushSrc<struct SemaNewExpression>)
	void action(cpp::new_expression_default* symbol, const SemaNewExpression& walker)
	{
		ExpressionType type = ExpressionType(getUniqueType(walker.type), false); // non lvalue
		expression = makeExpression(NewExpression(type, walker.newArray, walker.arguments));
		setExpressionType(symbol, expression.type);
	}
	SEMA_POLICY(cpp::cast_expression_default, SemaPolicyPushSrc<struct SemaExplicitTypeExpression>)
	void action(cpp::cast_expression_default* symbol, const SemaExplicitTypeExpressionResult& walker)
	{
		// [basic.lval] An expression which holds a temporary object resulting from a cast to a non-reference type is an rvalue
		ExpressionType type = ExpressionType(getUniqueType(walker.type), false); // non lvalue
		expression = makeExpression(CastExpression(type, walker.arguments));
		setExpressionType(symbol, expression.type);
	}
	/* 14.6.2.2-4
	Expressions of the following forms are never type-dependent (because the type of the expression cannot be
	dependent):
	literal
	postfix-expression . pseudo-destructor-name
	postfix-expression -> pseudo-destructor-name
	sizeof unary-expression
	sizeof ( type-id )
	sizeof ... ( identifier )
	alignof ( type-id )
	typeid ( expression )
	typeid ( type-id )
	::opt delete cast-expression
	::opt delete [ ] cast-expression
	throw assignment-expressionopt
	*/
	// TODO: destructor-call is not dependent
	/* temp.dep.constexpr
	Expressions of the following form are value-dependent if the unary-expression is type-dependent or the type-id is dependent
	(even if sizeof unary-expression and sizeof ( type-id ) are not type-dependent):
	sizeof unary-expression
	sizeof ( type-id )
	*/
	SEMA_POLICY(cpp::unary_expression_sizeof, SemaPolicyPushSrc<struct SemaExpression>)
	void action(cpp::unary_expression_sizeof* symbol, const SemaExpressionResult& walker)
	{
		expression = makeExpression(SizeofExpression(walker.expression));
		setExpressionType(symbol, expression.type);
	}
	SEMA_POLICY(cpp::unary_expression_sizeoftype, SemaPolicyPushSrc<struct SemaSizeofTypeExpression>)
	void action(cpp::unary_expression_sizeoftype* symbol, const SemaSizeofTypeExpression& walker)
	{
		expression = walker.expression;
		setExpressionType(symbol, expression.type);
	}
	SEMA_POLICY(cpp::delete_expression, SemaPolicyPushSrc<struct SemaExpression>)
	void action(cpp::delete_expression* symbol, const SemaExpressionResult& walker)
	{
		// [expr.delete] The result has type void.
		ExpressionType type = ExpressionType(gVoid, false); // non lvalue
		expression = makeExpression(ExplicitTypeExpression(type, walker.expression));
		setExpressionType(symbol, expression.type);
	}
	SEMA_POLICY(cpp::throw_expression, SemaPolicyPushSrc<struct SemaExpression>)
	void action(cpp::throw_expression* symbol, const SemaExpressionResult& walker)
	{
		// [except] A throw-expression is of type void.
		ExpressionType type = ExpressionType(gVoid, false); // non lvalue
		expression = makeExpression(ExplicitTypeExpression(type, walker.expression));
	}
};


struct SemaStaticAssertDeclaration : public SemaBase
{
	SEMA_BOILERPLATE;

	ExpressionWrapper expression; 

	SemaStaticAssertDeclaration(const SemaState& state)
		: SemaBase(state)
	{
	}

	SEMA_POLICY(cpp::constant_expression, SemaPolicyPush<struct SemaExpression>)
	void action(cpp::constant_expression* symbol, const SemaExpressionResult& walker)
	{
		expression = walker.expression;
	}
	SEMA_POLICY(cpp::string_literal, SemaPolicyIdentity)
	void action(cpp::string_literal* symbol)
	{
		const char* message = symbol->value.value.c_str();
		// [dcl.dcl] In a static_assert-declaration the constant-expression shall be a constant expression (5.19) that can be
		// contextually converted to bool
		if(expression.isValueDependent)
		{
			SEMANTIC_ASSERT(!string_equal(message, "\"?evaluated\"")); // assert if we were expecting a non-dependent expression
		}
		else
		{
			evaluateStaticAssert(expression, message, getInstantiationContext());
		}

		addDeferredExpression(expression, expression.isValueDependent ? message : 0);
	}
};


#endif
