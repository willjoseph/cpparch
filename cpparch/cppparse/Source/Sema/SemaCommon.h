
#ifndef INCLUDED_CPPPARSE_SEMA_SEMACOMMON_H
#define INCLUDED_CPPPARSE_SEMA_SEMACOMMON_H

#include "Ast/Type.h"
#include "Core/NameLookup.h"
#include "Core/Special.h"
#include "Core/TypeUnique.h"

struct SemanticError
{
	SemanticError()
	{
#ifdef ALLOCATOR_DEBUG
		DEBUG_BREAK();
#endif
	}
};

#define SEMANTIC_ASSERT(condition) if(!(condition)) { /* printPosition(context.parserContext.get_position()); std::cout << "semantic error" << std::endl;*/ throw SemanticError(); }

inline void semanticBreak()
{
}

inline void printDeclarations(const Scope::Declarations& declarations)
{
	std::cout << "{ ";
	for(Scope::Declarations::const_iterator i = declarations.begin(); i != declarations.end();)
	{
		std::cout << getValue((*i).second->getName());
		if(++i != declarations.end())
		{
			std::cout << ", ";
		}
	}
	std::cout << " }";
}

inline void printBases(const Types& bases)
{
	std::cout << "{ ";
	for(Types::const_iterator i = bases.begin(); i != bases.end();)
	{
		std::cout << getValue((*i).declaration->getName()) << ": ";
		Scope* scope = (*i).declaration->enclosed;
		if(scope != 0)
		{
			printDeclarations((*i).declaration->enclosed->declarations);
		}
		if(++i != bases.end())
		{
			std::cout << ", ";
		}
	}
	std::cout << " }";
}

inline void printScope(const Scope& scope)
{
	std::cout << getValue(scope.name) << ": ";
	std::cout << std::endl;
	std::cout << "  declarations: ";
	printDeclarations(scope.declarations);
	std::cout << std::endl;
	std::cout << "  bases: ";
	printBases(scope.bases);
	std::cout << std::endl;
	if(scope.parent != 0)
	{
		printScope(*scope.parent);
	}
}

inline void printName(const Scope& scope)
{
	if(scope.parent != 0)
	{
		printName(*scope.parent);
		std::cout << "::";
		std::cout << getValue(scope.name);
	}
}


struct IdentifierMismatch
{
	Identifier id;
	const char* expected;
	DeclarationPtr declaration;
	IdentifierMismatch()
	{
	}
	IdentifierMismatch(const Identifier& id, Declaration* declaration, const char* expected) :
	id(id), declaration(declaration), expected(expected)
	{
	}
};

extern IdentifierMismatch gIdentifierMismatch;

inline void printIdentifierMismatch(const IdentifierMismatch& e)
{
	printPosition(e.id.source);
	std::cout << "'" << getValue(e.id) << "' expected " << e.expected << ", " << (e.declaration == &gUndeclared ? "was undeclared" : "was declared here:") << std::endl;
	if(e.declaration != &gUndeclared)
	{
		printPosition(e.declaration->getName().source);
		std::cout << std::endl;
	}
}


inline void setDecoration(Identifier* id, const DeclarationInstance& declaration)
{
	SEMANTIC_ASSERT(declaration.name != 0);
	SEMANTIC_ASSERT(id != &gAnonymousId);
	id->dec.p = &declaration;
}




inline bool isEqual(const TypeId& l, const TypeId& r)
{
	SYMBOLS_ASSERT(l.unique != 0);
	SYMBOLS_ASSERT(r.unique != 0);
	return l.unique == r.unique;
}

inline bool isEqual(const Type& left, const Type& right)
{
	SYMBOLS_ASSERT(left.unique != 0);
	SYMBOLS_ASSERT(right.unique != 0);
	return left.unique == right.unique;
}

inline bool isEqual(const TemplateArgument& l, const TemplateArgument& r)
{
	if((l.type.declaration == &gNonType)
		!= (r.type.declaration == &gNonType))
	{
		return false;
	}
	return l.type.declaration == &gNonType
		? l.expression.p == r.expression.p
		: isEqual(l.type, r.type);
}

inline bool matchTemplateSpecialization(const Declaration& declaration, const TemplateArguments& arguments)
{
	// TODO: check that all arguments are specified!
	TemplateArguments::const_iterator a = arguments.begin();
	for(TemplateArguments::const_iterator i = declaration.templateArguments.begin(); i != declaration.templateArguments.end(); ++i)
	{
		SYMBOLS_ASSERT(a != arguments.end()); // a template-specialization must have no more arguments than the template parameters

		if(!isEqual(*i, *a))
		{
			return false;
		}
		++a;
	}
	SYMBOLS_ASSERT(a == arguments.end());
	return true;
}

inline Declaration* findTemplateSpecialization(Declaration* declaration, const TemplateArguments& arguments)
{
	for(; declaration != 0; declaration = declaration->overloaded)
	{
		if(!isSpecialization(*declaration))
		{
			continue;
		}

		if(matchTemplateSpecialization(*declaration, arguments))
		{
			return declaration;
		}
	}
	return 0;
}


inline bool isAnonymous(const Declaration& declaration)
{
	return *declaration.getName().value.c_str() == '$';
}

struct DeclarationError
{
	const char* description;
	DeclarationError(const char* description) : description(description)
	{
	}
};



// int i; // type -> int
// typedef int I; // type -> int
// I i; // type -> I -> int
// typedef I J; // type -> I -> int
// J j; // type -> J -> I -> int
// struct S; // type -> struct
// typedef struct S S; // type -> S -> struct
// typedef struct S {} S; // type -> S -> struct
// typedef enum E {} E; // type -> E -> enum

// returns the type of a declaration
// int i; -> built-in
// class A a; -> A
// enum E e; -> E
// typedef int T; T t; -> built-in
inline const Declaration* getType(const Declaration& declaration)
{
	if(declaration.specifiers.isTypedef)
	{
		return getType(*declaration.type.declaration);
	}
	return declaration.type.declaration;
}

inline const Declaration& getPrimaryDeclaration(const Declaration& first, const Declaration& second)
{
	if(isNamespace(first))
	{
		if(!isNamespace(second))
		{
			throw DeclarationError("non-namespace already declared as namespace");
		}
		return first; // namespace continuation
	}
	if(isUsing(first)
		|| isUsing(second))
	{
		return second;
	}
	if(isType(first))
	{
		if(!isType(second))
		{
			throw DeclarationError("non-type already declared as type");
		}
		if(getType(first) != getType(second))
		{
			throw DeclarationError("type already declared as different type");
		}
		if(isTypedef(first))
		{
			return second; // redeclaration of typedef, or definition of type previously used in typedef
		}
		if(isTypedef(second))
		{
			return second; // typedef of type previously declared: typedef struct S {} S;
		}
		if(isClass(first))
		{
			if(isSpecialization(second))
			{
				return second; // TODO: class template partial/explicit-specialization
			}
			if(isSpecialization(first))
			{
				return second; // TODO: class template partial/explicit-specialization
			}
			if(isIncomplete(second))
			{
				return second; // redeclaration of previously-declared class
			}
			if(isIncomplete(first))
			{
				return second; // definition of forward-declared class
			}
			throw DeclarationError("class-definition already defined");
		}
		if(isEnum(first))
		{
			throw DeclarationError("enum-definition already defined");
		}
		throw SymbolsError(); // should not be reachable
	}
	if(isType(second))
	{
		throw DeclarationError("type already declared as non-type");
	}
	if(isFunction(first)
		|| isFunction(second))// TODO: function overloading
	{
		return second; // multiple declarations allowed
	}
	if(isStaticMember(first))
	{
		// TODO: disallow inline definition of static member: class C { static int i; int i; };
		if(!isDataMember(second))
		{
			throw DeclarationError("non-member-object already declared as static member-object");
		}
		return second; // multiple declarations allowed
	}
	if(isExtern(first)
		|| isExtern(second))
	{
		return second; // multiple declarations allowed
	}
	// HACK: ignore multiple declarations for members of template - e.g. const char Tmpl<char>::VALUE; const int Tmpl<int>::VALUE;
	if(!first.templateParams.defaults.empty())
	{
		// if enclosing is a template
		return first;
	}
	throw DeclarationError("symbol already defined");
}

inline bool hasTemplateParamDefaults(const TemplateParameters& params)
{
	for(TemplateArguments::const_iterator i = params.defaults.begin(); i != params.defaults.end(); ++i)
	{
		if((*i).type.declaration != 0)
		{
			return true;
		}
	}
	return false;
}

// substitute references to template-parameters of 'otherParams' for template-parameters of 'params'
inline void fixTemplateParamDefault(TemplateArgument& argument, const TemplateParameters& params, const TemplateParameters& otherParams)
{
	if(argument.type.declaration == 0)
	{
		return;
	}
	std::size_t index = argument.type.declaration->templateParameter;
	if(index != INDEX_INVALID)
	{
		Types::const_iterator i = params.begin();
		std::advance(i, index);
		Types::const_iterator j = otherParams.begin();
		std::advance(j, index);
		if(argument.type.declaration->scope == (*j).declaration->scope)
		{
			argument.type.declaration = (*i).declaration;
		}
	}
	for(TemplateArguments::iterator i = argument.type.templateArguments.begin(); i != argument.type.templateArguments.end(); ++i)
	{
		fixTemplateParamDefault(*i, params, otherParams);
	}
}

inline void copyTemplateParamDefault(TemplateArgument& argument, const TemplateArgument& otherArgument, const TemplateParameters& params, const TemplateParameters& otherParams)
{
	argument = otherArgument;
	fixTemplateParamDefault(argument, params, otherParams);
}

inline void copyTemplateParamDefaults(TemplateParameters& params, const TemplateParameters& otherParams)
{
	SYMBOLS_ASSERT(params.defaults.empty());
	for(TemplateArguments::const_iterator i = otherParams.defaults.begin(); i != otherParams.defaults.end(); ++i)
	{
		params.defaults.push_back(TEMPLATEARGUMENT_NULL);
		copyTemplateParamDefault(params.defaults.back(), *i, params, otherParams);
	}
}

/// 14.1-10: the set of template param defaults is obtained by merging those from the definition and all declarations currently in scope (excluding explicit-specializations)
inline void mergeTemplateParamDefaults(TemplateParameters& params, const TemplateParameters& otherParams)
{
	if(params.defaults.empty())
	{
		copyTemplateParamDefaults(params, otherParams);
		return;
	}
	if(!hasTemplateParamDefaults(otherParams)) // ignore declarations with no default-arguments, e.g. explicit/partial-specializations
	{
		return;
	}
	SYMBOLS_ASSERT(!otherParams.defaults.empty());
	TemplateArguments::iterator d = params.defaults.begin();
	for(TemplateArguments::const_iterator i = otherParams.defaults.begin(); i != otherParams.defaults.end(); ++i)
	{
		SYMBOLS_ASSERT(d != params.defaults.end());
		SYMBOLS_ASSERT((*d).type.declaration == 0 || (*i).type.declaration == 0); // TODO: non-fatal error: default param defined more than once
		if((*d).type.declaration == 0)
		{
			copyTemplateParamDefault(*d, *i, params, otherParams);
		}
		++d;
	}
	SYMBOLS_ASSERT(d == params.defaults.end());
}

inline void mergeTemplateParamDefaults(Declaration& declaration, const TemplateParameters& templateParams)
{
	SYMBOLS_ASSERT(declaration.isTemplate);
	SYMBOLS_ASSERT(isClass(declaration));
	SYMBOLS_ASSERT(!isSpecialization(declaration)); // explicit/partial-specializations cannot have default-arguments
	mergeTemplateParamDefaults(declaration.templateParams, templateParams);
	SYMBOLS_ASSERT(!declaration.templateParams.defaults.empty());
}

//-----------------------------------------------------------------------------

inline bool isFunctionParameterEquivalent(UniqueTypeWrapper left, UniqueTypeWrapper right)
{
	return adjustFunctionParameter(left) == adjustFunctionParameter(right);
}

inline bool isEquivalent(const ParameterTypes& left, const ParameterTypes& right)
{
	ParameterTypes::const_iterator l = left.begin();
	ParameterTypes::const_iterator r = right.begin();
	for(;; ++l, ++r)
	{
		if(l == left.end())
		{
			return r == right.end();
		}
		if(r == right.end())
		{
			return false;
		}
		if(!isFunctionParameterEquivalent(*l, *r))
		{
			return false;
		}
	}
	return true;
}

inline bool isReturnTypeEqual(UniqueTypeWrapper left, UniqueTypeWrapper right)
{
	SYMBOLS_ASSERT(left.isFunction());
	SYMBOLS_ASSERT(right.isFunction());
	return isEqualInner(left, right);
}

inline bool isEquivalentSpecialization(const Declaration& declaration, const Declaration& other)
{
	return !(isComplete(declaration) && isComplete(other)) // if both are complete, assume that they have different argument lists!
		&& matchTemplateSpecialization(declaration, other.templateArguments);
}

inline bool isEquivalentTypedef(const Declaration& declaration, const Declaration& other)
{
	return getType(declaration) == getType(other);
}

inline bool isEquivalentTemplateParameter(const Type& left, const Type& right)
{
	extern Declaration gParam;
	if((left.declaration->type.declaration == &gParam)
		!= (right.declaration->type.declaration == &gParam))
	{
		return false;
	}
	return left.declaration->type.declaration == &gParam
		? isEqual(left, right)
		: isEqual(left.declaration->type, right.declaration->type);
}

inline bool isEquivalentTemplateParameters(const TemplateParameters& left, const TemplateParameters& right)
{
	if(std::distance(left.begin(), left.end()) != std::distance(right.begin(), right.end()))
	{
		return false;
	}
	TemplateParameters::const_iterator l = left.begin();
	for(TemplateParameters::const_iterator r = right.begin(); r != right.end(); ++l, ++r)
	{
		SYMBOLS_ASSERT(l != left.end());

		if(!isEquivalentTemplateParameter(*l, *r))
		{
			return false;
		}
	}
	SYMBOLS_ASSERT(l == left.end());
	return true;
}

// returns true if both declarations refer to the same explicit specialization of a class member
inline bool isEquivalentMemberSpecialization(const Declaration& declaration, const Declaration& other)
{
	SYMBOLS_ASSERT(declaration.isSpecialization && other.isSpecialization);
	SYMBOLS_ASSERT(declaration.enclosingType != 0 && other.enclosingType != 0);
	return declaration.enclosingType == other.enclosingType; // return true if both are members of the same unique class
}

inline bool isEquivalent(const Declaration& declaration, const Declaration& other)
{
	if(isClass(declaration)
		&& isClass(other))
	{
		// TODO: compare template-argument-lists of partial specializations
		return isSpecialization(declaration) == isSpecialization(other)
			&& (!isSpecialization(declaration) // both are not explicit/partial specializations
			|| isEquivalentSpecialization(declaration, other)); // both are specializations and have matching arguments
	}

	if(isUsing(declaration)
		|| isUsing(other))
	{
		return false; // TODO: defer evaluation of equivalence for dependent using-declaration
	}

	if(isEnum(declaration)
		|| isEnum(other))
	{
		return isEquivalentTypedef(declaration, other);
	}

	if(isClass(declaration)
		|| isClass(other))
	{
		return isEquivalentTypedef(declaration, other);
	}

	{
		UniqueTypeWrapper l = getUniqueType(declaration.type);
		UniqueTypeWrapper r = getUniqueType(other.type);
		if(l.isFunction())
		{
			if(declaration.specifiers.isFriend && isDependent(l)
				&& other.specifiers.isFriend && isDependent(r))
			{
				return false; // friend functions with dependent types cannot be declared more than once
			}

			// 13.2 [over.dcl] Two functions of the same name refer to the same function
			// if they are in the same scope and have equivalent parameter declarations.
			// TODO: also compare template parameter lists: <class, int> is not equivalent to <class, float>
			SYMBOLS_ASSERT(r.isFunction()); // TODO: non-fatal error: 'id' previously declared as non-function, second declaration is a function
			return declaration.isTemplate == other.isTemplate // early out
				&& isEquivalentTemplateParameters(declaration.templateParams, other.templateParams)
				&& l.value.getQualifiers() == r.value.getQualifiers()
				// [over.load] Function declarations that differ only in the return type cannot be overloaded.
				&& (declaration.getName().value == gConversionFunctionId
				? isReturnTypeEqual(l, r) // return-types match
				// (only template overloads may differ in return type, return-type is not used to distinguish overloads, except for conversion-function)
				: isEquivalent(getParameterTypes(l.value), getParameterTypes(r.value))); // and parameter-types match
		}
		// redeclaring an object, unless this is a new explicit specialization
		return isSpecialization(declaration) == isSpecialization(other)
			&& (!isSpecialization(declaration) // both are not explicit/partial specializations
			|| isEquivalentMemberSpecialization(declaration, other)); // both are specializations and have matching arguments
	}
	return false;
}

// Returns the most recently declared overload of the given declaration that has an equivalent signature.
inline const DeclarationInstance* findRedeclared(const Declaration& declaration, const DeclarationInstance* overloaded)
{
	for(const DeclarationInstance* p = overloaded; p != 0; p = p->overloaded)
	{
		if(isEquivalent(declaration, *(*p)))
		{
			return p;
		}
	}
	return 0;
}

inline UniqueTypeWrapper replaceArrayType(UniqueTypeWrapper type, std::size_t count)
{
	SEMANTIC_ASSERT(type.isArray());
	std::size_t bound = getArrayType(type.value).size;
	if(bound != 0)
	{
		SEMANTIC_ASSERT(count <= bound); // TODO: non-fatal error: initializer has more elements than array bound
		return type;
	}
	SEMANTIC_ASSERT(getArrayType(type.value).size == 0);
	return pushType(popType(type), ArrayType(count));
}




inline bool enclosesEts(ScopeType type)
{
	return type == SCOPETYPE_NAMESPACE
		|| type == SCOPETYPE_LOCAL;
}

inline const SimpleType* getEnclosingType(const SimpleType* enclosing)
{
	for(const SimpleType* i = enclosing; i != 0; i = (*i).enclosing)
	{
		if(!isAnonymousUnion(*i)) // ignore anonymous union
		{
			return i;
		}
	}
	return 0;
}

inline bool findScope(Scope* scope, Scope* other)
{
	if(scope == 0)
	{
		return false;
	}
	if(scope == other)
	{
		return true;
	}
	return findScope(scope->parent, other);
}

// Returns the innermost enclosing class or function scope
inline Scope* findEnclosingTemplateScope(Scope* scope)
{
	if(scope == 0)
	{
		return 0;
	}
	if(scope->type == SCOPETYPE_CLASS
		|| scope->type == SCOPETYPE_FUNCTION)
	{
		return scope;
	}
	return findEnclosingTemplateScope(scope->parent);
}

inline Declaration* findEnclosingClassTemplate(Declaration* dependent)
{
	if(dependent != 0
		&& (isClass(*dependent)
		|| isEnum(*dependent)) // type of enum within class template is dependent
		&& isMember(*dependent))
	{
		Scope* scope = getEnclosingClass(dependent->scope);
		if(scope == 0)
		{
			// enclosing class was anonymous and at namespace scope.
			return 0;
		}
		Declaration* declaration = getClassDeclaration(scope);
		return declaration->isTemplate
			? declaration
			: findEnclosingClassTemplate(declaration);
	}
	return 0;
}

inline bool isDependentImpl(Declaration* dependent, Scope* enclosing, Scope* enclosingTemplateScope)
{
	return dependent != 0
		&& (findScope(enclosing, dependent->scope) != 0
		|| findScope(enclosingTemplateScope, dependent->scope) != 0); // if we are within the candidate template-parameter's template-definition
}


struct SemaContext : public AstAllocator<int>
{
	ParserContext& parserContext;
	Scope global;
	Declaration globalDecl;
	TypeRef globalType;
	std::size_t declarationCount;
	ExpressionType typeInfoType;

	SemaContext(ParserContext& parserContext, const AstAllocator<int>& allocator) :
		AstAllocator<int>(allocator),
		parserContext(parserContext),
		global(allocator, gGlobalId, SCOPETYPE_NAMESPACE),
		globalDecl(allocator, 0, gGlobalId, TYPE_NAMESPACE, &global, false),
		globalType(Type(&globalDecl, allocator), allocator),
		declarationCount(0)
	{
	}
};

typedef std::list< DeferredParse<struct SemaState> > DeferredSymbolsList;

struct DeferredSymbols
{
	DeferredSymbolsList first;
	DeferredSymbolsList second;

	void splice(DeferredSymbols& other)
	{
		first.splice(first.end(), other.first);
		second.splice(second.end(), other.second);
	}
	bool empty() const
	{
		return first.empty() && second.empty();
	}
};


typedef bool (*IdentifierFunc)(const Declaration& declaration);
const char* getIdentifierType(IdentifierFunc func);


struct SemaState
	: public ContextBase
{
	typedef SemaState State;

	SemaContext& context;
	ScopePtr enclosingScope;
	const SimpleType* enclosingType;
	const SimpleType* enclosingFunction;
	Dependent enclosingDependent;
	TypePtr qualifying_p;
	DeclarationPtr qualifyingScope;
	const SimpleType* qualifyingClass;
	const SimpleType* memberClass;
	ExpressionWrapper objectExpression; // the lefthand side of a class member access expression
	SafePtr<const TemplateParameters> templateParams;
	ScopePtr enclosingTemplateScope;
	DeferredSymbols* enclosingDeferred;
	DeclarationPtr enclosingInstantiation; // the enclosing declaration which will be later instantiated
	DependentConstructs* enclosingDependentConstructs; // the container to which dependent types and expressions will be added
	std::size_t templateDepth;
	bool isExplicitInstantiation;

	SemaState(SemaContext& context)
		: context(context)
		, enclosingScope(0)
		, enclosingType(0)
		, enclosingFunction(0)
		, qualifying_p(0)
		, qualifyingScope(0)
		, qualifyingClass(0)
		, memberClass(0)
		, templateParams(0)
		, enclosingTemplateScope(0)
		, enclosingDeferred(0)
		, enclosingInstantiation(0)
		, enclosingDependentConstructs(0)
		, enclosingDependent(0)
		, templateDepth(0)
		, isExplicitInstantiation(false)
	{
	}
	const SemaState& getState() const
	{ 
		return *this;
	}
	Location getLocation() const
	{
		return Location(context.parserContext.get_source(), context.declarationCount);
	}
	InstantiationContext getInstantiationContext() const
	{
		return InstantiationContext(getLocation(), enclosingType, enclosingFunction, enclosingScope);
	}

	ExpressionType getTypeInfoType()
	{
		if(context.typeInfoType == gUniqueTypeNull)
		{
			// [expr.typeid] The result of a typeid expression is an lvalue of static type const std::type_info
			Identifier stdId = makeIdentifier(context.parserContext.makeIdentifier("std"));
			LookupResultRef declaration = ::findDeclaration(context.global, stdId, IsNestedName());
			SEMANTIC_ASSERT(declaration != 0);
			SEMANTIC_ASSERT(declaration->enclosed != 0);
			SEMANTIC_ASSERT(declaration->enclosed->type == SCOPETYPE_NAMESPACE);
			Identifier typeInfoId = makeIdentifier(context.parserContext.makeIdentifier("type_info"));
			declaration = ::findDeclaration(*declaration->enclosed, typeInfoId);
			SEMANTIC_ASSERT(declaration != 0);
			QualifiedDeclaration qualified = resolveQualifiedDeclaration(QualifiedDeclaration(0, declaration));
			SEMANTIC_ASSERT(isClass(*qualified.declaration));
			Type type(qualified.declaration, context);
			context.typeInfoType = ExpressionType(makeUniqueType(type, InstantiationContext(), false), true); // lvalue
			context.typeInfoType.value.setQualifiers(CvQualifiers(true, false));
		}
		return context.typeInfoType;
	}

	bool objectExpressionIsDependent() const 
	{
		return objectExpression.p != 0
			&& objectExpression.isTypeDependent
			&& memberClass != 0;
	}
	bool allowNameLookup() const
	{
		if(isDependentOld(qualifying_p))
		{
			return false;
		}
		if(objectExpressionIsDependent())
		{
			return false;
		}
		return true;
	}
	LookupResult lookupQualified(const Identifier& id, bool isDeclarator, LookupFilter filter = IsAny())
	{
		return isDeclarator
			? findDeclaratorDeclaration(id, filter)
			: lookupQualified(id, filter);
	}
	LookupResult findDeclaratorDeclaration(const Identifier& id, LookupFilter filter = IsAny())
	{
		SEMANTIC_ASSERT(getQualifyingScope() != 0);
		LookupResult result;
		if(result.append(::findDeclaration(*getQualifyingScope(), id, filter)))
		{
			return result;
		}
		result.filtered = &gUndeclaredInstance;
		return result;
	}
	LookupResult lookupQualified(const Identifier& id, LookupFilter filter = IsAny())
	{
		SEMANTIC_ASSERT(getQualifyingScope() != 0);
		LookupResult result;
		// [basic.lookup.qual]
		if(qualifyingClass != 0)
		{
			instantiateClass(*qualifyingClass, getInstantiationContext());
			if(result.append(::findDeclaration(*qualifyingClass, id, filter)))
			{
				return result;
			}
		}
		else if(result.append(::findNamespaceDeclaration(*getQualifyingScope(), id, filter)))
		{
#ifdef LOOKUP_DEBUG
			std::cout << "HIT: qualified" << std::endl;
#endif
			return result;
		}
		result.filtered = &gUndeclaredInstance;
		return result;
	}
	LookupResult findDeclaration(const Identifier& id, LookupFilter filter = IsAny(), bool isUnqualifiedId = false)
	{
		ProfileScope profile(gProfileLookup);
#ifdef LOOKUP_DEBUG
		std::cout << "lookup: " << getValue(id) << " (" << getIdentifierType(filter) << ")" << std::endl;
#endif
		LookupResult result;
		if(getQualifyingScope() != 0)
		{
			return lookupQualified(id, filter);
		}
		else
		{
			bool isQualified = objectExpression.p != 0
				&& memberClass != 0;
			SYMBOLS_ASSERT(!(isUnqualifiedId && objectExpression.isTypeDependent)); // in case of unqualified-id, should check allowNameLookup before calling
			if(isQualified
				&& !objectExpression.isTypeDependent)
			{
				// [basic.lookup.classref]
				SYMBOLS_ASSERT(memberClass != &gDependentSimpleType);
				if(result.append(::findDeclaration(*memberClass, id, filter)))
				{
#ifdef LOOKUP_DEBUG
					std::cout << "HIT: member" << std::endl;
#endif
					return result;
				}
				// else if we're parsing a nested-name-specifier prefix, drop through, look up in the current context
			}

			if(!isQualified || !isUnqualifiedId)
			{
				if(enclosingTemplateScope != 0)
				{
					// this occurs when looking up template parameters during parse of (but before the point of declaration of) a template class/function, 
					if(result.append(::findDeclaration(*enclosingTemplateScope, id, filter)))
					{
#ifdef LOOKUP_DEBUG
						std::cout << "HIT: enclosingTemplateScope" << std::endl;
#endif
						return result;
					}
				}
				if(result.append(::findClassOrNamespaceMemberDeclaration(*enclosingScope, id, filter)))
				{
#ifdef LOOKUP_DEBUG
					std::cout << "HIT: unqualified" << std::endl;
#endif
					return result;
				}
			}
		}
#ifdef LOOKUP_DEBUG
		std::cout << "FAIL" << std::endl;
#endif
		result.filtered = &gUndeclaredInstance;
		return result;
	}

	const DeclarationInstance& pointOfDeclaration(
		const AstAllocator<int>& allocator,
		Scope* parent,
		Identifier& name,
		const TypeId& type,
		Scope* enclosed,
		bool isType,
		DeclSpecifiers specifiers = DeclSpecifiers(),
		bool isTemplate = false,
		const TemplateParameters& params = TEMPLATEPARAMETERS_NULL,
		bool isSpecialization = false,
		const TemplateArguments& arguments = TEMPLATEARGUMENTS_NULL,
		size_t templateParameter = INDEX_INVALID,
		const Dependent& valueDependent = Dependent())
	{
		SEMANTIC_ASSERT(parent != 0);
		SEMANTIC_ASSERT(templateParameter == INDEX_INVALID || ::isTemplate(*parent));
		SEMANTIC_ASSERT(isTemplate || params.empty());
		SEMANTIC_ASSERT(isClassKey(*type.declaration) || !hasTemplateParamDefaults(params)); // 14.1-9: a default template-arguments may be specified in a class template declaration/definition (not for a function or class-member)
		SEMANTIC_ASSERT(!isClassKey(*type.declaration) || !isSpecialization || isTemplate); // if this is a class, only a template can be a specialization
		SEMANTIC_ASSERT(!isTemplate || isSpecialization || !params.empty()); // only a specialization may have an empty template parameter clause <>
		SEMANTIC_ASSERT(type.unique != 0 || isType || type.declaration == &gNamespace || type.declaration == &gUsing || type.declaration == &gEnumerator);
		if(parent != 0
			&& parent->type == SCOPETYPE_CLASS) // if this is a class member
		{
			SEMANTIC_ASSERT(enclosingType != 0);
		}
		context.parserContext.allocator.deferredBacktrack(); // flush cached parse-tree

		static size_t uniqueId = 0;

		SEMANTIC_ASSERT(!name.value.empty());

		SEMANTIC_ASSERT(type.declaration != &gUsing
				|| type.declaration != &gUsing == (specifiers.isTypedef
				|| type.declaration == &gSpecial
				|| type.declaration == &gArithmetic
				|| type.declaration == &gClass
				|| type.declaration == &gEnum));

		Declaration declaration(allocator, parent, name, type, enclosed, isType, specifiers, isTemplate, params, isSpecialization, arguments, templateParameter, valueDependent);
		declaration.enclosingType = enclosingType;
		declaration.isFunction = type.unique != 0 && getUniqueType(type).isFunction();
		SEMANTIC_ASSERT(!isTemplate || (isClass(declaration) || isFunction(declaration) || declaration.templateParameter != INDEX_INVALID)); // only a class, function or template-parameter can be a template
		declaration.location = getLocation();
		declaration.uniqueId = ++uniqueId;
		DeclarationInstance instance;
		const DeclarationInstance* existing = 0;
		if(!isAnonymous(declaration)) // unnamed class/struct/union/enum
		{
			LookupFilter filter = IsAnyIncludingFriend(); // enable redeclaration of friend
			if(type.declaration == &gCtor)
			{
				filter = IsConstructor(); // find existing constructor declaration
			}
			existing = ::findDeclaration(parent->declarations, name, filter);
		}
		/* 3.4.4-1
		An elaborated-type-specifier (7.1.6.3) may be used to refer to a previously declared class-name or enum-name
		even though the name has been hidden by a non-type declaration (3.3.10).
		*/
		if(existing != 0)
		{
			instance = *existing;
			try
			{
				const Declaration& primary = getPrimaryDeclaration(*instance, declaration);
				if(&primary == instance)
				{
					return *existing;
				}
			}
			catch(DeclarationError& e)
			{
				printPosition(name.source);
				std::cout << "'" << name.value.c_str() << "': " << e.description << std::endl;
				printPosition(instance->getName().source);
				throw SemanticError();
			}

			if(instance->isTemplateName
				&& !isUsing(declaration) // TODO: correctly interpret name as name of template when it is overloaded by a using-declaration
				&& isFunction(declaration))
			{
				// quick hack - if any template overload of a function has been declared, all subsequent overloads are template names
				declaration.isTemplateName = true;
			}

			declaration.overloaded = findOverloaded(instance);

			instance.p = 0;
			instance.overloaded = existing; // the new declaration refers to the previous existing declaration
			instance.redeclared = findRedeclared(declaration, existing);
			if(instance.redeclared != 0)
			{
				instance.p = *instance.redeclared;
				if(isClass(declaration)
					&& declaration.isTemplate)
				{
					TemplateParameters tmp(context);
					tmp.swap(instance->templateParams);
					instance->templateParams = declaration.templateParams;
					if(declaration.isSpecialization) // this is a partial-specialization
					{
						SEMANTIC_ASSERT(!hasTemplateParamDefaults(declaration.templateParams)); // TODO: non-fatal error: partial-specialization may not have default template-arguments
					}
					else
					{
						SEMANTIC_ASSERT(!declaration.templateParams.empty());
						mergeTemplateParamDefaults(*instance, tmp);
					}
				}
				if(isClass(declaration)
					&& isIncomplete(*instance)) // if this class-declaration was previously forward-declared
				{
					instance->enclosed = declaration.enclosed; // complete it
					instance->setName(declaration.getName()); // make this the definition
				}
				if(isFunction(declaration) // if we are redeclaring a function
					&& !declaration.specifiers.isFriend) // as a non-friend
				{
					instance->specifiers.isFriend = false; // make the declaration visible to qualified/unqualified name lookup.
				}
			}
		}
		if(instance.p == 0)
		{
			instance.p = allocatorNew(context, Declaration());
			instance->swap(declaration);
		}

		instance.name = &name;
		instance.visibility = context.declarationCount++;
		const DeclarationInstance& result = parent->declarations.insert(instance);
		parent->declarationList.push_back(instance);
		return result;
	}

	AstAllocator<int> getAllocator()
	{
#ifdef AST_ALLOCATOR_LINEAR
		return context.parserContext.allocator;
#else
		return DebugAllocator<int>();
#endif
	}

	void pushScope(Scope* scope)
	{
		SEMANTIC_ASSERT(findScope(enclosingScope, scope) == 0);
		scope->parent = enclosingScope;
		enclosingScope = scope;
	}

	Declaration* getDeclaratorQualifying() const
	{
		if(qualifying_p == TypePtr(0))
		{
			return 0;
		}
		Declaration* declaration = qualifying_p->declaration;
		if(isNamespace(*declaration))
		{
			return declaration;
		}
		SEMANTIC_ASSERT(isClass(*declaration)); // TODO: non-fatal error: declarator names must not be typedef names
		// only declarator names may be dependent
		if(declaration->isTemplate) // TODO: template partial specialization
		{
			Declaration* specialization = findTemplateSpecialization(declaration, qualifying_p->templateArguments);
			if(specialization != 0)
			{
				return specialization;
			}
			return findPrimaryTemplate(declaration);
		}
		return declaration; 
	}

	Scope* getQualifyingScope()
	{
		if(qualifyingScope == 0)
		{
			return 0;
		}
		SEMANTIC_ASSERT(qualifyingScope->enclosed != 0);
		return qualifyingScope->enclosed;
	}

	void clearMemberType()
	{
		objectExpression = ExpressionWrapper();
	}
	void clearQualifying()
	{
		qualifying_p = 0;
		qualifyingScope = 0;
		qualifyingClass = 0;
		memberClass = 0;
		clearMemberType();
	}

	const TemplateParameters& getTemplateParams() const
	{
		if(templateParams == 0)
		{
			return TEMPLATEPARAMETERS_NULL;
		}
		return *templateParams;
	}

	void clearTemplateParams()
	{
		enclosingTemplateScope = 0;
		templateParams = 0;
	}

	template<typename T>
	bool reportIdentifierMismatch(T* symbol, const Identifier& id, Declaration* declaration, const char* expected)
	{
#if 0
		gIdentifierMismatch = IdentifierMismatch(id, declaration, expected);
#endif
		return false;
	}

	Scope* getEtsScope() const
	{
		Scope* scope = enclosingScope;
		for(; !enclosesEts(scope->type); scope = scope->parent)
		{
		}
		return scope;
	}

	Scope* getFriendScope() const
	{
		SEMANTIC_ASSERT(enclosingScope->type == SCOPETYPE_CLASS);
		Scope* scope = enclosingScope;
		for(; scope->type != SCOPETYPE_NAMESPACE; scope = scope->parent)
		{
			if(scope->type == SCOPETYPE_LOCAL)
			{
				return enclosingScope; // friend declaration in a local class lives in class scope
			}
		}
		return scope;
	}

	Scope* getClassScope() const
	{
		return ::getEnclosingClass(enclosingScope);
	}

	void printScope()
	{
#if 1
		if(enclosingTemplateScope != 0)
		{
			std::cout << "template-params:" << std::endl;
			::printScope(*enclosingTemplateScope);
		}
#endif
		if(getQualifyingScope() != 0)
		{
			std::cout << "qualifying:" << std::endl;
			::printScope(*getQualifyingScope());
		}
		else
		{
			std::cout << "enclosing:" << std::endl;
			::printScope(*enclosingScope);
		}
	}


	bool isDependentImpl(Declaration* dependent) const
	{
		return ::isDependentImpl(dependent, enclosingScope, enclosingTemplateScope);
	}
	bool isDependentOld(const Dependent& dependent) const
	{
		bool result = (dependent.declaration.p != 0);
		SEMANTIC_ASSERT(result == isDependentImpl(dependent.declaration));
		return result;
	}
	bool isDependentOld(const Type& type) const
	{
		return isDependentOld(type.dependent);
	}
	bool isDependentOld(const Types& bases) const
	{
		Dependent dependent;
		setDependent(dependent, bases);
		return isDependentOld(dependent);
	}
	bool isDependentOld(const TypePtr& qualifying) const
	{
		Dependent dependent;
		setDependent(dependent, qualifying.get());
		return isDependentOld(dependent);
	}
	bool isDependentOld(const TemplateArguments& arguments) const
	{
		Dependent dependent;
		setDependent(dependent, arguments);
		return isDependentOld(dependent);
	}
	// the dependent-scope is the outermost template-definition
	void setDependentImpl(Dependent& dependent, Declaration* candidate) const
	{
		SEMANTIC_ASSERT(candidate == 0 || candidate->templateParameter != INDEX_INVALID);
		SEMANTIC_ASSERT(dependent.declaration == DeclarationPtr(0) || isDependentOld(dependent));
		if(!isDependentImpl(candidate))
		{
			return;
		}
		SEMANTIC_ASSERT(candidate->scope->type != SCOPETYPE_NAMESPACE);
		if(dependent.declaration.p != 0
			&& findScope(candidate->scope, dependent.declaration->scope)) // if the candidate template-parameter's template-definition is within the current dependent-scope
		{
			return; // already dependent on outer template
		}
		dependent = Dependent(candidate); // the candidate template-parameter is within the current dependent-scope
	}
	void setDependentImpl(Dependent& dependent, const Dependent& other) const
	{
		setDependentImpl(dependent, other.declaration);
	}
	void setDependentEnclosingTemplate(Dependent& dependent, Declaration* enclosingTemplate) const
	{
		if(enclosingTemplate != 0)
		{
			SEMANTIC_ASSERT(enclosingTemplate->isTemplate);
			// 'declaration' is a class that is dependent because it is a (possibly specialized) member of an enclosing template class
			SEMANTIC_ASSERT(enclosingTemplate->isSpecialization || !enclosingTemplate->templateParams.empty());
			if(!enclosingTemplate->templateParams.empty()) // if the enclosing template class is not an explicit specialization
			{
				// depend on the template parameter(s) of the enclosing template class
				setDependentImpl(dependent, enclosingTemplate->templateParams.back().declaration);
			}
		}
	}
	void setDependent(Dependent& dependent, Declaration& declaration) const
	{
		if(declaration.templateParameter != INDEX_INVALID)
		{
			setDependentImpl(dependent, &declaration);
		}
		else if(declaration.specifiers.isTypedef)
		{
			setDependentImpl(dependent, declaration.type.dependent);
		}
		else if(isClass(declaration)
			&& isComplete(declaration))
		{
			setDependent(dependent, declaration.enclosed->bases);
		}

		setDependentEnclosingTemplate(dependent, findEnclosingClassTemplate(&declaration));

		setDependentImpl(dependent, declaration.valueDependent);
	}
	void setDependent(Dependent& dependent, const Type* qualifying) const
	{
		if(qualifying == 0)
		{
			return;
		}
		setDependentImpl(dependent, qualifying->dependent);
		setDependent(dependent, qualifying->qualifying.get());
	}
	void setDependent(Dependent& dependent, const Qualifying& qualifying) const
	{
		setDependent(dependent, qualifying.get());
	}
	void setDependent(Dependent& dependent, const Types& bases) const
	{
		for(Types::const_iterator i = bases.begin(); i != bases.end(); ++i)
		{
			setDependentImpl(dependent, (*i).dependent);
		}
	}
	void setDependent(Dependent& dependent, const TemplateArguments& arguments) const
	{
		for(TemplateArguments::const_iterator i = arguments.begin(); i != arguments.end(); ++i)
		{
			setDependentImpl(dependent, (*i).type.dependent);
			setDependentImpl(dependent, (*i).valueDependent);
		}
	}
	void setDependent(Dependent& dependent, const Parameters& parameters) const
	{
		for(Parameters::const_iterator i = parameters.begin(); i != parameters.end(); ++i)
		{
			setDependentImpl(dependent, (*i).declaration->type.dependent);
		}
	}
	void setDependent(Type& type, Declaration* declaration) const
	{
		setDependentImpl(type.dependent, declaration);
	}
	void setDependent(Type& type) const
	{
		setDependent(type.dependent, *type.declaration);
	}

	void addDependent(Dependent& dependent, const Dependent& other)
	{
		Declaration* old = dependent.declaration.p;
		setDependentImpl(dependent, other);
		SEMANTIC_ASSERT(old == 0 || dependent.declaration.p != 0);
	}
	void addDependentName(Dependent& dependent, Declaration* declaration)
	{
		Declaration* old = dependent.declaration.p;
		setDependent(dependent, *declaration);
		SEMANTIC_ASSERT(old == 0 || dependent.declaration.p != 0);
	}
	void addDependentType(Dependent& dependent, Declaration* declaration)
	{
		addDependent(dependent, declaration->type.dependent);
	}
	void addDependent(Dependent& dependent, const Type& type)
	{
		addDependent(dependent, type.dependent);
	}
};


inline const char* getIdentifierType(IdentifierFunc func)
{
	if(func == isTypeName)
	{
		return "type-name";
	}
	if(func == isNamespaceName)
	{
		return "namespace-name";
	}
	if(func == isTemplateName)
	{
		return "template-name";
	}
	if(func == isNestedName)
	{
		return "nested-name";
	}
	return "<unknown>";
}


inline void popDeferredSubstitution(DependentConstructs* p, LexerAllocator& allocator)
{
	p->substitutions.pop_back(); // TODO: optimise - though this happens extremely rarely
}

inline BacktrackCallback makePopDeferredSubstitutionCallback(DependentConstructs* p)
{
	BacktrackCallback result = { BacktrackCallbackThunk<DependentConstructs, &popDeferredSubstitution >::thunk, p };
	return result;
}

inline void substituteDeferredBase(Type& type, const InstantiationContext& context)
{
	SimpleType& instance = *const_cast<SimpleType*>(context.enclosingType);

	// substitute dependent members
	SYMBOLS_ASSERT(type.isDependent);
	SYMBOLS_ASSERT(type.dependentIndex != INDEX_INVALID);

	SYMBOLS_ASSERT(type.dependentIndex == instance.substitutedTypes.size());
	UniqueTypeWrapper substituted = substitute(getUniqueType(type), context);
	SYMBOLS_ASSERT(instance.substitutedTypes.size() != instance.substitutedTypes.capacity());
	instance.substitutedTypes.push_back(substituted);
}

inline void substituteDeferredMemberDeclaration(Declaration& declaration, const InstantiationContext& context)
{
	// substitute the dependent types/expressions in the declaration
	const DeferredSubstitutions& substitutions = declaration.declarationDependent.substitutions;
	for(DeferredSubstitutions::const_iterator i = substitutions.begin(); i != substitutions.end(); ++i)
	{
		const DeferredSubstitution& substitution = *i;
		InstantiationContext childContext(substitution.location, context.enclosingType, 0, context.enclosingScope);
		substitution(childContext);
	}
}

inline void substituteDeferredMemberTemplateDeclaration(Declaration& declaration, const InstantiationContext& context)
{
	// dependent types/expressions may depend on a member template's parameters
	SimpleType instance = SimpleType(&declaration, context.enclosingType);
	makeUniqueTemplateParameters(declaration.templateParams, instance.templateArguments, context, true);

	substituteDeferredMemberDeclaration(declaration, setEnclosingTypeSafe(context, &instance));
}

inline void substituteDeferredMemberType(Declaration& declaration, const InstantiationContext& context)
{
	const SimpleType* enclosingType = isClass(*context.enclosingType->declaration) ? context.enclosingType : context.enclosingType->enclosing;
	SimpleType& instance = *const_cast<SimpleType*>(enclosingType);

	// substitute dependent members
	SYMBOLS_ASSERT(declaration.type.isDependent);
	SYMBOLS_ASSERT(declaration.type.dependentIndex != INDEX_INVALID);
	if(isUsing(declaration))
	{
		// TODO: substitute type of dependent using-declaration when class is instantiated
	}
	else
	{
		SYMBOLS_ASSERT(declaration.type.dependentIndex == instance.substitutedTypes.size());
		UniqueTypeWrapper substituted = substitute(getUniqueType(declaration.type), context);
		SYMBOLS_ASSERT(instance.substitutedTypes.size() != instance.substitutedTypes.capacity());
		instance.substitutedTypes.push_back(substituted);

		if(isCompleteTypeRequired(declaration))
		{
			requireCompleteObjectType(substituted, context);
		}
	}
}

#if 0 // TODO

inline bool isDependentOverloaded(Declaration* declaration)
{
	for(Declaration* p = declaration; p != 0; p = p->overloaded)
	{
		if(p->isTypeDependent)
		{
			return true;
		}
	}
	return false;
}

inline bool isTypeDependentExpression(const IdExpression& idExpression, const InstantiationContext& context)
{
	// [temp.dep.expr]
	// An id-expression is type-dependent if it contains
	// - an identifier associated by name lookup with one or more declarations declared with a dependent type,
	// - a template-id that is dependent,
	// - a conversion-function-id that specifies a dependent type, or
	// - a nested-name-specifier or a qualified-id that names a member of an unknown specialization;
	// or if it names a static data member of the current instantiation that has type "array of unknown bound of
	// T" for some T.
	return isDependentQualifying(idExpression.qualifying)
		|| isDependent(idExpression.templateArguments) // the id-expression may have an explicit template argument list
		|| (idExpression.qualifying == 0 // if qualified by a non-dependent type, named declaration cannot be dependent
			&& isDependentOverloaded(idExpression.declaration));
}

inline bool isValueDependentExpression(const IdExpression& idExpression, const InstantiationContext& context)
{
	// [temp.dep.constexpr]
	// An identifier is value-dependent if it is:
	//  - a name declared with a dependent type,
	// 	- the name of a non-type template parameter,
	// 	- a constant with literal type and is initialized with an expression that is value-dependent.
	return isDependentQualifying(idExpression.qualifying)
		|| (idExpression.qualifying == 0 // if qualified by a non-dependent type, named declaration cannot be dependent
			&& (idExpression.declaration->isTypeDependent
				|| idExpression.declaration->initializer.isValueDependent));
}

inline void checkDependent(const IdExpression& idExpression, bool isTypeDependent, bool isValueDependent, const InstantiationContext& context)
{
	SYMBOLS_ASSERT(isTypeDependentExpression(idExpression, context) == isTypeDependent);
	SYMBOLS_ASSERT(isValueDependentExpression(idExpression, context) == isValueDependent);
}
#endif

template<typename T>
void checkDependent(const T& e, bool isTypeDependent, bool isValueDependent)
{
}

template<typename T>
inline bool isTypeDependentExpression(const T&)
{
	return false;
}

template<typename T>
inline bool isValueDependentExpression(const T&)
{
	return false;
}

struct SemaBase : public SemaState
{
	typedef SemaState Base;

	SemaBase(SemaContext& context)
		: Base(context)
	{
	}
	SemaBase(const SemaState& state)
		: Base(state)
	{
	}
	Scope* newScope(const Identifier& name, ScopeType type = SCOPETYPE_UNKNOWN)
	{
		return allocatorNew(context, Scope(context, name, type));
	}



	void addDeferredSubstitution(const DeferredSubstitution& substitution)
	{
		enclosingDependentConstructs->substitutions.push_back(substitution);
		addBacktrackCallback(makePopDeferredSubstitutionCallback(enclosingDependentConstructs));
	}

	void addDeferredBase(Type& type)
	{
		if(!type.isDependent
			|| enclosingInstantiation == 0)
		{
			return;
		}
		SYMBOLS_ASSERT(type.dependentIndex == INDEX_INVALID);
		// substitute type of base when class is instantiated
		type.dependentIndex = enclosingInstantiation->dependentConstructs.typeCount++;
		addDeferredSubstitution(
			makeDeferredSubstitution<Type, substituteDeferredBase>(
				type, getLocation()));
	}

	void addDeferredMemberDeclaration(Declaration& declaration)
	{
		if(declaration.declarationDependent.substitutions.empty()
			|| enclosingInstantiation == 0)
		{
			return;
		}

		DeferredSubstitution substitution = declaration.isTemplate
			? makeDeferredSubstitution<Declaration, substituteDeferredMemberTemplateDeclaration>(declaration, getLocation())
			: makeDeferredSubstitution<Declaration, substituteDeferredMemberDeclaration>(declaration, getLocation());
		addDeferredSubstitution(substitution);
	}

	void addDeferredDeclarationType(Declaration& declaration)
	{
		if(!declaration.type.isDependent
			|| enclosingInstantiation == 0)
		{
			return;
		}
		if(isUsing(declaration))
		{
			// TODO: substitute type of dependent using-declaration when class is instantiated
		}
		else if(declaration.type.dependentIndex == INDEX_INVALID) // if this is not a redeclaration/definition
		{
			// substitute type of dependent declaration when class is instantiated
			declaration.type.dependentIndex = enclosingInstantiation->dependentConstructs.typeCount++;
			addDeferredSubstitution(
				makeDeferredSubstitution<Declaration, substituteDeferredMemberType>(
					declaration, getLocation()));
		}
	}

	void addDeferredExpression(const ExpressionWrapper& expression, const char* message = 0)
	{
		if(!expression.isDependent
			|| enclosingInstantiation == 0)
		{
			return;
		}
		addDeferredSubstitution(
			makeDeferredSubstitution<DeferredExpression, substituteDeferredExpression>(
				*allocatorNew(context, DeferredExpression(expression, TokenValue(message))),
				getLocation()));
	}

	template<typename T>
	ExpressionWrapper makeExpression(const T& node, bool isDependent = false, bool isTypeDependent = false, bool isValueDependent = false)
	{
		checkDependent(node, isTypeDependent, isValueDependent);

		// TODO: optimisation: if expression is not value-dependent, consider unique only if it is also an integral-constant-expression
		bool isUnique = isUniqueExpression(node);
		// TODO: the below is only necessary if dependent expressions are to contain indices into enclosing template
		Scope* enclosingTemplate = findEnclosingTemplateScope(enclosingScope); // don't share uniqued expressions across different template class/function scopes
		ExpressionNode* p = isUnique
			? makeUniqueExpression(node, enclosingTemplate)
			: allocatorNew(context, ExpressionNodeGeneric<T>(node, enclosingTemplate));
		ExpressionWrapper result(p, isTypeDependent, isValueDependent);
		result.isUnique = isUnique;
		result.isDependent = isDependent;
		if(!result.isTypeDependent) // if the expression is not type-dependent
		{
			result.type = typeOfExpression(node, getInstantiationContext());

			// [basic.lval] Class prvalues can have cv-qualified types; non-class prvalues always have cv-unqualified types
			if(!result.type.isLvalue // if this is a prvalue
				&& !isClass(result.type)) // and is not a class
			{
				SYMBOLS_ASSERT(result.type.value.getQualifiers() == CvQualifiers());
			}

			SEMANTIC_ASSERT(!result.isConstant);
			if(!result.isValueDependent) // if the expression is not value-dependent
			{
				ExpressionValue value = evaluateExpression(node, getInstantiationContext());
				result.isConstant = value.isConstant;
				result.value = value.value;
			}
		}

		if(!result.isDependent)
		{
			instantiateExpression(node, getInstantiationContext());
		}


		return result;
	}

	void addBase(Declaration* declaration, const Type& base)
	{
		declaration->enclosed->bases.push_front(base);
		addDeferredBase(declaration->enclosed->bases.front());
	}

	void addBacktrackCallback(const BacktrackCallback& callback)
	{
		context.parserContext.allocator.addBacktrackCallback(context.parserContext.allocator.position, callback);
	}

	void disableBacktrack()
	{
		addBacktrackCallback(makeBacktrackErrorCallback());
	}

	// Causes /p declaration to be undeclared when backtracking.
	// In practice this only happens for the declaration in an elaborated-type-specifier.
	void trackDeclaration(const DeclarationInstance& declaration)
	{
		addBacktrackCallback(makeUndeclareCallback(&declaration));
	}

	Declaration* declareClass(Scope* parent, Identifier* id, bool isCStyle, bool isUnion, bool isSpecialization, TemplateArguments& arguments)
	{
		Scope* enclosed = newScope(makeIdentifier("$class"), SCOPETYPE_CLASS);
		DeclarationInstanceRef declaration = pointOfDeclaration(context, parent, id == 0 ? parent->getUniqueName() : *id, TYPE_CLASS, enclosed, true, DeclSpecifiers(), templateParams != 0, getTemplateParams(), isSpecialization, arguments);
#ifdef ALLOCATOR_DEBUG
		trackDeclaration(declaration);
#endif
		if(id != 0)
		{
			setDecoration(id, declaration);
		}
		if(enclosingTemplateScope != 0)
		{
			enclosingTemplateScope->isClassTemplate = true;
		}
		enclosed->name = declaration->getName();
		declaration->isCStyle = isCStyle;
		declaration->isUnion = isUnion;
		return declaration;
	}

	// subsequent name lookups in this context will find names in this class and its enclosing classes and namespaces
	void beginClassDefinition(Declaration* declaration)
	{
		// [basic.lookup.unqual]
		// A name used in the definition of a class X outside of a member function body or nested class definition
		// shall be declared in one of the following ways:
		//  - before its use in class X or be a member of a base class of X, or
		// 	- if X is a nested class of class Y, before the definition of X in Y, or shall be a member of a base
		//	class of Y(this lookup applies in turn to Y's enclosing classes, starting with the innermost enclosing
		//	class), or
		// 	- if X is a local class or is a nested class of a local class, before the definition of class X in a block
		// 	enclosing the definition of class X, or
		// 	- if X is a member of namespace N, or is a nested class of a class that is a member of N, or is a local class
		// 	or a nested class within a local class of a function that is a member of N, before the definition of class
		// 	X in namespace N or in one of N's enclosing namespaces.

		pushScope(declaration->enclosed);
		if(enclosingTemplateScope != 0)
		{
			// insert the template-parameter scope to enclose the class scope
			SEMANTIC_ASSERT(findScope(enclosingScope, enclosingTemplateScope) == 0);
			enclosingTemplateScope->parent = enclosingScope->parent;
			enclosingScope->parent = enclosingTemplateScope; // required when looking up template-parameters from within a template class
		}
		if(declaration->isTemplate)
		{
			enclosingScope->templateDepth = templateDepth; // indicates that this is a template
		}
		declaration->templateParamScope = enclosingTemplateScope; // required by findEnclosingType

		if(!isAnonymousUnion(*declaration))
		{
			enclosingInstantiation = declaration; // any dependent constructs in the class definition will be added
		}
		enclosingDependentConstructs = &enclosingInstantiation->dependentConstructs;
	}

	void endMemberDeclaration(Declaration* declaration, const DependentConstructs& declarationDependent)
	{
		if(declaration == 0 // static-assert declaration
			|| declaration == &gCtor
			|| isClassKey(*declaration) // elaborated-type-specifier
			|| isClass(*declaration)
			|| isEnum(*declaration)) // nested class or enum
		{
			SEMANTIC_ASSERT(declaration == 0 || declarationDependent.substitutions.empty()); // TODO: static-assert
			return;
		}

		SEMANTIC_ASSERT(declaration->declarationDependent.substitutions.empty());
		declaration->declarationDependent = declarationDependent;
		addDeferredMemberDeclaration(*declaration);

		if(isUsing(*declaration)) // using-declaration
		{
			return;
		}

		SEMANTIC_ASSERT(declaration->type.unique != 0);
		UniqueTypeWrapper uniqueType = getUniqueType(declaration->type);

		// track whether class has (pure) virtual functions
		if(declaration->specifiers.isVirtual
			&& uniqueType.isFunction())
		{
			SimpleType* enclosingClass = const_cast<SimpleType*>(getEnclosingType(enclosingType));
			SYMBOLS_ASSERT(enclosingClass != 0);
			enclosingClass->isEmpty = false;
			enclosingClass->isPod = false;
			enclosingClass->isPolymorphic = true;
			if(declaration->specifiers.isPure)
			{
				enclosingClass->isAbstract = true;
			}
			if(declaration->isDestructor)
			{
				enclosingClass->hasVirtualDestructor = true;
			}
		}
	}

	Declaration* declareObject(Scope* parent, Identifier* id, const TypeId& type, Scope* enclosed, DeclSpecifiers specifiers, size_t templateParameter, const Dependent& valueDependent, bool isDestructor, bool isExplicitSpecialization)
	{
		// [namespace.memdef]
		// Every name first declared in a namespace is a member of that namespace. If a friend declaration in a non-local class
		// first declares a class or function (this implies that the name of the class or function is unqualified) the friend
		// class or function is a member of the innermost enclosing namespace. The name of the friend is not found by unqualified lookup (3.4.1) or by qualified lookup (3.4.3)
		// until a matching declaration is provided in that namespace scope (either before or after the class definition
		// granting friendship).
		if(specifiers.isFriend // is friend
			&& parent == enclosingScope) // is unqualified
		{
			parent = getFriendScope();
		}

		bool isTemplate = templateParams != 0;
		DeclarationInstanceRef declaration = pointOfDeclaration(context, parent, *id, type, enclosed, specifiers.isTypedef, specifiers, isTemplate, getTemplateParams(), isExplicitSpecialization, TEMPLATEARGUMENTS_NULL, templateParameter, valueDependent); // 3.3.1.1
#ifdef ALLOCATOR_DEBUG
		trackDeclaration(declaration);
#endif
		if(id != &gAnonymousId)
		{
			setDecoration(id, declaration);
		}

		declaration->isDestructor = isDestructor;

		if(isTemplate
			&& declaration->templateParamScope == 0)
		{
			declaration->templateParamScope = enclosingTemplateScope; // required by findEnclosingType
		}

		if(declaration->type.isDependent) // if the declaration's type is dependent
		{
			// record whether the declaration's type depends on a template parameter of the enclosing class
			declaration->isTypeDependent = !declaration->isTemplate // if this is not a function template declaration/definition
				|| declaration->type.dependent.declaration->scope->isClassTemplate; // or the function template declaration's type depends on the template parameter of a class template
		}

		// the type of an object is required to be complete
		// a member's type must be instantiated before the point of declaration of the member, to prevent the member being found by name lookup during the instantiation
		UniqueTypeWrapper uniqueType = getUniqueType(type);

		if(!type.isDependent
			&& isCompleteTypeRequired(*declaration))
		{
			const InstantiationContext& context = getInstantiationContext();
			requireCompleteObjectType(uniqueType, context);
		}

		if(type.isDependent)
		{
			addDeferredDeclarationType(*declaration);
		}

		SimpleType* enclosingClass = const_cast<SimpleType*>(getEnclosingType(enclosingType));
		// NOTE: this check must occur after the declaration because an out-of-line definition of a static member is otherwise not known to be static
		if(enclosingClass != 0 // if the enclosing class is valid
			&& isNonStaticDataMember(*declaration) // and the declaration is a non-static data member
			&& !type.isDependent
			&& !enclosingClass->declaration->isTemplate) // if this is a not member of a class template
		{
			addNonStaticMember(*enclosingClass, uniqueType);
		}// otherwise the layout is not known until the point of instantiation

		return declaration;
	}

	bool declareEts(Type& type, Identifier* forward)
	{
		if(forward == 0)
		{
			return false;
		}
		Identifier& id = *forward;

		// [basic.lookup.elab]
		// If the elaborated-type-specifier has no nested-name-specifier ...
		// ... the identifier is looked up according to 3.4.1 but ignoring any non-type names that have been declared. If
		// the elaborated-type-specifier is introduced by the enum keyword and this lookup does not find a previously
		// declared type-name, the elaborated-type-specifier is ill-formed. If the elaborated-type-specifier is introduced by
		// the class-key and this lookup does not find a previously declared type-name ...
		// the elaborated-type-specifier is a declaration that introduces the class-name as described in 3.3.1.
		LookupResultRef declaration = findDeclaration(id, IsTypeName());
		if(declaration == &gUndeclared // if there is no existing declaration
			|| isTypedef(*declaration) // or the existing declaration is a typedef
			|| declaration->isTemplate // or the existing declaration is a template class
			|| templateParams != 0 // or we are forward-declaring a template class
			|| (isClassKey(*type.declaration) && declaration->scope == getEtsScope())) // or this is a forward-declaration of a class/struct
		{
			if(!isClassKey(*type.declaration))
			{
				SEMANTIC_ASSERT(type.declaration == &gEnum);
				printPosition(id.source);
				std::cout << "'" << id.value.c_str() << "': elaborated-type-specifier refers to undefined enum" << std::endl;
				throw SemanticError();
			}
			/* 3.3.1-6
			if the elaborated-type-specifier is used in the decl-specifier-seq or parameter-declaration-clause of a
			function defined in namespace scope, the identifier is declared as a class-name in the namespace that
			contains the declaration; otherwise, except as a friend declaration, the identifier is declared in the
			smallest non-class, non-function-prototype scope that contains the declaration.
			*/
			DeclarationInstanceRef declaration = pointOfDeclaration(context, getEtsScope(), id, TYPE_CLASS, 0, true);

			trackDeclaration(declaration);
			setDecoration(&id, declaration);
			type = declaration;
			return true;
		}

#if 0 // elaborated type specifier cannot refer to a template in a different scope - this case will be treated as a redeclaration
		// template<typename T> class C
		if(declaration->isSpecialization) // if the lookup found a template explicit/partial-specialization
		{
			SEMANTIC_ASSERT(declaration->isTemplate);
			declaration = findPrimaryTemplateLastDeclaration(declaration); // the name is a plain identifier, not a template-id, therefore the name refers to the primary template
		}
#endif
		setDecoration(&id, declaration);
		// [dcl.type.elab]
		// 3.4.4 describes how name lookup proceeds for the identifier in an elaborated-type-specifier. If the identifier
		// resolves to a class-name or enum-name, the elaborated-type-specifier introduces it into the declaration the
		// same way a simple-type-specifier introduces its type-name. If the identifier resolves to a typedef-name, the
		// elaborated-type-specifier is ill-formed.
#if 0 // allow hiding a typedef with a forward-declaration
		if(isTypedef(*declaration))
		{
			printPosition(id.source);
			std::cout << "'" << id.value.c_str() << "': elaborated-type-specifier refers to a typedef" << std::endl;
			printPosition(declaration->getName().source);
			throw SemanticError();
		}
#endif
#if 0 // TODO: check for struct vs class
		/* 7.1.6.3-3
		The class-key or enum keyword present in the elaborated-type-specifier shall agree in kind with the declaration
		to which the name in the elaborated-type-specifier refers.
		*/
		if(declaration->type.declaration != type)
		{
			printPosition(id.source);
			std::cout << "'" << id.value.c_str() << "': elaborated-type-specifier key does not match declaration" << std::endl;
			printPosition(declaration->getName().source);
			throw SemanticError();
		}
#endif
		type = declaration;

		return false;
	}

	Declaration* declareUsing(Scope* parent, Identifier* id, UniqueTypeWrapper base, const DeclarationInstance& member, bool isType, bool isTemplate)
	{
		DeclarationInstanceRef declaration = pointOfDeclaration(context, parent, *id, TYPE_USING, 0, isType);
#ifdef ALLOCATOR_DEBUG
		trackDeclaration(declaration);
#endif
		setDecoration(id, declaration);

		declaration->usingBase = base;
		declaration->usingMember = &member;
		declaration->isTemplateName = isTemplate;
		return declaration;
	}

	bool consumeTemplateParams(const Qualifying& qualifying)
	{
		if(qualifying.empty())
		{
			return false;
		}
		const Type& type = qualifying.back();
		if(!type.declaration->isTemplate) // if the qualifying type is not a template
		{
			return consumeTemplateParams(type.qualifying);
		}
		Declaration* primary = findPrimaryTemplate(type.declaration);
		SEMANTIC_ASSERT(primary->templateParamScope->templateDepth <= templateDepth); // TODO: non-fatal error: not enough template-parameter-clauses in class declaration
		return primary->templateParamScope->templateDepth == templateDepth;
	}

	LookupResultRef lookupTemplate(const Identifier& id, LookupFilter filter)
	{
		if(!isDependentOld(qualifying_p))
		{
			return LookupResultRef(findDeclaration(id, filter));
		}
		return gDependentTemplateInstance;
	}

	void addDependentOverloads(Dependent& dependent, Declaration* declaration)
	{
		for(Declaration* p = declaration; p != 0; p = p->overloaded)
		{
			addDependent(dependent, p->type);
		}
	}
	static ExpressionType binaryOperatorIntegralType(Name operatorName, ExpressionType left, ExpressionType right)
	{
		SEMANTIC_ASSERT(!isFloating(left));
		SEMANTIC_ASSERT(!isFloating(right));
		return ExpressionType(usualArithmeticConversions(left, right), false); // non lvalue
	}

	template<typename T>
	void makeUniqueTypeImpl(T& type)
	{
		SYMBOLS_ASSERT(type.unique == 0); // type must not be uniqued twice
		type.isDependent = isDependentOld(type)
			|| objectExpressionIsDependent(); // this occurs when uniquing the dependent type name in a nested name-specifier in a class-member-access expression
		type.unique = makeUniqueType(type, getInstantiationContext(), type.isDependent).value;
	}
	void makeUniqueTypeSafe(Type& type)
	{
		makeUniqueTypeImpl(type);
	}
	void makeUniqueTypeSafe(TypeId& type)
	{
		makeUniqueTypeImpl(type);
	}

	ExpressionWrapper makeTransformedIdExpression(const ExpressionWrapper& expression, Dependent& typeDependent, Dependent& valueDependent)
	{
		if(!isTransformedIdExpression(expression, getInstantiationContext()))
		{
			return expression;
		}
		addDependent(typeDependent, enclosingDependent); // TODO: NOT dependent if not a member of the current instantiation and current instantiation has no dependent base class 
		ExpressionType objectExpressionType = typeOfEnclosingClass(getInstantiationContext());
		ExpressionWrapper left = makeExpression(ObjectExpression(objectExpressionType), isDependentOld(enclosingDependent));
		return makeExpression(
			ClassMemberAccessExpression(left, expression),
			left.isDependent | expression.isDependent, expression.isTypeDependent, expression.isValueDependent);
	}
};




struct Args0
{
};

template<typename A1>
struct Args1
{
	A1 a1;
	CPPP_INLINE Args1(A1 a1) : a1(a1)
	{
	}
};

template<typename A1, typename A2>
struct Args2
{
	A1 a1;
	A2 a2;
	CPPP_INLINE Args2(A1 a1, A2 a2) : a1(a1), a2(a2)
	{
	}
};

struct InvokeNone
{
	template<typename SemaT, typename T, typename Result>
	CPPP_INLINE static bool invokeAction(SemaT& walker, T* symbol, Result& result)
	{
		return true;
	}
};

struct InvokeChecked
{
	template<typename SemaT, typename T, typename Result>
	static bool invokeAction(SemaT& walker, T* symbol, Result& result)
	{
		return walker.action(symbol);
	}
};

struct InvokeUnchecked
{
	template<typename SemaT, typename T, typename Result>
	static bool invokeAction(SemaT& walker, T* symbol, Result& result)
	{
		walker.action(symbol);
		return true;
	}
};

struct InvokeCheckedResult
{
	template<typename SemaT, typename T, typename Result>
	static bool invokeAction(SemaT& walker, T* symbol, Result& result)
	{
		return walker.action(symbol, result);
	}
};

struct InvokeUncheckedResult
{
	template<typename SemaT, typename T, typename Result>
	static bool invokeAction(SemaT& walker, T* symbol, Result& result)
	{
		walker.action(symbol, result);
		return true;
	}
};


struct CommitNull
{
};

struct CommitEnable
{
};



// Inner policy which pushes a new semantic context onto the stack.
template<typename Inner, typename Commit = CommitNull, typename Args = Args0>
struct SemaPush : Args
{
	typedef Args ArgsType;
	CPPP_INLINE SemaPush(const Args& args)
		: Args(args)
	{
	}
};

// Specialization for zero arguments (does not derive from Args>
template<typename Inner, typename Commit>
struct SemaPush<Inner, Commit, Args0>
{
	typedef SemaPush ArgsType;
};



template<typename SemaT, LexTokenId ID, typename U = void>
struct HasAction
{
	static const bool value = false;
};

template<typename SemaT, LexTokenId ID>
struct HasAction<SemaT, ID, typename SfinaeNonType<void(SemaT::*)(cpp::terminal<ID>), &SemaT::action>::Type>
{
	static const bool value = true;
};


template<typename SemaT, LexTokenId ID>
CPPP_INLINE typename EnableIf<!HasAction<SemaT, ID>::value>::Type
	semaAction(SemaT& walker, cpp::terminal<ID>)
{
	// do nothing
}

template<typename SemaT, LexTokenId ID>
typename EnableIf<HasAction<SemaT, ID>::value>::Type
	semaAction(SemaT& walker, cpp::terminal<ID> symbol)
{
	walker.action(symbol);
}


template<typename SemaT, typename Inner, typename Args>
CPPP_INLINE void semaCommit(SemaT& walker, const SemaPush<Inner, CommitNull, Args>& inner)
{
	// don't call commit, even if declared
}

template<typename SemaT, typename Inner, typename Args>
void semaCommit(SemaT& walker, const SemaPush<Inner, CommitEnable, Args>& inner)
{
	walker.commit(); // we expect commit to be declared
}

struct Once
{
	bool done;
	Once()
		: done(false)
	{
	}
	void operator()()
	{
		SEMANTIC_ASSERT(!done);
		done = true;
	}
	void test() const
	{
		SEMANTIC_ASSERT(done);
	}
};

// Inner policy which continues with the current semantic context.
struct SemaIdentity
{
	typedef SemaIdentity ArgsType;
};



template<typename SemaT>
CPPP_INLINE void semaCommit(SemaT& walker, const SemaIdentity& inner)
{
	// do nothing
}

struct Nothing
{
};

struct AnnotateNull
{
	typedef Nothing Data;
	CPPP_INLINE static Data makeData(const Token& token)
	{
		return Nothing();
	}
	template<typename T>
	CPPP_INLINE static void annotate(T* symbol, const Nothing&)
	{
	}
};

struct AnnotateSrc
{
	typedef Source Data;
	CPPP_INLINE static Data makeData(const Token& token)
	{
		return token.source;
	}
	template<typename T>
	CPPP_INLINE static void annotate(T* symbol, const Source& source)
	{
		symbol->source = source;
	}
};

struct AnnotateId
{
	typedef Source Data;
	CPPP_INLINE static Data makeData(const Token& token)
	{
		return token.source;
	}
	template<typename T>
	CPPP_INLINE static void annotate(T* symbol, const Source& source)
	{
		symbol->value.source = source;
	}
};

struct SourceEvents : Source, IncludeEvents
{
	CPPP_INLINE SourceEvents(const Source& source, const IncludeEvents& events)
		: Source(source), IncludeEvents(events)
	{
	}
};

struct AnnotateTop
{
	typedef SourceEvents Data;
	CPPP_INLINE static Data makeData(const Token& token)
	{
		return SourceEvents(token.source, token.events);
	}
	CPPP_INLINE static void annotate(cpp::declaration* symbol, const Data& data)
	{
		symbol->source = data;
		symbol->events = data;
	}
};



template<typename Inner, typename Annotate = AnnotateNull, typename Invoke = InvokeUncheckedResult, typename Cache = DisableCache, typename Defer = DeferDefault>
struct SemaPolicyGeneric : Inner, Annotate, Invoke, Cache, Defer
{
	typedef typename Inner::ArgsType ArgsType;
	CPPP_FORCEINLINE SemaPolicyGeneric(const ArgsType& args = ArgsType())
		: Inner(args)
	{
	}
	CPPP_FORCEINLINE const Inner& getInnerPolicy() const
	{
		return *this;
	}
	typedef Annotate AnnotateType;
	CPPP_FORCEINLINE const Annotate& getAnnotatePolicy() const
	{
		return *this;
	}
	CPPP_FORCEINLINE const Cache& getCachePolicy() const
	{
		return *this;
	}
	CPPP_FORCEINLINE const Defer& getDeferPolicy() const
	{
		return *this;
	}
	CPPP_FORCEINLINE const Invoke& getActionPolicy() const
	{
		return *this;
	}
};

typedef SemaPolicyGeneric<SemaIdentity, AnnotateNull, InvokeNone> SemaPolicyNone;
typedef SemaPolicyGeneric<SemaIdentity, AnnotateNull, InvokeUnchecked> SemaPolicyIdentity;
typedef SemaPolicyGeneric<SemaIdentity, AnnotateSrc, InvokeUnchecked> SemaPolicySrc;
typedef SemaPolicyGeneric<SemaIdentity, AnnotateNull, InvokeChecked> SemaPolicyIdentityChecked;
typedef SemaPolicyGeneric<SemaIdentity, AnnotateNull, InvokeUnchecked, EnableCache> SemaPolicyIdentityCached;
typedef SemaPolicyGeneric<SemaIdentity, AnnotateNull, InvokeChecked, EnableCache> SemaPolicyIdentityCachedChecked;
template<typename SemaT>
struct SemaPolicyPush : SemaPolicyGeneric<SemaPush<SemaT>, AnnotateNull> {};
template<typename SemaT>
struct SemaPolicyPushCommit : SemaPolicyGeneric<SemaPush<SemaT, CommitEnable>, AnnotateNull> {};
template<typename SemaT>
struct SemaPolicyPushSrc : SemaPolicyGeneric<SemaPush<SemaT>, AnnotateSrc> {};
template<typename SemaT>
struct SemaPolicyPushId : SemaPolicyGeneric<SemaPush<SemaT>, AnnotateId> {};
template<typename SemaT>
struct SemaPolicyPushIdCommit : SemaPolicyGeneric<SemaPush<SemaT, CommitEnable>, AnnotateId> {};
template<typename SemaT>
struct SemaPolicyPushTop : SemaPolicyGeneric<SemaPush<SemaT>, AnnotateTop> {};
template<typename SemaT>
struct SemaPolicyPushSrcChecked : SemaPolicyGeneric<SemaPush<SemaT>, AnnotateSrc, InvokeCheckedResult> {};
template<typename SemaT>
struct SemaPolicyPushIdChecked : SemaPolicyGeneric<SemaPush<SemaT>, AnnotateId, InvokeCheckedResult> {};
template<typename SemaT>
struct SemaPolicyPushChecked : SemaPolicyGeneric<SemaPush<SemaT>, AnnotateNull, InvokeCheckedResult> {};
template<typename SemaT>
struct SemaPolicyPushCached : SemaPolicyGeneric<SemaPush<SemaT>, AnnotateNull, InvokeUncheckedResult, CachedWalk> {};
template<typename SemaT>
struct SemaPolicyPushCachedChecked : SemaPolicyGeneric<SemaPush<SemaT>, AnnotateNull, InvokeCheckedResult, CachedWalk> {};
template<typename SemaT>
struct SemaPolicyPushBool : SemaPolicyGeneric<SemaPush<SemaT, CommitNull, Args1<bool> >, AnnotateNull, InvokeUncheckedResult, DisableCache>
{
	CPPP_INLINE SemaPolicyPushBool(bool value) : SemaPolicyGeneric(Args1<bool>(value))
	{
	}
};
template<typename SemaT>
struct SemaPolicyPushCheckedBool : SemaPolicyGeneric<SemaPush<SemaT, CommitNull, Args1<bool> >, AnnotateNull, InvokeCheckedResult, DisableCache>
{
	CPPP_INLINE SemaPolicyPushCheckedBool(bool value) : SemaPolicyGeneric(Args1<bool>(value))
	{
	}
};
template<typename SemaT>
struct SemaPolicyPushCachedBool : SemaPolicyGeneric<SemaPush<SemaT, CommitNull, Args1<bool> >, AnnotateNull, InvokeUncheckedResult, CachedWalk>
{
	CPPP_INLINE SemaPolicyPushCachedBool(bool value) : SemaPolicyGeneric(Args1<bool>(value))
	{
	}
};
template<typename SemaT>
struct SemaPolicyPushCachedCheckedBool : SemaPolicyGeneric<SemaPush<SemaT, CommitNull, Args1<bool> >, AnnotateNull, InvokeCheckedResult, CachedWalk>
{
	CPPP_INLINE SemaPolicyPushCachedCheckedBool(bool value) : SemaPolicyGeneric(Args1<bool>(value))
	{
	}
};
template<typename SemaT>
struct SemaPolicyPushIndexCommit : SemaPolicyGeneric<SemaPush<SemaT, CommitEnable, Args1<std::size_t> >, AnnotateNull, InvokeUncheckedResult, DisableCache>
{
	CPPP_INLINE SemaPolicyPushIndexCommit(std::size_t value) : SemaPolicyGeneric(Args1<std::size_t>(value))
	{
	}
};

template<typename SemaT, typename Defer>
struct SemaPolicyPushDeferred : SemaPolicyGeneric<SemaPush<SemaT, CommitNull, Args0>, AnnotateNull, InvokeUncheckedResult, DisableCache, Defer>
{
};




#define SEMA_POLICY(Symbol, Policy) \
	CPPP_FORCEINLINE Policy makePolicy(Symbol*) \
	{ \
		return Policy(); \
	}

#define SEMA_POLICY_ARGS(Symbol, Policy, args) \
	CPPP_FORCEINLINE Policy makePolicy(Symbol*) \
	{ \
		return Policy(args); \
	}

#define SEMA_BOILERPLATE \
	template<typename T> \
	CPPP_FORCEINLINE SemaPolicyNone makePolicy(T* symbol) \
	{ \
		return SemaPolicyNone(); \
	}


struct SemaDeclSpecifierSeqResult
{
	Type type;
	IdentifierPtr forward; // if this is an elaborated-type-specifier, the 'identifier'
	CvQualifiers qualifiers;
	DeclSpecifiers specifiers;
	bool isUnion;
	SemaDeclSpecifierSeqResult(Declaration* declaration, const AstAllocator<int>& allocator)
		: type(declaration, allocator), forward(0), isUnion(false)
	{
	}
};

struct SemaDeclarationArgs
{
	bool isParameter;
	size_t templateParameter;
	SemaDeclarationArgs(bool isParameter = false, size_t templateParameter = INDEX_INVALID)
		: isParameter(isParameter), templateParameter(templateParameter)
	{
	}
};

template<typename SemaT>
struct SemaPolicyParameterDeclaration : SemaPolicyGeneric<SemaPush<SemaT, CommitNull, Args1<SemaDeclarationArgs> > >
{
	CPPP_INLINE SemaPolicyParameterDeclaration(SemaDeclarationArgs value) : SemaPolicyGeneric(value)
	{
	}
};

struct SemaExpressionResult
{
	IdentifierPtr id; // only valid when the expression is a (parenthesised) id-expression
	ExpressionWrapper expression;
	Dependent typeDependent;
	Dependent valueDependent;

	SemaExpressionResult()
		: id(0)
	{
	}
};

struct SemaTypeIdResult
{
	TypeId type;
	Once committed;
	SemaTypeIdResult(const AstAllocator<int>& allocator)
		: type(0, allocator)
	{
	}
};

struct SemaTemplateParameterClauseResult
{
	TemplateParameters params;
	SemaTemplateParameterClauseResult(const AstAllocator<int>& allocator)
		: params(allocator)
	{
	}
};

struct SemaTypeSpecifierResult
{
	Type type;
	unsigned fundamental;
	SemaTypeSpecifierResult(const AstAllocator<int>& allocator)
		: type(0, allocator), fundamental(0)
	{
	}
};

struct SemaDecltypeSpecifierResult
{
	Type type;
	SemaDecltypeSpecifierResult(const AstAllocator<int>& allocator)
		: type(0, allocator)
	{
	}
};

struct SemaTypenameSpecifierResult
{
	Type type;
	SemaTypenameSpecifierResult(const AstAllocator<int>& allocator)
		: type(0, allocator)
	{
	}
};

struct SemaNewTypeResult
{
	TypeId type;
	Dependent valueDependent;
	SemaNewTypeResult(const AstAllocator<int>& allocator)
		: type(0, allocator)
	{
	}
};

struct SemaSimpleDeclarationResult
{
	DeclarationPtr declaration; // the result of the declaration
	cpp::default_argument* defaultArgument; // parsing of this symbol will be deferred if this is a member-declaration

	SemaSimpleDeclarationResult()
		: declaration(0), defaultArgument(0)
	{
	}
};

struct SemaDeclarationResult
{
	DeclarationPtr declaration;

	SemaDeclarationResult()
		: declaration(0)
	{
	}
};

typedef SemaDeclarationResult SemaNamespaceResult;
typedef SemaDeclarationResult SemaEnumSpecifierResult;
typedef SemaDeclarationResult SemaExplicitInstantiationResult;
typedef SemaDeclarationResult SemaMemberDeclarationResult;

struct SemaQualifyingResult
{
	Qualifying qualifying;
	SemaQualifyingResult(const AstAllocator<int>& allocator)
		: qualifying(allocator)
	{
	}
};


struct SemaTemplateIdResult
{
	IdentifierPtr id;
	TemplateArguments arguments;
	SemaTemplateIdResult(AstAllocator<int>& allocator)
		: id(0), arguments(allocator)
	{
	}
};

struct SemaArgumentListResult
{
	Arguments arguments;
	Dependent typeDependent;
	Dependent valueDependent;
	bool isDependent; // true if any argument is dependent;
	SemaArgumentListResult()
		: isDependent(false)
	{
	}
};

struct SemaExplicitTypeExpressionResult : SemaArgumentListResult
{
	SEMA_BOILERPLATE;

	TypeId type;
	SemaExplicitTypeExpressionResult(AstAllocator<int>& allocator)
		: type(0, allocator)
	{
	}
};

struct SemaElaboratedTypeSpecifierResult
{
	SEMA_BOILERPLATE;

	Type type;
	IdentifierPtr id;
	SemaElaboratedTypeSpecifierResult(AstAllocator<int>& allocator)
		: type(0, allocator), id(0)
	{
	}
};


struct SemaClassSpecifierResult
{
	DeclarationPtr declaration;
	IdentifierPtr id;
	TemplateArguments arguments;
	bool isUnion;
	bool isSpecialization;
	SemaClassSpecifierResult(AstAllocator<int>& allocator)
		: declaration(0), id(0), arguments(allocator), isUnion(false), isSpecialization(false)
	{
	}
};


struct SemaQualified : public SemaBase, SemaQualifyingResult
{
	SemaQualified(const SemaState& state)
		: SemaBase(state), SemaQualifyingResult(context)
	{
	}

	void setQualifyingGlobal()
	{
		SEMANTIC_ASSERT(qualifying.empty());
		qualifying_p = context.globalType.get_ref();
		qualifyingScope = qualifying_p->declaration;
		qualifyingClass = 0;
	}

	void swapQualifying(const Type& type, bool isDeclarator = false)
	{
#if 0 // allow incomplete types as qualifying, for nested-name-specifier in ptr-operator (defining member-function-ptr)
		if(type.declaration->enclosed == 0)
		{
			// TODO
			//printPosition(symbol->id->value.position);
			std::cout << "'" << getValue(type.declaration->name) << "' is incomplete, declared here:" << std::endl;
			printPosition(type.declaration->getName().position);
			throw SemanticError();
		}
#endif
		Qualifying tmp(type, context);
		swapQualifying(tmp, isDeclarator);
	}
	void swapQualifying(const Qualifying& other, bool isDeclarator = false)
	{
		qualifying = other;
		qualifying_p = qualifying.get_ref();
		if(isDeclarator)
		{
			qualifyingScope = getDeclaratorQualifying();
		}
		else if(qualifying_p != TypePtr(0))
		{
			Declaration* declaration = qualifying_p->declaration;
			if(isNamespace(*declaration))
			{
				qualifyingScope = declaration;
			}
			else
			{
				UniqueTypeWrapper type = getUniqueType(*qualifying_p, getInstantiationContext(), isDeclarator || qualifying_p->isDependent);
				if(!type.isSimple())
				{
					qualifyingScope = 0;
				}
				else
				{
					qualifyingClass = &getSimpleType(type.value);
					qualifyingScope = qualifyingClass->declaration;
				}
			}
		}
	}
};

#endif
