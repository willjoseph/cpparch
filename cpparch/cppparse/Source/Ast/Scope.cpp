
#include "Scope.h"

UniqueNames gUniqueNames;

size_t gScopeCount = 0;


// special-case
Identifier gUndeclaredId = makeIdentifier("$undeclared");
Declaration gUndeclared(AST_ALLOCATOR_NULL, 0, gUndeclaredId, TYPE_NULL, 0, false);
const DeclarationInstance gUndeclaredInstance(&gUndeclared);

Identifier gGlobalId = makeIdentifier("$global");
Identifier gAnonymousId = makeIdentifier("$anonymous");
