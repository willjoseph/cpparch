
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
		addDependent(typeDependent, walker.typeDependent);
		addDependent(valueDependent, walker.valueDependent);
		isDependent |= walker.expression.isDependent;
	}
};

struct SemaSubscript : public SemaBase
{
	SEMA_BOILERPLATE;

	ExpressionWrapper expression;
	Dependent typeDependent;
	Dependent valueDependent;
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
		addDependent(typeDependent, walker.typeDependent);
		addDependent(valueDependent, walker.valueDependent);
	}
};


struct SemaPostfixExpressionMember : public SemaQualified
{
	SEMA_BOILERPLATE;

	ExpressionWrapper expression;
	LookupResultRef declaration;
	IdentifierPtr id; // only valid when the expression is a (parenthesised) id-expression
	TemplateArguments arguments; // only used if the identifier is a template-name
	Dependent typeDependent;
	Dependent valueDependent;
	bool isTemplate;
	SemaPostfixExpressionMember(const SemaState& state)
		: SemaQualified(state), id(0), arguments(context), isTemplate(false)
	{
	}
	SEMA_POLICY(cpp::member_operator, SemaPolicyIdentity)
	void action(cpp::member_operator* symbol)
	{
		objectExpression = makeTransformedIdExpression(objectExpression, typeDependent, valueDependent);

		bool isArrow = symbol->id == cpp::member_operator::ARROW;

		memberClass = &gDependentSimpleType;
		SEMANTIC_ASSERT(objectExpression.p != 0);
		objectExpression = makeExpression(MemberOperatorExpression(objectExpression, isArrow), objectExpression.isDependent, objectExpression.isTypeDependent, false);
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
		addDependent(typeDependent, walker.typeDependent);
		addDependent(valueDependent, walker.valueDependent);
		expression = walker.expression;
	}
};

struct SemaTypeTraitsIntrinsic : public SemaBase
{
	SEMA_BOILERPLATE;

	Dependent valueDependent;
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
		addDependent(valueDependent, walker.type);

		UniqueTypeWrapper type = getUniqueType(walker.type);
		setExpressionType(symbol, type);

		(first == gUniqueTypeNull ? first : second) = type;
	}
};

struct SemaOffsetof : public SemaBase
{
	SEMA_BOILERPLATE;

	Dependent valueDependent;
	UniqueTypeWrapper type;
	ExpressionWrapper member;
	SemaOffsetof(const SemaState& state)
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
		// [support.types] The expression offsetof(type, member-designator) is never type-dependent and it is value-dependent if and only if type is dependent.
		addDependent(valueDependent, walker.type);

		type = getUniqueType(walker.type);
		setExpressionType(symbol, type);
	}
	SEMA_POLICY(cpp::id_expression, SemaPolicyPush<struct SemaIdExpression>)
	void action(cpp::id_expression* symbol, SemaIdExpression& walker)
	{
		bool isObjectName = walker.commit();
		SEMANTIC_ASSERT(isObjectName); // TODO: non-fatal error: expected object name
		member = walker.expression;
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
		SEMANTIC_ASSERT(isDependentOld(walker.typeDependent) == isDependentOld(walker.qualifying.get_ref()));
		SEMANTIC_ASSERT(isDependentOld(walker.valueDependent) == isDependentOld(walker.qualifying.get_ref()));
		SEMANTIC_ASSERT(!isDependentOld(walker.typeDependent));
		SEMANTIC_ASSERT(!isDependentOld(walker.valueDependent));

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
	Dependent typeDependent;
	Dependent valueDependent;
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
		addDependent(typeDependent, walker.typeDependent);
		addDependent(valueDependent, walker.valueDependent);
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
		// [temp.dep.expr]
		// Expressions of the following forms are type-dependent only if the type specified by
		// the type-id, simple-type-specifier or new-type-id is dependent, even if any subexpression is type-dependent:
		//  simple-type-specifier ( expression-list )
		addDependent(typeDependent, walker.type);
		// [temp.dep.constexpr]
		// Expressions of the following form are value-dependent if either the type-id or simple-type-specifier is dependent
		// or the expression or cast-expression is value-dependent:
		//	simple-type-specifier ( expression-list )
		addDependent(valueDependent, walker.type.dependent);
		addDependent(valueDependent, walker.valueDependent);
		expression = makeExpression(CastExpression(type, walker.arguments),
			walker.isDependent | walker.type.isDependent,
			isDependentOld(typeDependent),
			isDependentOld(valueDependent));
		expression.isTemplateArgumentAmbiguity = symbol->args == 0;
		setExpressionType(symbol, expression.type);
		updateMemberType();
	}
	SEMA_POLICY(cpp::postfix_expression_cast, SemaPolicyPushSrc<struct SemaExplicitTypeExpression>)
	void action(cpp::postfix_expression_cast* symbol, const SemaExplicitTypeExpressionResult& walker)
	{
		// [basic.lval] An expression which holds a temporary object resulting from a cast to a non-reference type is an rvalue
		ExpressionType type = ExpressionType(getUniqueType(walker.type), false); // non lvalue
		// [temp.dep.expr]
		// Expressions of the following forms are type-dependent only if the type specified by
		// the type-id, simple-type-specifier or new-type-id is dependent, even if any subexpression is type-dependent:
		//  dynamic_cast < type-id > ( expression )
		//  static_cast <type-id> (expression)
		//  const_cast <type-id> (expression)
		//  reinterpret_cast <type-id> (expression)
		addDependent(typeDependent, walker.type);
		// [temp.dep.constexpr]
		// Expressions of the following form are value-dependent if either the type-id or simple-type-specifier is dependent
		// or the expression or cast-expression is value-dependent:
		//  static_cast <type-id> (expression)
		//  const_cast <type-id> (expression)
		//  reinterpret_cast <type-id> (expression)
#if 0
		if(symbol->op->id != cpp::cast_operator::DYNAMIC)
#endif
		{
			addDependent(valueDependent, walker.type.dependent);
		}
		addDependent(valueDependent, walker.valueDependent);
		expression = makeExpression(CastExpression(type, walker.arguments),
			walker.isDependent | walker.type.isDependent,
			isDependentOld(typeDependent),
			isDependentOld(valueDependent));
		setExpressionType(symbol, expression.type);
		updateMemberType();
	}
	SEMA_POLICY(cpp::postfix_expression_typeid, SemaPolicyPush<struct SemaExpression>)
	void action(cpp::postfix_expression_typeid* symbol, const SemaExpressionResult& walker)
	{
		// TODO: operand type required to be complete?
		ExpressionType type = getTypeInfoType();
		expression = makeExpression(ExplicitTypeExpression(type, walker.expression), walker.expression.isDependent);
		updateMemberType();
	}
	SEMA_POLICY(cpp::postfix_expression_typeidtype, SemaPolicyPushCommit<struct SemaTypeId>)
	void action(cpp::postfix_expression_typeidtype* symbol, const SemaTypeIdResult& walker)
	{
		// TODO: operand type required to be complete?
		ExpressionType type = getTypeInfoType();
		expression = makeExpression(ExplicitTypeExpression(type), walker.type.isDependent);
		updateMemberType();
	}

	// suffix
	SEMA_POLICY(cpp::postfix_expression_subscript, SemaPolicyPushSrc<struct SemaSubscript>)
	void action(cpp::postfix_expression_subscript* symbol, const SemaSubscript& walker)
	{
		expression = makeTransformedIdExpression(expression, typeDependent, valueDependent);

		addDependent(typeDependent, walker.typeDependent);
		addDependent(valueDependent, walker.valueDependent);
		id = 0; // don't perform overload resolution for a[i](x);

		expression = makeExpression(SubscriptExpression(expression, walker.expression),
			expression.isDependent | walker.expression.isDependent,
			isDependentOld(typeDependent),
			isDependentOld(valueDependent));
		setExpressionType(symbol, expression.type);
		updateMemberType();
	}
	SEMA_POLICY(cpp::postfix_expression_call, SemaPolicyPushSrc<struct SemaArgumentList>)
	void action(cpp::postfix_expression_call* symbol, const SemaArgumentList& walker)
	{
		// [class.mfct.nonstatic] An id-expression (that is not part of a class-member-access expression, and is found in the context of a nonstatic member)
		// that names a nonstatic member is transformed to a class-member-access expression prefixed by (*this)
		expression = makeTransformedIdExpression(expression, typeDependent, valueDependent);

		addDependent(typeDependent, walker.typeDependent);
		valueDependent = Dependent();

		if(!isDependentOld(typeDependent)
			&& expression.isNonStaticMemberName // if the id-expression names a nonstatic member
			&& memberClass == 0) // and this is not a class-member-access expression
		{
			SEMANTIC_ASSERT(enclosingType != 0); // TODO: check that the id-expression is found in the context of a non-static member (i.e. 'this' is valid)
			// [class.mfct.nonstatic] An id-expression (that is not part of a class-member-access expression, and is found in the context of a nonstatic member)
			// that names a nonstatic member is transformed to a class-member-access expression prefixed by (*this)

			addDependent(typeDependent, enclosingDependent);
			// when a nonstatic member name is used in a function call, overload resolution is dependent on the type of the implicit object parameter
		}


		if(isDependentOld(typeDependent)) // if either the argument list or the id-expression are dependent
			// TODO: check valueDependent too?
		{
			if(id != 0)
			{
				id->dec.deferred = true;
			}
		}

		expression = makeExpression(FunctionCallExpression(expression, walker.arguments),
			expression.isDependent | walker.isDependent,
			isDependentOld(typeDependent),
			false);
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
		addDependent(typeDependent, walker.typeDependent);
		valueDependent = Dependent();

		SEMANTIC_ASSERT(walker.expression.p != 0);
		expression = makeExpression(
			ClassMemberAccessExpression(walker.objectExpression, walker.expression),
			walker.objectExpression.isDependent | walker.expression.isDependent,
			isDependentOld(typeDependent),
			false
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
		expression = makeTransformedIdExpression(expression, typeDependent, valueDependent);

		expression = makeExpression(PostfixOperatorExpression(getOverloadedOperatorId(symbol), expression), expression.isDependent, isDependentOld(typeDependent), isDependentOld(valueDependent));
		setExpressionType(symbol, expression.type);
		id = 0;
		updateMemberType();
	}
	SEMA_POLICY(cpp::postfix_expression_typetraits_unary, SemaPolicyPush<struct SemaTypeTraitsIntrinsic>)
	void action(cpp::postfix_expression_typetraits_unary* symbol, const SemaTypeTraitsIntrinsic& walker)
	{
		addDependent(valueDependent, walker.valueDependent);
		bool isValueDependent = isDependentOld(valueDependent);
		UnaryTypeTraitsOp operation = getUnaryTypeTraitsOp(symbol->trait);
		Name name = getTypeTraitName(symbol);
		expression = makeExpression(TypeTraitsUnaryExpression(name, operation, walker.first), isValueDependent, false, isValueDependent);
	}
	SEMA_POLICY(cpp::postfix_expression_typetraits_binary, SemaPolicyPush<struct SemaTypeTraitsIntrinsic>)
	void action(cpp::postfix_expression_typetraits_binary* symbol, const SemaTypeTraitsIntrinsic& walker)
	{
		addDependent(valueDependent, walker.valueDependent);
		bool isValueDependent = isDependentOld(valueDependent);
		BinaryTypeTraitsOp operation = getBinaryTypeTraitsOp(symbol->trait);
		Name name = getTypeTraitName(symbol);
		expression = makeExpression(TypeTraitsBinaryExpression(name, operation, walker.first, walker.second), isValueDependent, false, isValueDependent);
	}
	SEMA_POLICY(cpp::postfix_expression_offsetof, SemaPolicyPush<struct SemaOffsetof>)
	void action(cpp::postfix_expression_offsetof* symbol, const SemaOffsetof& walker)
	{
		addDependent(valueDependent, walker.valueDependent);
		bool isValueDependent = isDependentOld(valueDependent);
		expression = makeExpression(OffsetofExpression(walker.type, walker.member), isValueDependent, false, isValueDependent);
	}
	SEMA_POLICY(cpp::postfix_expression_isinstantiated, SemaPolicyPush<struct SemaIsInstantiated>)
	void action(cpp::postfix_expression_isinstantiated* symbol, const SemaIsInstantiated& walker)
	{
		expression = makeConstantExpression(IntegralConstantExpression(ExpressionType(gBool, false), walker.value));
	}
};

#endif
