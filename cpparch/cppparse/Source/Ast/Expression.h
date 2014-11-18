
#ifndef INCLUDED_CPPPARSE_AST_EXPRESSION_H
#define INCLUDED_CPPPARSE_AST_EXPRESSION_H

#include "Common/Allocator.h"
#include "Common/IndirectSet.h"
#include "UniqueType.h"

struct Scope;

// [expr.const]
// An integral constant-expression can involve only literals, enumerators, const variables or static
// data members of integral or enumeration types initialized with constant expressions, non-type template
// parameters of integral or enumeration types, and sizeof expressions
struct ExpressionNodeVisitor
{
	virtual void visit(const struct IntegralConstantExpression&) = 0; // literal
	virtual void visit(const struct CastExpression&) = 0;
	virtual void visit(const struct NonTypeTemplateParameter&) = 0; // non-type template parameter
	virtual void visit(const struct DependentIdExpression&) = 0; // T::name
	virtual void visit(const struct IdExpression&) = 0; // enumerator, const variable, static data member, non-type template parameter
	virtual void visit(const struct SizeofExpression&) = 0;
	virtual void visit(const struct SizeofTypeExpression&) = 0;
	virtual void visit(const struct UnaryExpression&) = 0;
	virtual void visit(const struct BinaryExpression&) = 0;
	virtual void visit(const struct TernaryExpression&) = 0;
	virtual void visit(const struct TypeTraitsUnaryExpression&) = 0;
	virtual void visit(const struct TypeTraitsBinaryExpression&) = 0;
	virtual void visit(const struct ExplicitTypeExpression&) = 0;
	virtual void visit(const struct ObjectExpression&) = 0; // transformed 'c.'
	virtual void visit(const struct MemberOperatorExpression&) = 0; // 'dependent->' or 'dependent.'
	virtual void visit(const struct ClassMemberAccessExpression&) = 0;
	virtual void visit(const struct OffsetofExpression&) = 0;
	virtual void visit(const struct FunctionCallExpression&) = 0;
	virtual void visit(const struct SubscriptExpression&) = 0;
	virtual void visit(const struct PostfixOperatorExpression&) = 0;
};

struct ExpressionNode : TypeInfo
{
	Scope* enclosingTemplate; // if the expression is dependent, the enclosing template class/function
	ExpressionNode(TypeInfo type, Scope* enclosingTemplate)
		: TypeInfo(type), enclosingTemplate(enclosingTemplate)
	{
	}
	virtual ~ExpressionNode()
	{
	}
	virtual void accept(ExpressionNodeVisitor& visitor) const = 0;
	virtual bool lessThan(const ExpressionNode& other) const = 0;
};

inline bool operator<(const ExpressionNode& left, const ExpressionNode& right)
{
	return left.enclosingTemplate < right.enclosingTemplate ||
		(!(right.enclosingTemplate < left.enclosingTemplate) && left.lessThan(right));
}


typedef SafePtr<ExpressionNode> ExpressionPtr;

template<typename T>
struct ExpressionNodeGeneric : ExpressionNode
{
	T value;
	ExpressionNodeGeneric(const T& value, Scope* enclosingTemplate = 0)
		: ExpressionNode(getTypeInfo<ExpressionNodeGeneric>(), enclosingTemplate), value(value)
	{
	}
	void accept(ExpressionNodeVisitor& visitor) const
	{
		visitor.visit(value);
	}
	bool lessThan(const ExpressionNode& other) const
	{
		return abstractLess(*this, other);
	}
};

template<typename T>
inline bool operator<(const ExpressionNodeGeneric<T>& left, const ExpressionNodeGeneric<T>& right)
{
	return left.value < right.value;
}

typedef ExpressionNode* UniqueExpression;

typedef IndirectSet<UniqueExpression> UniqueExpressions;

extern UniqueExpressions gBuiltInExpressions;
extern UniqueExpressions gUniqueExpressions;

template<typename T>
inline UniqueExpression makeBuiltInExpression(const T& value)
{
	ExpressionNodeGeneric<T> node(value);
	return *gBuiltInExpressions.insert(node);
}

template<typename T>
inline UniqueExpression makeUniqueExpression(const T& value, Scope* enclosingTemplate = 0)
{
	ExpressionNodeGeneric<T> node(value, enclosingTemplate);
	{
		UniqueExpressions::iterator i = gBuiltInExpressions.find(node);
		if(i != gBuiltInExpressions.end())
		{
			return *i;
		}
	}
	return *gUniqueExpressions.insert(node);
}




// ----------------------------------------------------------------------------
// [expr.const]
struct IntegralConstant
{
	int value;
	IntegralConstant() : value(0)
	{
	}
	explicit IntegralConstant(int value) : value(value)
	{
	}
	explicit IntegralConstant(double value) : value(int(value))
	{
	}
	explicit IntegralConstant(size_t value) : value(int(value))
	{
	}
};


struct ExpressionType : UniqueTypeWrapper
{
	bool isLvalue; // true if the expression is an lvalue
	bool isMutable; // true if the expression is an id-expression naming a mutable object
	ExpressionType() : isLvalue(false), isMutable(false)
	{
	}
	ExpressionType(UniqueTypeWrapper type, bool isLvalue)
		: UniqueTypeWrapper(type), isLvalue(isLvalue), isMutable(false)
	{
	}
};


struct ExpressionValue
{
	IntegralConstant value;
	bool isConstant;
	ExpressionValue(IntegralConstant value, bool isConstant)
		: value(value), isConstant(isConstant)
	{
	}
};

inline ExpressionValue makeConstantValue(IntegralConstant value)
{
	return ExpressionValue(value, true);
}

const ExpressionValue EXPRESSIONRESULT_INVALID = ExpressionValue(IntegralConstant(0), false);
const ExpressionValue EXPRESSIONRESULT_ZERO = ExpressionValue(IntegralConstant(0), true);

inline bool isNullPointerConstantValue(ExpressionValue value)
{
	return value.isConstant
		&& value.value.value == 0;
}

struct ExpressionWrapper : ExpressionPtr
{
	ExpressionType type; // valid if this expression is not type-dependent
	IntegralConstant value; // valid if this is expression is integral-constant and not value-dependent
	bool isUnique;
	bool isConstant;
	bool isDependent; // true if any subexpression is type-dependent or value-dependent.
	bool isTypeDependent;
	bool isValueDependent;
	bool isTemplateArgumentAmbiguity; // [temp.arg] In a template argument, an ambiguity between a typeid and an expression is resolved to a typeid
	bool isNonStaticMemberName;
	bool isQualifiedNonStaticMemberName;
	bool isParenthesised; // true if the expression is surrounded by one or more sets of parentheses
	ExpressionWrapper()
		: ExpressionPtr(0)
		, isUnique(false)
		, isConstant(false)
		, isDependent(false)
		, isTypeDependent(false)
		, isValueDependent(false)
		, isTemplateArgumentAmbiguity(false)
		, isNonStaticMemberName(false)
		, isQualifiedNonStaticMemberName(false)
		, isParenthesised(false)
	{
	}
	explicit ExpressionWrapper(ExpressionNode* node, bool isTypeDependent = false, bool isValueDependent = false)
		: ExpressionPtr(node)
		, isUnique(false)
		, isConstant(false)
		, isTypeDependent(isTypeDependent)
		, isValueDependent(isValueDependent)
		, isTemplateArgumentAmbiguity(false)
		, isNonStaticMemberName(false)
		, isQualifiedNonStaticMemberName(false)
		, isParenthesised(false)
	{
	}
};

#endif
