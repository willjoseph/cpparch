
#ifndef INCLUDED_CPPPARSE_AST_EXPRESSIONIMPL_H
#define INCLUDED_CPPPARSE_AST_EXPRESSIONIMPL_H

#include "Ast/Expression.h"
#include "Ast/Type.h"

struct Instance;
struct InstantiationContext;

// ----------------------------------------------------------------------------
inline ExpressionWrapper makeArgument(ExpressionWrapper expression, ExpressionType type)
{
	ExpressionWrapper result(expression);
	result.type = type;
	return result;
}
typedef ExpressionWrapper Argument;
typedef std::vector<Argument> Arguments;

inline bool lessUniqueExpression(const ExpressionWrapper& left, const ExpressionWrapper& right)
{
	SYMBOLS_ASSERT(left.isUnique);
	SYMBOLS_ASSERT(right.isUnique);
	return left.p < right.p;
}

inline bool lessArguments(const Arguments& left, const Arguments& right)
{
	return std::lexicographical_compare(left.begin(), left.end(),
		right.begin(), right.end(), lessUniqueExpression);
}

inline bool isUniqueArguments(const Arguments& arguments)
{
	for(Arguments::const_iterator i = arguments.begin(); i != arguments.end(); ++i)
	{
		if(!(*i).isUnique)
		{
			return false;
		}
	}
	return true;
}

// ----------------------------------------------------------------------------
struct ExpressionList
{
	Arguments expressions;
	ExpressionList(const Arguments& expressions)
		: expressions(expressions)
	{
	}
};

inline bool operator<(const ExpressionList& left, const ExpressionList& right)
{
	return lessArguments(left.expressions, right.expressions);
}

inline bool isUniqueExpression(const ExpressionList& e)
{
	return isUniqueArguments(e.expressions);
}

inline bool isExpressionList(ExpressionNode* node)
{
	return isEqual(getTypeInfo(*node), getTypeInfo<ExpressionNodeGeneric<ExpressionList> >());
}

inline const ExpressionList& getExpressionList(ExpressionNode* node)
{
	SYMBOLS_ASSERT(isExpressionList(node));
	return static_cast<const ExpressionNodeGeneric<ExpressionList>*>(node)->value;
}

// ----------------------------------------------------------------------------
struct IntegralConstantExpression
{
	ExpressionType type;
	IntegralConstant value;
	IntegralConstantExpression(ExpressionType type, IntegralConstant value)
		: type(type), value(value)
	{
	}
};

inline bool operator<(const IntegralConstantExpression& left, const IntegralConstantExpression& right)
{
	return left.type != right.type
		? left.type < right.type
		: left.value.value < right.value.value;
}

inline ExpressionWrapper makeConstantExpression(const IntegralConstantExpression& node)
{
	ExpressionWrapper result(makeUniqueExpression(node));
	result.isUnique = true;
	result.type = node.type;
	result.value.value = node.value;
	result.value.isConstant = true;
	return result;
}

inline bool isIntegralConstantExpression(ExpressionNode* node)
{
	return isEqual(getTypeInfo(*node), getTypeInfo<ExpressionNodeGeneric<IntegralConstantExpression> >());
}

inline const IntegralConstantExpression& getIntegralConstantExpression(ExpressionNode* node)
{
	SYMBOLS_ASSERT(isIntegralConstantExpression(node));
	return static_cast<const ExpressionNodeGeneric<IntegralConstantExpression>*>(node)->value;
}


// ----------------------------------------------------------------------------
struct CastExpression
{
	UniqueTypeWrapper type;
	Arguments arguments;
	CastExpression(UniqueTypeWrapper type, const Arguments& arguments)
		: type(type), arguments(arguments)
	{
	}
};

inline bool operator<(const CastExpression& left, const CastExpression& right)
{
	return left.type != right.type
		? left.type < right.type
		: lessArguments(left.arguments, right.arguments);
}

inline bool isUniqueExpression(const CastExpression& e)
{
	return isUniqueArguments(e.arguments);
}

inline bool isCastExpression(ExpressionNode* node)
{
	return isEqual(getTypeInfo(*node), getTypeInfo<ExpressionNodeGeneric<CastExpression> >());
}

inline const CastExpression& getCastExpression(ExpressionNode* node)
{
	SYMBOLS_ASSERT(isCastExpression(node));
	return static_cast<const ExpressionNodeGeneric<CastExpression>*>(node)->value;
}


// ----------------------------------------------------------------------------
struct DependentIdExpression
{
	Name name;
	UniqueTypeWrapper qualifying;
	TemplateArgumentsInstance templateArguments;
	bool isQualified;
	DependentIdExpression(Name name, UniqueTypeWrapper qualifying, TemplateArgumentsInstance templateArguments, bool isQualified)
		: name(name), qualifying(qualifying), templateArguments(templateArguments), isQualified(isQualified)
	{
		SYMBOLS_ASSERT(qualifying.value.p != 0);
	}
};

inline bool operator<(const DependentIdExpression& left, const DependentIdExpression& right)
{
	return left.name != right.name
		? left.name < right.name
		: left.qualifying != right.qualifying
		? left.qualifying < right.qualifying
		: left.templateArguments != right.templateArguments
		? left.templateArguments < right.templateArguments
		: left.isQualified < right.isQualified;
}

inline bool isUniqueExpression(const DependentIdExpression& e)
{
	return true;
}

inline bool isDependentIdExpression(ExpressionNode* node)
{
	return isEqual(getTypeInfo(*node), getTypeInfo<ExpressionNodeGeneric<DependentIdExpression> >());
}

inline const DependentIdExpression& getDependentIdExpression(ExpressionNode* node)
{
	SYMBOLS_ASSERT(isDependentIdExpression(node));
	return static_cast<const ExpressionNodeGeneric<DependentIdExpression>*>(node)->value;
}


// ----------------------------------------------------------------------------
struct IdExpression
{
	DeclarationInstanceRef declaration;
	const Instance* enclosingInstance;
	// TODO: handle empty template-argument list '<>'. If specified, overload resolution should ignore non-templates
	TemplateArgumentsInstance templateArguments;
	bool isQualified;
	IdExpression(DeclarationInstanceRef declaration, const Instance* enclosingInstance, const TemplateArgumentsInstance& templateArguments, bool isQualified)
		: declaration(declaration), enclosingInstance(enclosingInstance), templateArguments(templateArguments), isQualified(isQualified)
	{
	}
};

inline bool operator<(const IdExpression& left, const IdExpression& right)
{
	return left.declaration.p != right.declaration.p
		? left.declaration.p < right.declaration.p
		: left.enclosingInstance != right.enclosingInstance
		? left.enclosingInstance < right.enclosingInstance
		: left.isQualified != right.isQualified
		? left.isQualified < right.isQualified
		: left.templateArguments < right.templateArguments;
}

// IdExpression may name an identifier with a dependent type, which may or may not be part of an integral-constant-expression.
inline bool isUniqueExpression(const IdExpression& e)
{
	return true;
}

inline bool isIdExpression(ExpressionNode* node)
{
	return isEqual(getTypeInfo(*node), getTypeInfo<ExpressionNodeGeneric<IdExpression> >());
}

inline const IdExpression& getIdExpression(ExpressionNode* node)
{
	SYMBOLS_ASSERT(isIdExpression(node));
	return static_cast<const ExpressionNodeGeneric<IdExpression>*>(node)->value;
}


// ----------------------------------------------------------------------------
struct NonTypeTemplateParameter
{
	DeclarationPtr declaration;
	UniqueTypeWrapper type; // template<int x> vs template<bool y>: distinguishes expression 'x' from 'y' (note: type may be dependent)
	NonTypeTemplateParameter(DeclarationPtr declaration, UniqueTypeWrapper type)
		: declaration(declaration), type(type)
	{
	}
};

inline bool operator<(const NonTypeTemplateParameter& left, const NonTypeTemplateParameter& right)
{
	return left.declaration->scope->templateDepth != right.declaration->scope->templateDepth
		? left.declaration->scope->templateDepth < right.declaration->scope->templateDepth
		: left.declaration->templateParameter != right.declaration->templateParameter
		? left.declaration->templateParameter < right.declaration->templateParameter
		: left.type.value.getPointer() < right.type.value.getPointer(); // ignore cv-qualifiers
}

inline bool isUniqueExpression(const NonTypeTemplateParameter& e)
{
	return true;
}

inline bool isNonTypeTemplateParameter(ExpressionNode* node)
{
	return isEqual(getTypeInfo(*node), getTypeInfo<ExpressionNodeGeneric<NonTypeTemplateParameter> >());
}

inline const NonTypeTemplateParameter& getNonTypeTemplateParameter(ExpressionNode* node)
{
	SYMBOLS_ASSERT(isNonTypeTemplateParameter(node));
	return static_cast<const ExpressionNodeGeneric<NonTypeTemplateParameter>*>(node)->value;
}

// ----------------------------------------------------------------------------
struct SizeofExpression
{
	// [expr.sizeof] The operand is ... an expression, which is not evaluated
	ExpressionWrapper operand;
	SizeofExpression(ExpressionWrapper operand)
		: operand(operand)
	{
	}
};

inline bool operator<(const SizeofExpression& left, const SizeofExpression& right)
{
	return left.operand.p < right.operand.p;
}

inline bool isUniqueExpression(const SizeofExpression& e)
{
	return true;
}

struct SizeofTypeExpression
{
	// [expr.sizeof] The operand is ... a parenthesized type-id
	UniqueTypeWrapper type;
	SizeofTypeExpression(UniqueTypeWrapper type)
		: type(type)
	{
	}
};

inline bool operator<(const SizeofTypeExpression& left, const SizeofTypeExpression& right)
{
	return left.type < right.type;
}

inline bool isUniqueExpression(const SizeofTypeExpression& e)
{
	return true;
}

// ----------------------------------------------------------------------------
typedef IntegralConstant (*UnaryIceOp)(IntegralConstant);

struct UnaryExpression
{
	Name operatorName;
	UnaryIceOp operation;
	ExpressionWrapper first;
	UnaryExpression(Name operatorName, UnaryIceOp operation, ExpressionWrapper first)
		: operatorName(operatorName), operation(operation), first(first)
	{
	}
};

inline bool operator<(const UnaryExpression& left, const UnaryExpression& right)
{
	return left.operation != right.operation
		? left.operation < right.operation
		: left.first.p < right.first.p;
}

inline bool isUniqueExpression(const UnaryExpression& e)
{
	return e.first.isUnique && e.operation != 0;
}

inline bool isUnaryExpression(ExpressionNode* node)
{
	return isEqual(getTypeInfo(*node), getTypeInfo<ExpressionNodeGeneric<UnaryExpression> >());
}

inline const UnaryExpression& getUnaryExpression(ExpressionNode* node)
{
	SYMBOLS_ASSERT(isUnaryExpression(node));
	return static_cast<const ExpressionNodeGeneric<UnaryExpression>*>(node)->value;
}

// ----------------------------------------------------------------------------
typedef IntegralConstant (*BinaryIceOp)(IntegralConstant, IntegralConstant);
typedef ExpressionType (*BinaryTypeOp)(Name operatorName, Argument first, Argument second, const InstantiationContext& context);

struct BinaryExpression
{
	Name operatorName;
	BinaryIceOp operation;
	BinaryTypeOp type;
	ExpressionWrapper first;
	ExpressionWrapper second;
	BinaryExpression(Name operatorName, BinaryIceOp operation, BinaryTypeOp type, ExpressionWrapper first, ExpressionWrapper second)
		: operatorName(operatorName), operation(operation), type(type), first(first), second(second)
	{
	}
};

inline bool operator<(const BinaryExpression& left, const BinaryExpression& right)
{
	return left.operation != right.operation
		? left.operation < right.operation
		: left.type != right.type
		? left.type < right.type
		: left.first.p != right.first.p
		? left.first.p < right.first.p
		: left.second.p < right.second.p;
}

inline bool isUniqueExpression(const BinaryExpression& e)
{
	return e.first.isUnique && e.second.isUnique && e.operation != 0;
}

// ----------------------------------------------------------------------------
typedef IntegralConstant (*TernaryIceOp)(IntegralConstant, IntegralConstant, IntegralConstant);

struct TernaryExpression
{
	TernaryIceOp operation;
	ExpressionWrapper first;
	ExpressionWrapper second;
	ExpressionWrapper third;
	TernaryExpression(TernaryIceOp operation, ExpressionWrapper first, ExpressionWrapper second, ExpressionWrapper third)
		: operation(operation), first(first), second(second), third(third)
	{
	}
};

inline bool operator<(const TernaryExpression& left, const TernaryExpression& right)
{
	return left.operation != right.operation
		? left.operation < right.operation
		: left.first.p != right.first.p
		? left.first.p < right.first.p
		: left.second.p != right.second.p
		? left.second.p < right.second.p
		: left.third.p < right.third.p;
}

inline bool isUniqueExpression(const TernaryExpression& e)
{
	return e.first.isUnique && e.second.isUnique && e.third.isUnique != 0;
}

// ----------------------------------------------------------------------------
typedef bool (*UnaryTypeTraitsOp)(UniqueTypeWrapper, const InstantiationContext& context);
typedef bool (*BinaryTypeTraitsOp)(UniqueTypeWrapper, UniqueTypeWrapper, const InstantiationContext& context);

struct TypeTraitsUnaryExpression
{
	Name traitName;
	UnaryTypeTraitsOp operation;
	UniqueTypeWrapper type;
	TypeTraitsUnaryExpression(Name traitName, UnaryTypeTraitsOp operation, UniqueTypeWrapper type)
		: traitName(traitName), operation(operation), type(type)
	{
	}
};

inline bool operator<(const TypeTraitsUnaryExpression& left, const TypeTraitsUnaryExpression& right)
{
	return left.operation != right.operation
		? left.operation < right.operation
		: left.type < right.type;
}

inline bool isUniqueExpression(const TypeTraitsUnaryExpression& e)
{
	return true;
}

// ----------------------------------------------------------------------------
struct TypeTraitsBinaryExpression
{
	Name traitName;
	BinaryTypeTraitsOp operation;
	UniqueTypeWrapper first;
	UniqueTypeWrapper second;
	TypeTraitsBinaryExpression(Name traitName, BinaryTypeTraitsOp operation, UniqueTypeWrapper first, UniqueTypeWrapper second)
		: traitName(traitName), operation(operation), first(first), second(second)
	{
		SYMBOLS_ASSERT(first != gUniqueTypeNull);
		SYMBOLS_ASSERT(second != gUniqueTypeNull);
	}
};

inline bool operator<(const TypeTraitsBinaryExpression& left, const TypeTraitsBinaryExpression& right)
{
	return left.operation != right.operation
		? left.operation < right.operation
		: left.first != right.first
		? left.first < right.first
		: left.second < right.second;
}

inline bool isUniqueExpression(const TypeTraitsBinaryExpression& e)
{
	return true;
}



// ----------------------------------------------------------------------------
struct ExplicitTypeExpression
{
	UniqueTypeWrapper type;
	ExpressionWrapper argument;
	// delete expr
	// throw expr
	// typeid(expr)
	ExplicitTypeExpression(UniqueTypeWrapper type, const ExpressionWrapper& argument)
		: type(type), argument(argument)
	{
		SYMBOLS_ASSERT(type != gUniqueTypeNull);
	}
	// typeid(T)
	// this
	ExplicitTypeExpression(UniqueTypeWrapper type)
		: type(type)
	{
		SYMBOLS_ASSERT(type != gUniqueTypeNull);
	}
};

inline bool operator<(const ExplicitTypeExpression& left, const ExplicitTypeExpression& right)
{
	SYMBOLS_ASSERT(false);
	return false;
}

inline bool isUniqueExpression(const ExplicitTypeExpression& e)
{
	return false;
}

inline bool isExplicitTypeExpression(ExpressionNode* node)
{
	return isEqual(getTypeInfo(*node), getTypeInfo<ExpressionNodeGeneric<ExplicitTypeExpression> >());
}

inline const ExplicitTypeExpression& getExplicitTypeExpression(ExpressionNode* node)
{
	SYMBOLS_ASSERT(isExplicitTypeExpression(node));
	return static_cast<const ExpressionNodeGeneric<ExplicitTypeExpression>*>(node)->value;
}

// ----------------------------------------------------------------------------
// new T(args)
// new (p) T(args)
struct NewExpression
{
	UniqueTypeWrapper type;
	ExpressionWrapper newArray;
	Arguments arguments; // NOTE: includes placement-new location
	NewExpression(UniqueTypeWrapper type, ExpressionWrapper newArray, const Arguments& arguments)
		: type(type), newArray(newArray), arguments(arguments)
	{
		SYMBOLS_ASSERT(type != gUniqueTypeNull);
	}
};

inline bool operator<(const NewExpression& left, const NewExpression& right)
{
	SYMBOLS_ASSERT(false);
}

inline bool isUniqueExpression(const NewExpression& e)
{
	return false;
}

inline bool isNewExpression(ExpressionNode* node)
{
	return isEqual(getTypeInfo(*node), getTypeInfo<ExpressionNodeGeneric<NewExpression> >());
}

inline const NewExpression& getNewExpression(ExpressionNode* node)
{
	SYMBOLS_ASSERT(isNewExpression(node));
	return static_cast<const ExpressionNodeGeneric<NewExpression>*>(node)->value;
}

// ----------------------------------------------------------------------------
struct MemberOperatorExpression
{
	ExpressionWrapper left;
	bool isArrow;
	MemberOperatorExpression(ExpressionWrapper left, bool isArrow)
		: left(left), isArrow(isArrow)
	{
	}
};

inline bool operator<(const MemberOperatorExpression& left, const MemberOperatorExpression& right)
{
	SYMBOLS_ASSERT(false);
	return false;
}

inline bool isUniqueExpression(const MemberOperatorExpression& e)
{
	return false;
}

inline bool isDependentObjectExpression(ExpressionNode* node)
{
	return isEqual(getTypeInfo(*node), getTypeInfo<ExpressionNodeGeneric<MemberOperatorExpression> >());
}

inline const MemberOperatorExpression& getDependentObjectExpression(ExpressionNode* node)
{
	SYMBOLS_ASSERT(isDependentObjectExpression(node));
	return static_cast<const ExpressionNodeGeneric<MemberOperatorExpression>*>(node)->value;
}


// ----------------------------------------------------------------------------
// (*this)
struct ObjectExpression
{
	ExpressionType type;
	ObjectExpression(const ExpressionType& type)
		: type(type)
	{
	}
};

inline bool operator<(const ObjectExpression& left, const ObjectExpression& right)
{
	SYMBOLS_ASSERT(false);
	return false;
}

inline bool isUniqueExpression(const ObjectExpression& e)
{
	return false;
}

inline bool isObjectExpression(ExpressionNode* node)
{
	return isEqual(getTypeInfo(*node), getTypeInfo<ExpressionNodeGeneric<ObjectExpression> >());
}

inline const ObjectExpression& getObjectExpression(ExpressionNode* node)
{
	SYMBOLS_ASSERT(isObjectExpression(node));
	return static_cast<const ExpressionNodeGeneric<ObjectExpression>*>(node)->value;
}


// ----------------------------------------------------------------------------
struct ClassMemberAccessExpression
{
	ExpressionWrapper left; // ObjectExpression or MemberOperatorExpression
	ExpressionWrapper right; // IdExpression or DependentIdExpression
	ClassMemberAccessExpression(ExpressionWrapper left, ExpressionWrapper right)
		: left(left), right(right)
	{
	}
};

inline bool operator<(const ClassMemberAccessExpression& left, const ClassMemberAccessExpression& right)
{
	return left.right.p < right.right.p; // a class-member-access may be an integral constant expression if the righthand side is an integral constant expression
}

inline bool isUniqueExpression(const ClassMemberAccessExpression& e)
{
	return false;
}

inline bool isClassMemberAccessExpression(ExpressionNode* node)
{
	return isEqual(getTypeInfo(*node), getTypeInfo<ExpressionNodeGeneric<ClassMemberAccessExpression> >());
}

inline const ClassMemberAccessExpression& getClassMemberAccessExpression(ExpressionNode* node)
{
	SYMBOLS_ASSERT(isClassMemberAccessExpression(node));
	return static_cast<const ExpressionNodeGeneric<ClassMemberAccessExpression>*>(node)->value;
}


// ----------------------------------------------------------------------------
struct OffsetofExpression
{
	UniqueTypeWrapper type;
	DeclarationPtr member;
	OffsetofExpression(UniqueTypeWrapper type, DeclarationPtr member)
		: type(type), member(member)
	{
	}
};

inline bool operator<(const OffsetofExpression& left, const OffsetofExpression& right)
{
	return left.type != right.type
		? left.type < right.type
		: left.member.p < right.member.p;
}

inline bool isUniqueExpression(const OffsetofExpression& e)
{
	return true;
}

// ----------------------------------------------------------------------------
// id-expression ( expression-list )
// overload resolution is required if the lefthand side is
// - a (parenthesised) id-expression
// - of class type
struct FunctionCallExpression
{
	ExpressionWrapper left; // TODO: extract objectExpressionClass, id, idEnclosing, type, templateArguments
	Arguments arguments;
	FunctionCallExpression(ExpressionWrapper left, const Arguments& arguments)
		: left(left), arguments(arguments)
	{
	}
};

inline bool operator<(const FunctionCallExpression& left, const FunctionCallExpression& right)
{
	SYMBOLS_ASSERT(false);
	return false;
}

inline bool isUniqueExpression(const FunctionCallExpression& e)
{
	return false;
}


// ----------------------------------------------------------------------------
struct SubscriptExpression
{
	ExpressionWrapper left;
	ExpressionWrapper right;
	SubscriptExpression(ExpressionWrapper left, ExpressionWrapper right)
		: left(left), right(right)
	{
	}
};

inline bool operator<(const SubscriptExpression& left, const SubscriptExpression& right)
{
	SYMBOLS_ASSERT(false);
	return false;
}

inline bool isUniqueExpression(const SubscriptExpression& e)
{
	return false;
}


// ----------------------------------------------------------------------------
struct PostfixOperatorExpression
{
	Name operatorName;
	ExpressionWrapper operand;
	PostfixOperatorExpression(Name operatorName, ExpressionWrapper operand)
		: operatorName(operatorName), operand(operand)
	{
	}
};

inline bool operator<(const PostfixOperatorExpression& left, const PostfixOperatorExpression& right)
{
	SYMBOLS_ASSERT(false);
	return false;
}

inline bool isUniqueExpression(const PostfixOperatorExpression& e)
{
	return false;
}


#endif
