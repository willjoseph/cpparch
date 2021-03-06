
#ifndef INCLUDED_CPPPARSE_CORE_EXPRESSIONEVALUATE_H
#define INCLUDED_CPPPARSE_CORE_EXPRESSIONEVALUATE_H

#include "Parse/Grammar.h"
#include "Ast/Type.h"
#include "Ast/Scope.h"
#include "Ast/ExpressionImpl.h"
#include "NameLookup.h"
#include "TypeInstantiate.h"
#include "Special.h"
#include "OperatorId.h"
#include "KoenigLookup.h"
#include "TypeSubstitute.h"
#include "OverloadResolve.h"
#include "Common/Util.h"

inline ExpressionWrapper makeConstantExpressionZero()
{
	return makeConstantExpression(IntegralConstantExpression(ExpressionType(gSignedInt, false), IntegralConstant(0)));
}

inline ExpressionWrapper makeConstantExpressionOne()
{
	return makeConstantExpression(IntegralConstantExpression(ExpressionType(gSignedInt, false), IntegralConstant(1)));
}

inline IntegralConstant identity(IntegralConstant left)
{
	return left;
}

inline IntegralConstant assign(IntegralConstant left, IntegralConstant right)
{
	return left = right;
}

// [expr.unary.op]
inline IntegralConstant operator++(IntegralConstant left)
{
	return IntegralConstant(++left.value);
}
inline IntegralConstant operator--(IntegralConstant left)
{
	return IntegralConstant(--left.value);
}
inline IntegralConstant operator+(IntegralConstant left)
{
	return IntegralConstant(+left.value);
}
inline IntegralConstant operator-(IntegralConstant left)
{
	return IntegralConstant(-left.value);
}
inline IntegralConstant operator!(IntegralConstant left)
{
	return IntegralConstant(!left.value);
}
inline IntegralConstant operator~(IntegralConstant left)
{
	return IntegralConstant(~left.value);
}
inline IntegralConstant addressOf(IntegralConstant left)
{
	return IntegralConstant(0); // TODO
}
inline IntegralConstant dereference(IntegralConstant left)
{
	return IntegralConstant(0); // TODO
}

inline UnaryIceOp getUnaryIceOp(cpp::unary_expression_op* symbol)
{
	switch(symbol->op->id)
	{
	case cpp::unary_operator::PLUSPLUS: return operator++;
	case cpp::unary_operator::MINUSMINUS: return operator--;
	case cpp::unary_operator::STAR: return dereference;
	case cpp::unary_operator::AND: return addressOf;
	case cpp::unary_operator::PLUS: return operator+;
	case cpp::unary_operator::MINUS: return operator-;
	case cpp::unary_operator::NOT: return operator!;
	case cpp::unary_operator::COMPL: return operator~;
	default: break;
	}
	throw SymbolsError();
}

// [expr.mptr.oper]
inline BinaryIceOp getBinaryIceOp(cpp::pm_expression_default* symbol)
{
	return 0; // N/A
}


// [expr.mul]
inline IntegralConstant operator*(IntegralConstant left, IntegralConstant right)
{
	return IntegralConstant(left.value * right.value);
}
inline IntegralConstant operator/(IntegralConstant left, IntegralConstant right)
{
	return IntegralConstant(left.value / right.value);
}
inline IntegralConstant operator%(IntegralConstant left, IntegralConstant right)
{
	return IntegralConstant(left.value % right.value);
}

inline BinaryIceOp getBinaryIceOp(cpp::multiplicative_expression_default* symbol)
{
	switch(symbol->op->id)
	{
	case cpp::multiplicative_operator::STAR: return operator*;
	case cpp::multiplicative_operator::DIVIDE: return operator/;
	case cpp::multiplicative_operator::PERCENT: return operator%;
	default: break;
	}
	throw SymbolsError();
}

// [expr.add]
inline IntegralConstant operator+(IntegralConstant left, IntegralConstant right)
{
	return IntegralConstant(left.value + right.value);
}
inline IntegralConstant operator-(IntegralConstant left, IntegralConstant right)
{
	return IntegralConstant(left.value - right.value);
}

inline BinaryIceOp getBinaryIceOp(cpp::additive_expression_default* symbol)
{
	switch(symbol->op->id)
	{
	case cpp::additive_operator::PLUS: return operator+;
	case cpp::additive_operator::MINUS: return operator-;
	default: break;
	}
	throw SymbolsError();
}

// [expr.shift]
inline IntegralConstant operator<<(IntegralConstant left, IntegralConstant right)
{
	return IntegralConstant(left.value << right.value);
}
inline IntegralConstant operator>>(IntegralConstant left, IntegralConstant right)
{
	return IntegralConstant(left.value >> right.value);
}

inline BinaryIceOp getBinaryIceOp(cpp::shift_expression_default* symbol)
{
	switch(symbol->op->id)
	{
	case cpp::shift_operator::SHIFTLEFT: return operator<<;
	case cpp::shift_operator::SHIFTRIGHT: return operator>>;
	default: break;
	}
	throw SymbolsError();
}

// [expr.rel]
inline IntegralConstant operator<(IntegralConstant left, IntegralConstant right)
{
	return IntegralConstant(left.value < right.value);
}
inline IntegralConstant operator>(IntegralConstant left, IntegralConstant right)
{
	return IntegralConstant(left.value > right.value);
}
inline IntegralConstant operator<=(IntegralConstant left, IntegralConstant right)
{
	return IntegralConstant(left.value <= right.value);
}
inline IntegralConstant operator>=(IntegralConstant left, IntegralConstant right)
{
	return IntegralConstant(left.value >= right.value);
}

inline BinaryIceOp getBinaryIceOp(cpp::relational_expression_default* symbol)
{
	switch(symbol->op->id)
	{
	case cpp::relational_operator::LESS: return operator<;
	case cpp::relational_operator::GREATER: return operator>;
	case cpp::relational_operator::LESSEQUAL: return operator<=;
	case cpp::relational_operator::GREATEREQUAL: return operator>=;
	default: break;
	}
	throw SymbolsError();
}

// [expr.eq]
inline IntegralConstant operator==(IntegralConstant left, IntegralConstant right)
{
	return IntegralConstant(left.value == right.value);
}
inline IntegralConstant operator!=(IntegralConstant left, IntegralConstant right)
{
	return IntegralConstant(left.value != right.value);
}

inline BinaryIceOp getBinaryIceOp(cpp::equality_expression_default* symbol)
{
	switch(symbol->op->id)
	{
	case cpp::equality_operator::EQUAL: return operator==;
	case cpp::equality_operator::NOTEQUAL: return operator!=;
	default: break;
	}
	throw SymbolsError();
}

// [expr.bit.and]
inline IntegralConstant operator&(IntegralConstant left, IntegralConstant right)
{
	return IntegralConstant(left.value & right.value);
}
inline BinaryIceOp getBinaryIceOp(cpp::and_expression_default* symbol)
{
	return operator&;
}

// [expr.xor]
inline IntegralConstant operator^(IntegralConstant left, IntegralConstant right)
{
	return IntegralConstant(left.value ^ right.value);
}
inline BinaryIceOp getBinaryIceOp(cpp::exclusive_or_expression_default* symbol)
{
	return operator^;
}

// [expr.or]
inline IntegralConstant operator|(IntegralConstant left, IntegralConstant right)
{
	return IntegralConstant(left.value | right.value);
}
inline BinaryIceOp getBinaryIceOp(cpp::inclusive_or_expression_default* symbol)
{
	return operator|;
}

// [expr.log.and]
inline IntegralConstant operator&&(IntegralConstant left, IntegralConstant right)
{
	return IntegralConstant(left.value && right.value);
}

inline BinaryIceOp getBinaryIceOp(cpp::logical_and_expression_default* symbol)
{
	return operator&&;
}

// [expr.log.or]
inline IntegralConstant operator||(IntegralConstant left, IntegralConstant right)
{
	return IntegralConstant(left.value || right.value);
}

inline BinaryIceOp getBinaryIceOp(cpp::logical_or_expression_default* symbol)
{
	return operator||;
}

// [expr.ass]
inline BinaryIceOp getBinaryIceOp(cpp::assignment_expression_suffix* symbol)
{
	switch(symbol->op->id)
	{
	case cpp::assignment_operator::ASSIGN: return assign;
	case cpp::assignment_operator::STAR: return operator*;
	case cpp::assignment_operator::DIVIDE: return operator/;
	case cpp::assignment_operator::PERCENT: return operator%;
	case cpp::assignment_operator::PLUS: return operator+;
	case cpp::assignment_operator::MINUS: return operator-;
	case cpp::assignment_operator::SHIFTRIGHT: return operator>>;
	case cpp::assignment_operator::SHIFTLEFT: return operator<<;
	case cpp::assignment_operator::AND: return operator&;
	case cpp::assignment_operator::XOR: return operator^;
	case cpp::assignment_operator::OR: return operator|;
	default: break;
	}
	throw SymbolsError();
}

// [expr.cond]
inline IntegralConstant conditional(IntegralConstant first, IntegralConstant second, IntegralConstant third)
{
	return IntegralConstant(first.value ? second.value : third.value);
}




// ----------------------------------------------------------------------------
// expression evaluation

IntegralConstant evaluate(ExpressionNode* node, const InstantiationContext& context);

inline IntegralConstant evaluateExpression(ExpressionNode* node, const InstantiationContext& context)
{
	if(isDependentPointerToMemberExpression(node))
	{
		// TODO: check this names a valid non-static member
		return IntegralConstant(0); // TODO: unique value for address of member
	}
	return evaluate(node, context);
}

inline IntegralConstant evaluateExpression(const ExpressionWrapper& expression, const InstantiationContext& context)
{
	if(isPointerToMemberExpression(expression))
	{
		return IntegralConstant(0); // TODO: unique value for address of member
	}
	if(isPointerToFunctionExpression(expression))
	{
		return IntegralConstant(0); // TODO: unique value for address of function
	}
	SYMBOLS_ASSERT(expression.isConstant);
	return evaluateExpression(expression.p, context);
}

inline ExpressionType typeOfExpression(ExpressionNode* node, const InstantiationContext& context);
inline bool isOverloaded(const DeclarationInstance& declaration);

inline bool isOverloadedFunction(const DeclarationInstance& declaration)
{
	return UniqueTypeWrapper(declaration->type.unique).isFunction()
		&& isOverloaded(declaration);
}

inline bool isOverloadedFunctionIdExpression(ExpressionNode* node)
{
	if(!isIdExpression(node))
	{
		return false;
	}

	const IdExpression& idExpression = getIdExpression(node);
	if(isOverloadedFunction(idExpression.declaration)) // if this id-expression names an overloaded function
	{
		return true;
	}
	return false;
}

inline bool isMemberIdExpression(ExpressionNode* node)
{
	if(!isIdExpression(node))
	{
		return false;
	}
	const IdExpression& idExpression = getIdExpression(node);
	return isMember(*idExpression.declaration); // this id-expression names a member
}

inline bool isLvalue(const Declaration& declaration)
{
	return isObject(declaration) // functions, variables and data members are lvalues
		&& declaration.templateParameter == INDEX_INVALID // template parameters are not lvalues
		&& !isEnum(UniqueTypeWrapper(declaration.type.unique)); // enumerators are not lvalues
}

inline ExpressionType typeOfExpressionSafe(ExpressionNode* node, const InstantiationContext& context)
{
	if(isMemberIdExpression(node)) // if attempting to evaluate type of non-static member with no context
	{
		// can't evaluate id-expression within member-access-expression
		return gNullExpressionType; // do not evaluate the type!
	}
	return typeOfExpression(node, context);
}

inline ExpressionType typeOfExpressionWrapper(const ExpressionWrapper& expression, const InstantiationContext& context)
{
#if 0
	return typeOfExpression(expression.p, context);
#else
	ExpressionType type = typeOfExpression(expression.p, context);
	if(expression.isTypeDependent)
	{
		return type;
	}
	if(isMemberIdExpression(expression.p))
	{
		return type;
	}
#if 0 // TODO: test what this change does
	if(expression.type == gOverloadedExpressionType) // if evaluation of type of overloaded function was deferred 
	{
		return type; // evaluate it now
	}
#endif
	SYMBOLS_ASSERT(type == expression.type);

	// TODO: move into typeOfExpression?
	// [basic.lval] Class prvalues can have cv-qualified types; non-class prvalues always have cv-unqualified types
	if(!type.isLvalue // if this is a prvalue
		&& !isClass(type)) // and is not a class
	{
		type.value.setQualifiers(CvQualifiers()); // remove cv-qualifiers
	}
	return type;
#endif
}

inline bool isSpecialMember(const Declaration& declaration)
{
	return &declaration == gDestructorInstance.p
		|| &declaration == gCopyAssignmentOperatorInstance.p;
}


inline const SimpleType* getIdExpressionClass(const SimpleType* qualifying, const DeclarationInstance& declaration, const SimpleType* enclosingType)
{
	if(!isMember(*declaration)) // if the declaration is not a class member
	{
		return 0; // the declaration is at namespace-scope, therefore has no enclosing class
	}

	const SimpleType* idEnclosing = qualifying != 0 ? qualifying : enclosingType;

	SYMBOLS_ASSERT(idEnclosing != 0);

	if(isSpecialMember(*declaration)) // temporary hack to handle explicit call of operator=()
	{
		return idEnclosing; // assume this is a member of the qualifying class
	}

	// the identifier may name a member in a base-class of the qualifying type; findEnclosingType resolves this.
	idEnclosing = findEnclosingType(idEnclosing, declaration->scope); // it must be a member of (a base of) the qualifying class: find which one.
	SYMBOLS_ASSERT(idEnclosing != 0);

	return idEnclosing;
}



typedef std::vector<Overload> OverloadSet;

inline void addUniqueOverload(OverloadSet& result, const Overload& overload)
{
	if(std::find(result.begin(), result.end(), overload) == result.end())
	{
		result.push_back(overload);
	}
}


inline bool isOverloaded(const DeclarationInstance& declaration)
{
	SYMBOLS_ASSERT(UniqueTypeWrapper(declaration->type.unique).isFunction());
	bool found = false;
	for(Declaration* p = findOverloaded(declaration); p != 0; p = p->overloaded)
	{
		if(p->specifiers.isFriend)
		{
			continue; // ignore (namespace-scope) friend functions
		}
		if(p->isTemplate)
		{
			// [over.over]
			// A use of an overloaded function name without arguments is resolved in certain contexts to a function, a
			// pointer to function or a pointer to member function for a specific function from the overload set. A function
			// template name is considered to name a set of overloaded functions in such contexts.
			return true;
		}
		if(found) // if a previous iteration found a non-friend function
		{
			return true;
		}
		found = true; // this iteration found a non-friend function
	}
	return false;
}

inline void addOverloaded(OverloadSet& result, const DeclarationInstance& declaration, const SimpleType* memberEnclosing)
{
	SYMBOLS_ASSERT(UniqueTypeWrapper(declaration->type.unique).isFunction());
	for(Declaration* p = findOverloaded(declaration); p != 0; p = p->overloaded)
	{
		if(p->specifiers.isFriend)
		{
			SYMBOLS_ASSERT(memberEnclosing == 0);
			continue; // ignore (namespace-scope) friend functions
		}
		addUniqueOverload(result, Overload(p, memberEnclosing));
	}
}

inline void addOverloaded(OverloadSet& result, const DeclarationInstance& declaration, const KoenigAssociated& associated = KoenigAssociated())
{
	SYMBOLS_ASSERT(UniqueTypeWrapper(declaration->type.unique).isFunction());
	for(Declaration* p = findOverloaded(declaration); p != 0; p = p->overloaded)
	{
		const SimpleType* memberEnclosing = 0;
		if(p->specifiers.isFriend)
		{
			Scope* enclosingClass = getEnclosingClass(p->enclosed);
			memberEnclosing = findKoenigAssociatedClass(associated, *getDeclaration(enclosingClass->name));
			if(memberEnclosing == 0)
			{
				continue; // friend should only be visible if member of an associated class
			}
		}
		addUniqueOverload(result, Overload(p, memberEnclosing));
	}
}

inline void addArgumentDependentOverloads(OverloadSet& result, const Identifier& id, const Arguments& arguments)
{
	KoenigAssociated associated;
	// [basic.lookup.koenig]
	// For each argument type T in the function call, there is a set of zero or more associated namespaces and a set
	// of zero or more associated classes to be considered. The sets of namespaces and classes is determined
	// entirely by the types of the function arguments (and the namespace of any template template argument).
	// Typedef names and using-declarations used to specify the types do not contribute to this set.
	for(Arguments::const_iterator i = arguments.begin(); i != arguments.end(); ++i)
	{
		UniqueTypeWrapper type = (*i).type;
		addKoenigAssociated(associated, type);
	}
	// TODO:
	// In addition, if the argument is the name or address of a set of overloaded functions and/or function templates,
	// its associated classes and namespaces are the union of those associated with each of the members of
	// the set: the namespace in which the function or function template is defined and the classes and namespaces
	// associated with its (non-dependent) parameter types and return type.
	for(KoenigAssociated::Namespaces::const_iterator i = associated.namespaces.begin(); i != associated.namespaces.end(); ++i)
	{
		// TODO: Any namespace-scope friend functions declared in associated classes are visible within their respective
		// namespaces even if they are not visible during an ordinary lookup.
		// All names except those of (possibly overloaded) functions and function templates are ignored.
		if(const DeclarationInstance* p = findDeclaration((*i)->declarations, id, IsAdlFunctionName()))
		{
			const DeclarationInstance& declaration = *p;
			addOverloaded(result, *p, associated);
		}
	}
}

FunctionOverload findBestOverloadedOperator(const Identifier& id, const Arguments& arguments, const InstantiationContext& context);
FunctionOverload findBestOverloadedFunction(const OverloadSet& overloads, const TemplateArgumentsInstance* templateArguments, const Arguments& arguments, const InstantiationContext& context);

//-----------------------------------------------------------------------------

inline ExpressionType getBuiltInUnaryOperatorReturnType(Name operatorName, ExpressionType type)
{
	if(operatorName == gOperatorAndId) // address-of
	{
		UniqueTypeWrapper result = type;
		result.push_front(PointerType()); // produces a non-const pointer
		return ExpressionType(result, false); // non lvalue
	}
	else if(operatorName == gOperatorStarId) // dereference
	{
		UniqueTypeId result = applyLvalueToRvalueConversion(type);
		SYMBOLS_ASSERT(!result.empty());
		// [expr.unary] The unary * operator performs indirection: the expression to which it is applied shall be a pointer to an
		// object type, or a pointer to a function type and the result is an lvalue referring to the object or function to
		// which the expression points. If the type of the expression is "pointer to T," the type of the result is "T."
		SYMBOLS_ASSERT(result.isPointer());
		result.pop_front();
		return ExpressionType(result, true);
	}
	else if(operatorName == gOperatorPlusId
		|| operatorName == gOperatorMinusId)
	{
		if(!isFloating(type))
		{
			// TODO: check type is integral or enumeration
			return ExpressionType(promoteToIntegralType(type), false); // non lvalue
		}
		return ExpressionType(type, false); // non lvalue
	}
	else if(operatorName == gOperatorNotId)
	{
		return ExpressionType(gBool, false); // non lvalue
	}
	else if(operatorName == gOperatorComplId)
	{
		// TODO: check type is integral or enumeration
		return ExpressionType(promoteToIntegralType(type), false); // non lvalue
	}
	SYMBOLS_ASSERT(operatorName == gOperatorPlusPlusId || operatorName == gOperatorMinusMinusId);
	return ExpressionType(type, true); // lvalue
}

inline ExpressionType typeOfUnaryExpression(Name operatorName, Argument operand, const InstantiationContext& context)
{
	Identifier id;
	id.value = operatorName;
	id.source = context.source;

	Arguments arguments(1, operand);
	FunctionOverload overload = findBestOverloadedOperator(id, arguments, context);
	if(overload.declaration == &gUnknown
		|| (overload.declaration == 0
		&& operatorName == gOperatorAndId)) // TODO: unary operator& has no built-in candidates  
	{
		if(operatorName == gOperatorAndId
			&& operand.isQualifiedNonStaticMemberName)
		{
			// [expr.unary.op]
			// The result of the unary & operator is a pointer to its operand. The operand shall be an lvalue or a qualified-id.
			// In the first case, if the type of the expression is "T," the type of the result is "pointer to T." In particular,
			// the address of an object of type "cv T" is "pointer to cv T," with the same cv-qualifiers.
			// For a qualified-id, if the member is a static member of type "T", the type of the result is plain "pointer to T."
			// If the member is a non-static member of class C of type T, the type of the result is "pointer to member of class C of type
			// T."
			UniqueTypeWrapper classType = makeUniqueSimpleType(*getIdExpression(operand).enclosing);
			UniqueTypeWrapper type = operand.type;
			type.push_front(MemberPointerType(classType)); // produces a non-const pointer
			return ExpressionType(type, false); // non lvalue
		}
		else
		{
			return getBuiltInUnaryOperatorReturnType(operatorName, operand.type);
		}
	}
	else
	{
		SYMBOLS_ASSERT(overload.declaration != 0);
		SYMBOLS_ASSERT(overload.type != gUniqueTypeNull);
		return overload.type;
	}
}

inline ExpressionType typeOfPostfixOperatorExpression(Name operatorName, Argument operand, const InstantiationContext& context)
{
	Identifier id;
	id.value = operatorName;
	id.source = context.source;

	ExpressionWrapper zero = makeConstantExpressionZero();

	Arguments arguments;
	arguments.push_back(operand);
	arguments.push_back(makeArgument(zero, zero.type));
	FunctionOverload overload = findBestOverloadedOperator(id, arguments, context);
	if(overload.declaration == &gUnknown)
	{
		// [expr.post.incr] The operand shall be a modifiable lvalue.
		// The type of the operand shall be an arithmetic type or a pointer to a complete object type.
		// The result is an rvalue. The type of the result is the cv-unqualified version of the type of the operand.
		UniqueTypeWrapper type = operand.type;
		// TODO: assert lvalue
		type.value.setQualifiers(CvQualifiers());
		requireCompleteObjectType(type, context);
		return ExpressionType(operand.type, false); // non lvalue
	}
	else
	{
		SYMBOLS_ASSERT(overload.declaration != 0);
		SYMBOLS_ASSERT(overload.type != gUniqueTypeNull);
		return overload.type;
	}
}

typedef ExpressionType (*BuiltInBinaryTypeOp)(Name operatorName, ExpressionType, ExpressionType);

template<BuiltInBinaryTypeOp typeOp>
inline ExpressionType typeOfBinaryExpression(Name operatorName, Argument left, Argument right, const InstantiationContext& context)
{
	SYMBOLS_ASSERT(left.type != gUniqueTypeNull);
	SYMBOLS_ASSERT(right.type != gUniqueTypeNull);
	Identifier id;
	id.value = operatorName;
	id.source = context.source;
	FunctionOverload overload(&gUnknown, gNullExpressionType);
	if(!id.value.empty()) // if the operator can be overloaded
	{
		Arguments arguments;
		arguments.push_back(left);
		arguments.push_back(right);
		overload = findBestOverloadedOperator(id, arguments, context);
	}
	if(overload.declaration == &gUnknown
		|| (overload.declaration == 0 && id.value == gOperatorAssignId)) // TODO: declare implicit assignment operator
	{
		return  typeOp(operatorName, left.type, right.type);
	}

	SYMBOLS_ASSERT(overload.declaration != 0);
	return overload.type;
}

inline ExpressionType binaryOperatorAssignment(Name operatorName, ExpressionType left, ExpressionType right)
{
	return left;
}

inline ExpressionType binaryOperatorComma(Name operatorName, ExpressionType left, ExpressionType right)
{
	return right;
}

inline ExpressionType binaryOperatorBoolean(Name operatorName, ExpressionType left, ExpressionType right)
{
	return ExpressionType(gBool, false); // non lvalue
}

inline ExpressionType binaryOperatorMemberPointer(Name operatorName, ExpressionType left, ExpressionType right)
{
	// [expr.mptr.oper] C++11
	// The expression E1->*E2 is converted into the equivalent form (*(E1)).*E2.
	// [expr.mptr.oper] C++11
	// The result of a .* expression whose second operand is a pointer to a data member is of the same value
	// category (3.10) as its first operand. The result of a .* expression whose second operand is a pointer to a
	// member function is a prvalue.
	// [expr.mptr.oper] C++03
	// The result of a .* expression is an lvalue only if its first operand is an lvalue and its second operand is a
	// pointer to data member. The result of an ->* expression is an lvalue only if its second operand is a pointer
	// to data member.
	bool isLvalue = right.isMemberPointer() // the result is an lvalue if the second operand is a pointer to member
		&& !popType(right).isFunction() // and is not a function, therefore is a data member
		&& (operatorName == gOperatorArrowStarId || left.isLvalue); // and the expression is ->* or the first operand is an lvalue
	return ExpressionType(popType(right), isLvalue);
}


// [class.copy] The implicitly-declared copy assignment operator for class X has the return type X&
inline ExpressionType makeCopyAssignmentOperatorType(const SimpleType& classType)
{
	UniqueTypeWrapper type = makeUniqueSimpleType(classType);
	UniqueTypeWrapper parameter = type;
	parameter.value.setQualifiers(CvQualifiers(true, false));
	parameter.push_front(ReferenceType());
	type.push_front(ReferenceType());
	FunctionType function;
	function.parameterTypes.reserve(1);
	function.parameterTypes.push_back(parameter);
	type.push_front(function);
	return ExpressionType(type, true);
}

inline ExpressionType typeOfEnclosingClass(const InstantiationContext& context)
{
	Scope* scope = getEnclosingFunction(context.enclosingScope);
	SYMBOLS_ASSERT(scope != 0); // TODO: report non-fatal error: 'this' allowed only within function body
	Declaration* declaration = getFunctionDeclaration(scope);
	UniqueTypeWrapper functionType = getUniqueType(declaration->type, context, true);
	ExpressionType result(makeUniqueSimpleType(*context.enclosingType), true);
	result.value.setQualifiers(functionType.value.getQualifiers());
	return result;
}

inline ExpressionType typeOfNonStaticClassMemberAccessExpression(ExpressionType left, ExpressionType right)
{
	ExpressionType result = right;
	result.isLvalue = left.isLvalue;
	CvQualifiers qualifiers = left.value.getQualifiers();
	if(right.isMutable)
	{
		SYMBOLS_ASSERT(!right.value.getQualifiers().isConst); // TODO: non-fatal error: can't be both const and mutable
		qualifiers.isConst = false;
	}
	result.value.addQualifiers(qualifiers);
	return result;
}

inline ExpressionType typeOfIdExpression(const SimpleType* qualifying, const DeclarationInstance& declaration, const TemplateArgumentsInstance& templateArguments, const InstantiationContext& context)
{
	if(declaration == gDestructorInstance.p)
	{
		return ExpressionType(pushType(gUniqueTypeNull, FunctionType()), false); // type of destructor is 'function taking no parameter and returning no type'
	}
	else if(declaration == gCopyAssignmentOperatorInstance.p)
	{
		const SimpleType* idEnclosing = qualifying != 0 ? qualifying : context.enclosingType;
		SYMBOLS_ASSERT(idEnclosing != 0);
		return makeCopyAssignmentOperatorType(*idEnclosing);
	}

	if(isOverloadedFunction(declaration))
	{
		// [temp.arg.explicit]
		// In contexts where deduction is done and fails, or in contexts where deduction
		// is not done, if a template argument list is specified and it, along with any default template arguments,
		// identifies a single function template specialization, then the template-id is an lvalue for the function template
		// specialization.
		return selectOverloadedFunctionImpl(gUniqueTypeNull, IdExpression(declaration, qualifying, templateArguments), context);
	}

	const SimpleType* idEnclosing = getIdExpressionClass(qualifying, declaration, context.enclosingType);

	// a member of a class template may have a type which depends on a template parameter
	UniqueTypeWrapper type = getUniqueType(declaration->type, setEnclosingType(context, idEnclosing), declaration->isTemplate);
	ExpressionType result(type, isLvalue(*declaration));
	result.isMutable = declaration->specifiers.isMutable;

	return result;
}

ExpressionType getOverloadedMemberOperatorType(ExpressionType operand, const InstantiationContext& context);

inline ExpressionType getMemberOperatorType(Argument operand, bool isArrow, const InstantiationContext& context)
{
	ExpressionType type = operand.type;
	if(isArrow)
	{
		while(isClass(type))
		{
			type = getOverloadedMemberOperatorType(type, context);
		}
	}

	bool isPointer = type.isPointer();
	SYMBOLS_ASSERT(isPointer == isArrow);
	if(isPointer)
	{
		type.pop_front();
		type.isLvalue = true; // E1->E2 is transformed to (*E1).E2, and (*E) is always lvalue.
	}
	// the left-hand side is (pointer-to) operand
	SYMBOLS_ASSERT(type.isSimple());
	const SimpleType& classType = getSimpleType(type.value);
	if(isClass(*classType.declaration)) // if this is a class type. Note: special case where it might not be a class: (0).~decltype(0)();
	{
		// [expr.ref] [the type of the operand-expression shall be complete]
		instantiateClass(classType, context);
	}
	return type;
}

inline ExpressionType binaryOperatorArithmeticType(Name operatorName, ExpressionType left, ExpressionType right)
{
	return ExpressionType(usualArithmeticConversions(left, right), false); // non lvalue
}

inline ExpressionType binaryOperatorAdditiveType(Name operatorName, ExpressionType left, ExpressionType right)
{
	SYMBOLS_ASSERT(left != gUniqueTypeNull);
	SYMBOLS_ASSERT(right != gUniqueTypeNull);
	left = applyLvalueToRvalueConversion(left);
	right = applyLvalueToRvalueConversion(right);

	if(left.isPointer())
	{
		if(isIntegral(right)
			|| isEnumeration(right))
		{
			return left;
		}
		if(right.isPointer())
		{
			return ExpressionType(gSignedLongLongInt, false); // TODO: ptrdiff_t
		}
	}
	return ExpressionType(usualArithmeticConversions(left, right), false);
}

inline UniqueTypeWrapper makePointerCvUnion(UniqueTypeWrapper left, UniqueTypeWrapper right)
{
	CvQualifiers qualifiers = left.value.getQualifiers();
	qualifiers.isConst |= right.value.getQualifiers().isConst;
	qualifiers.isVolatile |= right.value.getQualifiers().isVolatile;

	UniqueTypeWrapper result = left;
	if((left.isPointer() && right.isPointer())
		|| (left.isMemberPointer() && right.isMemberPointer()
		&& getMemberPointerType(left.value).type == getMemberPointerType(right.value).type))
	{
		result = makePointerCvUnion(popType(left), popType(right));
		if(left.isPointer())
		{
			result.push_front(PointerType());
		}
		else
		{
			result.push_front(getMemberPointerType(left.value));
		}
	}
	else
	{
		SYMBOLS_ASSERT(left.value.getPointer() == right.value.getPointer()); // TODO: error: pointer types not similar
	}
	result.value.setQualifiers(qualifiers);
	return result;
}

inline ExpressionType binaryOperatorPointerType(Name operatorName, ExpressionType left, ExpressionType right)
{
	SYMBOLS_ASSERT(left.isPointer() || left.isMemberPointer());
	SYMBOLS_ASSERT(right.isPointer() || right.isMemberPointer());
	UniqueTypeWrapper result = left;
	// if one of the operands has type "pointer to cv1 void", then the other has type "pointer to cv2 T" and the composite
	// pointer type is "pointer to cv12 void", where cv12 is the union of cv1 and cv2.
	if(isVoidPointer(left)
		|| isVoidPointer(right))
	{
		SYMBOLS_ASSERT(left.isPointer() && right.isPointer());
		CvQualifiers qualifiers = left.value.getQualifiers();
		qualifiers.isConst |= right.value.getQualifiers().isConst;
		qualifiers.isVolatile |= right.value.getQualifiers().isVolatile;
		left.value.setQualifiers(qualifiers);
		left.push_front(PointerType());
		return left;
	}
	// Otherwise, the composite pointer type is a pointer type similar (4.4) to the type of one of the operands, with a cv-qualification signature
	// (4.4) that is the union of the cv-qualification signatures of the operand types.
	return ExpressionType(makePointerCvUnion(left, right), false); // non lvalue
}

inline ExpressionType getConditionalOperatorType(ExpressionType leftType, ExpressionType rightType)
{
	SYMBOLS_ASSERT(leftType != gUniqueTypeNull);
	SYMBOLS_ASSERT(rightType != gUniqueTypeNull);
	// [expr.cond]
	// If either the second or the third operand has type (possibly cv-qualified) void, then the lvalue-to-rvalue,
	// array-to-pointer, and function-to-pointer standard conversions are performed on the second and third operands,
	// and one of the following shall hold:
	//  - The second or the third operand (but not both) is a throw-expression; the result is of the type of
	//    the other and is an rvalue.
	//  - Both the second and the third operands have type void; the result is of type void and is an rvalue.
	//    [Note: this includes the case where both operands are throw-expressions.

	// TODO: technically not correct to remove toplevel qualifiers here, as it will change the overload resolution result when later choosing a conversion-function
	leftType.value.setQualifiers(CvQualifiers());
	rightType.value.setQualifiers(CvQualifiers());
	if(leftType == gVoid)
	{
		return rightType;
	}
	if(rightType == gVoid)
	{
		return leftType;
	}
	if(leftType.isLvalue
		&& rightType.isLvalue
		&& leftType == rightType)
	{
		// If the second and third operands are lvalues and have the same type, the result is of that type and is an lvalue.
		return leftType;
	}
	// Otherwise, the result is an rvalue.
	if(leftType != rightType
		&& (isClass(leftType) || isClass(rightType)))
	{
		SYMBOLS_ASSERT(false); // TODO: user-defined conversions
		return gNullExpressionType;
	}
	// Lvalue-to-rvalue (4.1), array-to-pointer (4.2), and function-to-pointer (4.3) standard conversions are performed
	// on the second and third operands. After those conversions, one of the following shall hold:
	ExpressionType left = applyLvalueToRvalueConversion(leftType);
	ExpressionType right = applyLvalueToRvalueConversion(rightType);
	// - The second and third operands have the same type; the result is of that type.
	if(left == right)
	{
		return left;
	}
	// - The second and third operands have arithmetic or enumeration type; the usual arithmetic conversions
	// 	 are performed to bring them to a common type, and the result is of that type.
	if((isArithmetic(left) || isEnumeration(left))
		&& (isArithmetic(right) || isEnumeration(right)))
	{
		return binaryOperatorArithmeticType(Name(), left, right);
	}
	// - The second and third operands have pointer type, or one has pointer type and the other is a null pointer
	// 	 constant; pointer conversions (4.10) and qualification conversions (4.4) are performed to bring them to
	// 	 their composite pointer type (5.9). The result is of the composite pointer type.
	// - The second and third operands have pointer to member type, or one has pointer to member type and the
	// 	 other is a null pointer constant; pointer to member conversions (4.11) and qualification conversions
	// 	 (4.4) are performed to bring them to a common type, whose cv-qualification shall match the cvqualification
	// 	 of either the second or the third operand. The result is of the common type.
	bool leftPointer = left.isPointer() || left.isMemberPointer();
	bool rightPointer = right.isPointer() || right.isMemberPointer();
	SYMBOLS_ASSERT(leftPointer || rightPointer);
	// TODO: assert that other pointer is null-pointer-constant: must be deferred if expression is value-dependent
	if(leftPointer && !right.isPointer())
	{
		return left;
	}
	if(rightPointer && !left.isPointer())
	{
		return right;
	}
	SYMBOLS_ASSERT(leftPointer && rightPointer);
	return binaryOperatorPointerType(Name(), left, right);
}


inline ExpressionType typeOfSubscriptExpression(Argument left, Argument right, const InstantiationContext& context)
{
	SYMBOLS_ASSERT(left.type != gUniqueTypeNull);
	if(isClass(left.type))
	{
		// [over.sub]
		// operator[] shall be a non-static member function with exactly one parameter.
		SYMBOLS_ASSERT(isComplete(left.type)); // TODO: non-fatal parse error
		const SimpleType& object = getSimpleType(left.type.value);
		instantiateClass(object, context); // searching for overloads requires a complete type
		Identifier tmp;
		tmp.value = gOperatorSubscriptId;
		tmp.source = context.source;
		LookupResultRef declaration = ::findDeclaration(object, tmp, IsAny());
		SYMBOLS_ASSERT(declaration != 0); // TODO: non-fatal error: expected array

		const SimpleType* memberEnclosing = findEnclosingType(&object, declaration->scope); // find the base class which contains the member-declaration
		SYMBOLS_ASSERT(memberEnclosing != 0);

		// The argument list submitted to overload resolution consists of the argument expressions present in the function
		// call syntax preceded by the implied object argument (E).
		Arguments arguments;
		arguments.push_back(left);
		arguments.push_back(right);

		OverloadSet overloads;
		addOverloaded(overloads, declaration, memberEnclosing);
		FunctionOverload overload = findBestOverloadedFunction(overloads, 0, arguments, setEnclosingTypeSafe(context, memberEnclosing));
		SYMBOLS_ASSERT(overload.declaration != 0);
		return overload.type;
	}

	UniqueTypeWrapper type = left.type;
	SYMBOLS_ASSERT(type.isArray() || type.isPointer()); // TODO: non-fatal error: attempting to dereference non-array/pointer
	type.pop_front(); // dereference left-hand side
	// [expr.sub] The result is an lvalue of type T. The type "T" shall be a completely defined object type.
	requireCompleteObjectType(type, context);
	return ExpressionType(type, true);
}

inline ExpressionType typeOfFunctionCallExpression(Argument left, const Arguments& arguments, const InstantiationContext& context)
{
	ExpressionWrapper expression = left;
	UniqueTypeWrapper type = left.type;

	SYMBOLS_ASSERT(expression.p != 0);

	if(isDependentIdExpression(expression)) // if this is an expression of the form 'undeclared-id(args)', the name must be found via ADL 
	{
		SYMBOLS_ASSERT(type == gUniqueTypeNull); // check that the id-expression names an overloaded function
		SYMBOLS_ASSERT(!arguments.empty()); // check that the argument-list was not empty
		SYMBOLS_ASSERT(getDependentIdExpression(expression).templateArguments.empty()); // cannot be a template-id
		SYMBOLS_ASSERT(getDependentIdExpression(expression).qualifying == gOverloaded); // cannot be qualified

		Identifier id;
		id.value = getDependentIdExpression(expression).name;
		OverloadSet overloads;
		addArgumentDependentOverloads(overloads, id, arguments);

		SYMBOLS_ASSERT(!overloads.empty()); // check that the declaration was found via ADL

		FunctionOverload overload = findBestOverloadedFunction(overloads, 0, arguments, context);
		SYMBOLS_ASSERT(overload.declaration != 0);
#if 0 // TODO: find the corresponding declaration-instance for a name found via ADL
		{
			DeclarationInstanceRef instance = findLastDeclaration(getDeclaration(*id), overload.declaration);
			setDecoration(id, instance);
		}
#endif
		return overload.type;
	}

	type = removeReference(type);
	if(isClass(type))
	{
		// [over.call.object]
		// If the primary-expression E in the function call syntax evaluates to a class object of type "cv T", then the set
		// of candidate functions includes at least the function call operators of T. The function call operators of T are
		// obtained by ordinary lookup of the name operator() in the context of (E).operator().
		SYMBOLS_ASSERT(isComplete(type)); // TODO: non-fatal parse error
		const SimpleType& object = getSimpleType(type.value);
		instantiateClass(object, context); // searching for overloads requires a complete type
		Identifier tmp;
		tmp.value = gOperatorFunctionId;
		tmp.source = context.source;
		LookupResultRef declaration = ::findDeclaration(object, tmp, IsAny());
		SYMBOLS_ASSERT(declaration != 0); // TODO: non-fatal error: expected function

		const SimpleType* memberEnclosing = findEnclosingType(&object, declaration->scope); // find the base class which contains the member-declaration
		SYMBOLS_ASSERT(memberEnclosing != 0);

		// The argument list submitted to overload resolution consists of the argument expressions present in the function
		// call syntax preceded by the implied object argument (E).
		Arguments augmentedArguments;
		augmentedArguments.push_back(left);
		augmentedArguments.insert(augmentedArguments.end(), arguments.begin(), arguments.end());

		OverloadSet overloads;
		addOverloaded(overloads, declaration, memberEnclosing);

		SYMBOLS_ASSERT(!overloads.empty());

		FunctionOverload overload = findBestOverloadedFunction(overloads, 0, augmentedArguments, setEnclosingTypeSafe(context, memberEnclosing));
		SYMBOLS_ASSERT(overload.declaration != 0);
#if 0 // TODO: record which overload was chosen, for dependency-tracking
		{
			DeclarationInstanceRef instance = findLastDeclaration(declaration, overload.declaration);
			setDecoration(&declaration->getName(), instance);
		}
#endif
		return overload.type;
	}

	if(type.isPointer()) // if this is a pointer to function
	{
		type.pop_front();
		SYMBOLS_ASSERT(type.isFunction());
		return getFunctionCallExpressionType(popType(type));
	}

	bool isClassMemberAccess = isClassMemberAccessExpression(expression);
	bool isNamed = isClassMemberAccess
		|| isIdExpression(expression);

	if(!isNamed) // if the left-hand expression does not contain an (optionally parenthesised) id-expression (and is not a class which supports 'operator()')
	{
		// the call does not require overload resolution
		SYMBOLS_ASSERT(type.isFunction());
		return getFunctionCallExpressionType(popType(type)); // get the return type: T
	}

	const IdExpression& idExpression = getIdExpression(
		isClassMemberAccess ? getClassMemberAccessExpression(expression).right : expression);
	DeclarationInstanceRef declaration = idExpression.declaration;
	const TemplateArgumentsInstance& templateArguments = idExpression.templateArguments;

	// [over.call.func] Call to named function
	SYMBOLS_ASSERT(declaration.p != 0);

	// if this is a qualified member-function-call, the class type of the object-expression
	ExpressionWrapper objectExpression = isClassMemberAccess ? getClassMemberAccessExpression(expression).left : ExpressionWrapper();
	ExpressionType objectExpressionType = isClassMemberAccess ? typeOfExpressionWrapper(objectExpression, context) : gNullExpressionType;
	const SimpleType* memberClass = isClassMemberAccess ? &getSimpleType(objectExpressionType.value) : 0;

	if(declaration.p == &gDestructorInstance)
	{
		return gNullExpressionType;
	}

#if 0
	if(declaration.p == &gCopyAssignmentOperatorInstance)
	{
		// [class.copy] If the class definition does not explicitly declare a copy assignment operator, one is declared implicitly.
		// TODO: ignore using-declaration with same id.
		// TODO: check correct lookup behaviour: base-class copy-assign should always be hidden by derived.
		// TODO: correct argument type depending on base class copy-assign declarations.

		// either the call is qualified or 'this' is valid
		SYMBOLS_ASSERT(memberClass != 0 || context.enclosingType != 0);
		SYMBOLS_ASSERT(memberClass == 0 || memberClass != &gDependentSimpleType);

		return popType(type);
	}
#endif

	// the identifier names an overloadable function

	SYMBOLS_ASSERT(declaration != &gDependentObject); // the id-expression should not be dependent
	SYMBOLS_ASSERT(UniqueTypeWrapper(declaration->type.unique).isFunction());

	// if this is a member-function-call, the type of the class containing the member
	const SimpleType* memberEnclosing = getIdExpressionClass(idExpression.enclosing, idExpression.declaration, memberClass != 0 ? memberClass : context.enclosingType);

	ExpressionNodeGeneric<ObjectExpression> transientExpression = ObjectExpression(gNullExpressionType);
	Arguments augmentedArguments;
	if(isMember(*declaration))
	{
		// either the call is qualified, 'this' is valid, or the member is static
		SYMBOLS_ASSERT(memberClass != 0 || context.enclosingType != 0 || isStatic(*declaration));
		SYMBOLS_ASSERT(memberClass == 0 || memberClass != &gDependentSimpleType);

		if(memberClass == 0) // unqualified function call
		{
			objectExpressionType = context.enclosingType != 0 && getEnclosingFunction(context.enclosingScope) != 0
				// If the keyword 'this' is in scope and refers to the class of that member function, or a derived class thereof,
				// then the function call is transformed into a normalized qualified function call using (*this) as the postfix-expression
				// to the left of the . operator.
				? typeOfEnclosingClass(context) // implicit '(*this).'
				// If the keyword 'this' is not in scope or refers to another class, then name resolution found a static member of some
				// class T. In this case, all overloaded declarations of the function name in T become candidate functions and
				// a contrived object of type T becomes the implied object argument
				: ExpressionType(makeUniqueSimpleType(*memberEnclosing), true);
			transientExpression = ObjectExpression(objectExpressionType);
			objectExpression = ExpressionWrapper(&transientExpression, false);
		}
		augmentedArguments.push_back(makeArgument(objectExpression, objectExpressionType));
	}

	augmentedArguments.insert(augmentedArguments.end(), arguments.begin(), arguments.end());

	OverloadSet overloads;
	addOverloaded(overloads, declaration, memberEnclosing);
	if(!isMember(*declaration))
	{
		// [basic.lookup.koenig]
		// If the ordinary unqualified lookup of the name finds the declaration of a class member function, the associated
		// namespaces and classes are not considered. Otherwise the set of declarations found by the lookup of
		// the function name is the union of the set of declarations found using ordinary unqualified lookup and the set
		// of declarations found in the namespaces and classes associated with the argument types.
		addArgumentDependentOverloads(overloads, declaration->getName(), augmentedArguments);
	}
	else
	{ // TODO: temporary hack: add special overload for implicitly declared copy-assignment operator
		SYMBOLS_ASSERT(memberEnclosing != 0);
		if(declaration->getName().value == gOperatorAssignId // if we found a user-defined operator=
			&& !memberEnclosing->hasCopyAssignmentOperator) // and the class does not have a copy-assignment operator
		{
			overloads.push_back(Overload(gCopyAssignmentOperatorInstance.p, memberEnclosing));
		}
	}

	SYMBOLS_ASSERT(!overloads.empty());

	// TODO: handle empty template-argument list '<>'. If specified, overload resolution should ignore non-templates
	FunctionOverload overload = findBestOverloadedFunction(overloads, templateArguments.empty() ? 0 : &templateArguments, augmentedArguments, context);
	SYMBOLS_ASSERT(overload.declaration != 0);
#if 0 // TODO: record which overload was chosen, for dependency-tracking
	{
		// TODO: this will give the wrong result if the declaration was found via ADL and is in a different namespace
		DeclarationInstanceRef instance = findLastDeclaration(declaration, overload.declaration);
		setDecoration(id, instance);
	}
#endif
	SYMBOLS_ASSERT(!::isDependent(overload.type));
	return overload.type;
}


inline bool isTransformedIdExpression(const ExpressionWrapper& expression, const InstantiationContext& context)
{
	return expression.isNonStaticMemberName // the id-expression names a non-static member
		&& context.enclosingType != 0
		&& getEnclosingFunction(context.enclosingScope) != 0; // in a context where 'this' can be used
}

inline ExpressionType typeOfClassMemberAccessExpression(const ExpressionWrapper& left, const ExpressionWrapper& right, const InstantiationContext& context)
{
	// [expr.ref]
	// If E2 is declared to have type "reference to T," then E1.E2 is an lvalue; the type of E1.E2 is T. Otherwise,
	// one of the following rules applies.
	//	- If E2 is a static data member and the type of E2 is T, then E1.E2 is an lvalue; the expression designates
	//	the named member of the class. The type of E1.E2 is T.
	//	- If E2 is a non-static data member and the type of E1 is "cq1 vq1 X", and the type of E2 is "cq2 vq2 T",
	//		the expression designates the named member of the object designated by the first expression. If E1 is
	//		an lvalue, then E1.E2 is an lvalue; if E1 is an xvalue, then E1.E2 is an xvalue; otherwise, it is a prvalue.
	//		Let the notation vq12 stand for the "union" of vq1 and vq2 ; that is, if vq1 or vq2 is volatile, then
	//		vq12 is volatile. Similarly, let the notation cq12 stand for the "union" of cq1 and cq2 ; that is, if cq1
	//		or cq2 is const, then cq12 is const. If E2 is declared to be a mutable member, then the type of E1.E2
	//		is "vq12 T". If E2 is not declared to be a mutable member, then the type of E1.E2 is "cq12 vq12 T".
	//	- If E2 is a (possibly overloaded) member function, function overload resolution (13.3) is used to determine
	//		whether E1.E2 refers to a static or a non-static member function.
	ExpressionType type = typeOfExpressionWrapper(left, context);
	SYMBOLS_ASSERT(!isDependent(type));
	const SimpleType& classType = getSimpleType(type.value);
	ExpressionType result = typeOfExpression(right, setEnclosingTypeSafe(context, &classType));
	if(right.isNonStaticMemberName)
	{
		result = typeOfNonStaticClassMemberAccessExpression(type, result);
	}
	return result;
}

inline UniqueTypeWrapper typeOfDecltypeSpecifier(const ExpressionWrapper& expression, const InstantiationContext& context)
{
	// [dcl.type.simple]
	// The type denoted by decltype(e) is defined as follows:
	//  - if e is an unparenthesized id-expression or an unparenthesized class member access (5.2.5), decltype(e)
	//  	is the type of the entity named by e. If there is no such entity, or if e names a set of overloaded functions,
	//  	the program is ill-formed;
	//  - otherwise, if e is an xvalue, decltype(e) is T&&, where T is the type of e;
	//  - otherwise, if e is an lvalue, decltype(e) is T&, where T is the type of e;
	//  - otherwise, decltype(e) is the type of e.
	if(!expression.isParenthesised
		&& (isIdExpression(expression)
		|| isClassMemberAccessExpression(expression)))
	{
		if(isClassMemberAccessExpression(expression))
		{
			const ClassMemberAccessExpression& cma = getClassMemberAccessExpression(expression);
			ExpressionType type = typeOfExpressionWrapper(cma.left, context);
			SYMBOLS_ASSERT(!isDependent(type));
			const SimpleType& classType = getSimpleType(type.value);
			return typeOfExpressionWrapper(cma.right, setEnclosingTypeSafe(context, &classType));
		}
		return typeOfExpressionWrapper(expression, context); // return the type of the id-expression;
	}
	ExpressionType result = typeOfExpressionWrapper(expression, context);
	if(result.isLvalue
		&& !result.isReference())
	{
		result = ExpressionType(pushType(result, ReferenceType()), true);
	}
	return result;
}

inline void evaluateStaticAssert(const ExpressionWrapper& expression, const char* message, const InstantiationContext& context)
{
	bool failed = evaluateExpression(expression, context).value == 0;
	// [dcl.dcl] If the value of the expression when so converted is true, the
	// declaration has no effect. Otherwise, the program is ill-formed, and the resulting diagnostic message (1.4)
	// shall include the text of the string-literal
	if(failed)
	{
		std::cout << "error: " << message << std::endl;
	}
	SYMBOLS_ASSERT(!failed || string_equal(message, "\"?false\"")); // TODO: report non-fatal error: "<message>"
}

// returns true if the given expression is "(S*)0"
inline bool isNullPointerCastExpression(const ExpressionWrapper& expression)
{
	if(!isCastExpression(expression))
	{
		return false;
	}
	const CastExpression& castExpression = getCastExpression(expression);
	if(castExpression.operand.p == 0 // T()
		|| !isIntegralConstantExpression(castExpression.operand))
	{
		return false;
	}
	return getIntegralConstantExpression(castExpression.operand).value.value == 0;
}


#endif
