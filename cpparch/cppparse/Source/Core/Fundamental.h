
#ifndef INCLUDED_CPPPARSE_CORE_FUNDAMENTAL_H
#define INCLUDED_CPPPARSE_CORE_FUNDAMENTAL_H


#include "Ast/Type.h"
#include "Special.h"


// fundamental types
extern BuiltInTypeDeclaration gCharDeclaration;
extern BuiltInTypeDeclaration gSignedCharDeclaration;
extern BuiltInTypeDeclaration gUnsignedCharDeclaration;
extern BuiltInTypeDeclaration gSignedShortIntDeclaration;
extern BuiltInTypeDeclaration gUnsignedShortIntDeclaration;
extern BuiltInTypeDeclaration gSignedIntDeclaration;
extern BuiltInTypeDeclaration gUnsignedIntDeclaration;
extern BuiltInTypeDeclaration gSignedLongIntDeclaration;
extern BuiltInTypeDeclaration gUnsignedLongIntDeclaration;
extern BuiltInTypeDeclaration gSignedLongLongIntDeclaration;
extern BuiltInTypeDeclaration gUnsignedLongLongIntDeclaration;
extern BuiltInTypeDeclaration gWCharTDeclaration;
extern BuiltInTypeDeclaration gBoolDeclaration;
extern BuiltInTypeDeclaration gFloatDeclaration;
extern BuiltInTypeDeclaration gDoubleDeclaration;
extern BuiltInTypeDeclaration gLongDoubleDeclaration;
extern BuiltInTypeDeclaration gVoidDeclaration;
extern BuiltInTypeId gChar;
extern BuiltInTypeId gSignedChar;
extern BuiltInTypeId gUnsignedChar;
extern BuiltInTypeId gSignedShortInt;
extern BuiltInTypeId gUnsignedShortInt;
extern BuiltInTypeId gSignedInt;
extern BuiltInTypeId gUnsignedInt;
extern BuiltInTypeId gSignedLongInt;
extern BuiltInTypeId gUnsignedLongInt;
extern BuiltInTypeId gSignedLongLongInt;
extern BuiltInTypeId gUnsignedLongLongInt;
extern BuiltInTypeId gWCharT;
extern BuiltInTypeId gBool;
extern BuiltInTypeId gFloat;
extern BuiltInTypeId gDouble;
extern BuiltInTypeId gLongDouble;
extern BuiltInTypeId gVoid;

typedef ArrayRange<BuiltInType> BuiltInTypeArrayRange;


typedef UniqueTypeGeneric<false> UserType;

template<bool builtIn, int N>
struct TypeTuple;

template<bool builtIn>
struct TypeTuple<builtIn, 1> : UniqueTypeGeneric<builtIn>
{
	TypeTuple(UniqueTypeGeneric<builtIn> type)
		: UniqueTypeGeneric<builtIn>(type)
	{
	}
};

template<bool builtIn>
struct TypeTuple<builtIn, 2> : std::pair<UniqueTypeGeneric<builtIn>, UniqueTypeGeneric<builtIn> >
{
	typedef UniqueTypeGeneric<builtIn> Value;
	TypeTuple(Value first, Value second)
		: std::pair<Value, Value>(first, second)
	{
	}
};

template<int N>
struct BuiltInGenericType : BuiltInType
{
	typedef UserType (*Substitute)(TypeTuple<false, N> args);
	Substitute substitute;
	BuiltInGenericType(BuiltInType type, Substitute substitute) : BuiltInType(type), substitute(substitute)
	{
	}
};

typedef BuiltInGenericType<1> BuiltInGenericType1;
typedef BuiltInGenericType<2> BuiltInGenericType2;

typedef ArrayRange<BuiltInGenericType1> BuiltInGenericType1ArrayRange;
typedef ArrayRange<BuiltInGenericType2> BuiltInGenericType2ArrayRange;


inline bool isVoid(UniqueTypeWrapper type)
{
	return type.value.getPointer() == gVoid.value.getPointer();
}

inline bool isVoidPointer(UniqueTypeWrapper type)
{
	if(!type.isPointer())
	{
		return false;
	}
	type.pop_front();
	return isVoid(type);
}

inline bool isVoidParameter(const TypeId& type)
{
	return type.declaration == &gVoidDeclaration
		&& type.typeSequence.empty();
}


inline unsigned combineFundamental(unsigned fundamental, unsigned token)
{
	unsigned mask = 1 << token;
	if((fundamental & mask) != 0)
	{
		mask <<= 16;
	}
	return fundamental | mask;
}

#define MAKE_FUNDAMENTAL(token) (1 << cpp::simple_type_specifier_builtin::token)
#define MAKE_FUNDAMENTAL2(token) (MAKE_FUNDAMENTAL(token) << 16)

inline Declaration* getFundamentalType(unsigned fundamental)
{
	switch(fundamental)
	{
	case MAKE_FUNDAMENTAL(CHAR): return &gCharDeclaration;
	case MAKE_FUNDAMENTAL(SIGNED) | MAKE_FUNDAMENTAL(CHAR): return &gSignedCharDeclaration;
	case MAKE_FUNDAMENTAL(UNSIGNED) | MAKE_FUNDAMENTAL(CHAR): return &gUnsignedCharDeclaration;
	case MAKE_FUNDAMENTAL(SHORT):
	case MAKE_FUNDAMENTAL(SHORT) | MAKE_FUNDAMENTAL(INT):
	case MAKE_FUNDAMENTAL(SIGNED) | MAKE_FUNDAMENTAL(SHORT):
	case MAKE_FUNDAMENTAL(SIGNED) | MAKE_FUNDAMENTAL(SHORT) | MAKE_FUNDAMENTAL(INT): return &gSignedShortIntDeclaration;
	case MAKE_FUNDAMENTAL(UNSIGNED) | MAKE_FUNDAMENTAL(SHORT):
	case MAKE_FUNDAMENTAL(UNSIGNED) | MAKE_FUNDAMENTAL(SHORT) | MAKE_FUNDAMENTAL(INT): return &gUnsignedShortIntDeclaration;
	case MAKE_FUNDAMENTAL(INT):
	case MAKE_FUNDAMENTAL(SIGNED):
	case MAKE_FUNDAMENTAL(SIGNED) | MAKE_FUNDAMENTAL(INT): return &gSignedIntDeclaration;
	case MAKE_FUNDAMENTAL(UNSIGNED):
	case MAKE_FUNDAMENTAL(UNSIGNED) | MAKE_FUNDAMENTAL(INT): return &gUnsignedIntDeclaration;
	case MAKE_FUNDAMENTAL(LONG):
	case MAKE_FUNDAMENTAL(LONG) | MAKE_FUNDAMENTAL(INT):
	case MAKE_FUNDAMENTAL(SIGNED) | MAKE_FUNDAMENTAL(LONG):
	case MAKE_FUNDAMENTAL(SIGNED) | MAKE_FUNDAMENTAL(LONG) | MAKE_FUNDAMENTAL(INT): return &gSignedLongIntDeclaration;
	case MAKE_FUNDAMENTAL(UNSIGNED) | MAKE_FUNDAMENTAL(LONG):
	case MAKE_FUNDAMENTAL(UNSIGNED) | MAKE_FUNDAMENTAL(LONG) | MAKE_FUNDAMENTAL(INT): return &gUnsignedLongIntDeclaration;
	case MAKE_FUNDAMENTAL(LONG) | MAKE_FUNDAMENTAL2(LONG):
	case MAKE_FUNDAMENTAL(LONG) | MAKE_FUNDAMENTAL2(LONG) | MAKE_FUNDAMENTAL(INT):
	case MAKE_FUNDAMENTAL(SIGNED) | MAKE_FUNDAMENTAL(LONG) | MAKE_FUNDAMENTAL2(LONG):
	case MAKE_FUNDAMENTAL(SIGNED) | MAKE_FUNDAMENTAL(LONG) | MAKE_FUNDAMENTAL2(LONG) | MAKE_FUNDAMENTAL(INT): return &gSignedLongLongIntDeclaration;
	case MAKE_FUNDAMENTAL(UNSIGNED) | MAKE_FUNDAMENTAL(LONG) | MAKE_FUNDAMENTAL2(LONG):
	case MAKE_FUNDAMENTAL(UNSIGNED) | MAKE_FUNDAMENTAL(LONG) | MAKE_FUNDAMENTAL2(LONG) | MAKE_FUNDAMENTAL(INT): return &gUnsignedLongLongIntDeclaration;
	case MAKE_FUNDAMENTAL(WCHAR_T): return &gWCharTDeclaration;
	case MAKE_FUNDAMENTAL(BOOL): return &gBoolDeclaration;
	case MAKE_FUNDAMENTAL(FLOAT): return &gFloatDeclaration;
	case MAKE_FUNDAMENTAL(DOUBLE): return &gDoubleDeclaration;
	case MAKE_FUNDAMENTAL(LONG) | MAKE_FUNDAMENTAL(DOUBLE): return &gLongDoubleDeclaration;
	case MAKE_FUNDAMENTAL(VOID): return &gVoidDeclaration;
	case MAKE_FUNDAMENTAL(INT64):
	case MAKE_FUNDAMENTAL(SIGNED) | MAKE_FUNDAMENTAL(INT64): return &gSignedLongLongIntDeclaration;
	case MAKE_FUNDAMENTAL(UNSIGNED) | MAKE_FUNDAMENTAL(INT64): return &gUnsignedLongLongIntDeclaration;
	}
	SYMBOLS_ASSERT(false);
	return 0;
}

#define MAKE_INTEGERLITERALSUFFIX(token) (1 << cpp::simple_type_specifier_builtin::token)



inline bool isCStyle(const Instance& classInstance)
{
	return classInstance.declaration->isCStyle;
}

inline bool isUnion(const Instance& classInstance)
{
	return classInstance.declaration->isUnion;
}

inline bool isClass(const Instance& classInstance)
{
	return isClass(*classInstance.declaration);
}

inline bool isClass(UniqueTypeWrapper type)
{
	return type.isSimple() && isClass(getInstance(type.value));
}

inline bool isEnum(const Instance& classInstance)
{
	return isEnum(*classInstance.declaration);
}

inline bool isEnum(UniqueTypeWrapper type)
{
	return type.isSimple() && isEnum(getInstance(type.value));
}

inline bool isObject(UniqueTypeWrapper type)
{
	return type != gVoid && !type.isReference() && !type.isFunction();
}

inline bool isComplete(const Instance& classInstance)
{
	return isComplete(*classInstance.declaration);
}

inline bool isComplete(const UniqueTypeId& type)
{
	return type.isSimple() && isComplete(getInstance(type.value));
}

inline bool isArithmetic(const Instance& classInstance)
{
	return isArithmetic(*classInstance.declaration);
}

inline bool isArithmetic(const UniqueTypeId& type)
{
	return type.isSimple() && isArithmetic(getInstance(type.value));
}

inline bool isFloating(const UniqueTypeId& type)
{
	return isEqual(type, gFloat)
		|| isEqual(type, gDouble)
		|| isEqual(type, gLongDouble);
}

inline bool isIntegral(const UniqueTypeId& type)
{
	return isArithmetic(type) && !isFloating(type);
}

inline bool isPromotedIntegral(UniqueTypeWrapper type)
{
	return type == gSignedInt
		|| type == gUnsignedInt
		|| type == gSignedLongInt
		|| type == gUnsignedLongInt
		|| type == gSignedLongLongInt
		|| type == gUnsignedLongLongInt;
}

inline bool isPromotedArithmetic(UniqueTypeWrapper type)
{
	return isPromotedIntegral(type) || isFloating(type);
}


inline bool isEnumeration(const UniqueTypeId& type)
{
	return isEnum(type);
}


#endif
