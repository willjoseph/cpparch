
#include "Type.h"
#include "Ast/AstAllocator.h"

const TypeElementEmpty gTypeElementEmpty;
UniqueTypes gUniqueTypes;
UniqueTypes gBuiltInTypes;

// meta types
Identifier gArithmeticId = makeIdentifier("$arithmetic");
Declaration gArithmetic(AST_ALLOCATOR_NULL, 0, gArithmeticId, TYPE_NULL, 0, false);
Identifier gSpecialId = makeIdentifier("$special");
Declaration gSpecial(AST_ALLOCATOR_NULL, 0, gSpecialId, TYPE_NULL, 0, false);
Identifier gClassId = makeIdentifier("$class");
Declaration gClass(AST_ALLOCATOR_NULL, 0, gClassId, TYPE_NULL, 0, false);
Identifier gUsingId = makeIdentifier("$using");
Declaration gUsing(AST_ALLOCATOR_NULL, 0, gUsingId, TYPE_NULL, 0, false);
Identifier gEnumId = makeIdentifier("$enum");
Declaration gEnum(AST_ALLOCATOR_NULL, 0, gEnumId, TYPE_NULL, 0, false);


// types
Identifier gNamespaceId = makeIdentifier("$namespace");
Declaration gNamespace(AST_ALLOCATOR_NULL, 0, gNamespaceId, TYPE_NULL, 0, false);

Identifier gCtorId = makeIdentifier("$ctor");
Declaration gCtor(AST_ALLOCATOR_NULL, 0, gCtorId, TYPE_SPECIAL, 0, false);
Identifier gEnumeratorId = makeIdentifier("$enumerator");
Declaration gEnumerator(AST_ALLOCATOR_NULL, 0, gEnumeratorId, TYPE_SPECIAL, 0, false);
Identifier gUnknownId = makeIdentifier("$unknown");
Declaration gUnknown(AST_ALLOCATOR_NULL, 0, gUnknownId, TYPE_SPECIAL, 0, false);
Identifier gBuiltInOperatorId = makeIdentifier("$built-in");
Declaration gBuiltInOperator(AST_ALLOCATOR_NULL, 0, gBuiltInOperatorId, TYPE_SPECIAL, 0, false);


// template placeholders
Identifier gDependentTypeId = makeIdentifier("$type");
Declaration gDependentType(AST_ALLOCATOR_NULL, 0, gDependentTypeId, TYPE_SPECIAL, 0, true);
const DeclarationInstance gDependentTypeInstance(&gDependentType);
Identifier gDependentObjectId = makeIdentifier("$object");
Declaration gDependentObject(AST_ALLOCATOR_NULL, 0, gDependentObjectId, TYPE_UNKNOWN, 0, false);
const DeclarationInstance gDependentObjectInstance(&gDependentObject);
Identifier gDependentTemplateId = makeIdentifier("$template");
Declaration gDependentTemplate(AST_ALLOCATOR_NULL, 0, gDependentTemplateId, TYPE_SPECIAL, 0, true, DeclSpecifiers(), true);
const DeclarationInstance gDependentTemplateInstance(&gDependentTemplate);
Identifier gDependentNestedId = makeIdentifier("$nested");
Declaration gDependentNested(AST_ALLOCATOR_NULL, 0, gDependentNestedId, TYPE_SPECIAL, 0, true);
const DeclarationInstance gDependentNestedInstance(&gDependentNested);
Identifier gDependentNestedTemplateId = makeIdentifier("$nested-template");
Declaration gDependentNestedTemplate(AST_ALLOCATOR_NULL, 0, gDependentNestedTemplateId, TYPE_SPECIAL, 0, true, DeclSpecifiers(), true);
const DeclarationInstance gDependentNestedTemplateInstance(&gDependentNestedTemplate);

Identifier gParamId = makeIdentifier("$param");
Declaration gParam(AST_ALLOCATOR_NULL, 0, gParamId, TYPE_CLASS, 0, true);
Identifier gNonTypeId = makeIdentifier("$non-type");
Declaration gNonType(AST_ALLOCATOR_NULL, 0, gNonTypeId, TYPE_UNKNOWN, 0, false);

Identifier gOverloadedId = makeIdentifier("$overloaded");
BuiltInTypeDeclaration gOverloadedDeclaration(gOverloadedId, TYPE_SPECIAL);
BuiltInTypeId gOverloaded(&gOverloadedDeclaration, AST_ALLOCATOR_NULL);

