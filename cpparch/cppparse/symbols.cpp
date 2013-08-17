
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

Identifier gGlobalId = makeIdentifier("$global");

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
BuiltInTypeId gChar(&gCharDeclaration, TREEALLOCATOR_NULL);
Identifier gSignedCharId = makeIdentifier("signed char");
BuiltInTypeDeclaration gSignedCharDeclaration(gSignedCharId);
BuiltInTypeId gSignedChar(&gSignedCharDeclaration, TREEALLOCATOR_NULL);
Identifier gUnsignedCharId = makeIdentifier("unsigned char");
BuiltInTypeDeclaration gUnsignedCharDeclaration(gUnsignedCharId);
BuiltInTypeId gUnsignedChar(&gUnsignedCharDeclaration, TREEALLOCATOR_NULL);
Identifier gSignedShortIntId = makeIdentifier("short");
BuiltInTypeDeclaration gSignedShortIntDeclaration(gSignedShortIntId);
BuiltInTypeId gSignedShortInt(&gSignedShortIntDeclaration, TREEALLOCATOR_NULL);
Identifier gUnsignedShortIntId = makeIdentifier("unsigned short");
BuiltInTypeDeclaration gUnsignedShortIntDeclaration(gUnsignedShortIntId);
BuiltInTypeId gUnsignedShortInt(&gUnsignedShortIntDeclaration, TREEALLOCATOR_NULL);
Identifier gSignedIntId = makeIdentifier("int");
BuiltInTypeDeclaration gSignedIntDeclaration(gSignedIntId);
BuiltInTypeId gSignedInt(&gSignedIntDeclaration, TREEALLOCATOR_NULL);
Identifier gUnsignedIntId = makeIdentifier("unsigned int");
BuiltInTypeDeclaration gUnsignedIntDeclaration(gUnsignedIntId);
BuiltInTypeId gUnsignedInt(&gUnsignedIntDeclaration, TREEALLOCATOR_NULL);
Identifier gSignedLongIntId = makeIdentifier("long");
BuiltInTypeDeclaration gSignedLongIntDeclaration(gSignedLongIntId);
BuiltInTypeId gSignedLongInt(&gSignedLongIntDeclaration, TREEALLOCATOR_NULL);
Identifier gUnsignedLongIntId = makeIdentifier("unsigned long");
BuiltInTypeDeclaration gUnsignedLongIntDeclaration(gUnsignedLongIntId);
BuiltInTypeId gUnsignedLongInt(&gUnsignedLongIntDeclaration, TREEALLOCATOR_NULL);
Identifier gSignedLongLongIntId = makeIdentifier("long long");
BuiltInTypeDeclaration gSignedLongLongIntDeclaration(gSignedLongLongIntId);
BuiltInTypeId gSignedLongLongInt(&gSignedLongLongIntDeclaration, TREEALLOCATOR_NULL);
Identifier gUnsignedLongLongIntId = makeIdentifier("unsigned long long");
BuiltInTypeDeclaration gUnsignedLongLongIntDeclaration(gUnsignedLongLongIntId);
BuiltInTypeId gUnsignedLongLongInt(&gUnsignedLongLongIntDeclaration, TREEALLOCATOR_NULL);
Identifier gWCharId = makeIdentifier("wchar_t");
BuiltInTypeDeclaration gWCharTDeclaration(gWCharId);
BuiltInTypeId gWCharT(&gWCharTDeclaration, TREEALLOCATOR_NULL);
Identifier gBoolId = makeIdentifier("bool");
BuiltInTypeDeclaration gBoolDeclaration(gBoolId);
BuiltInTypeId gBool(&gBoolDeclaration, TREEALLOCATOR_NULL);
Identifier gFloatId = makeIdentifier("float");
BuiltInTypeDeclaration gFloatDeclaration(gFloatId);
BuiltInTypeId gFloat(&gFloatDeclaration, TREEALLOCATOR_NULL);
Identifier gDoubleId = makeIdentifier("double");
BuiltInTypeDeclaration gDoubleDeclaration(gDoubleId);
BuiltInTypeId gDouble(&gDoubleDeclaration, TREEALLOCATOR_NULL);
Identifier gLongDoubleId = makeIdentifier("long double");
BuiltInTypeDeclaration gLongDoubleDeclaration(gLongDoubleId);
BuiltInTypeId gLongDouble(&gLongDoubleDeclaration, TREEALLOCATOR_NULL);
Identifier gVoidId = makeIdentifier("void");
BuiltInTypeDeclaration gVoidDeclaration(gVoidId, TYPE_SPECIAL);
BuiltInTypeId gVoid(&gVoidDeclaration, TREEALLOCATOR_NULL);

StringLiteralTypeId gStringLiteral(&gCharDeclaration, TREEALLOCATOR_NULL);
StringLiteralTypeId gWideStringLiteral(&gWCharTDeclaration, TREEALLOCATOR_NULL);

// built-in operator placeholders
Identifier gArithmeticPlaceholderId = makeIdentifier("$arithmetic-placeholder");
BuiltInTypeDeclaration gArithmeticPlaceholderDeclaration(gArithmeticPlaceholderId, TYPE_UNKNOWN);
BuiltInTypeId gArithmeticPlaceholder(&gArithmeticPlaceholderDeclaration, TREEALLOCATOR_NULL);
Identifier gIntegralPlaceholderId = makeIdentifier("$integral-placeholder");
BuiltInTypeDeclaration gIntegralPlaceholderDeclaration(gIntegralPlaceholderId, TYPE_UNKNOWN);
BuiltInTypeId gIntegralPlaceholder(&gIntegralPlaceholderDeclaration, TREEALLOCATOR_NULL);
Identifier gPromotedArithmeticPlaceholderId = makeIdentifier("$promoted-arithmetic-placeholder");
BuiltInTypeDeclaration gPromotedArithmeticPlaceholderDeclaration(gPromotedArithmeticPlaceholderId, TYPE_UNKNOWN);
BuiltInTypeId gPromotedArithmeticPlaceholder(&gPromotedArithmeticPlaceholderDeclaration, TREEALLOCATOR_NULL);
Identifier gPromotedIntegralPlaceholderId = makeIdentifier("$promoted-integral-placeholder");
BuiltInTypeDeclaration gPromotedIntegralPlaceholderDeclaration(gPromotedIntegralPlaceholderId, TYPE_UNKNOWN);
BuiltInTypeId gPromotedIntegralPlaceholder(&gPromotedIntegralPlaceholderDeclaration, TREEALLOCATOR_NULL);
Identifier gEnumerationPlaceholderId = makeIdentifier("$enumeration-placeholder");
BuiltInTypeDeclaration gEnumerationPlaceholderDeclaration(gEnumerationPlaceholderId, TYPE_UNKNOWN);
BuiltInTypeId gEnumerationPlaceholder(&gEnumerationPlaceholderDeclaration, TREEALLOCATOR_NULL);
Identifier gPointerToAnyPlaceholderId = makeIdentifier("$pointer-to-any-placeholder");
BuiltInTypeDeclaration gPointerToAnyPlaceholderDeclaration(gPointerToAnyPlaceholderId, TYPE_UNKNOWN);
BuiltInTypeId gPointerToAnyPlaceholder(&gPointerToAnyPlaceholderDeclaration, TREEALLOCATOR_NULL);
Identifier gPointerToObjectPlaceholderId = makeIdentifier("$pointer-to-object-placeholder");
BuiltInTypeDeclaration gPointerToObjectPlaceholderDeclaration(gPointerToObjectPlaceholderId, TYPE_UNKNOWN);
BuiltInTypeId gPointerToObjectPlaceholder(&gPointerToObjectPlaceholderDeclaration, TREEALLOCATOR_NULL);
Identifier gPointerToClassPlaceholderId = makeIdentifier("$pointer-to-class-placeholder");
BuiltInTypeDeclaration gPointerToClassPlaceholderDeclaration(gPointerToClassPlaceholderId, TYPE_UNKNOWN);
BuiltInTypeId gPointerToClassPlaceholder(&gPointerToClassPlaceholderDeclaration, TREEALLOCATOR_NULL);
Identifier gPointerToFunctionPlaceholderId = makeIdentifier("$pointer-to-function-placeholder");
BuiltInTypeDeclaration gPointerToFunctionPlaceholderDeclaration(gPointerToFunctionPlaceholderId, TYPE_UNKNOWN);
BuiltInTypeId gPointerToFunctionPlaceholder(&gPointerToFunctionPlaceholderDeclaration, TREEALLOCATOR_NULL);
Identifier gPointerToMemberPlaceholderId = makeIdentifier("$member-pointer-placeholder");
BuiltInTypeDeclaration gPointerToMemberPlaceholderDeclaration(gPointerToMemberPlaceholderId, TYPE_UNKNOWN);
BuiltInTypeId gPointerToMemberPlaceholder(&gPointerToMemberPlaceholderDeclaration, TREEALLOCATOR_NULL);

// special
Identifier gImplicitObjectParameterId = makeIdentifier("$implicit-object-parameter");
BuiltInTypeDeclaration gImplicitObjectParameterDeclaration(gImplicitObjectParameterId, TYPE_UNKNOWN);
BuiltInTypeId gImplicitObjectParameter(&gImplicitObjectParameterDeclaration, TREEALLOCATOR_NULL);

Identifier gSpecialMemberFunctionId = makeIdentifier("$special-member-function");
Scope gSpecialMemberFunctionScope(TREEALLOCATOR_NULL, gSpecialMemberFunctionId, SCOPETYPE_CLASS);
TypeId gSpecialMemberFunctionType(&gVoidDeclaration, TREEALLOCATOR_NULL);

Identifier gCopyAssignmentOperatorId = makeIdentifier("$copy-assignment-operator");
Declaration gCopyAssignmentOperatorDeclaration(TREEALLOCATOR_NULL, &gSpecialMemberFunctionScope, gCopyAssignmentOperatorId, gSpecialMemberFunctionType, 0);
const DeclarationInstance gCopyAssignmentOperatorInstance(&gCopyAssignmentOperatorDeclaration);

Identifier gDestructorId = makeIdentifier("$destructor");
Declaration gDestructorDeclaration(TREEALLOCATOR_NULL, &gSpecialMemberFunctionScope, gDestructorId, gSpecialMemberFunctionType, 0);
const DeclarationInstance gDestructorInstance(&gDestructorDeclaration);

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


const SimpleType gDependentSimpleType(&gDependentType, 0);

struct PointerTypeId : BuiltInTypeId
{
	PointerTypeId(Declaration* declaration, const TreeAllocator<int>& allocator, CvQualifiers qualifiers = CvQualifiers())
		: BuiltInTypeId(declaration, allocator)
	{
		value.setQualifiers(qualifiers);
		value = pushBuiltInType(value, PointerType());
	}
};

struct PointerPointerTypeId : BuiltInTypeId
{
	PointerPointerTypeId(Declaration* declaration, const TreeAllocator<int>& allocator, CvQualifiers qualifiers = CvQualifiers())
		: BuiltInTypeId(declaration, allocator)
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
BuiltInTypeId gBaseClass(&gBaseClassDeclaration, TREEALLOCATOR_NULL);

Types2::Pointer::Value gBaseClassNode = Types2::Node(Type(&gBaseClassDeclaration, TREEALLOCATOR_NULL));

struct DerivedClassTypeId : BuiltInTypeId
{
	DerivedClassTypeId(Declaration* declaration, const TreeAllocator<int>& allocator)
		: BuiltInTypeId(declaration, allocator)
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


struct DependentTypeId : BuiltInType
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


Types2::Pointer::Value gTemplateParam2 = Types2::Node(Type(&gNonTypeTemplateParameterDeclaration, TREEALLOCATOR_NULL));
Types2::Pointer::Value gTemplateParam1 = Types2::Node(Type(&gTemplateParameterDeclaration, TREEALLOCATOR_NULL));

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

struct BuiltInTemplateTemplateArgument : BuiltInType
{
	BuiltInTemplateTemplateArgument(Declaration* declaration)
	{
		value = pushBuiltInType(value, TemplateTemplateArgument(declaration, 0));
	}
};

BuiltInTemplateTemplateArgument gTemplateTemplateArgument(&gTemplateClassDeclaration);

struct BuiltInNonType : BuiltInType
{
	BuiltInNonType(IntegralConstant constant)
	{
		value = pushBuiltInType(value, NonType(constant));
	}
};

BuiltInNonType gZero(IntegralConstant(0));
BuiltInNonType gOne(IntegralConstant(1));

struct BuiltInDependentNonType : BuiltInType
{
	BuiltInDependentNonType(DeclarationPtr declaration)
	{
		value = pushBuiltInType(value, DependentNonType(makeBuiltInExpression(NonTypeTemplateParameter(declaration))));
	}
};

BuiltInDependentNonType gDependentNonType(&gNonTypeTemplateParameterDeclaration);




template<typename T>
inline BuiltInType pushType(BuiltInType type, const T& t)
{
	return BuiltInType(pushBuiltInType(type, t));
}

template<typename T>
inline UserType pushType(UserType type, const T& t)
{
	return UserType(pushType(UniqueTypeWrapper(type), t));
}

inline BuiltInType popType(BuiltInType type)
{
	type.pop_front();
	return type;
}

inline UserType popType(UserType type)
{
	type.pop_front();
	return type;
}

inline BuiltInType qualifyType(BuiltInType type, CvQualifiers qualifiers)
{
	type.value.setQualifiers(qualifiers);
	return type;
}

template<bool builtIn>
static UniqueTypeGeneric<builtIn> makeFunctionType(UniqueTypeGeneric<builtIn> inner)
{
	return UniqueTypeGeneric<builtIn>(pushType(inner, FunctionType()));
}

template<bool builtIn>
static UniqueTypeGeneric<builtIn> makeFunctionType(UniqueTypeGeneric<builtIn> inner, UniqueTypeGeneric<builtIn> a1)
{
	FunctionType function;
	function.parameterTypes.push_back(a1);
	return pushType(inner, function);
}

template<bool builtIn>
static UniqueTypeGeneric<builtIn> makeFunctionType(UniqueTypeGeneric<builtIn> inner, UniqueTypeGeneric<builtIn> a1, UniqueTypeGeneric<builtIn> a2)
{
	FunctionType function;
	function.parameterTypes.push_back(a1);
	function.parameterTypes.push_back(a2);
	return pushType(inner, function);
}

// ----------------------------------------------------------------------------


const BuiltInType gIntegralTypes[] = {
	gChar,
	gSignedChar,
	gUnsignedChar,
	gSignedShortInt,
	gUnsignedShortInt,
	gSignedInt,
	gUnsignedInt,
	gSignedLongInt,
	gUnsignedLongInt,
	gSignedLongLongInt,
	gUnsignedLongLongInt,
	gWCharT,
	gBool
};
BuiltInTypeArrayRange gIntegralTypesRange = ARRAY_RANGE(gIntegralTypes);

const BuiltInType gPromotedIntegralTypes[] = {
	gSignedInt,
	gUnsignedInt,
	gSignedLongInt,
	gUnsignedLongInt,
	gSignedLongLongInt,
	gUnsignedLongLongInt
};
BuiltInTypeArrayRange gPromotedIntegralTypesRange = ARRAY_RANGE(gPromotedIntegralTypes);

const BuiltInType gArithmeticTypes[] = {
	gChar,
	gSignedChar,
	gUnsignedChar,
	gSignedShortInt,
	gUnsignedShortInt,
	gSignedInt,
	gUnsignedInt,
	gSignedLongInt,
	gUnsignedLongInt,
	gSignedLongLongInt,
	gUnsignedLongLongInt,
	gWCharT,
	gBool,
	gFloat,
	gDouble,
	gLongDouble
};
BuiltInTypeArrayRange gArithmeticTypesRange = ARRAY_RANGE(gArithmeticTypes);

const BuiltInType gPromotedArithmeticTypes[] = {
	gSignedInt,
	gUnsignedInt,
	gSignedLongInt,
	gUnsignedLongInt,
	gSignedLongLongInt,
	gUnsignedLongLongInt,
	gFloat,
	gDouble,
	gLongDouble
};
BuiltInTypeArrayRange gPromotedArithmeticTypesRange = ARRAY_RANGE(gPromotedArithmeticTypes);

struct GeneratedTypeArray : std::vector<BuiltInType>
{
	BuiltInTypeArrayRange makeRange() const
	{
		return BuiltInTypeArrayRange(&(*begin()), &(*begin()) + size());
	}
};

// [over.built]
struct PostIncOperatorTypes : GeneratedTypeArray
{
	PostIncOperatorTypes()
	{
		for(const BuiltInType* i = gArithmeticTypes; i != ARRAY_END(gArithmeticTypes); ++i)
		{
			BuiltInType type = *i;
			BuiltInType ref = pushType(type, ReferenceType());
			BuiltInType vref = pushType(qualifyType(type, CvQualifiers(false, true)), ReferenceType());
			push_back(makeFunctionType(type, ref, gSignedInt)); // T(T&, int)
			push_back(makeFunctionType(type, vref, gSignedInt)); // volatile T(volatile T&, int)
		}
	}
};

PostIncOperatorTypes gUnaryPostIncOperatorTypeArray;
BuiltInTypeArrayRange gUnaryPostIncOperatorTypes = gUnaryPostIncOperatorTypeArray.makeRange();

struct PreIncOperatorTypes : GeneratedTypeArray
{
	PreIncOperatorTypes()
	{
		for(const BuiltInType* i = gArithmeticTypes; i != ARRAY_END(gArithmeticTypes); ++i)
		{
			BuiltInType type = *i;
			BuiltInType ref = pushType(type, ReferenceType());
			BuiltInType vref = pushType(qualifyType(type, CvQualifiers(false, true)), ReferenceType());
			push_back(makeFunctionType(ref, ref)); // T&(T&)
			push_back(makeFunctionType(vref, vref)); // volatile T&(volatile T&)
		}
	}
};

PreIncOperatorTypes gUnaryPreIncOperatorTypeArray;
BuiltInTypeArrayRange gUnaryPreIncOperatorTypes = gUnaryPreIncOperatorTypeArray.makeRange();

struct UnaryArithmeticOperatorTypes : GeneratedTypeArray
{
	UnaryArithmeticOperatorTypes(BuiltInTypeArrayRange types)
	{
		for(const BuiltInType* i = types.first; i != types.last; ++i)
		{
			BuiltInType type = *i;
			push_back(makeFunctionType(type, type)); // T(T)
		}
	}
};

UnaryArithmeticOperatorTypes gUnaryArithmeticOperatorTypeArray(ARRAY_RANGE(gPromotedArithmeticTypes));
BuiltInTypeArrayRange gUnaryArithmeticOperatorTypes = gUnaryArithmeticOperatorTypeArray.makeRange();

UnaryArithmeticOperatorTypes gUnaryIntegralOperatorTypeArray(ARRAY_RANGE(gPromotedIntegralTypes));
BuiltInTypeArrayRange gUnaryIntegralOperatorTypes = gUnaryIntegralOperatorTypeArray.makeRange();

struct BinaryArithmeticOperatorTypes : GeneratedTypeArray
{
	BinaryArithmeticOperatorTypes(BuiltInTypeArrayRange types)
	{
		for(const BuiltInType* i = types.first; i != types.last; ++i)
		{
			BuiltInType left = *i;
			for(const BuiltInType* i = types.first; i != types.last; ++i)
			{
				BuiltInType right = *i;
				BuiltInType result = usualArithmeticConversions(left, right);
				push_back(makeFunctionType(result, left, right)); // LR(L, R)
			}
		}
	}
};

BinaryArithmeticOperatorTypes gBinaryArithmeticOperatorTypeArray(ARRAY_RANGE(gPromotedArithmeticTypes));
BuiltInTypeArrayRange gBinaryArithmeticOperatorTypes = gBinaryArithmeticOperatorTypeArray.makeRange();

BinaryArithmeticOperatorTypes gBinaryIntegralOperatorTypeArray(ARRAY_RANGE(gPromotedIntegralTypes));
BuiltInTypeArrayRange gBinaryIntegralOperatorTypes = gBinaryIntegralOperatorTypeArray.makeRange();

struct RelationalArithmeticOperatorTypes : GeneratedTypeArray
{
	RelationalArithmeticOperatorTypes()
	{
		for(const BuiltInType* i = gPromotedArithmeticTypes; i != ARRAY_END(gPromotedArithmeticTypes); ++i)
		{
			BuiltInType left = *i;
			for(const BuiltInType* i = gPromotedArithmeticTypes; i != ARRAY_END(gPromotedArithmeticTypes); ++i)
			{
				BuiltInType right = *i;
				push_back(makeFunctionType(gBool, left, right)); // bool(L, R)
			}
		}
	}
};

RelationalArithmeticOperatorTypes gRelationalArithmeticOperatorTypeArray;
BuiltInTypeArrayRange gRelationalArithmeticOperatorTypes = gRelationalArithmeticOperatorTypeArray.makeRange();

struct ShiftOperatorTypes : GeneratedTypeArray
{
	ShiftOperatorTypes()
	{
		for(const BuiltInType* i = gPromotedArithmeticTypes; i != ARRAY_END(gPromotedArithmeticTypes); ++i)
		{
			BuiltInType left = *i;
			for(const BuiltInType* i = gPromotedArithmeticTypes; i != ARRAY_END(gPromotedArithmeticTypes); ++i)
			{
				BuiltInType right = *i;
				push_back(makeFunctionType(left, left, right)); // L(L, R)
			}
		}
	}
};

ShiftOperatorTypes gShiftOperatorTypeArray;
BuiltInTypeArrayRange gShiftOperatorTypes = gShiftOperatorTypeArray.makeRange();

struct AssignArithmeticOperatorTypes : GeneratedTypeArray
{
	AssignArithmeticOperatorTypes(BuiltInTypeArrayRange leftTypes, BuiltInTypeArrayRange rightTypes)
	{
		for(const BuiltInType* i = leftTypes.first; i != leftTypes.last; ++i)
		{
			BuiltInType left = *i;
			for(const BuiltInType* i = rightTypes.first; i != rightTypes.last; ++i)
			{
				BuiltInType right = *i;
				BuiltInType ref = pushType(left, ReferenceType());
				BuiltInType vref = pushType(qualifyType(left, CvQualifiers(false, true)), ReferenceType());
				push_back(makeFunctionType(ref, ref, right)); // L&(L&, R)
				push_back(makeFunctionType(vref, vref, right)); // volatile L&(volatile L&, R)
			}
		}
	}
};

AssignArithmeticOperatorTypes gAssignArithmeticOperatorTypeArray(ARRAY_RANGE(gArithmeticTypes), ARRAY_RANGE(gPromotedArithmeticTypes));
BuiltInTypeArrayRange gAssignArithmeticOperatorTypes = gAssignArithmeticOperatorTypeArray.makeRange();

AssignArithmeticOperatorTypes gAssignIntegralOperatorTypeArray(ARRAY_RANGE(gIntegralTypes), ARRAY_RANGE(gPromotedIntegralTypes));
BuiltInTypeArrayRange gAssignIntegralOperatorTypes = gAssignIntegralOperatorTypeArray.makeRange();


BuiltInType gBinaryLogicalOperatorTypeArray[] = {
	makeFunctionType(gBool, gBool, gBool) // bool(bool, bool)
};
BuiltInTypeArrayRange gBinaryLogicalOperatorTypes = ARRAY_RANGE(gBinaryLogicalOperatorTypeArray);

BuiltInType gUnaryLogicalOperatorTypeArray[] = {
	makeFunctionType(gBool, gBool) // bool(bool)
};
BuiltInTypeArrayRange gUnaryLogicalOperatorTypes = ARRAY_RANGE(gUnaryLogicalOperatorTypeArray);


#define MAKE_GENERICTYPE1(substitute, placeholder) BuiltInGenericType1(substitute<true>(placeholder), &substitute<false>)
#define MAKE_GENERICTYPE2(substitute, placeholder) BuiltInGenericType2(substitute<true>(placeholder), &substitute<false>)

BuiltInType gPtrDiffT = gSignedLongLongInt;

template<bool builtIn>
UniqueTypeGeneric<builtIn> makePointerArithmeticOperatorType(TypeTuple<builtIn, 1> type)
{
	return makeFunctionType(type, type, UniqueTypeGeneric<builtIn>(gPtrDiffT)); // T*(T*, ptrdiff_t)
}

template<bool builtIn>
UniqueTypeGeneric<builtIn> makePointerArithmeticOperatorTypeSwapped(TypeTuple<builtIn, 1> type)
{
	return makeFunctionType(type, UniqueTypeGeneric<builtIn>(gPtrDiffT), type); // T*(ptrdiff_t, T*)
}

template<bool builtIn>
UniqueTypeGeneric<builtIn> makeSubscriptOperatorType(TypeTuple<builtIn, 1> type)
{
	UniqueTypeGeneric<builtIn> ref = pushType(popType(type), ReferenceType());
	return makeFunctionType(ref, type, UniqueTypeGeneric<builtIn>(gPtrDiffT)); // T&(T*, ptrdiff_t)
}

template<bool builtIn>
UniqueTypeGeneric<builtIn> makeSubscriptOperatorTypeSwapped(TypeTuple<builtIn, 1> type)
{
	UniqueTypeGeneric<builtIn> ref = pushType(popType(type), ReferenceType());
	return makeFunctionType(ref, UniqueTypeGeneric<builtIn>(gPtrDiffT), type); // T&(ptrdiff_t, T*)
}

template<bool builtIn>
UniqueTypeGeneric<builtIn> makeComparisonOperatorType(TypeTuple<builtIn, 1> type)
{
	return makeFunctionType(UniqueTypeGeneric<builtIn>(gBool), type, type); // bool(T, T)
}

CvQualifiers makeUnion(CvQualifiers left, CvQualifiers right)
{
	return CvQualifiers(left.isConst | right.isConst, left.isVolatile | right.isVolatile);
}

template<bool builtIn>
UniqueTypeGeneric<builtIn> makeMemberPointerOperatorType(TypeTuple<builtIn, 2> args)
{
	CvQualifiers qualifiers = popType(args.first).value.getQualifiers();
	UniqueTypeGeneric<builtIn> result = popType(args.second);
	result.value.setQualifiers(makeUnion(result.value.getQualifiers(), qualifiers));
	return makeFunctionType(result, args.first, args.second); // CV12 T&(CV1 C1*, CV2 T C2::*)
}


BuiltInGenericType1 gPointerAddOperatorTypeArray[] = {
	MAKE_GENERICTYPE1(makePointerArithmeticOperatorType, gPointerToObjectPlaceholder),
	MAKE_GENERICTYPE1(makePointerArithmeticOperatorTypeSwapped, gPointerToObjectPlaceholder),
};
BuiltInGenericType1ArrayRange gPointerAddOperatorTypes = ARRAY_RANGE(gPointerAddOperatorTypeArray);

BuiltInGenericType1 gPointerSubtractOperatorTypeArray[] = {
	MAKE_GENERICTYPE1(makePointerArithmeticOperatorType, gPointerToObjectPlaceholder),
};
BuiltInGenericType1ArrayRange gPointerSubtractOperatorTypes = ARRAY_RANGE(gPointerSubtractOperatorTypeArray);

BuiltInGenericType1 gSubscriptOperatorTypeArray[] = {
	MAKE_GENERICTYPE1(makeSubscriptOperatorType, gPointerToObjectPlaceholder),
	MAKE_GENERICTYPE1(makeSubscriptOperatorTypeSwapped, gPointerToObjectPlaceholder),
};
BuiltInGenericType1ArrayRange gSubscriptOperatorTypes = ARRAY_RANGE(gSubscriptOperatorTypeArray);

BuiltInGenericType1 gRelationalOperatorTypeArray[] = {
	MAKE_GENERICTYPE1(makeComparisonOperatorType, gPointerToAnyPlaceholder),
	MAKE_GENERICTYPE1(makeComparisonOperatorType, gEnumerationPlaceholder),
};
BuiltInGenericType1ArrayRange gRelationalOperatorTypes = ARRAY_RANGE(gRelationalOperatorTypeArray);

BuiltInGenericType1 gEqualityOperatorTypeArray[] = {
	MAKE_GENERICTYPE1(makeComparisonOperatorType, gPointerToAnyPlaceholder),
	MAKE_GENERICTYPE1(makeComparisonOperatorType, gEnumerationPlaceholder),
	MAKE_GENERICTYPE1(makeComparisonOperatorType, gPointerToMemberPlaceholder),
};
BuiltInGenericType1ArrayRange gEqualityOperatorTypes = ARRAY_RANGE(gEqualityOperatorTypeArray);

typedef TypeTuple<true, 2> BuiltInType2;

BuiltInGenericType2 gMemberPointerOperatorTypeArray[] = {
	MAKE_GENERICTYPE2(makeMemberPointerOperatorType, BuiltInType2(gPointerToClassPlaceholder, gPointerToMemberPlaceholder)),
};
BuiltInGenericType2ArrayRange gMemberPointerOperatorTypes = ARRAY_RANGE(gMemberPointerOperatorTypeArray);

// ----------------------------------------------------------------------------


struct Base
{
};
struct Derived : Base
{
};

class PointerToAny
{
};

class PointerToObject
{
};

class PointerToClass
{
};

class PointerToFunction
{
};

class PointerToMember
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
	static BuiltInType apply();
};

template<>
struct MakeType<bool>
{
	static BuiltInType apply()
	{
		return gBool;
	}
};

template<>
struct MakeType<int>
{
	static BuiltInType apply()
	{
		return gSignedInt;
	}
};

template<>
struct MakeType<float>
{
	static BuiltInType apply()
	{
		return gFloat;
	}
};

template<>
struct MakeType<char>
{
	static BuiltInType apply()
	{
		return gChar;
	}
};

template<>
struct MakeType<wchar_t>
{
	static BuiltInType apply()
	{
		return gWCharT;
	}
};

template<>
struct MakeType<void>
{
	static BuiltInType apply()
	{
		return gVoid;
	}
};

template<>
struct MakeType<Base>
{
	static BuiltInType apply()
	{
		return gBaseClass;
	}
};

template<>
struct MakeType<Derived>
{
	static BuiltInType apply()
	{
		return gDerivedClass;
	}
};

template<>
struct MakeType<PointerToAny>
{
	static BuiltInType apply()
	{
		return gPointerToAnyPlaceholder;
	}
};

template<>
struct MakeType<PointerToObject>
{
	static BuiltInType apply()
	{
		return gPointerToObjectPlaceholder;
	}
};

template<>
struct MakeType<PointerToClass>
{
	static BuiltInType apply()
	{
		return gPointerToClassPlaceholder;
	}
};

template<>
struct MakeType<PointerToFunction>
{
	static BuiltInType apply()
	{
		return gPointerToFunctionPlaceholder;
	}
};

template<>
struct MakeType<PointerToMember>
{
	static BuiltInType apply()
	{
		return gPointerToMemberPlaceholder;
	}
};

template<>
struct MakeType<Enumeration>
{
	static BuiltInType apply()
	{
		return gEnumerationPlaceholder;
	}
};

template<>
struct MakeType<Arithmetic>
{
	static BuiltInType apply()
	{
		return gArithmeticPlaceholder;
	}
};

template<>
struct MakeType<Integral>
{
	static BuiltInType apply()
	{
		return gIntegralPlaceholder;
	}
};

template<>
struct MakeType<PromotedArithmetic>
{
	static BuiltInType apply()
	{
		return gPromotedArithmeticPlaceholder;
	}
};

template<>
struct MakeType<PromotedIntegral>
{
	static BuiltInType apply()
	{
		return gPromotedIntegralPlaceholder;
	}
};

struct MakeTemplate
{
	static BuiltInType apply(Declaration* declaration, BuiltInType a1, BuiltInType a2)
	{
		SimpleType result(declaration, 0);
		result.templateArguments.push_back(a1);
		result.templateArguments.push_back(a2);
		return BuiltInType(pushBuiltInType(gUniqueTypeNull, result));
	}
};

template<typename T, int i>
struct MakeType< Template<T, i> >
{
	static BuiltInType apply()
	{
		return MakeTemplate::apply(&gTemplateClassDeclaration, MakeType<T>::apply(), BuiltInNonType(IntegralConstant(i)));
	}
};


template<typename T>
struct MakeType< Template<T, NONTYPE_PARAM> >
{
	static BuiltInType apply()
	{
		return MakeTemplate::apply(&gTemplateClassDeclaration,  MakeType<T>::apply(), gDependentNonType);
	}
};

template<>
struct MakeType<T>
{
	static BuiltInType apply()
	{
		return gTemplateParameter;
	}
};

struct MakeTemplateTemplateParameter
{
	static BuiltInType apply(Declaration* declaration, BuiltInType a1, BuiltInType a2)
	{
		TemplateArgumentsInstance templateArguments;
		templateArguments.push_back(a1);
		templateArguments.push_back(a2);
		DependentType result(declaration, templateArguments, 2);
		return BuiltInType(pushBuiltInType(gUniqueTypeNull, result));
	}
};

template<typename T, int i>
struct MakeType< TT<T, i> >
{
	static BuiltInType apply()
	{
		return MakeTemplateTemplateParameter::apply(&gTemplateTemplateParameterDeclaration, MakeType<T>::apply(), BuiltInNonType(IntegralConstant(i)));
	}
};

template<typename T>
struct MakeType< TT<T, NONTYPE_PARAM> >
{
	static BuiltInType apply()
	{
		return MakeTemplateTemplateParameter::apply(&gTemplateTemplateParameterDeclaration, MakeType<T>::apply(), gDependentNonType);
	}
};

struct MakeConst
{
	static BuiltInType apply(BuiltInType inner)
	{
		inner.value.setQualifiers(CvQualifiers(true, false));
		return inner;
	}
};

template<typename T>
struct MakeType<const T>
{
	static BuiltInType apply()
	{
		return MakeConst::apply(MakeType<T>::apply());
	}
};

struct MakeVolatile
{
	static BuiltInType apply(BuiltInType inner)
	{
		inner.value.setQualifiers(CvQualifiers(false, true));
		return inner;
	}
};

template<typename T>
struct MakeType<volatile T>
{
	static BuiltInType apply()
	{
		return MakeVolatile::apply(MakeType<T>::apply());
	}
};

struct MakeConstVolatile
{
	static BuiltInType apply(BuiltInType inner)
	{
		inner.value.setQualifiers(CvQualifiers(true, true));
		return inner;
	}
};

template<typename T>
struct MakeType<const volatile T>
{
	static BuiltInType apply()
	{
		return MakeConstVolatile::apply(MakeType<T>::apply());
	}
};

template<typename T>
struct MakeType<T*>
{
	static BuiltInType apply()
	{
		return pushType(MakeType<T>::apply(), PointerType());
	}
};

template<typename T>
struct MakeType<T&>
{
	static BuiltInType apply()
	{
		return pushType(MakeType<T>::apply(), ReferenceType());
	}
};

template<std::size_t size>
struct MakeArray
{
	static BuiltInType apply(BuiltInType inner)
	{
		return pushType(inner, ArrayType(size));
	}
};

template<typename T>
struct MakeType<T[]>
{
	static BuiltInType apply()
	{
		return MakeArray<0>::apply(MakeType<T>::apply());
	}
};

template<typename T, std::size_t size>
struct MakeType<T[size]>
{
	static BuiltInType apply()
	{
		return MakeArray<size>::apply(MakeType<T>::apply());
	}
};

struct MakeFunction
{
	static BuiltInType apply(BuiltInType inner)
	{
		return makeFunctionType(inner);
	}
};

template<typename A1>
struct MakeFunction1
{
	static BuiltInType apply(BuiltInType inner)
	{
		return makeFunctionType(inner, MakeType<A1>::apply());
	}
};

template<typename T>
struct MakeType<T()>
{
	static BuiltInType apply()
	{
		return MakeFunction::apply(MakeType<T>::apply());
	}
};

template<typename T, typename A1>
struct MakeType<T(A1)>
{
	static BuiltInType apply()
	{
		return MakeFunction1<A1>::apply(MakeType<T>::apply());
	}
};

template<typename C>
struct MakeMemberPointer
{
	static BuiltInType apply(BuiltInType inner)
	{
		return pushType(inner, MemberPointerType(MakeType<C>::apply()));
	}
};

template<typename T, typename C>
struct MakeType<T C::*>
{
	static BuiltInType apply()
	{
		return MakeMemberPointer<C>::apply(MakeType<T>::apply());
	}
};

template<typename T, typename C>
struct MakeType<T (C::*)()>
{
	static BuiltInType apply()
	{
		return MakeMemberPointer<C>::apply(MakeFunction::apply(MakeType<T>::apply()));
	}
};

template<typename T, typename C, typename A1>
struct MakeType<T (C::*)(A1)>
{
	static BuiltInType apply()
	{
		return MakeMemberPointer<C>::apply(MakeFunction1<A1>::apply(MakeType<T>::apply()));
	}
};

template<typename T, typename C>
struct MakeType<T (C::*)() const>
{
	static BuiltInType apply()
	{
		return MakeMemberPointer<C>::apply(MakeConst::apply(MakeFunction::apply(MakeType<T>::apply())));
	}
};

template<typename T, typename C, typename A1>
struct MakeType<T (C::*)(A1) const>
{
	static BuiltInType apply()
	{
		return MakeMemberPointer<C>::apply(MakeConst::apply(MakeFunction1<A1>::apply(MakeType<T>::apply())));
	}
};

#if 0
template<BuiltInTypeId* type>
struct BuiltIn
{
};

template<BuiltInTypeId* type>
struct MakeType<BuiltIn<type> >
{
	static BuiltInType apply()
	{
		return *type;
	}
};

typedef BuiltIn<&gChar> BuiltInChar;
typedef BuiltIn<&gSignedChar> BuiltInSignedChar;
typedef BuiltIn<&gUnsignedChar> BuiltInUnsignedChar;
typedef BuiltIn<&gSignedShortInt> BuiltInSignedShortInt;
typedef BuiltIn<&gUnsignedShortInt> BuiltInUnsignedShortInt;
typedef BuiltIn<&gSignedInt> BuiltInSignedInt;
typedef BuiltIn<&gUnsignedInt> BuiltInUnsignedInt;
typedef BuiltIn<&gSignedLongInt> BuiltInSignedLongInt;
typedef BuiltIn<&gUnsignedLongInt> BuiltInUnsignedLongInt;
typedef BuiltIn<&gSignedLongLongInt> BuiltInSignedLongLongInt;
typedef BuiltIn<&gUnsignedLongLongInt> BuiltInUnsignedLongLongInt;
typedef BuiltIn<&gWCharT> BuiltInWCharT;
typedef BuiltIn<&gBool> BuiltInBool;
typedef BuiltIn<&gFloat> BuiltInFloat;
typedef BuiltIn<&gDouble> BuiltInDouble;
typedef BuiltIn<&gLongDouble> BuiltInLongDouble;

typedef TYPELIST13(
	BuiltInChar,
	BuiltInSignedChar,
	BuiltInUnsignedChar,
	BuiltInSignedShortInt,
	BuiltInUnsignedShortInt,
	BuiltInSignedInt,
	BuiltInUnsignedInt,
	BuiltInSignedLongInt,
	BuiltInUnsignedLongInt,
	BuiltInSignedLongLongInt,
	BuiltInUnsignedLongLongInt,
	BuiltInWCharT,
	BuiltInBool
) IntegralTypes;

typedef TYPELIST6(
	BuiltInSignedInt,
	BuiltInUnsignedInt,
	BuiltInSignedLongInt,
	BuiltInUnsignedLongInt,
	BuiltInSignedLongLongInt,
	BuiltInUnsignedLongLongInt
) PromotedIntegralTypes;

typedef TYPELIST16(
	BuiltInChar,
	BuiltInSignedChar,
	BuiltInUnsignedChar,
	BuiltInSignedShortInt,
	BuiltInUnsignedShortInt,
	BuiltInSignedInt,
	BuiltInUnsignedInt,
	BuiltInSignedLongInt,
	BuiltInUnsignedLongInt,
	BuiltInSignedLongLongInt,
	BuiltInUnsignedLongLongInt,
	BuiltInWCharT,
	BuiltInBool,
	BuiltInFloat,
	BuiltInDouble,
	BuiltInLongDouble
) ArithmeticTypes;

template<typename Types, template<typename> class Op>
struct GeneratedTypeArray : UniqueTypeArray
{
	GeneratedTypeArray()
	{
		iterate(Types());
	}
	template<typename Item, typename Next>
	void iterate(TypeList<Item, Next>)
	{
		push_back(Op<Item>::apply());
	}
	void iterate(TypeListEnd)
	{
	}
	BuiltInTypeArrayRange makeRange() const
	{
		return BuiltInTypeArrayRange(&(*begin()), &(*end()));
	}
};
#endif


inline void testTypeGen()
{
	typedef const int ConstInt;
	typedef ConstInt* PtrToConstInt;
	typedef int* PtrToInt;
	typedef const PtrToInt ConstPtrToInt;
	typedef const PtrToConstInt ConstPtrToConstInt;
	BuiltInType test1 = MakeType<ConstInt>::apply();
	BuiltInType test2 = MakeType<PtrToConstInt>::apply();
	BuiltInType test3 = MakeType<ConstPtrToInt>::apply();
	BuiltInType test4 = MakeType<ConstPtrToConstInt>::apply();

	BuiltInType test5 = MakeType<int (*)()>::apply();
	BuiltInType test6 = MakeType<int (Base::*)()>::apply();
	BuiltInType test7 = MakeType<int (Base::*)() const>::apply();
	BuiltInType test8 = MakeType<int (Base::*const)()>::apply();

	BuiltInType test9 = MakeType<int[]>::apply();
	BuiltInType test10 = MakeType<int[1]>::apply();

	BuiltInType test11 = MakeType<int(int)>::apply();
	BuiltInType test12 = MakeType<int (*)(int)>::apply();
	BuiltInType test13 = MakeType<int (Base::*)(int)>::apply();
	BuiltInType test14 = MakeType<int (Base::*)(int) const>::apply();
	BuiltInType test15 = MakeType<int (Base::*const)(int)>::apply();

	BuiltInType test16 = MakeType< Template<int, 1> >::apply();

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


BuiltInType gSignedIntMemberPointer = MakeType<int Base::*>::apply();


BuiltInType gFloatingTypes[] = {
	gFloat,
	gDouble,
	gLongDouble,
};

BuiltInType gIntegerTypes[] = {
	gChar,
	gSignedChar,
	gUnsignedChar,
	gSignedShortInt,
	gUnsignedShortInt,
	gSignedInt,
	gUnsignedInt,
	gSignedLongInt,
	gUnsignedLongInt,
	gSignedLongLongInt,
	gUnsignedLongLongInt,
	gWCharT,
	gBool,
};

BuiltInType gPointerTypes[] = {
	gVoidPointer,
	gSignedIntPointer,
	gSignedIntPointerPointer,
	gSignedCharPointer,
	gWCharTPointer,
	gBaseClassPointer,
	gDerivedClassPointer,
};

BuiltInType gSimplePointerTypes[] = {
	gSignedIntPointer,
	gSignedCharPointer,
	gWCharTPointer,
	gBaseClassPointer,
	gDerivedClassPointer,
};

BuiltInType gMemberPointerTypes[] = {
	gSignedIntMemberPointer,
};

IcsRank getArithmeticIcsRank(BuiltInType to, BuiltInType from)
{
	if(to == from)
	{
		return ICSRANK_STANDARDEXACT;
	}
	if(to == gSignedInt)
	{
		if(from == gChar
			|| from == gSignedChar
			|| from == gUnsignedChar
			|| from == gSignedShortInt
			|| from == gUnsignedShortInt
			|| from == gWCharT
			|| from == gBool)
		{
			return ICSRANK_STANDARDPROMOTION;
		}
	}
	// TODO bitfield -> integer
	if(to == gDouble)
	{
		if(from == gFloat)
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
			BuiltInType matched = MakeType<Matched>::apply();
			SYMBOLS_ASSERT(conversion.sequence.matched == matched);
		}
	}
};

inline void testIcsRank()
{
	for(const BuiltInType* i = gArithmeticTypes; i != ARRAY_END(gArithmeticTypes); ++i)
	{
		BuiltInType to = *i;
		for(const BuiltInType* i = gArithmeticTypes; i != ARRAY_END(gArithmeticTypes); ++i)
		{
			BuiltInType from = *i;
			IcsRank expected = getArithmeticIcsRank(to, from);
			IcsRank rank = getIcsRank(to, from, Location(), 0);
			SYMBOLS_ASSERT(expected == rank);
		}
	}

	// 0 -> T*
	for(const BuiltInType* i = gIntegerTypes; i != ARRAY_END(gIntegerTypes); ++i)
	{
		BuiltInType type = *i;
		for(const BuiltInType* i = gPointerTypes; i != ARRAY_END(gPointerTypes); ++i)
		{
			BuiltInType other = *i;
			{
				IcsRank rank = getIcsRank(other, type, Location(), 0, true);
				SYMBOLS_ASSERT(rank == ICSRANK_STANDARDCONVERSION);
			}
			{
				IcsRank rank = getIcsRank(other, type, Location(), 0, false);
				SYMBOLS_ASSERT(rank == ICSRANK_INVALID);
			}
		}
	}

	// T* -> bool, T::* -> bool
	for(const BuiltInType* i = gIntegerTypes; i != ARRAY_END(gIntegerTypes); ++i)
	{
		BuiltInType type = *i;
		for(const BuiltInType* i = gPointerTypes; i != ARRAY_END(gPointerTypes); ++i)
		{
			BuiltInType other = *i;
			IcsRank rank = getIcsRank(type, other, Location(), 0);
			SYMBOLS_ASSERT(rank == (type == gBool ? ICSRANK_STANDARDCONVERSION : ICSRANK_INVALID));
		}
		for(const BuiltInType* i = gMemberPointerTypes; i != ARRAY_END(gMemberPointerTypes); ++i)
		{
			BuiltInType other = *i;
			IcsRank rank = getIcsRank(type, other, Location(), 0);
			SYMBOLS_ASSERT(rank == (type == gBool ? ICSRANK_STANDARDCONVERSION : ICSRANK_INVALID));
		}
	}

	// T* -> void* (where T is an object type)
	for(const BuiltInType* i = gSimplePointerTypes; i != ARRAY_END(gSimplePointerTypes); ++i)
	{
		BuiltInType type = *i;
		{
			IcsRank rank = getIcsRank(gVoidPointer, type, Location(), 0);
			SYMBOLS_ASSERT(rank == ICSRANK_STANDARDCONVERSION);
		}
		{
			IcsRank rank = getIcsRank(gConstVoidPointer, type, Location(), 0);
			SYMBOLS_ASSERT(rank == ICSRANK_STANDARDCONVERSION);
		}
	}
	for(const BuiltInType* i = gFloatingTypes; i != ARRAY_END(gFloatingTypes); ++i)
	{
		BuiltInType type = *i;
		{
			IcsRank rank = getIcsRank(gVoidPointer, type, Location(), 0, true);
			SYMBOLS_ASSERT(rank == ICSRANK_INVALID);
		}
		{
			IcsRank rank = getIcsRank(type, gVoidPointer, Location(), 0, true);
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
	TestIcsRank<PointerToAny, Base*, Base*>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<PointerToAny, int*, int*>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<PointerToAny, float*, float*>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<PointerToAny, void(*)(), void(*)()>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<PointerToAny, int>::match(ICSRANK_INVALID);

	TestIcsRank<PointerToClass, Base*, Base*>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<PointerToClass, int*>::match(ICSRANK_INVALID);

	TestIcsRank<PointerToObject, Base*, Base*>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<PointerToObject, int*, int*>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<PointerToObject, void**, void**>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<PointerToObject, void(*)()>::match(ICSRANK_INVALID);
	TestIcsRank<PointerToObject, void*>::match(ICSRANK_INVALID);

	TestIcsRank<PointerToFunction, void(*)(),  void(*)()>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<PointerToFunction, Base*>::apply(ICSRANK_INVALID);
	TestIcsRank<PointerToFunction, int*>::match(ICSRANK_INVALID);

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

	TestIcsRank<PointerToMember, int Base::*, int Base::*>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<PointerToMember&, int Base::*&, int Base::*>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<PointerToMember volatile&, int Base::* volatile&, int Base::*>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<PointerToMember, int*>::match(ICSRANK_INVALID);

	TestIcsRank<PointerToAny&, Base*&,Base*>::match(ICSRANK_STANDARDEXACT);
	TestIcsRank<PointerToAny volatile&, Base* volatile&, Base*>::match(ICSRANK_STANDARDEXACT);
}


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


template<typename P, typename A, typename R = void>
struct TestSubstitution
{
	static void apply(UniqueTypeWrapper expected = MakeType<R>::apply())
	{
		TemplateArgumentsInstance templateArguments(1, MakeType<A>::apply());
		SimpleType enclosing(&gTemplateClassDeclaration, 0);
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

int testAll()
{
	testTypeGen();
	testIcsRank();
	testDeduction();
	testOrdering();
	testSubstitution();
	return 0;
}


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
	InstantiationName(const SimpleType& instance)
		: Concatenate(
			makeRange(getValue(instance.declaration->getName())),
			makeRange(InstantiationSuffix(&instance).c_str()))
	{
	}
};

struct InstantiationPath : public Concatenate
{
	InstantiationPath(const SimpleType& instance)
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

void printTypeReadable(const SimpleType& type, std::ostream& out, bool escape = true)
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

inline void dumpTemplateInstantiations(const SimpleType& instance, bool root)
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

	typedef std::map<const SimpleType*, Location> InstanceMap;
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
