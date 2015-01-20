
#ifndef INCLUDED_CPPPARSE_CORE_TYPETRAITS_H
#define INCLUDED_CPPPARSE_CORE_TYPETRAITS_H

#include "TypeInstantiate.h"
#include "Fundamental.h"
#include "OverloadResolve.h"


template<typename T>
inline Name getTypeTraitName(T* symbol)
{
	return symbol->trait->value.value;
}


inline bool isUnion(UniqueTypeWrapper type, const InstantiationContext& context)
{
	return type.isSimple() && isUnion(getInstance(type.value));
}

inline bool isClass(UniqueTypeWrapper type, const InstantiationContext& context)
{
	return isClass(type);
}

inline bool isEnum(UniqueTypeWrapper type, const InstantiationContext& context)
{
	return isEnum(type);
}


inline bool hasNothrowAssign(UniqueTypeWrapper type, const InstantiationContext& context)
{
	return false; // TODO: safe but sub-optimal
}

inline bool hasNothrowConstructor(UniqueTypeWrapper type, const InstantiationContext& context)
{
	return false; // TODO: safe but sub-optimal
}

inline bool hasNothrowCopy(UniqueTypeWrapper type, const InstantiationContext& context)
{
	return false; // TODO: safe but sub-optimal
}

inline bool hasTrivialAssign(UniqueTypeWrapper type, const InstantiationContext& context)
{
	return false; // TODO: safe but sub-optimal
}

inline bool hasTrivialConstructor(UniqueTypeWrapper type, const InstantiationContext& context)
{
	return false; // TODO: safe but sub-optimal
}

inline bool hasTrivialCopy(UniqueTypeWrapper type, const InstantiationContext& context)
{
	return false; // TODO: safe but sub-optimal
}

// returns true if type has no destructor and no members or bases with non-trivial destructors
inline bool hasTrivialDestructor(UniqueTypeWrapper type, const InstantiationContext& context)
{
	return false; // TODO: safe but sub-optimal
}

inline bool isConvertibleTo(UniqueTypeWrapper from, UniqueTypeWrapper to, const InstantiationContext& context)
{
	return getIcsRank(to, from, context, false, from.isReference()) != ICSRANK_INVALID;
}

inline bool isInstantiated(UniqueTypeWrapper type, const InstantiationContext& context)
{
	return !isClass(type) ? true : getInstance(type.value).instantiated;
}


inline UnaryTypeTraitsOp getUnaryTypeTraitsOp(cpp::typetraits_unary* symbol)
{
	switch(symbol->id)
	{
	case cpp::typetraits_unary::HAS_NOTHROW_ASSIGN: return hasNothrowAssign;
	case cpp::typetraits_unary::HAS_NOTHROW_CONSTRUCTOR: return hasNothrowConstructor;
	case cpp::typetraits_unary::HAS_NOTHROW_COPY: return hasNothrowCopy;
	case cpp::typetraits_unary::HAS_TRIVIAL_ASSIGN: return hasTrivialAssign;
	case cpp::typetraits_unary::HAS_TRIVIAL_CONSTRUCTOR: return hasTrivialConstructor;
	case cpp::typetraits_unary::HAS_TRIVIAL_COPY: return hasTrivialCopy;
	case cpp::typetraits_unary::HAS_TRIVIAL_DESTRUCTOR: return hasTrivialDestructor;
	case cpp::typetraits_unary::HAS_VIRTUAL_DESTRUCTOR: return hasVirtualDestructor;
	case cpp::typetraits_unary::IS_ABSTRACT: return isAbstract;
	case cpp::typetraits_unary::IS_CLASS: return isClass;
	case cpp::typetraits_unary::IS_EMPTY: return isEmpty;
	case cpp::typetraits_unary::IS_ENUM: return isEnum;
	case cpp::typetraits_unary::IS_POD: return isPod;
	case cpp::typetraits_unary::IS_POLYMORPHIC: return isPolymorphic;
	case cpp::typetraits_unary::IS_UNION: return isUnion;
	default: break;
	}
	throw SymbolsError();
}

inline BinaryTypeTraitsOp getBinaryTypeTraitsOp(cpp::typetraits_binary* symbol)
{
	switch(symbol->id)
	{
	case cpp::typetraits_binary::IS_BASE_OF: return isBaseOf;
	case cpp::typetraits_binary::IS_CONVERTIBLE_TO: return isConvertibleTo;
	default: break;
	}
	throw SymbolsError();
}

#endif
