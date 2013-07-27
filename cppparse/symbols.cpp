
#include "symbols.h"

const TypeElementEmpty gTypeElementEmpty;

UniqueNames gUniqueNames;
UniqueTypes gUniqueTypes;
UniqueTypes gBuiltInTypes;
UniqueExpressions gBuiltInExpressions;
UniqueExpressions gUniqueExpressions;

size_t gScopeCount = 0;

// special-case
Identifier gUndeclaredId = makeIdentifier("$undeclared");
Declaration gUndeclared(TREEALLOCATOR_NULL, 0, gUndeclaredId, TYPE_NULL, 0);
const DeclarationInstance gUndeclaredInstance(&gUndeclared);

// meta types
Identifier gArithmeticId = makeIdentifier("$arithmetic");
Declaration gArithmetic(TREEALLOCATOR_NULL, 0, gArithmeticId, TYPE_NULL, 0);
Identifier gSpecialId = makeIdentifier("$special");
Declaration gSpecial(TREEALLOCATOR_NULL, 0, gSpecialId, TYPE_NULL, 0);
Identifier gClassId = makeIdentifier("$class");
Declaration gClass(TREEALLOCATOR_NULL, 0, gClassId, TYPE_NULL, 0);
Identifier gEnumId = makeIdentifier("$enum");
Declaration gEnum(TREEALLOCATOR_NULL, 0, gEnumId, TYPE_NULL, 0);

// types
Identifier gNamespaceId = makeIdentifier("$namespace");
Declaration gNamespace(TREEALLOCATOR_NULL, 0, gNamespaceId, TYPE_NULL, 0);

Identifier gCtorId = makeIdentifier("$ctor");
Declaration gCtor(TREEALLOCATOR_NULL, 0, gCtorId, TYPE_SPECIAL, 0);
Identifier gEnumeratorId = makeIdentifier("$enumerator");
Declaration gEnumerator(TREEALLOCATOR_NULL, 0, gEnumeratorId, TYPE_SPECIAL, 0);
Identifier gUnknownId = makeIdentifier("$unknown");
Declaration gUnknown(TREEALLOCATOR_NULL, 0, gUnknownId, TYPE_SPECIAL, 0);

// fundamental types
Identifier gCharId = makeIdentifier("char");
BuiltInTypeDeclaration gCharDeclaration(gCharId);
ObjectTypeId gChar(&gCharDeclaration, TREEALLOCATOR_NULL);
Identifier gSignedCharId = makeIdentifier("signed char");
BuiltInTypeDeclaration gSignedCharDeclaration(gSignedCharId);
ObjectTypeId gSignedChar(&gSignedCharDeclaration, TREEALLOCATOR_NULL);
Identifier gUnsignedCharId = makeIdentifier("unsigned char");
BuiltInTypeDeclaration gUnsignedCharDeclaration(gUnsignedCharId);
ObjectTypeId gUnsignedChar(&gUnsignedCharDeclaration, TREEALLOCATOR_NULL);
Identifier gSignedShortIntId = makeIdentifier("short");
BuiltInTypeDeclaration gSignedShortIntDeclaration(gSignedShortIntId);
ObjectTypeId gSignedShortInt(&gSignedShortIntDeclaration, TREEALLOCATOR_NULL);
Identifier gUnsignedShortIntId = makeIdentifier("unsigned short");
BuiltInTypeDeclaration gUnsignedShortIntDeclaration(gUnsignedShortIntId);
ObjectTypeId gUnsignedShortInt(&gUnsignedShortIntDeclaration, TREEALLOCATOR_NULL);
Identifier gSignedIntId = makeIdentifier("int");
BuiltInTypeDeclaration gSignedIntDeclaration(gSignedIntId);
ObjectTypeId gSignedInt(&gSignedIntDeclaration, TREEALLOCATOR_NULL);
Identifier gUnsignedIntId = makeIdentifier("unsigned int");
BuiltInTypeDeclaration gUnsignedIntDeclaration(gUnsignedIntId);
ObjectTypeId gUnsignedInt(&gUnsignedIntDeclaration, TREEALLOCATOR_NULL);
Identifier gSignedLongIntId = makeIdentifier("long");
BuiltInTypeDeclaration gSignedLongIntDeclaration(gSignedLongIntId);
ObjectTypeId gSignedLongInt(&gSignedLongIntDeclaration, TREEALLOCATOR_NULL);
Identifier gUnsignedLongIntId = makeIdentifier("unsigned long");
BuiltInTypeDeclaration gUnsignedLongIntDeclaration(gUnsignedLongIntId);
ObjectTypeId gUnsignedLongInt(&gUnsignedLongIntDeclaration, TREEALLOCATOR_NULL);
Identifier gSignedLongLongIntId = makeIdentifier("long long");
BuiltInTypeDeclaration gSignedLongLongIntDeclaration(gSignedLongLongIntId);
ObjectTypeId gSignedLongLongInt(&gSignedLongLongIntDeclaration, TREEALLOCATOR_NULL);
Identifier gUnsignedLongLongIntId = makeIdentifier("unsigned long long");
BuiltInTypeDeclaration gUnsignedLongLongIntDeclaration(gUnsignedLongLongIntId);
ObjectTypeId gUnsignedLongLongInt(&gUnsignedLongLongIntDeclaration, TREEALLOCATOR_NULL);
Identifier gWCharId = makeIdentifier("wchar_t");
BuiltInTypeDeclaration gWCharTDeclaration(gWCharId);
ObjectTypeId gWCharT(&gWCharTDeclaration, TREEALLOCATOR_NULL);
Identifier gBoolId = makeIdentifier("bool");
BuiltInTypeDeclaration gBoolDeclaration(gBoolId);
ObjectTypeId gBool(&gBoolDeclaration, TREEALLOCATOR_NULL);
Identifier gFloatId = makeIdentifier("float");
BuiltInTypeDeclaration gFloatDeclaration(gFloatId);
ObjectTypeId gFloat(&gFloatDeclaration, TREEALLOCATOR_NULL);
Identifier gDoubleId = makeIdentifier("double");
BuiltInTypeDeclaration gDoubleDeclaration(gDoubleId);
ObjectTypeId gDouble(&gDoubleDeclaration, TREEALLOCATOR_NULL);
Identifier gLongDoubleId = makeIdentifier("long double");
BuiltInTypeDeclaration gLongDoubleDeclaration(gLongDoubleId);
ObjectTypeId gLongDouble(&gLongDoubleDeclaration, TREEALLOCATOR_NULL);
Identifier gVoidId = makeIdentifier("void");
BuiltInTypeDeclaration gVoidDeclaration(gVoidId, TYPE_SPECIAL);
ObjectTypeId gVoid(&gVoidDeclaration, TREEALLOCATOR_NULL);

StringLiteralTypeId gStringLiteral(&gCharDeclaration, TREEALLOCATOR_NULL);
StringLiteralTypeId gWideStringLiteral(&gWCharTDeclaration, TREEALLOCATOR_NULL);

// built-in operator placeholders
Identifier gAnyTypePlaceholderId = makeIdentifier("$any-placeholder");
BuiltInTypeDeclaration gAnyTypePlaceholderDeclaration(gAnyTypePlaceholderId, TYPE_UNKNOWN);
ObjectTypeId gAnyTypePlaceholder(&gAnyTypePlaceholderDeclaration, TREEALLOCATOR_NULL);
Identifier gArithmeticPlaceholderId = makeIdentifier("$arithmetic-placeholder");
BuiltInTypeDeclaration gArithmeticPlaceholderDeclaration(gArithmeticPlaceholderId, TYPE_UNKNOWN);
ObjectTypeId gArithmeticPlaceholder(&gArithmeticPlaceholderDeclaration, TREEALLOCATOR_NULL);
Identifier gIntegralPlaceholderId = makeIdentifier("$integral-placeholder");
BuiltInTypeDeclaration gIntegralPlaceholderDeclaration(gIntegralPlaceholderId, TYPE_UNKNOWN);
ObjectTypeId gIntegralPlaceholder(&gIntegralPlaceholderDeclaration, TREEALLOCATOR_NULL);
Identifier gEnumerationPlaceholderId = makeIdentifier("$enumeration-placeholder");
BuiltInTypeDeclaration gEnumerationPlaceholderDeclaration(gEnumerationPlaceholderId, TYPE_UNKNOWN);
ObjectTypeId gEnumerationPlaceholder(&gEnumerationPlaceholderDeclaration, TREEALLOCATOR_NULL);
Identifier gPromotedArithmeticPlaceholderId = makeIdentifier("$promoted-arithmetic-placeholder");
BuiltInTypeDeclaration gPromotedArithmeticPlaceholderDeclaration(gPromotedArithmeticPlaceholderId, TYPE_UNKNOWN);
ObjectTypeId gPromotedArithmeticPlaceholder(&gPromotedArithmeticPlaceholderDeclaration, TREEALLOCATOR_NULL);
Identifier gPromotedIntegralPlaceholderId = makeIdentifier("$promoted-integral-placeholder");
BuiltInTypeDeclaration gPromotedIntegralPlaceholderDeclaration(gPromotedIntegralPlaceholderId, TYPE_UNKNOWN);
ObjectTypeId gPromotedIntegralPlaceholder(&gPromotedIntegralPlaceholderDeclaration, TREEALLOCATOR_NULL);
Identifier gObjectTypePlaceholderId = makeIdentifier("$object-type-placeholder");
BuiltInTypeDeclaration gObjectTypePlaceholderDeclaration(gObjectTypePlaceholderId, TYPE_UNKNOWN);
ObjectTypeId gObjectTypePlaceholder(&gObjectTypePlaceholderDeclaration, TREEALLOCATOR_NULL);
Identifier gClassTypePlaceholderId = makeIdentifier("$class-type-placeholder");
BuiltInTypeDeclaration gClassTypePlaceholderDeclaration(gClassTypePlaceholderId, TYPE_UNKNOWN);
ObjectTypeId gClassTypePlaceholder(&gClassTypePlaceholderDeclaration, TREEALLOCATOR_NULL);
Identifier gFunctionTypePlaceholderId = makeIdentifier("$function-type-placeholder");
BuiltInTypeDeclaration gFunctionTypePlaceholderDeclaration(gFunctionTypePlaceholderId, TYPE_UNKNOWN);
ObjectTypeId gFunctionTypePlaceholder(&gFunctionTypePlaceholderDeclaration, TREEALLOCATOR_NULL);
Identifier gMemberPointerPlaceholderId = makeIdentifier("$member-pointer-placeholder");
BuiltInTypeDeclaration gMemberPointerPlaceholderDeclaration(gMemberPointerPlaceholderId, TYPE_UNKNOWN);
ObjectTypeId gMemberPointerPlaceholder(&gMemberPointerPlaceholderDeclaration, TREEALLOCATOR_NULL);


// template placeholders
Identifier gDependentTypeId = makeIdentifier("$type");
Declaration gDependentType(TREEALLOCATOR_NULL, 0, gDependentTypeId, TYPE_SPECIAL, 0);
const DeclarationInstance gDependentTypeInstance(&gDependentType);
Identifier gDependentObjectId = makeIdentifier("$object");
Declaration gDependentObject(TREEALLOCATOR_NULL, 0, gDependentObjectId, TYPE_UNKNOWN, 0);
const DeclarationInstance gDependentObjectInstance(&gDependentObject);
Identifier gDependentTemplateId = makeIdentifier("$template");
Declaration gDependentTemplate(TREEALLOCATOR_NULL, 0, gDependentTemplateId, TYPE_SPECIAL, 0, DeclSpecifiers(), true);
const DeclarationInstance gDependentTemplateInstance(&gDependentTemplate);
Identifier gDependentNestedId = makeIdentifier("$nested");
Declaration gDependentNested(TREEALLOCATOR_NULL, 0, gDependentNestedId, TYPE_SPECIAL, 0);
const DeclarationInstance gDependentNestedInstance(&gDependentNested);
Identifier gDependentNestedTemplateId = makeIdentifier("$nested-template");
Declaration gDependentNestedTemplate(TREEALLOCATOR_NULL, 0, gDependentNestedTemplateId, TYPE_SPECIAL, 0, DeclSpecifiers(), true);
const DeclarationInstance gDependentNestedTemplateInstance(&gDependentNestedTemplate);

Identifier gParamId = makeIdentifier("$param");
Declaration gParam(TREEALLOCATOR_NULL, 0, gParamId, TYPE_CLASS, 0);
Identifier gNonTypeId = makeIdentifier("$non-type");
Declaration gNonType(TREEALLOCATOR_NULL, 0, gNonTypeId, TYPE_UNKNOWN, 0);

// objects
Name gOperatorNewId = Name("operator new");
Name gOperatorDeleteId = Name("operator delete");
Name gOperatorNewArrayId = Name("operator new[]");
Name gOperatorDeleteArrayId = Name("operator delete[]");
Name gOperatorPlusId = Name("operator+");
Name gOperatorMinusId = Name("operator-");
Name gOperatorStarId = Name("operator*");
Name gOperatorDivideId = Name("operator/");
Name gOperatorPercentId = Name("operator%");
Name gOperatorXorId = Name("operator^");
Name gOperatorAndId = Name("operator&");
Name gOperatorOrId = Name("operator|");
Name gOperatorComplId = Name("operator~");
Name gOperatorNotId = Name("operator!");
Name gOperatorAssignId = Name("operator=");
Name gOperatorLessId = Name("operator<");
Name gOperatorGreaterId = Name("operator>");
Name gOperatorPlusAssignId = Name("operator+=");
Name gOperatorMinusAssignId = Name("operator-=");
Name gOperatorStarAssignId = Name("operator*=");
Name gOperatorDivideAssignId = Name("operator/=");
Name gOperatorPercentAssignId = Name("operator%=");
Name gOperatorXorAssignId = Name("operator^=");
Name gOperatorAndAssignId = Name("operator&=");
Name gOperatorOrAssignId = Name("operator|=");
Name gOperatorShiftLeftId = Name("operator<<");
Name gOperatorShiftRightId = Name("operator>>");
Name gOperatorShiftRightAssignId = Name("operator>>=");
Name gOperatorShiftLeftAssignId = Name("operator<<=");
Name gOperatorEqualId = Name("operator==");
Name gOperatorNotEqualId = Name("operator!=");
Name gOperatorLessEqualId = Name("operator<=");
Name gOperatorGreaterEqualId = Name("operator>=");
Name gOperatorAndAndId = Name("operator&&");
Name gOperatorOrOrId = Name("operator||");
Name gOperatorPlusPlusId = Name("operator++");
Name gOperatorMinusMinusId = Name("operator--");
Name gOperatorCommaId = Name("operator,");
Name gOperatorArrowStarId = Name("operator->*");
Name gOperatorArrowId = Name("operator->");
Name gOperatorFunctionId = Name("operator()");
Name gOperatorSubscriptId = Name("operator[]");

Identifier gConversionFunctionId = makeIdentifier("operator T");
Identifier gOperatorFunctionTemplateId = makeIdentifier("operator () <>");
// TODO: don't declare if id is anonymous?
Identifier gAnonymousId = makeIdentifier("$anonymous");

Identifier gDestructorId = makeIdentifier("$destructor");

const TypeInstance gDependentObjectType(&gDependentType, 0);

struct PointerTypeId : ObjectTypeId
{
	PointerTypeId(Declaration* declaration, const TreeAllocator<int>& allocator, CvQualifiers qualifiers = CvQualifiers())
		: ObjectTypeId(declaration, allocator)
	{
		value.setQualifiers(qualifiers);
		value = pushBuiltInType(value, PointerType());
	}
};

struct PointerPointerTypeId : ObjectTypeId
{
	PointerPointerTypeId(Declaration* declaration, const TreeAllocator<int>& allocator, CvQualifiers qualifiers = CvQualifiers())
		: ObjectTypeId(declaration, allocator)
	{
		value.setQualifiers(qualifiers);
		value = pushBuiltInType(value, PointerType());
		value = pushBuiltInType(value, PointerType());
	}
};

PointerTypeId gVoidPointer(&gVoidDeclaration, TREEALLOCATOR_NULL);
PointerTypeId gConstVoidPointer(&gVoidDeclaration, TREEALLOCATOR_NULL, CvQualifiers(true, false));
PointerTypeId gSignedIntPointer(&gSignedIntDeclaration, TREEALLOCATOR_NULL);
PointerTypeId gSignedCharPointer(&gSignedCharDeclaration, TREEALLOCATOR_NULL);
PointerTypeId gWCharTPointer(&gWCharTDeclaration, TREEALLOCATOR_NULL);
PointerPointerTypeId gSignedIntPointerPointer(&gSignedIntDeclaration, TREEALLOCATOR_NULL);


Identifier gBaseClassId = makeIdentifier("$base");
Scope gBaseClassScope(TREEALLOCATOR_NULL, gBaseClassId, SCOPETYPE_CLASS);
Declaration gBaseClassDeclaration(TREEALLOCATOR_NULL, 0, gBaseClassId, TYPE_CLASS, &gBaseClassScope);
ObjectTypeId gBaseClass(&gBaseClassDeclaration, TREEALLOCATOR_NULL);

Types2::Pointer::Value gBaseClassNode = Types2::Node(Type(&gBaseClassDeclaration, TREEALLOCATOR_NULL));

struct DerivedClassTypeId : ObjectTypeId
{
	DerivedClassTypeId(Declaration* declaration, const TreeAllocator<int>& allocator)
		: ObjectTypeId(declaration, allocator)
	{
		gBaseClassNode.value.id =  &gBaseClassId;
		declaration->enclosed->bases.head.next = declaration->enclosed->bases.tail = &gBaseClassNode;
		declaration->enclosed->bases.back().unique = gBaseClass.value;
	}
};

Identifier gDerivedClassId = makeIdentifier("$derived");
Scope gDerivedClassScope(TREEALLOCATOR_NULL, gDerivedClassId, SCOPETYPE_CLASS);
Declaration gDerivedClassDeclaration(TREEALLOCATOR_NULL, 0, gDerivedClassId, TYPE_CLASS, &gDerivedClassScope);
DerivedClassTypeId gDerivedClass(&gDerivedClassDeclaration, TREEALLOCATOR_NULL);

PointerTypeId gBaseClassPointer(&gBaseClassDeclaration, TREEALLOCATOR_NULL);
PointerTypeId gDerivedClassPointer(&gDerivedClassDeclaration, TREEALLOCATOR_NULL);


struct DependentTypeId : UniqueTypeId
{
	DependentTypeId(Declaration* declaration, std::size_t index)
	{
		declaration->templateParameter = index;
		value = pushBuiltInType(value, DependentType(declaration));
	}
};

Scope gTemplateParameterScope(TREEALLOCATOR_NULL,  makeIdentifier("$template"), SCOPETYPE_TEMPLATE);

Identifier gTemplateParameterId = makeIdentifier("T");
Declaration gTemplateParameterDeclaration(TREEALLOCATOR_NULL, &gTemplateParameterScope, gTemplateParameterId, TYPE_PARAM, 0);
DependentTypeId gTemplateParameter(&gTemplateParameterDeclaration, 0);

Identifier gNonTypeTemplateParameterId = makeIdentifier("i");
Declaration gNonTypeTemplateParameterDeclaration(TREEALLOCATOR_NULL, &gTemplateParameterScope, gNonTypeTemplateParameterId, TYPE_PARAM, 0);
DependentTypeId gNonTypeTemplateParameter(&gNonTypeTemplateParameterDeclaration, 1);


Types2::Pointer::Value gTemplateParam1 = Types2::Node(Type(&gTemplateParameterDeclaration, TREEALLOCATOR_NULL));
Types2::Pointer::Value gTemplateParam2 = Types2::Node(Type(&gNonTypeTemplateParameterDeclaration, TREEALLOCATOR_NULL));

Identifier gTemplateClassId = makeIdentifier("$template");
struct TemplateClassDeclaration : Declaration
{
	TemplateClassDeclaration() : Declaration(TREEALLOCATOR_NULL, 0, gTemplateClassId, TYPE_CLASS, 0, DeclSpecifiers(), true)
	{
		templateParamScope = &gTemplateParameterScope;
		templateParams.head.next = &gTemplateParam1;
		gTemplateParam1.next = templateParams.tail = &gTemplateParam2;
	}
};

TemplateClassDeclaration gTemplateClassDeclaration;


Identifier gTemplateTemplateParameterId = makeIdentifier("TT");
Declaration gTemplateTemplateParameterDeclaration(TREEALLOCATOR_NULL, &gTemplateParameterScope, gTemplateTemplateParameterId, TYPE_CLASS, 0, DeclSpecifiers(), true, TEMPLATEPARAMETERS_NULL, false, TEMPLATEARGUMENTS_NULL, 0);

struct BuiltInTemplateTemplateArgument : UniqueTypeId
{
	BuiltInTemplateTemplateArgument(Declaration* declaration)
	{
		value = pushBuiltInType(value, TemplateTemplateArgument(declaration, 0));
	}
};

BuiltInTemplateTemplateArgument gTemplateTemplateArgument(&gTemplateClassDeclaration);

struct BuiltInNonType : UniqueTypeId
{
	BuiltInNonType(IntegralConstant constant)
	{
		value = pushBuiltInType(value, NonType(constant));
	}
};

BuiltInNonType gZero(IntegralConstant(0));
BuiltInNonType gOne(IntegralConstant(1));

struct BuiltInDependentNonType : UniqueTypeId
{
	BuiltInDependentNonType(DeclarationPtr declaration)
	{
		value = pushBuiltInType(value, DependentNonType(makeBuiltInExpression(NonTypeTemplateParameter(declaration))));
	}
};

BuiltInDependentNonType gDependentNonType(&gNonTypeTemplateParameterDeclaration);

struct Base
{
};
struct Derived : Base
{
};

class Any
{
};

class Object
{
};

class Class
{
};

class Function
{
};

class MemberPointer
{
};

class Enumeration
{
};

class Arithmetic
{
};

class Integral
{
};

class PromotedArithmetic
{
};

class PromotedIntegral
{
};

template<typename T, int i>
struct Template
{
};

const int NONTYPE_PARAM = INT_MAX;

struct T
{
};

template<typename T, int i>
struct TT
{
};

template<typename T>
struct MakeType
{
private:
	static UniqueTypeWrapper apply();
};

template<>
struct MakeType<bool>
{
	static UniqueTypeWrapper apply()
	{
		return gBool;
	}
};

template<>
struct MakeType<int>
{
	static UniqueTypeWrapper apply()
	{
		return gSignedInt;
	}
};

template<>
struct MakeType<float>
{
	static UniqueTypeWrapper apply()
	{
		return gFloat;
	}
};

template<>
struct MakeType<char>
{
	static UniqueTypeWrapper apply()
	{
		return gChar;
	}
};

template<>
struct MakeType<wchar_t>
{
	static UniqueTypeWrapper apply()
	{
		return gWCharT;
	}
};

template<>
struct MakeType<void>
{
	static UniqueTypeWrapper apply()
	{
		return gVoid;
	}
};

template<>
struct MakeType<Base>
{
	static UniqueTypeWrapper apply()
	{
		return gBaseClass;
	}
};

template<>
struct MakeType<Derived>
{
	static UniqueTypeWrapper apply()
	{
		return gDerivedClass;
	}
};

template<>
struct MakeType<Any>
{
	static UniqueTypeWrapper apply()
	{
		return gAnyTypePlaceholder;
	}
};

template<>
struct MakeType<Object>
{
	static UniqueTypeWrapper apply()
	{
		return gObjectTypePlaceholder;
	}
};

template<>
struct MakeType<Class>
{
	static UniqueTypeWrapper apply()
	{
		return gClassTypePlaceholder;
	}
};

template<>
struct MakeType<Function>
{
	static UniqueTypeWrapper apply()
	{
		return gFunctionTypePlaceholder;
	}
};

template<>
struct MakeType<MemberPointer>
{
	static UniqueTypeWrapper apply()
	{
		return gMemberPointerPlaceholder;
	}
};

template<>
struct MakeType<Enumeration>
{
	static UniqueTypeWrapper apply()
	{
		return gEnumerationPlaceholder;
	}
};

template<>
struct MakeType<Arithmetic>
{
	static UniqueTypeWrapper apply()
	{
		return gArithmeticPlaceholder;
	}
};

template<>
struct MakeType<Integral>
{
	static UniqueTypeWrapper apply()
	{
		return gIntegralPlaceholder;
	}
};

template<>
struct MakeType<PromotedArithmetic>
{
	static UniqueTypeWrapper apply()
	{
		return gPromotedArithmeticPlaceholder;
	}
};

template<>
struct MakeType<PromotedIntegral>
{
	static UniqueTypeWrapper apply()
	{
		return gPromotedIntegralPlaceholder;
	}
};

struct MakeTemplate
{
	static UniqueTypeWrapper apply(Declaration* declaration, UniqueTypeWrapper a1, UniqueTypeWrapper a2)
	{
		ObjectType result(TypeInstance(declaration, 0));
		result.type.templateArguments.push_back(a1);
		result.type.templateArguments.push_back(a2);
		return UniqueTypeWrapper(pushBuiltInType(UNIQUETYPE_NULL, result));
	}
};

template<typename T, int i>
struct MakeType< Template<T, i> >
{
	static UniqueTypeWrapper apply()
	{
		return MakeTemplate::apply(&gTemplateClassDeclaration, MakeType<T>::apply(), BuiltInNonType(IntegralConstant(i)));
	}
};


template<typename T>
struct MakeType< Template<T, NONTYPE_PARAM> >
{
	static UniqueTypeWrapper apply()
	{
		return MakeTemplate::apply(&gTemplateClassDeclaration,  MakeType<T>::apply(), gDependentNonType);
	}
};

template<>
struct MakeType<T>
{
	static UniqueTypeWrapper apply()
	{
		return gTemplateParameter;
	}
};

struct MakeTemplateTemplateParameter
{
	static UniqueTypeWrapper apply(Declaration* declaration, UniqueTypeWrapper a1, UniqueTypeWrapper a2)
	{
		TemplateArgumentsInstance templateArguments;
		templateArguments.push_back(a1);
		templateArguments.push_back(a2);
		DependentType result(declaration, templateArguments, 2);
		return UniqueTypeWrapper(pushBuiltInType(UNIQUETYPE_NULL, result));
	}
};

template<typename T, int i>
struct MakeType< TT<T, i> >
{
	static UniqueTypeWrapper apply()
	{
		return MakeTemplateTemplateParameter::apply(&gTemplateTemplateParameterDeclaration, MakeType<T>::apply(), BuiltInNonType(IntegralConstant(i)));
	}
};

template<typename T>
struct MakeType< TT<T, NONTYPE_PARAM> >
{
	static UniqueTypeWrapper apply()
	{
		return MakeTemplateTemplateParameter::apply(&gTemplateTemplateParameterDeclaration, MakeType<T>::apply(), gDependentNonType);
	}
};

struct MakeConst
{
	static UniqueTypeWrapper apply(UniqueTypeWrapper inner)
	{
		inner.value.setQualifiers(CvQualifiers(true, false));
		return inner;
	}
};

template<typename T>
struct MakeType<const T>
{
	static UniqueTypeWrapper apply()
	{
		return MakeConst::apply(MakeType<T>::apply());
	}
};

struct MakeVolatile
{
	static UniqueTypeWrapper apply(UniqueTypeWrapper inner)
	{
		inner.value.setQualifiers(CvQualifiers(false, true));
		return inner;
	}
};

template<typename T>
struct MakeType<volatile T>
{
	static UniqueTypeWrapper apply()
	{
		return MakeVolatile::apply(MakeType<T>::apply());
	}
};

struct MakeConstVolatile
{
	static UniqueTypeWrapper apply(UniqueTypeWrapper inner)
	{
		inner.value.setQualifiers(CvQualifiers(true, true));
		return inner;
	}
};

template<typename T>
struct MakeType<const volatile T>
{
	static UniqueTypeWrapper apply()
	{
		return MakeConstVolatile::apply(MakeType<T>::apply());
	}
};

template<typename T>
struct MakeType<T*>
{
	static UniqueTypeWrapper apply()
	{
		return UniqueTypeWrapper(pushBuiltInType(MakeType<T>::apply().value, PointerType()));
	}
};

template<typename T>
struct MakeType<T&>
{
	static UniqueTypeWrapper apply()
	{
		return UniqueTypeWrapper(pushBuiltInType(MakeType<T>::apply().value, ReferenceType()));
	}
};

template<std::size_t size>
struct MakeArray
{
	static UniqueTypeWrapper apply(UniqueTypeWrapper inner)
	{
		return UniqueTypeWrapper(pushBuiltInType(inner.value, ArrayType(size)));
	}
};

template<typename T>
struct MakeType<T[]>
{
	static UniqueTypeWrapper apply()
	{
		return MakeArray<0>::apply(MakeType<T>::apply());
	}
};

template<typename T, std::size_t size>
struct MakeType<T[size]>
{
	static UniqueTypeWrapper apply()
	{
		return MakeArray<size>::apply(MakeType<T>::apply());
	}
};

struct MakeFunction
{
	static UniqueTypeWrapper apply(UniqueTypeWrapper inner)
	{
		return UniqueTypeWrapper(pushBuiltInType(inner.value, FunctionType()));
	}
};

template<typename A1>
struct MakeFunction1
{
	static UniqueTypeWrapper apply(UniqueTypeWrapper inner)
	{
		FunctionType function;
		function.parameterTypes.push_back(MakeType<A1>::apply());
		return UniqueTypeWrapper(pushBuiltInType(inner.value, function));
	}
};

template<typename T>
struct MakeType<T()>
{
	static UniqueTypeWrapper apply()
	{
		return MakeFunction::apply(MakeType<T>::apply());
	}
};

template<typename T, typename A1>
struct MakeType<T(A1)>
{
	static UniqueTypeWrapper apply()
	{
		return MakeFunction1<A1>::apply(MakeType<T>::apply());
	}
};

template<typename C>
struct MakeMemberPointer
{
	static UniqueTypeWrapper apply(UniqueTypeWrapper inner)
	{
		return UniqueTypeWrapper(pushBuiltInType(inner.value, MemberPointerType(MakeType<C>::apply())));
	}
};

template<typename T, typename C>
struct MakeType<T C::*>
{
	static UniqueTypeWrapper apply()
	{
		return MakeMemberPointer<C>::apply(MakeType<T>::apply());
	}
};

template<typename T, typename C>
struct MakeType<T (C::*)()>
{
	static UniqueTypeWrapper apply()
	{
		return MakeMemberPointer<C>::apply(MakeFunction::apply(MakeType<T>::apply()));
	}
};

template<typename T, typename C, typename A1>
struct MakeType<T (C::*)(A1)>
{
	static UniqueTypeWrapper apply()
	{
		return MakeMemberPointer<C>::apply(MakeFunction1<A1>::apply(MakeType<T>::apply()));
	}
};

template<typename T, typename C>
struct MakeType<T (C::*)() const>
{
	static UniqueTypeWrapper apply()
	{
		return MakeMemberPointer<C>::apply(MakeConst::apply(MakeFunction::apply(MakeType<T>::apply())));
	}
};

template<typename T, typename C, typename A1>
struct MakeType<T (C::*)(A1) const>
{
	static UniqueTypeWrapper apply()
	{
		return MakeMemberPointer<C>::apply(MakeConst::apply(MakeFunction1<A1>::apply(MakeType<T>::apply())));
	}
};

inline void testTypeGen()
{
	typedef const int ConstInt;
	typedef ConstInt* PtrToConstInt;
	typedef int* PtrToInt;
	typedef const PtrToInt ConstPtrToInt;
	typedef const PtrToConstInt ConstPtrToConstInt;
	UniqueTypeWrapper test1 = MakeType<ConstInt>::apply();
	UniqueTypeWrapper test2 = MakeType<PtrToConstInt>::apply();
	UniqueTypeWrapper test3 = MakeType<ConstPtrToInt>::apply();
	UniqueTypeWrapper test4 = MakeType<ConstPtrToConstInt>::apply();

	UniqueTypeWrapper test5 = MakeType<int (*)()>::apply();
	UniqueTypeWrapper test6 = MakeType<int (Base::*)()>::apply();
	UniqueTypeWrapper test7 = MakeType<int (Base::*)() const>::apply();
	UniqueTypeWrapper test8 = MakeType<int (Base::*const)()>::apply();

	UniqueTypeWrapper test9 = MakeType<int[]>::apply();
	UniqueTypeWrapper test10 = MakeType<int[1]>::apply();

	UniqueTypeWrapper test11 = MakeType<int(int)>::apply();
	UniqueTypeWrapper test12 = MakeType<int (*)(int)>::apply();
	UniqueTypeWrapper test13 = MakeType<int (Base::*)(int)>::apply();
	UniqueTypeWrapper test14 = MakeType<int (Base::*)(int) const>::apply();
	UniqueTypeWrapper test15 = MakeType<int (Base::*const)(int)>::apply();

	UniqueTypeWrapper test16 = MakeType< Template<int, 1> >::apply();

	FileTokenPrinter tokenPrinter(std::cout);
	SymbolPrinter symbolPrinter(tokenPrinter);

	symbolPrinter.printType(test1);
	std::cout << std::endl;
	symbolPrinter.printType(test2);
	std::cout << std::endl;
	symbolPrinter.printType(test3);
	std::cout << std::endl;
	symbolPrinter.printType(test4);
	std::cout << std::endl;
	symbolPrinter.printType(test5);
	std::cout << std::endl;
	symbolPrinter.printType(test6);
	std::cout << std::endl;
	symbolPrinter.printType(test7);
	std::cout << std::endl;
	symbolPrinter.printType(test8);
	std::cout << std::endl;
	symbolPrinter.printType(test9);
	std::cout << std::endl;
	symbolPrinter.printType(test10);
	std::cout << std::endl;
	symbolPrinter.printType(test11);
	std::cout << std::endl;
	symbolPrinter.printType(test12);
	std::cout << std::endl;
	symbolPrinter.printType(test13);
	std::cout << std::endl;
	symbolPrinter.printType(test14);
	std::cout << std::endl;
	symbolPrinter.printType(test15);
	std::cout << std::endl;
	symbolPrinter.printType(test16);
	std::cout << std::endl;
}

struct TypeGenTest
{
	TypeGenTest()
	{
		testTypeGen();
	}
} gTypeGenTest;


UniqueTypeWrapper gSignedIntMemberPointer = MakeType<int Base::*>::apply();


typedef const UniqueTypeId* UniqueTypeIdConstPointer;
UniqueTypeIdConstPointer gArithmeticTypes[] = {
	&gChar,
	&gSignedChar,
	&gUnsignedChar,
	&gSignedShortInt,
	&gUnsignedShortInt,
	&gSignedInt,
	&gUnsignedInt,
	&gSignedLongInt,
	&gUnsignedLongInt,
	&gSignedLongLongInt,
	&gUnsignedLongLongInt,
	&gWCharT,
	&gBool,
	&gFloat,
	&gDouble,
	&gLongDouble,
};

UniqueTypeIdConstPointer gFloatingTypes[] = {
	&gFloat,
	&gDouble,
	&gLongDouble,
};

UniqueTypeIdConstPointer gIntegerTypes[] = {
	&gChar,
	&gSignedChar,
	&gUnsignedChar,
	&gSignedShortInt,
	&gUnsignedShortInt,
	&gSignedInt,
	&gUnsignedInt,
	&gSignedLongInt,
	&gUnsignedLongInt,
	&gSignedLongLongInt,
	&gUnsignedLongLongInt,
	&gWCharT,
	&gBool,
};

UniqueTypeIdConstPointer gPointerTypes[] = {
	&gVoidPointer,
	&gSignedIntPointer,
	&gSignedIntPointerPointer,
	&gSignedCharPointer,
	&gWCharTPointer,
	&gBaseClassPointer,
	&gDerivedClassPointer,
};

UniqueTypeIdConstPointer gSimplePointerTypes[] = {
	&gSignedIntPointer,
	&gSignedCharPointer,
	&gWCharTPointer,
	&gBaseClassPointer,
	&gDerivedClassPointer,
};

UniqueTypeIdConstPointer gMemberPointerTypes[] = {
	&gSignedIntMemberPointer,
};

IcsRank getArithmeticIcsRank(const UniqueTypeId& to, const UniqueTypeId& from)
{
	if(&to == &from)
	{
		return ICSRANK_STANDARDEXACT;
	}
	if(&to == &gSignedInt)
	{
		if(&from == &gChar
			|| &from == &gSignedChar
			|| &from == &gUnsignedChar
			|| &from == &gSignedShortInt
			|| &from == &gUnsignedShortInt
			|| &from == &gWCharT
			|| &from == &gBool)
		{
			return ICSRANK_STANDARDPROMOTION;
		}
	}
	// TODO bitfield -> integer
	if(&to == &gDouble)
	{
		if(&from == &gFloat)
		{
			return ICSRANK_STANDARDPROMOTION;
		}
	}
	return ICSRANK_STANDARDCONVERSION;
}

template<typename To, typename From, typename Matched = void>
struct TestIcsRank
{
	static void apply(IcsRank expected, bool isNullPointerConstant = false, bool isLvalue = false)
	{
		IcsRank rank = getIcsRank(MakeType<To>::apply(), MakeType<From>::apply(), Location(), 0, isNullPointerConstant, isLvalue);
		SYMBOLS_ASSERT(rank == expected);
	}
	static void match(IcsRank expected)
	{
		ImplicitConversion conversion = makeImplicitConversionSequence(TargetType(MakeType<To>::apply()), MakeType<From>::apply(), Location(), 0);
		IcsRank rank = getIcsRank(conversion.sequence.rank);
		SYMBOLS_ASSERT(rank == expected);
		if(expected != ICSRANK_INVALID)
		{
			UniqueTypeWrapper matched = MakeType<Matched>::apply();
			SYMBOLS_ASSERT(conversion.sequence.matched == matched);
		}
	}
};

inline void testIcsRank()
{
	for(const UniqueTypeIdConstPointer* i = gArithmeticTypes; i != ARRAY_END(gArithmeticTypes); ++i)
	{
		UniqueTypeIdConstPointer to = *i;
		for(const UniqueTypeIdConstPointer* i = gArithmeticTypes; i != ARRAY_END(gArithmeticTypes); ++i)
		{
			UniqueTypeIdConstPointer from = *i;
			IcsRank expected = getArithmeticIcsRank(*to, *from);
			IcsRank rank = getIcsRank(*to, *from, Location(), 0);
			SYMBOLS_ASSERT(expected == rank);
		}
	}

	// 0 -> T*
	for(const UniqueTypeIdConstPointer* i = gIntegerTypes; i != ARRAY_END(gIntegerTypes); ++i)
	{
		UniqueTypeIdConstPointer type = *i;
		for(const UniqueTypeIdConstPointer* i = gPointerTypes; i != ARRAY_END(gPointerTypes); ++i)
		{
			UniqueTypeIdConstPointer other = *i;
			{
				IcsRank rank = getIcsRank(*other, *type, Location(), 0, true);
				SYMBOLS_ASSERT(rank == ICSRANK_STANDARDCONVERSION);
			}
			{
				IcsRank rank = getIcsRank(*other, *type, Location(), 0, false);
				SYMBOLS_ASSERT(rank == ICSRANK_INVALID);
			}
		}
	}

	// T* -> bool, T::* -> bool
	for(const UniqueTypeIdConstPointer* i = gIntegerTypes; i != ARRAY_END(gIntegerTypes); ++i)
	{
		UniqueTypeIdConstPointer type = *i;
		for(const UniqueTypeIdConstPointer* i = gPointerTypes; i != ARRAY_END(gPointerTypes); ++i)
		{
			UniqueTypeIdConstPointer other = *i;
			IcsRank rank = getIcsRank(*type, *other, Location(), 0);
			SYMBOLS_ASSERT(rank == (type == &gBool ? ICSRANK_STANDARDCONVERSION : ICSRANK_INVALID));
		}
		for(const UniqueTypeIdConstPointer* i = gMemberPointerTypes; i != ARRAY_END(gMemberPointerTypes); ++i)
		{
			UniqueTypeIdConstPointer other = *i;
			IcsRank rank = getIcsRank(*type, *other, Location(), 0);
			SYMBOLS_ASSERT(rank == (type == &gBool ? ICSRANK_STANDARDCONVERSION : ICSRANK_INVALID));
		}
	}

	// T* -> void* (where T is an object type)
	for(const UniqueTypeIdConstPointer* i = gSimplePointerTypes; i != ARRAY_END(gSimplePointerTypes); ++i)
	{
		UniqueTypeIdConstPointer type = *i;
		{
			IcsRank rank = getIcsRank(gVoidPointer, *type, Location(), 0);
			SYMBOLS_ASSERT(rank == ICSRANK_STANDARDCONVERSION);
		}
		{
			IcsRank rank = getIcsRank(gConstVoidPointer, *type, Location(), 0);
			SYMBOLS_ASSERT(rank == ICSRANK_STANDARDCONVERSION);
		}
	}
	for(const UniqueTypeIdConstPointer* i = gFloatingTypes; i != ARRAY_END(gFloatingTypes); ++i)
	{
		UniqueTypeIdConstPointer type = *i;
		{
			IcsRank rank = getIcsRank(gVoidPointer, *type, Location(), 0, true);
			SYMBOLS_ASSERT(rank == ICSRANK_INVALID);
		}
		{
			IcsRank rank = getIcsRank(*type, gVoidPointer, Location(), 0, true);
			SYMBOLS_ASSERT(rank == ICSRANK_INVALID);
		}
	}

	// T[] -> T*
	TestIcsRank<int*, int[]>::apply(ICSRANK_STANDARDEXACT);
	TestIcsRank<int*, int[1]>::apply(ICSRANK_STANDARDEXACT);
	TestIcsRank<int*, float[1]>::apply(ICSRANK_INVALID);
	TestIcsRank<int**, int[1]>::apply(ICSRANK_INVALID);
	TestIcsRank<int[], int*>::apply(ICSRANK_INVALID);
	TestIcsRank<const int*, int[]>::apply(ICSRANK_STANDARDEXACT);
	TestIcsRank<const int*, const int[]>::apply(ICSRANK_STANDARDEXACT);
	TestIcsRank<int*, const int[]>::apply(ICSRANK_INVALID);

	// T() -> T(*)()
	TestIcsRank<int(*)(), int()>::apply(ICSRANK_STANDARDEXACT);
	TestIcsRank<int(), int(*)()>::apply(ICSRANK_INVALID);

	// D -> B
	TestIcsRank<Base, Derived>::apply(ICSRANK_STANDARDCONVERSION);
	TestIcsRank<Derived, Base>::apply(ICSRANK_INVALID);

	// D* -> B*
	TestIcsRank<Base*, Derived*>::apply(ICSRANK_STANDARDCONVERSION);
	TestIcsRank<Derived*, Base*>::apply(ICSRANK_INVALID);
	TestIcsRank<const Base*, Derived*>::apply(ICSRANK_STANDARDCONVERSION);
	TestIcsRank<const Base*, const Derived*>::apply(ICSRANK_STANDARDCONVERSION);
	TestIcsRank<Base*, const Derived*>::apply(ICSRANK_INVALID);

	// T D::* -> T B::*
	TestIcsRank<int Base::*, int Derived::*>::apply(ICSRANK_STANDARDCONVERSION);
	TestIcsRank<int Derived::*, int Base::*>::apply(ICSRANK_INVALID);

	// T& -> T&
	TestIcsRank<Base&, Base&>::apply(ICSRANK_STANDARDEXACT);
	TestIcsRank<const Base&, Base&>::apply(ICSRANK_STANDARDEXACT);
	TestIcsRank<const Base&, const Base&>::apply(ICSRANK_STANDARDEXACT);
	TestIcsRank<Base&, const Base&>::apply(ICSRANK_INVALID);

	// D& -> B&
	TestIcsRank<Base&, Derived&>::apply(ICSRANK_STANDARDCONVERSION);
	TestIcsRank<Derived&, Base&>::apply(ICSRANK_INVALID);
	TestIcsRank<const Base&, Derived&>::apply(ICSRANK_STANDARDCONVERSION);
	TestIcsRank<const Base&, const Derived&>::apply(ICSRANK_STANDARDCONVERSION);
	TestIcsRank<Base&, const Derived&>::apply(ICSRANK_INVALID);

	// bind to reference
	TestIcsRank<const int&, int&>::apply(ICSRANK_STANDARDEXACT);
	TestIcsRank<const int&, char>::apply(ICSRANK_STANDARDPROMOTION);
	TestIcsRank<const float&, int>::apply(ICSRANK_STANDARDCONVERSION);
	TestIcsRank<const int&, float>::apply(ICSRANK_STANDARDCONVERSION);
	TestIcsRank<const bool&, Base*>::apply(ICSRANK_STANDARDCONVERSION);

	// convert from reference 
	TestIcsRank<int, int&>::apply(ICSRANK_STANDARDEXACT);
	TestIcsRank<int, char&>::apply(ICSRANK_STANDARDPROMOTION);
	TestIcsRank<int, float&>::apply(ICSRANK_STANDARDCONVERSION);
	TestIcsRank<Base, Derived&>::apply(ICSRANK_STANDARDCONVERSION);
	TestIcsRank<int*, int(&)[1]>::apply(ICSRANK_STANDARDEXACT);

	// lvalue T -> T&
	TestIcsRank<Base&, Base>::apply(ICSRANK_STANDARDEXACT, false, true);
	// rvalue T -> T&
	TestIcsRank<Base&, Base>::apply(ICSRANK_INVALID, false, false);

	// placeholders
	TestIcsRank<Any*, Base*, Base>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<Any*, int*, int>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<Any*, float*, float>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<Any*, void(*)(), void()>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<Any*, int>::match(ICSRANK_INVALID);

	TestIcsRank<Class*, Base*, Base>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<Class*, int*>::match(ICSRANK_INVALID);

	TestIcsRank<Object*, Base*, Base>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<Object*, int*, int>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<Object*, void**, void*>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<Object*, void(*)()>::match(ICSRANK_INVALID);
	TestIcsRank<Object*, void*>::match(ICSRANK_INVALID);

	TestIcsRank<Function*, void(*)(), void()>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<Function*, Base*, void()>::apply(ICSRANK_INVALID);
	TestIcsRank<Function*, int*>::match(ICSRANK_INVALID);

	TestIcsRank<Arithmetic, int, int>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<Arithmetic, float, float>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<Arithmetic, bool, bool>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<Arithmetic, int*>::match(ICSRANK_INVALID);

	TestIcsRank<PromotedArithmetic, int, int>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<PromotedArithmetic, float, float>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<PromotedArithmetic, bool, int>::match(ICSRANK_STANDARDPROMOTION);
	TestIcsRank<PromotedArithmetic, int*>::match(ICSRANK_INVALID);

	TestIcsRank<PromotedIntegral, int, int>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<PromotedIntegral, bool, int>::match(ICSRANK_STANDARDPROMOTION);
	TestIcsRank<PromotedIntegral, float, int>::match(ICSRANK_STANDARDCONVERSION);
	TestIcsRank<PromotedIntegral, int*>::match(ICSRANK_INVALID);

	TestIcsRank<MemberPointer, int Base::*, int Base::*>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<MemberPointer&, int Base::*&, int Base::*>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<MemberPointer volatile&, int Base::* volatile&, int Base::*>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<MemberPointer, int*>::match(ICSRANK_INVALID);

	TestIcsRank<Any*&, Base*&,Base>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<Any* volatile&, Base* volatile&, Base>::match(ICSRANK_STANDARDEXACT);
}

struct IcsRankTest
{
	IcsRankTest()
	{
		testIcsRank();
	}
} gIcsRankTest;


template<typename P, typename A, typename R = void>
struct TestDeduction
{
	typedef bool (*DeduceFunction)(const ParameterTypes& parameters, const UniqueTypeArray& arguments, TemplateArgumentsInstance& result);
	static void apply(const TemplateArgumentsInstance& expected, DeduceFunction deduce = deducePairs)
	{
		TemplateArgumentsInstance templateArguments(expected.size(), gUniqueTypeNull);
		ParameterTypes parameters(1, MakeType<P>::apply());
		UniqueTypeArray arguments(1, MakeType<A>::apply());
		bool result = deduce(parameters, arguments, templateArguments);
		if(result)
		{
			SYMBOLS_ASSERT(templateArguments == expected);
		}
		else
		{
			SYMBOLS_ASSERT(gUniqueTypeNull == expected[0]);
		}
	}
	static bool deduceFunctionCallWrapper(const ParameterTypes& parameters, const UniqueTypeArray& arguments, TemplateArgumentsInstance& result)
	{
		return deduceFunctionCall(parameters, arguments, result, Location(), 0);
	}

	static void apply(UniqueTypeWrapper expected, DeduceFunction deduce = deducePairs)
	{
		TemplateArgumentsInstance tmp;
		tmp.push_back(expected);
		apply(tmp, deduce);
	}
	static void applyFunction(UniqueTypeWrapper expected)
	{
		apply(expected, deduceFunctionCallWrapper);
	}
	static void apply(DeduceFunction deduceFunction = deducePairs)
	{
		apply(MakeType<R>::apply(), deduceFunction);
	}
	static void applyFunction()
	{
		apply(deduceFunctionCallWrapper);
	}
	static void apply(UniqueTypeWrapper expected, UniqueTypeWrapper expected2, DeduceFunction deduce = deducePairs)
	{
		TemplateArgumentsInstance tmp;
		tmp.push_back(expected);
		tmp.push_back(expected2);
		apply(tmp, deduce);
	}
	static void applyFunction(UniqueTypeWrapper expected, UniqueTypeWrapper expected2)
	{
		apply(expected, expected2, deduceFunctionCallWrapper);
	}
};


// [temp.deduct]
/*
	A template type argument T, a template template argument TT or a template non-type
	argument i can be deduced if P and A have one of the following forms:
	T cv-list
	T
	T*
	T&
	T[integer-constant]
	template-name<T> (where template-name refers to a class template)
	type(*)(T)
	T(*)()
	T(*)(T)
	T type::*
	type T::*
	T T::*
	T (type::*)()
	type (T::*)()
	type (type::*)(T)
	type (T::*)(T)
	T (type::*)(T)
	T (T::*)()
	T (T::*)(T)
	type[i]
	template-name<i> (where template-name refers to a class template)
	TT<T>
	TT<i>
	TT<>
	where (T) represents argument lists where at least one argument type contains a T, and () represents
	argument lists where no parameter contains a T. Similarly, <T> represents template argument lists where
	at least one argument contains a T, <i> represents template argument lists where at least one argument
	contains an i and <> represents template argument lists where no argument contains a T or an i.
	*/

// cv-qualifiers
// P	11	00	11
// A	11	11	00
// R	00	11	xx

void testDeduction()
{
	TestDeduction<T, int>::apply(gSignedInt);
	TestDeduction<T*, int*>::apply(gSignedInt);
	TestDeduction<T&, int&>::apply(gSignedInt);
	TestDeduction<T[], int[]>::apply(gSignedInt);

	TestDeduction<const T, const int, int>::apply();
	TestDeduction<const T*, const int*, int>::apply();
	TestDeduction<const T&, const int&, int>::apply();
	TestDeduction<const T[], const int[], int>::apply();

	TestDeduction<const T**, const int**, int>::apply();
	TestDeduction<T**, int**, int>::apply();

	TestDeduction<T, const int, const int>::apply();
	TestDeduction<T*, const int*, const int>::apply();
	TestDeduction<T&, const int&, const int>::apply();
	TestDeduction<T[], const int[], const int>::apply();

	TestDeduction<int(*)(T),	int(*)(int)>::apply(gSignedInt);
	TestDeduction<T(*)(),		int(*)()>::apply(gSignedInt);
	TestDeduction<T(*)(T),		int(*)(int)>::apply(gSignedInt);

	typedef Base type;
	TestDeduction<T type::*,			type type::*>::apply(gBaseClass);
	TestDeduction<type T::*,			type type::*>::apply(gBaseClass);
	TestDeduction<T T::*,				type type::*>::apply(gBaseClass);
	TestDeduction<T (type::*)(),		type (type::*)()>::apply(gBaseClass);
	TestDeduction<type (T::*)(),		type (type::*)()>::apply(gBaseClass);
	TestDeduction<type (type::*)(T),	type (type::*)(type)>::apply(gBaseClass);
	TestDeduction<type (T::*)(T),		type (type::*)(type)>::apply(gBaseClass);
	TestDeduction<T (type::*)(T),		type (type::*)(type)>::apply(gBaseClass);
	TestDeduction<T (T::*)(),			type (type::*)()>::apply(gBaseClass);
	TestDeduction<T (T::*)(T),			type (type::*)(type)>::apply(gBaseClass);

	const int i = NONTYPE_PARAM;

	TestDeduction<Template<T, i>, Template<int, 1> >::apply(gSignedInt, gOne);
	TestDeduction<TT<int, i>, Template<int, 1> >::apply(gTemplateTemplateArgument, gOne);
	TestDeduction<TT<int, 1>, Template<int, 1> >::apply(gTemplateTemplateArgument);

	// TODO: type[i]

	// expected to fail because T cannot be deduced
	TestDeduction<int, int>::apply(gUniqueTypeNull);
	TestDeduction<T*, int>::apply(gUniqueTypeNull);
	TestDeduction<T&, int>::apply(gUniqueTypeNull);
	TestDeduction<T[], int>::apply(gUniqueTypeNull);
	TestDeduction<int(*)(T), int>::apply(gUniqueTypeNull);
	TestDeduction<T(*)(), int>::apply(gUniqueTypeNull);
	TestDeduction<T(*)(T), int>::apply(gUniqueTypeNull);
	TestDeduction<const T, int>::apply(gUniqueTypeNull);
	TestDeduction<const T*, int*>::apply(gUniqueTypeNull);
	TestDeduction<TT<int, 1>, int >::apply(gUniqueTypeNull);
	TestDeduction<Template<T, 1>, int >::apply(gUniqueTypeNull);

	// expected to fail due to multiple deductions of T
	TestDeduction<T(*)(T), int(*)(float)>::apply(gUniqueTypeNull);

	// function call
	TestDeduction<const T&, int, int>::applyFunction();
	TestDeduction<const T&, const int&, int>::applyFunction();
	TestDeduction<T&, int, int>::applyFunction();
	TestDeduction<T, int[1], int*>::applyFunction();
	TestDeduction<T, const int[1], const int*>::applyFunction();
	TestDeduction<const Template<T, i>&, Template<int, 1> >::applyFunction(gSignedInt, gOne);

	typedef const int ConstInt;
	typedef int* PointerToInt;
	typedef ConstInt* PointerToConstInt;
	typedef const PointerToInt ConstPointerToInt;
	typedef const PointerToConstInt ConstPointerToConstInt;
	typedef PointerToInt* PointerToPointerToInt;
	typedef PointerToConstInt* PointerToPointerToConstInt;
	typedef ConstPointerToInt* PointerToConstPointerToInt;
	typedef ConstPointerToConstInt* PointerToConstPointerToConstInt;
	typedef const T ConstT;
	typedef T* PointerToT;
	typedef ConstT* PointerToConstT;
	typedef const PointerToT ConstPointerToT;
	typedef const PointerToConstT ConstPointerToConstT;
	typedef PointerToT* PointerToPointerToT;
	typedef PointerToConstT* PointerToPointerToConstT;
	typedef ConstPointerToT* PointerToConstPointerToT;
	typedef ConstPointerToConstT* PointerToConstPointerToConstT;

	TestDeduction<PointerToConstT, PointerToInt, int>::applyFunction(); // 1, 0
	TestDeduction<PointerToT, PointerToInt, int>::applyFunction(); // 0, 0
	TestDeduction<PointerToT, PointerToConstInt, const int>::applyFunction(); // 0, 1
	TestDeduction<PointerToConstT, PointerToInt, int>::applyFunction(); // 1, 0
	TestDeduction<PointerToConstT, PointerToConstInt, int>::applyFunction(); // 1, 1
	TestDeduction<PointerToPointerToT, PointerToPointerToInt, int>::applyFunction(); // 00, 00
	TestDeduction<PointerToPointerToT, PointerToPointerToConstInt, const int>::applyFunction(); // 00, 01
	TestDeduction<PointerToPointerToConstT, PointerToPointerToInt>::applyFunction(gUniqueTypeNull); // 01, 00
	TestDeduction<PointerToPointerToConstT, PointerToPointerToConstInt, int>::applyFunction(); // 01, 01
	TestDeduction<PointerToConstPointerToT, PointerToPointerToInt, int>::applyFunction(); // 10, 00
	TestDeduction<PointerToConstPointerToT, PointerToPointerToConstInt, const int>::applyFunction(); // 10, 01
	TestDeduction<PointerToConstPointerToT, PointerToConstPointerToInt, int>::applyFunction(); // 10, 10
	TestDeduction<PointerToConstPointerToT, PointerToConstPointerToConstInt, const int>::applyFunction(); // 10, 11
	TestDeduction<PointerToConstPointerToConstT, PointerToPointerToInt, int>::applyFunction(); // 11, 00
	TestDeduction<PointerToConstPointerToConstT, PointerToPointerToConstInt, int>::applyFunction(); // 11, 01
	TestDeduction<PointerToConstPointerToConstT, PointerToConstPointerToInt, int>::applyFunction(); // 11, 10
	TestDeduction<PointerToConstPointerToConstT, PointerToConstPointerToConstInt, int>::applyFunction(); // 11, 11
}


struct DeductionTest
{
	DeductionTest()
	{
		testDeduction();
	}
} gDeductionTest;



template<typename L, typename R>
struct TestOrdering
{
	static void apply(bool expected)
	{
		UniqueTypeArray templateParameters(1, MakeType<T>::apply());
		bool result = isMoreSpecialized(
			FunctionTemplate(ParameterTypes(1, MakeType<L>::apply()), templateParameters),
			FunctionTemplate(ParameterTypes(1, MakeType<R>::apply()), templateParameters));
		SYMBOLS_ASSERT(result == expected);
	}
};

void testOrdering()
{
	TestOrdering<T, T>::apply(false);
	TestOrdering<T*, T>::apply(true);
	TestOrdering<T, T*>::apply(false);
	TestOrdering<const T*, T*>::apply(true);
	TestOrdering<T*, const T*>::apply(false);
	TestOrdering<const T&, T&>::apply(true);
	TestOrdering<T&, const T&>::apply(false);

	// ambiguous
	TestOrdering<T&, T>::apply(false);
	TestOrdering<T, T&>::apply(false);
	TestOrdering<const T, T>::apply(false);
	TestOrdering<T, const T>::apply(false);
	TestOrdering<const T&, T>::apply(false);
	TestOrdering<T, const T&>::apply(false);
}


struct OrderingTest
{
	OrderingTest()
	{
		testOrdering();
	}
} gOrderingTest;



template<typename P, typename A, typename R = void>
struct TestSubstitution
{
	static void apply(UniqueTypeWrapper expected = MakeType<R>::apply())
	{
		TemplateArgumentsInstance templateArguments(1, MakeType<A>::apply());
		TypeInstance enclosing(&gTemplateClassDeclaration, 0);
		enclosing.templateArguments.swap(templateArguments);
		enclosing.instantiated = true;
		try
		{
			UniqueTypeWrapper result = substitute(MakeType<P>::apply(), Location(), enclosing);
			SYMBOLS_ASSERT(result == expected);
		}
		catch(TypeError&)
		{
			SYMBOLS_ASSERT(expected == gUniqueTypeNull);
		}
	}
};

void testSubstitution()
{
	TestSubstitution<T, int, int>::apply();
	TestSubstitution<T*, int, int*>::apply();
	TestSubstitution<T[], int, int[]>::apply();
	TestSubstitution<T, int*, int*>::apply();
	TestSubstitution<T, int&, int&>::apply();
	TestSubstitution<T, int[], int[]>::apply();

	TestSubstitution<const T, int, const int>::apply();
	TestSubstitution<T, const int, const int>::apply();
	TestSubstitution<const T, const int, const int>::apply();
	TestSubstitution<const T&, int, const int&>::apply();
	TestSubstitution<T&, const int, const int&>::apply();
	TestSubstitution<const T&, const int, const int&>::apply();
	TestSubstitution<const T*, int, const int*>::apply();
	TestSubstitution<T*, const int, const int*>::apply();
	TestSubstitution<const T*, const int, const int*>::apply();

	TestSubstitution<int(T), int, int(int)>::apply();
	TestSubstitution<T, int(int), int(int)>::apply();
	TestSubstitution<int(*)(T), int, int(*)(int)>::apply();
	TestSubstitution<T, int(*)(int), int(*)(int)>::apply();

	// TODO:
	// T::Type
	// &T::member
	// non-type

	// [temp.deduct] Attempting to create a pointer to reference type.
	TestSubstitution<T*, int&>::apply(gUniqueTypeNull);

	// [temp.deduct] Attempting to create a reference to a reference type or a reference to void
	TestSubstitution<T&, int&>::apply(gUniqueTypeNull);
	TestSubstitution<T&, void>::apply(gUniqueTypeNull);

	// [temp.deduct] Attempting to create an array with an element type that is void, a function type, or a reference type,
	//	or attempting to create an array with a size that is zero or negative.
	TestSubstitution<T[], void>::apply(gUniqueTypeNull);
	TestSubstitution<T[], int()>::apply(gUniqueTypeNull);
	TestSubstitution<T[], int&>::apply(gUniqueTypeNull);

	// [temp.deduct] Attempting to create a function type in which a parameter has a type of void.
	TestSubstitution<void(T), void>::apply(gUniqueTypeNull);
}


struct SubstitutionTest
{
	SubstitutionTest()
	{
		testSubstitution();
	}
} gSubstitutionTest;


#if 0

namespace Test
{
	struct Base
	{
		int m();
	};

	template<typename T>
	struct MakeType
	{
	};

	template<>
	struct MakeType<int>
	{
		static void apply()
		{
		}
	};

	template<>
	struct MakeType<Base>
	{
		static void apply()
		{
		}
	};

	template<typename T>
	struct MakeType<const T>
	{
		static void apply()
		{
			return MakeType<T>::apply();
		}
	};

	template<typename T>
	struct MakeType<T*>
	{
		static void apply()
		{
			return MakeType<T>::apply();
		}
	};

	template<typename T>
	struct MakeType<T&>
	{
		static void apply()
		{
			return MakeType<T>::apply();
		}
	};

	template<typename T>
	struct MakeType<T()>
	{
		static void apply()
		{
			return MakeType<T>::apply();
		}
	};

	template<typename T, typename C>
	struct MakeType<T C::*>
	{
		static void apply()
		{
			return MakeType<T>::apply();
		}
	};

	void f()
	{
		MakeType<int (Base::*)()>::apply();
	}
}
#endif

#include <fstream>
#include "util.h"


struct InstantiationSuffix
{
	char value[12];
	InstantiationSuffix(const void* p)
	{
		sprintf(value, "_%08x", p);
	}
	const char* c_str() const
	{
		return value;
	}
};

struct InstantiationName : public Concatenate
{
	InstantiationName(const TypeInstance& instance)
		: Concatenate(
			makeRange(getValue(instance.declaration->getName())),
			makeRange(InstantiationSuffix(&instance).c_str()))
	{
	}
};

struct InstantiationPath : public Concatenate
{
	InstantiationPath(const TypeInstance& instance)
		: Concatenate(
			makeRange(InstantiationName(instance).c_str()),
			makeRange(".html"))
	{
	}
};

inline void printPosition(const Source& source, std::ostream& out)
{
	out << source.absolute.c_str() << "(" << source.line << ", " << source.column << "): ";
}

void printTypeReadable(const TypeInstance& type, std::ostream& out, bool escape = true)
{
	if(type.enclosing != 0)
	{
		printTypeReadable(*type.enclosing, out, escape);
		out << ".";
	}
	else
	{
		printName(type.declaration->scope, out);
	}
	out << getValue(type.declaration->getName());
	if(type.declaration->isTemplate)
	{
		out << (escape ? "&lt;" : "<");
		bool separator = false;
		for(TemplateArgumentsInstance::const_iterator i = type.templateArguments.begin(); i != type.templateArguments.end(); ++i)
		{
			if(separator)
			{
				out << ",";
			}
			out << std::endl << "\t";
			printType(*i, out, escape);
			separator = true;
		}
		if(!type.templateArguments.empty())
		{
			out << std::endl;
		}
		out << (escape ? "&gt;" : ">");
	}
}

inline void dumpTemplateInstantiations(const TypeInstance& instance, bool root)
{
	if(instance.dumped)
	{
		return;
	}
	instance.dumped = true;
	SYMBOLS_ASSERT(!instance.visited);
	instance.visited = true;
	std::ofstream out(Concatenate(makeRange(root ? "debug/!" : "debug/"), makeRange(InstantiationPath(instance).c_str())).c_str());
	SYMBOLS_ASSERT(out.is_open());

	out << "<html>\n"
		"<head>\n"
		"</head>\n"
		"<body>\n"
		"<pre style='color:#000000;background:#ffffff;'>\n";
	printPosition(instance.declaration->getName().source, out);
	out << std::endl;
	printTypeReadable(instance, out, true);
	out << std::endl << std::endl;

	typedef std::map<const TypeInstance*, Location> InstanceMap;
	InstanceMap instanceMap;
	for(ChildInstantiations::const_iterator i = instance.childInstantiations.begin(); i != instance.childInstantiations.end(); ++i)
	{
		instanceMap.insert(InstanceMap::value_type((*i).instance, (*i).source));
	}
	for(InstanceMap::const_iterator i = instanceMap.begin(); i != instanceMap.end(); ++i)
	{
		printPosition((*i).second, out);
		out << std::endl;
		out << "<a href='" << InstantiationPath(*(*i).first).c_str() << "'>";
		printTypeReadable(*(*i).first, out, true);
		out << "</a>";
		out << std::endl;
		dumpTemplateInstantiations(*(*i).first);
	}
	out << "</pre>\n"
		"</body>\n"
		"</html>\n";
	instance.visited = false;
}
