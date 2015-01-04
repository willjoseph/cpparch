
#ifndef INCLUDED_CPPPARSE_SEMA_SEMAPOSTFIXEXPRESSION_H
#define INCLUDED_CPPPARSE_SEMA_SEMAPOSTFIXEXPRESSION_H

#include "SemaCommon.h"
#include "SemaPrimaryExpression.h"
#include "Core/TypeTraits.h"

struct SemaArgumentList : public SemaBase, SemaArgumentListResult
{
	SEMA_BOILERPLATE;

	SemaArgumentList(const SemaState& state)
		: SemaBase(state)
	{
		clearQualifying();
	}
	SEMA_POLICY(cpp::assignment_expression, SemaPolicyPush<struct SemaExpression>)
	void action(cpp::assignment_expression* symbol, const SemaExpressionResult& walker)
	{
		arguments.push_back(makeArgument(walker.expression, walker.expression.type));
		isDependent |= isDependentExpression(walker.expression);
	}
};

struct SemaSubscript : public SemaBase
{
	SEMA_BOILERPLATE;

	ExpressionWrapper expression;
	SemaSubscript(const SemaState& state)
		: SemaBase(state)
	{
	}
	void action(cpp::terminal<boost::wave::T_LEFTBRACKET>)
	{
		clearQualifying(); // the expression in [] is looked up in the context of the entire postfix expression
	}
	SEMA_POLICY(cpp::expression, SemaPolicyPush<struct SemaExpression>)
	void action(cpp::expression* symbol, const SemaExpressionResult& walker)
	{
		expression = walker.expression;
	}
};


struct SemaPostfixExpressionMember : public SemaQualified
{
	SEMA_BOILERPLATE;

	ExpressionWrapper expression;
	LookupResultRef declaration;
	IdentifierPtr id; // only valid when the expression is a (parenthesised) id-expression
	TemplateArguments arguments; // only used if the identifier is a template-name
	bool isTemplate;
	SemaPostfixExpressionMember(const SemaState& state)
		: SemaQualified(state), id(0), arguments(context), isTemplate(false)
	{
	}
	SEMA_POLICY(cpp::member_operator, SemaPolicyIdentity)
	void action(cpp::member_operator* symbol)
	{
		objectExpression = makeTransformedIdExpression(objectExpression);

		bool isArrow = symbol->id == cpp::member_operator::ARROW;

		memberClass = isObjectExpression(objectExpression)
			? &getSimpleType(removePointer(getObjectExpression(objectExpression).type).value)
			: &gDependentSimpleType;
		SEMANTIC_ASSERT(objectExpression.p != 0);
		objectExpression = makeExpression(MemberOperatorExpression(objectExpression, isArrow));
		if(!objectExpression.isTypeDependent) // if the type of the object expression is not dependent
		{
			memberClass = &getSimpleType(objectExpression.type.value);
			enclosingType = memberClass; // when evaluating the type of the following id-expression, use the object-expression class in the instantiation context
		}
	}
	void action(cpp::terminal<boost::wave::T_TEMPLATE> symbol)
	{
		isTemplate = true;
	}
	SEMA_POLICY_ARGS(cpp::id_expression, SemaPolicyPushBool<struct SemaIdExpression>, isTemplate)
	void action(cpp::id_expression* symbol, SemaIdExpression& walker)
	{
		bool isObjectName = walker.commit();
		SEMANTIC_ASSERT(isObjectName); // TODO: non-fatal error: expected object name
		id = walker.id;
		arguments = walker.arguments;
		declaration = walker.declaration;
		swapQualifying(walker.qualifying);
		expression = walker.expression;
	}
};

struct SemaTypeTraitsIntrinsic : public SemaBase
{
	SEMA_BOILERPLATE;

	UniqueTypeWrapper first;
	UniqueTypeWrapper second;
	SemaTypeTraitsIntrinsic(const SemaState& state)
		: SemaBase(state)
	{
	}
	void action(cpp::terminal<boost::wave::T_LEFTPAREN> symbol)
	{
		// debugging
	}
	SEMA_POLICY(cpp::type_id, SemaPolicyPushCommit<struct SemaTypeId>)
	void action(cpp::type_id* symbol, const SemaTypeIdResult& walker)
	{
		walker.committed.test();

		UniqueTypeWrapper type = getUniqueType(walker.type);
		setExpressionType(symbol, type);

		(first == gUniqueTypeNull ? first : second) = type;
	}
};

struct SemaOffsetof : public SemaQualified
{
	SEMA_BOILERPLATE;

	Type type;
	DeclarationPtr member;
	SemaOffsetof(const SemaState& state)
		: SemaQualified(state), type(0, context)
	{
	}
	void action(cpp::terminal<boost::wave::T_LEFTPAREN> symbol)
	{
		// debugging
	}
	SEMA_POLICY(cpp::type_id, SemaPolicyPushCommit<struct SemaTypeId>)
	void action(cpp::type_id* symbol, const SemaTypeIdResult& walker)
	{
		walker.committed.test();
		type = walker.type;
		swapQualifying(type);
	}
	SEMA_POLICY(cpp::unqualified_id, SemaPolicyPush<struct SemaUnqualifiedId>)
	void action(cpp::unqualified_id* symbol, SemaUnqualifiedId& walker)
	{
		member = walker.declaration;
	}
};

struct SemaIsInstantiated : public SemaBase
{
	SEMA_BOILERPLATE;

	IntegralConstant value;
	SemaIsInstantiated(const SemaState& state)
		: SemaBase(state)
	{
	}
	void action(cpp::terminal<boost::wave::T_LEFTPAREN> symbol)
	{
		// debugging
	}
	SEMA_POLICY(cpp::simple_type_specifier_name, SemaPolicyPush<struct SemaTypeSpecifier>)
	void action(cpp::simple_type_specifier_name* symbol, const SemaTypeSpecifierResult& walker)
	{
		if(walker.type.declaration == 0)
		{
			value.value = true;
			return; // fundamental type
		}
		Type type = walker.type;
		makeUniqueTypeSafe(type);
		SEMANTIC_ASSERT(!type.isDependent);
		value.value = isInstantiated(getUniqueType(type), getInstantiationContext());
	}
	SEMA_POLICY(cpp::id_expression, SemaPolicyPush<struct SemaIdExpression>)
	void action(cpp::id_expression* symbol, SemaIdExpression& walker)
	{
		SEMANTIC_ASSERT(!isDependentSafe(walker.qualifying.get_ref()));
		SEMANTIC_ASSERT(!isDependentSafe(walker.qualifying.get_ref()));

		const InstantiationContext& context = getInstantiationContext();

		UniqueTypeWrapper qualifyingType = makeUniqueQualifying(walker.qualifying, context);
		const SimpleType* qualifying = qualifyingType == gUniqueTypeNull ? 0 : &getSimpleType(qualifyingType.value);
		TemplateArgumentsInstance templateArguments;
		makeUniqueTemplateArguments(walker.arguments, templateArguments, context);

		SEMANTIC_ASSERT(!isOverloadedFunction(walker.declaration, qualifying, context)); // TODO: non-fatal error: only non-overloaded functions supported

		QualifiedDeclaration qualified = resolveQualifiedDeclaration(QualifiedDeclaration(qualifying, walker.declaration), context);
		const SimpleType* idEnclosing = getIdExpressionClass(qualified.enclosing, *qualified.declaration, context.enclosingType);
		const SimpleType& uniqueObject = makeUniqueObject(qualified.declaration, idEnclosing, templateArguments);
		value.value = uniqueObject.instantiated;
	}
};

struct SemaPostfixExpression : public SemaBase
{
	SEMA_BOILERPLATE;

	ExpressionWrapper expression;
	IdentifierPtr id; // only valid when the expression is a (parenthesised) id-expression
	TemplateArguments arguments; // only valid when the expression is a (qualified) template-id
	bool isUndeclared;
	SemaPostfixExpression(const SemaState& state)
		: SemaBase(state), id(0), arguments(context), isUndeclared(false)
	{
	}
	void updateMemberType()
	{
		memberClass = 0;
		if(expression.type.isFunction())
		{
			clearMemberType();
		}
		else
		{
			objectExpression = expression;
			SEMANTIC_ASSERT(objectExpression.isTypeDependent || !::isDependent(objectExpression.type));
		}
	}
	SEMA_POLICY(cpp::primary_expression, SemaPolicyPush<struct SemaPrimaryExpression>)
	void action(cpp::primary_expression* symbol, const SemaPrimaryExpression& walker)
	{
		expression = walker.expression;
		id = walker.id;
		arguments = walker.arguments;
		isUndeclared = walker.isUndeclared;
		setExpressionType(symbol, expression.type);
		updateMemberType();
	}
	SEMA_POLICY(cpp::postfix_expression_construct, SemaPolicyPushSrc<struct SemaExplicitTypeExpression>)
	void action(cpp::postfix_expression_construct* symbol, const SemaExplicitTypeExpressionResult& walker)
	{
		// [basic.lval] An expression which holds a temporary object resulting from a cast to a non-reference type is an rvalue
		ExpressionType type = ExpressionType(getUniqueType(walker.type), false); // non lvalue
		expression = makeExpression(CastExpression(type, walker.arguments));
		expression.isTemplateArgumentAmbiguity = symbol->args == 0;
		setExpressionType(symbol, expression.type);
		updateMemberType();
	}
	SEMA_POLICY(cpp::postfix_expression_cast, SemaPolicyPushSrc<struct SemaExplicitTypeExpression>)
	void action(cpp::postfix_expression_cast* symbol, const SemaExplicitTypeExpressionResult& walker)
	{
		// [basic.lval] An expression which holds a temporary object resulting from a cast to a non-reference type is an rvalue
		ExpressionType type = ExpressionType(getUniqueType(walker.type), false); // non lvalue
		expression = makeExpression(CastExpression(type, walker.arguments));
		setExpressionType(symbol, expression.type);
		updateMemberType();
	}
	SEMA_POLICY(cpp::postfix_expression_typeid, SemaPolicyPush<struct SemaExpression>)
	void action(cpp::postfix_expression_typeid* symbol, const SemaExpressionResult& walker)
	{
		// TODO: operand type required to be complete?
		ExpressionType type = getTypeInfoType();
		expression = makeExpression(ExplicitTypeExpression(type, walker.expression));
		updateMemberType();
	}
	SEMA_POLICY(cpp::postfix_expression_typeidtype, SemaPolicyPushCommit<struct SemaTypeId>)
	void action(cpp::postfix_expression_typeidtype* symbol, const SemaTypeIdResult& walker)
	{
		// TODO: operand type required to be complete?
		ExpressionType type = getTypeInfoType();
		expression = makeExpression(ExplicitTypeExpression(type));
		updateMemberType();
	}

	// suffix
	SEMA_POLICY(cpp::postfix_expression_subscript, SemaPolicyPushSrc<struct SemaSubscript>)
	void action(cpp::postfix_expression_subscript* symbol, const SemaSubscript& walker)
	{
		expression = makeTransformedIdExpression(expression);

		id = 0; // don't perform overload resolution for a[i](x);

		expression = makeExpression(SubscriptExpression(expression, walker.expression));
		setExpressionType(symbol, expression.type);
		updateMemberType();
	}
	SEMA_POLICY(cpp::postfix_expression_call, SemaPolicyPushSrc<struct SemaArgumentList>)
	void action(cpp::postfix_expression_call* symbol, const SemaArgumentList& walker)
	{
		// [class.mfct.nonstatic] An id-expression (that is not part of a class-member-access expression, and is found in the context of a nonstatic member)
		// that names a nonstatic member is transformed to a class-member-access expression prefixed by (*this)
		expression = makeTransformedIdExpression(expression);

		expression = makeExpression(FunctionCallExpression(expression, walker.arguments));

		if(expression.isTypeDependent) // if either the argument list or the id-expression are dependent
			// TODO: check value-dependent too?
		{
			if(id != 0)
			{
				id->dec.deferred = true;
			}
		}

		setExpressionType(symbol, expression.type);
		// TODO: set of pointers-to-function
		id = 0; // don't perform overload resolution for a(x)(x);

		updateMemberType();
		isUndeclared = false; // for an expression of the form 'undeclared-id(args)'
	}

	SEMA_POLICY(cpp::postfix_expression_member, SemaPolicyPushSrc<struct SemaPostfixExpressionMember>)
	void action(cpp::postfix_expression_member* symbol, const SemaPostfixExpressionMember& walker)
	{
		id = walker.id; // perform overload resolution for a.m(x);
		arguments = walker.arguments;
		LookupResultRef declaration = walker.declaration;

		SEMANTIC_ASSERT(walker.expression.p != 0);
		expression = makeExpression(
			ClassMemberAccessExpression(walker.objectExpression, walker.expression)
		);

		setExpressionType(symbol, expression.type);
		updateMemberType();

		if(expression.type.isFunction())
		{
			// type determination is deferred until overload resolution is complete
			memberClass = walker.memberClass; // store the type of the implied object argument in a qualified function call.
		}
	}
	SEMA_POLICY(cpp::postfix_expression_destructor, SemaPolicySrc)
	void action(cpp::postfix_expression_destructor* symbol)
	{
		id = 0;
		expression = ExpressionWrapper();
		setExpressionType(symbol, expression.type);
		// TODO: name-lookup for destructor name
		clearMemberType();
	}
	SEMA_POLICY(cpp::postfix_operator, SemaPolicySrc)
	void action(cpp::postfix_operator* symbol)
	{
		expression = makeTransformedIdExpression(expression);

		expression = makeExpression(PostfixOperatorExpression(getOverloadedOperatorId(symbol), expression));
		setExpressionType(symbol, expression.type);
		id = 0;
		updateMemberType();
	}
	SEMA_POLICY(cpp::postfix_expression_typetraits_unary, SemaPolicyPush<struct SemaTypeTraitsIntrinsic>)
	void action(cpp::postfix_expression_typetraits_unary* symbol, const SemaTypeTraitsIntrinsic& walker)
	{
		UnaryTypeTraitsOp operation = getUnaryTypeTraitsOp(symbol->trait);
		Name name = getTypeTraitName(symbol);
		expression = makeExpression(TypeTraitsUnaryExpression(name, operation, walker.first));
	}
	SEMA_POLICY(cpp::postfix_expression_typetraits_binary, SemaPolicyPush<struct SemaTypeTraitsIntrinsic>)
	void action(cpp::postfix_expression_typetraits_binary* symbol, const SemaTypeTraitsIntrinsic& walker)
	{
		BinaryTypeTraitsOp operation = getBinaryTypeTraitsOp(symbol->trait);
		Name name = getTypeTraitName(symbol);
		expression = makeExpression(TypeTraitsBinaryExpression(name, operation, walker.first, walker.second));
	}
	SEMA_POLICY(cpp::postfix_expression_offsetof, SemaPolicyPush<struct SemaOffsetof>)
	void action(cpp::postfix_expression_offsetof* symbol, const SemaOffsetof& walker)
	{
		expression = makeExpression(OffsetofExpression(getUniqueType(*walker.qualifying.get()), walker.member));
		setExpressionType(symbol, expression.type);
	}
	SEMA_POLICY(cpp::postfix_expression_isinstantiated, SemaPolicyPush<struct SemaIsInstantiated>)
	void action(cpp::postfix_expression_isinstantiated* symbol, const SemaIsInstantiated& walker)
	{
		expression = makeConstantExpression(IntegralConstantExpression(ExpressionType(gBool, false), walker.value));
	}
};

#endif
