

#include "semantic.h"
#include "util.h"

#include "profiler.h"
#include "symbols.h"

#include "parser_symbols.h"

#include <iostream>

struct SemanticError
{
	SemanticError()
	{
#ifdef WIN32
		__debugbreak();
#endif
	}
};

#define SEMANTIC_ASSERT(condition) if(!(condition)) { throw SemanticError(); }

inline void semanticBreak()
{
}

void printDeclarations(const Scope::Declarations& declarations)
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

void printBases(const Types& bases)
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

void printScope(const Scope& scope)
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

void printName(const Scope& scope)
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

const IdentifierMismatch IDENTIFIERMISMATCH_NULL = IdentifierMismatch(IDENTIFIER_NULL, 0, 0);
IdentifierMismatch gIdentifierMismatch = IDENTIFIERMISMATCH_NULL;

void printIdentifierMismatch(const IdentifierMismatch& e)
{
	printPosition(e.id.position);
	std::cout << "'" << getValue(e.id) << "' expected " << e.expected << ", " << (e.declaration == &gUndeclared ? "was undeclared" : "was declared here:") << std::endl;
	if(e.declaration != &gUndeclared)
	{
		printPosition(e.declaration->getName().position);
		std::cout << std::endl;
	}
}


inline UniqueTypeWrapper adjustFunctionParameter(UniqueTypeWrapper type)
{
	UniqueTypeWrapper result(type.value.getPointer());  // ignore cv-qualifiers
	if(type.isFunction()) // T() becomes T(*)()
	{
		pushUniqueType(result.value, DeclaratorPointer());
	}
	else if(type.isArray()) // T[] becomes T*
	{
		popUniqueType(result.value);
		pushUniqueType(result.value, DeclaratorPointer());
	}
	return result;
}

inline void setDecoration(Identifier* id, const DeclarationInstance& declaration)
{
	SEMANTIC_ASSERT(declaration.name != 0);
	id->dec.p = &declaration;
}

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

inline bool isEquivalent(const Declaration& declaration, const Declaration& other)
{
	if(isClass(declaration))
	{
		SYMBOLS_ASSERT(isClass(other));
		// TODO: compare template-argument-lists of partial specializations
		return isSpecialization(declaration) == isSpecialization(other)
			&& (!isSpecialization(declaration) // both are not explicit/partial specializations
			|| isEquivalentSpecialization(declaration, other)); // both are specializations and have matching arguments

	}
	else
	{
		// TODO: is enclosing type required to differentiate between member functions?
		UniqueTypeWrapper l = makeUniqueType(declaration.type, 0, true);
		UniqueTypeWrapper r = makeUniqueType(other.type, 0, true);
		if(l.isFunction())
		{
			// 13.2 [over.dcl] Two functions of the same name refer to the same function
			// if they are in the same scope and have equivalent parameter declarations.
			// TODO: also compare template parameter lists: <class, int> is not equivalent to <class, float>
			SYMBOLS_ASSERT(r.isFunction()); // TODO: non-fatal error: 'id' previously declared as non-function, second declaration is a function
			return declaration.isTemplate == other.isTemplate // early out
				&& isReturnTypeEqual(l, r) // return-types match
				&& isEquivalent(getParameterTypes(l.value), getParameterTypes(r.value)); // and parameter-types match
		}
	}
	return false;
}

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


Identifier gGlobalId = makeIdentifier("$global");


struct WalkerContext : public TreeAllocator<int>
{
	Scope global;
	Declaration globalDecl;
	TypeRef globalType;

	WalkerContext(const TreeAllocator<int>& allocator) :
		TreeAllocator<int>(allocator),
		global(allocator, gGlobalId, SCOPETYPE_NAMESPACE),
		globalDecl(allocator, 0, gGlobalId, TYPE_NULL, &global),
		globalType(Type(&globalDecl, allocator), allocator)
	{
	}
};

typedef std::list< DeferredParse<struct WalkerBase, struct WalkerState> > DeferredSymbols;

typedef bool (*IdentifierFunc)(const Declaration& declaration);
const char* getIdentifierType(IdentifierFunc func);


struct WalkerState
	: public ContextBase
{
	typedef WalkerState State;

	WalkerContext& context;
	ScopePtr enclosing;
	const TypeInstance* enclosingType;
	TypePtr qualifying_p;
	DeclarationPtr qualifyingScope;
	ScopePtr memberObject;
	SafePtr<TemplateParameters> templateParams;
	ScopePtr templateParamScope;
	ScopePtr templateEnclosing;
	DeferredSymbols* deferred;
	size_t templateDepth;
	bool isExplicitInstantiation;

	WalkerState(WalkerContext& context)
		: context(context), enclosing(0), enclosingType(0), qualifying_p(0), qualifyingScope(0), memberObject(0), templateParams(0), templateParamScope(0), templateEnclosing(0), deferred(0), templateDepth(0), isExplicitInstantiation(false)
	{
	}
	const WalkerState& getState() const
	{ 
		return *this;
	}

	LookupResult findDeclaration(const Identifier& id, LookupFilter filter = IsAny())
	{
		ProfileScope profile(gProfileLookup);
#ifdef LOOKUP_DEBUG
		std::cout << "lookup: " << getValue(id) << " (" << getIdentifierType(filter) << ")" << std::endl;
#endif
		LookupResult result;
		Scope* qualifying = getQualifyingScope();
		if(qualifying != 0)
		{
			// 3.4.3: qualified name lookup
			if(result.append(::findDeclarationWithin(*qualifying, id, filter)))
			{
#ifdef LOOKUP_DEBUG
				std::cout << "HIT: qualified" << std::endl;
#endif
				return result;
			}
		}
		else
		{
#if 1
			if(memberObject != 0)
			{
				// 3.4.5 class member acess
				if(result.append(::findDeclarationWithin(*memberObject, id, filter)))
				{
#ifdef LOOKUP_DEBUG
					std::cout << "HIT: member" << std::endl;
#endif
					return result;
				}
			}

			if(templateParamScope != 0)
			{
				if(result.append(::findDeclaration(*templateParamScope, id, filter)))
				{
#ifdef LOOKUP_DEBUG
					std::cout << "HIT: templateParamScope" << std::endl;
#endif
					return result;
				}
			}
#endif
			if(result.append(::findDeclaration(*enclosing, id, filter)))
			{
#ifdef LOOKUP_DEBUG
				std::cout << "HIT: unqualified" << std::endl;
#endif
				return result;
			}
		}
#ifdef LOOKUP_DEBUG
		std::cout << "FAIL" << std::endl;
#endif
		result.filtered = &gUndeclaredInstance;
		return result;
	}

	const DeclarationInstance& pointOfDeclaration(
		const TreeAllocator<int>& allocator,
		Scope* parent,
		Identifier& name,
		const TypeId& type,
		Scope* enclosed,
		DeclSpecifiers specifiers = DeclSpecifiers(),
		bool isTemplate = false,
		const TemplateParameters& params = TEMPLATEPARAMETERS_NULL,
		bool isSpecialization = false,
		const TemplateArguments& arguments = TEMPLATEARGUMENTS_NULL,
		size_t templateParameter = INDEX_INVALID,
		const Dependent& valueDependent = DEPENDENT_NULL)
	{
		SEMANTIC_ASSERT(parent != 0);
		SEMANTIC_ASSERT(templateParameter == INDEX_INVALID || ::isTemplate(*parent));
		SEMANTIC_ASSERT(isTemplate || params.empty());
		SEMANTIC_ASSERT(isClassKey(*type.declaration) || !hasTemplateParamDefaults(params)); // 14.1-9: a default template-arguments may be specified in a class template declaration/definition (not for a function or class-member)

		parser->context.allocator.deferredBacktrack(); // flush cached parse-tree

		SEMANTIC_ASSERT(!name.value.empty());
		Declaration other(allocator, parent, name, type, enclosed, specifiers, isTemplate, params, isSpecialization, arguments, templateParameter, valueDependent);
		DeclarationInstance declaration;
		const DeclarationInstance* overloaded = 0;
		if(!isAnonymous(other)) // unnamed class/struct/union/enum
		{
			overloaded = ::findDeclaration(parent->declarations, name);
		}
		/* 3.4.4-1
		An elaborated-type-specifier (7.1.6.3) may be used to refer to a previously declared class-name or enum-name
		even though the name has been hidden by a non-type declaration (3.3.10).
		*/
		if(overloaded != 0)
		{
			declaration = *overloaded;
			try
			{
				const Declaration& primary = getPrimaryDeclaration(*declaration, other);
				if(&primary == declaration)
				{
					return *overloaded;
				}
			}
			catch(DeclarationError& e)
			{
				printPosition(name.position);
				std::cout << "'" << name.value.c_str() << "': " << e.description << std::endl;
				printPosition(declaration->getName().position);
				throw SemanticError();
			}

			if(!isNamespace(other)
				&& !isType(other)
				&& isFunction(other))
			{
				// quick hack - if any template overload of a function has been declared, all subsequent declarations are template functions
				if(declaration->isTemplate)
				{
					other.isTemplate = true;
				}
			}
			other.overloaded = declaration;
			declaration.p = 0;
			declaration.overloaded = overloaded;
			declaration.redeclared = findRedeclared(other, overloaded);
			if(declaration.redeclared != 0)
			{
				declaration.p = *declaration.redeclared;
				if(isClass(other)
					&& other.isTemplate)
				{
					TemplateParameters tmp(context);
					tmp.swap(declaration->templateParams);
					declaration->templateParams = other.templateParams;
					if(other.isSpecialization) // this is a partial-specialization
					{
						SEMANTIC_ASSERT(!hasTemplateParamDefaults(other.templateParams)); // TODO: non-fatal error: partial-specialization may not have default template-arguments
					}
					else
					{
						SEMANTIC_ASSERT(!other.templateParams.empty());
						mergeTemplateParamDefaults(*declaration, tmp);
					}
				}
				if(isClass(other)
					&& isIncomplete(*declaration)) // if this class-declaration was previously forward-declared
				{
					declaration->enclosed = other.enclosed; // complete it
				}
			}
		}
		if(declaration.p == 0)
		{
			declaration.p = allocatorNew(context, Declaration());
			declaration->swap(other);
		}

		declaration.name = &name;
		const DeclarationInstance& result = parent->declarations.insert(declaration);
		parent->declarationList.push_back(declaration);
		return result;
	}

	TreeAllocator<int> getAllocator()
	{
#ifdef TREEALLOCATOR_LINEAR
		return parser->context.allocator;
#else
		return DebugAllocator<int>();
#endif
	}

	void pushScope(Scope* scope)
	{
		SEMANTIC_ASSERT(findScope(enclosing, scope) == 0);
		scope->parent = enclosing;
		enclosing = scope;
	}

	void addBase(Declaration* declaration, const Type& base)
	{
		if(getInstantiatedType(base).declaration == declaration)
		{
			return; // TODO: implement template-instantiation, and disallow inheriting from current-instantiation
		}
		declaration->enclosed->bases.push_front(base);
		if(isDependent(base))
		{
			::addDeferredLookupType(&declaration->enclosed->bases.front(), getEnclosingTemplate(declaration->scope));
		}
	}

	Declaration* getQualifying(bool isDeclarator)
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
		SEMANTIC_ASSERT(!isDeclarator || isClass(*declaration)); // TODO: non-fatal error: declarator names must not be typedef names
		if(!isDeclarator)
		{
			if(isDependent(qualifying_p))
			{
				return 0;
			}
			return getObjectType(makeUniqueType(*qualifying_p, enclosingType).value).declaration;
		}
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

	void clearQualifying()
	{
		qualifying_p = 0;
		qualifyingScope = 0;
		memberObject = 0;
	}

	const TemplateParameters& getTemplateParams(Scope* parent) const
	{
		return templateParams != 0 && parent == templateEnclosing ? *templateParams : TEMPLATEPARAMETERS_NULL;
	}

	void clearTemplateParams()
	{
		templateParamScope = 0;
		templateParams = 0;
	}

	template<typename T>
	void reportIdentifierMismatch(T* symbol, const Identifier& id, Declaration* declaration, const char* expected)
	{
		result = 0;
#if 0
		gIdentifierMismatch = IdentifierMismatch(id, declaration, expected);
#endif
	}

	Scope* getEtsScope() const
	{
		Scope* scope = enclosing;
		for(; !enclosesEts(scope->type); scope = scope->parent)
		{
		}
		return scope;
	}

	Scope* getFriendScope() const
	{
		SEMANTIC_ASSERT(enclosing->type == SCOPETYPE_CLASS);
		Scope* scope = enclosing;
		for(; scope->type != SCOPETYPE_NAMESPACE; scope = scope->parent)
		{
			if(scope->type == SCOPETYPE_LOCAL)
			{
				return enclosing; // friend declaration in a local class lives in class scope
			}
		}
		return scope;
	}

	Scope* getClassScope() const
	{
		return ::getEnclosingClass(enclosing);
	}

	void printScope()
	{
#if 1
		if(templateParamScope != 0)
		{
			std::cout << "template-params:" << std::endl;
			::printScope(*templateParamScope);
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
			::printScope(*enclosing);
		}
	}

	bool isDependent(const Type& type) const
	{
		//std::cout << "isDependent(Type)" << std::endl;
		bool result = isDependentNew(type);
#ifdef DEPENDENT_OLD
		SEMANTIC_ASSERT(result == ::isDependentOld(type, DependentContext(*enclosing, templateParamScope != 0 ? *templateParamScope : SCOPE_NULL)));
#endif
		return result;
	}

	bool isDependent(const TemplateArguments& arguments) const
	{
		//std::cout << "isDependent(TemplateArguments)" << std::endl;
		bool result = isDependentNew(arguments);
#ifdef DEPENDENT_OLD
		SEMANTIC_ASSERT(result == ::isDependentOld(arguments, DependentContext(*enclosing, templateParamScope != 0 ? *templateParamScope : SCOPE_NULL)));
#endif
		return result;
	}

	bool isDependent(const Types& bases) const
	{
		//std::cout << "isDependent(Types)" << std::endl;
		bool result = isDependentNew(bases);
#ifdef DEPENDENT_OLD
		SEMANTIC_ASSERT(result == ::isDependentOld(bases, DependentContext(*enclosing, templateParamScope != 0 ? *templateParamScope : SCOPE_NULL)));
#endif
		return result;
	}

	bool isDependent(const TypePtr& qualifying) const
	{
		if(qualifying != TypePtr(0))
		{
			//std::cout << "isDependent(Type*)" << std::endl;
		}
		bool result = isDependentNew(qualifying);
#ifdef DEPENDENT_OLD
		SEMANTIC_ASSERT(result == ::isDependentOld(qualifying.get(), DependentContext(*enclosing, templateParamScope != 0 ? *templateParamScope : SCOPE_NULL)));
#endif
		return result;
	}

	bool isDependent(Dependent& dependent) const
	{
		bool result = isDependentNew(dependent);
#ifdef DEPENDENT_OLD
		SEMANTIC_ASSERT(result == ::evaluateDependent(dependent, DependentContext(*enclosing, templateParamScope != 0 ? *templateParamScope : SCOPE_NULL)));
#endif
		return result;
	}


	bool isDependentNew(Declaration* dependent) const
	{
		return dependent != 0
			&& (dependent->scope->type == SCOPETYPE_TEMPLATE // hack, workaround for template-scope being copied
				|| findScope(enclosing, dependent->scope) != 0
				|| findScope(templateParamScope, dependent->scope) != 0); // if we are within the candidate template-parameter's template-definition
	}
	bool isDependentNew(const Type& type) const
	{
		return isDependentNew(type.dependent);
	}
	bool isDependentNew(const Types& bases) const
	{
		DeclarationPtr dependent(0);
		setDependent(dependent, bases);
		return isDependentNew(dependent);
	}
	bool isDependentNew(const TypePtr& qualifying) const
	{
		DeclarationPtr dependent(0);
		setDependent(dependent, qualifying.get());
		return isDependentNew(dependent);
	}
	bool isDependentNew(const TemplateArguments& arguments) const
	{
		DeclarationPtr dependent(0);
		setDependent(dependent, arguments);
		return isDependentNew(dependent);
	}
	bool isDependentNew(const Dependent& dependent) const
	{
		return isDependentNew(dependent.enclosingTemplate);
	}
	// the dependent-scope is the outermost template-definition
	void setDependent(DeclarationPtr& dependent, Declaration* candidate) const
	{
		SEMANTIC_ASSERT(dependent == DeclarationPtr(0) || isDependentNew(dependent));
		if(!isDependentNew(candidate))
		{
			return;
		}
		SEMANTIC_ASSERT(candidate->scope->type != SCOPETYPE_NAMESPACE);
		if(dependent != 0
			&& findScope(candidate->scope, dependent->scope)) // if the candidate template-parameter's template-definition is within the current dependent-scope
		{
			return; // already dependent on outer template
		}
		dependent = candidate; // the candidate template-parameter is within the current dependent-scope
	}
	void setDependent(DeclarationPtr& dependent, Declaration& declaration) const
	{
		if(declaration.templateParameter != INDEX_INVALID)
		{
			setDependent(dependent, &declaration);
		}
		else if(declaration.specifiers.isTypedef)
		{
			setDependent(dependent, declaration.type.dependent);
		}
		else if(isClass(declaration)
			&& isComplete(declaration))
		{
			setDependent(dependent, declaration.enclosed->bases);
		}

		setDependent(dependent, declaration.valueDependent.enclosingTemplate);
	}
	void setDependent(DeclarationPtr& dependent, const Type* qualifying) const
	{
		if(qualifying == 0)
		{
			return;
		}
		setDependent(dependent, qualifying->dependent);
		setDependent(dependent, qualifying->qualifying.get());
	}
	void setDependent(DeclarationPtr& dependent, const Qualifying& qualifying) const
	{
		setDependent(dependent, qualifying.get());
	}
	void setDependent(DeclarationPtr& dependent, const Types& bases) const
	{
		for(Types::const_iterator i = bases.begin(); i != bases.end(); ++i)
		{
			setDependent(dependent, (*i).dependent);
		}
	}
	void setDependent(DeclarationPtr& dependent, const TemplateArguments& arguments) const
	{
		for(TemplateArguments::const_iterator i = arguments.begin(); i != arguments.end(); ++i)
		{
			setDependent(dependent, (*i).type.dependent);
			setDependent(dependent, (*i).dependent.enclosingTemplate);
		}
	}
	struct TypeSequenceSetDependent : TypeElementVisitor
	{
		const WalkerState& walker;
		DeclarationPtr& dependent;
		TypeSequenceSetDependent(const WalkerState& walker, DeclarationPtr& dependent)
			: walker(walker), dependent(dependent)
		{
		}
		virtual void visit(const DeclaratorDependent&)
		{
			throw SemanticError(); // not reachable
		}
		virtual void visit(const DeclaratorObject&)
		{
			throw SemanticError(); // not reachable
		}
		virtual void visit(const DeclaratorPointer&)
		{
		}
		virtual void visit(const DeclaratorReference&)
		{
		}
		virtual void visit(const DeclaratorArray&)
		{
		}
		virtual void visit(const DeclaratorMemberPointer& element)
		{
			walker.setDependent(dependent, element.type.dependent);
		}
		virtual void visit(const DeclaratorFunction& element)
		{
			for(Parameters::const_iterator i = element.parameters.begin(); i != element.parameters.end(); ++i)
			{
				walker.setDependent(dependent, (*i).declaration->type.dependent);
			}
		}
	};
	void setDependent(DeclarationPtr& dependent, const TypeSequence& typeSequence) const
	{
		TypeSequenceSetDependent visitor(*this, dependent);
		typeSequence.accept(visitor);
	}
	void setDependent(Type& type, Declaration* declaration) const
	{
		setDependent(type.dependent, declaration);
	}
	void setDependent(Type& type) const
	{
		setDependent(type.dependent, *type.declaration);
	}

#ifdef DEPENDENT_OLD
	void addDependent(Dependent& dependent, const DependencyCallback& callback)
	{
		dependent.push_front(callback);
	}
#endif
	void addDependentName(Dependent& dependent, Declaration* declaration)
	{
		setDependent(dependent.enclosingTemplate, *declaration);
#ifdef DEPENDENT_OLD
		static DependencyCallbacks<Declaration> callbacks = makeDependencyCallbacks(isDependentName);
		addDependent(dependent, makeDependencyCallback(declaration, &callbacks));
#endif
	}
	void addDependentType(Dependent& dependent, Declaration* declaration)
	{
		setDependent(dependent.enclosingTemplate, declaration->type.dependent);
#ifdef DEPENDENT_OLD
		static DependencyCallbacks<const Type> callbacks = makeDependencyCallbacks(isDependentType);
		SEMANTIC_ASSERT(declaration->type.declaration != 0);
		addDependent(dependent, makeDependencyCallback(static_cast<const Type*>(&declaration->type), &callbacks));
#endif
	}
	void addDependent(Dependent& dependent, const Type& type)
	{
		setDependent(dependent.enclosingTemplate, type.dependent);
#ifdef DEPENDENT_OLD
		TypeRef tmp(type, context);
		SEMANTIC_ASSERT(type.declaration != 0);
		static DependencyCallbacks<TypePtr::Value> callbacks = makeDependencyCallbacks(isDependentTypeRef, ReferenceCallbacks<const Type>::increment, ReferenceCallbacks<const Type>::decrement);
		addDependent(dependent, makeDependencyCallback(tmp.get_ref().p, &callbacks));
#endif
	}
	void addDependent(Dependent& dependent, Scope* scope)
	{
		setDependent(dependent.enclosingTemplate, scope->bases);
#ifdef DEPENDENT_OLD
		static DependencyCallbacks<Scope> callbacks = makeDependencyCallbacks(isDependentClass);
		addDependent(dependent, makeDependencyCallback(scope, &callbacks));
#endif
	}
#if 0
	void addDependent(Dependent& dependent, Dependent& other)
	{
		dependent.splice(other);
	}
#else
	void addDependent(Dependent& dependent, Dependent& other)
	{
		setDependent(dependent.enclosingTemplate, other.enclosingTemplate);
#ifdef DEPENDENT_OLD
		if(other.empty())
		{
			return;
		}
		if(dependent.empty())
		{
			dependent = other;
			return;
		}
		CopiedReference<Dependent, TreeAllocator<int> > tmp(other, context);
		static DependencyCallbacks<Reference<Dependent>::Value> callbacks = makeDependencyCallbacks(isDependentListRef, ReferenceCallbacks<Dependent>::increment, ReferenceCallbacks<Dependent>::decrement);
		addDependent(dependent, makeDependencyCallback(tmp.get_ref().p, &callbacks));
#endif
	}
#endif
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

struct WalkerBase : public WalkerState
{
	typedef WalkerBase Base;

	WalkerBase(WalkerContext& context)
		: WalkerState(context)
	{
	}
	WalkerBase(const WalkerState& state)
		: WalkerState(state)
	{
	}
	Scope* newScope(const Identifier& name, ScopeType type = SCOPETYPE_UNKNOWN)
	{
		return allocatorNew(context, Scope(context, name, type));
	}

	// Causes /p declaration to be undeclared when backtracking.
	// In practice this only happens for the declaration in an elaborated-type-specifier.
	void trackDeclaration(const DeclarationInstance& declaration)
	{
		parser->addBacktrackCallback(makeUndeclareCallback(&declaration));
	}

	Declaration* declareClass(Identifier* id, bool isSpecialization, TemplateArguments& arguments)
	{
		Scope* enclosed = templateParamScope != 0 ? static_cast<Scope*>(templateParamScope) : newScope(makeIdentifier("$class"));
		enclosed->type = SCOPETYPE_CLASS; // convert template-param-scope to class-scope if present
		DeclarationInstanceRef declaration = pointOfDeclaration(context, enclosing, id == 0 ? enclosing->getUniqueName() : *id, TYPE_CLASS, enclosed, DeclSpecifiers(), enclosing == templateEnclosing, getTemplateParams(enclosing), isSpecialization, arguments);
#ifdef ALLOCATOR_DEBUG
		trackDeclaration(declaration);
#endif
		if(id != 0)
		{
			setDecoration(id, declaration);
		}
		enclosed->name = declaration->getName();
		return declaration;
	}

	void addDeferredLookupType(Declaration& declaration)
	{
		if(!isDependent(declaration.type))
		{
			return;
		}
		::addDeferredLookupType(&declaration.type, getEnclosingTemplate(enclosing)); // TODO: getEnclosingTemplate fails for member-template friend function declaration
	}

	Declaration* declareObject(Scope* parent, Identifier* id, const TypeId& type, Scope* enclosed, DeclSpecifiers specifiers, size_t templateParameter, const Dependent& valueDependent)
	{
		// 7.3.1.2 Namespace member definitions
		// Paragraph 3
		// Every name first declared in a namespace is a member of that namespace. If a friend declaration in a non-local class
		// first declares a class or function (this implies that the name of the class or function is unqualified) the friend
		// class or function is a member of the innermost enclosing namespace.
		if(specifiers.isFriend // is friend
			&& parent == enclosing) // is unqualified
		{
			parent = getFriendScope();
		}
		DeclarationInstanceRef declaration = pointOfDeclaration(context, parent, *id, type, enclosed, specifiers, parent == templateEnclosing, getTemplateParams(parent), false, TEMPLATEARGUMENTS_NULL, templateParameter, valueDependent); // 3.3.1.1
#ifdef ALLOCATOR_DEBUG
		trackDeclaration(declaration);
#endif
		if(id != &gAnonymousId)
		{
			setDecoration(id, declaration);
		}

		return declaration;
	}

	bool declareEts(Type& type, Identifier* forward)
	{
		if(isClassKey(*type.declaration))
		{
			SEMANTIC_ASSERT(forward != 0);
			/* 3.3.1-6
			if the elaborated-type-specifier is used in the decl-specifier-seq or parameter-declaration-clause of a
			function defined in namespace scope, the identifier is declared as a class-name in the namespace that
			contains the declaration; otherwise, except as a friend declaration, the identifier is declared in the
			smallest non-class, non-function-prototype scope that contains the declaration.
			*/
			DeclarationInstanceRef declaration = pointOfDeclaration(context, getEtsScope(), *forward, TYPE_CLASS, 0, DeclSpecifiers(), enclosing == templateEnclosing);
			trackDeclaration(declaration);
			setDecoration(forward, declaration);
			type = declaration;
			return true;
		}
		return false;
	}

	DeclarationInstanceRef lookupTemplate(const Identifier& id, LookupFilter filter)
	{
		if(!isDependent(qualifying_p))
		{
			return DeclarationInstanceRef(findDeclaration(id, filter));
		}
		return gDependentTemplateInstance;
	}

	Declaration* findBestOverloadedOperator(const Identifier& id, const UniqueTypeId& type)
	{
		if((isClass(type) || isEnumeration(type)) // if the operand has class or enum type
			&& (type.isSimple() || type.isReference())) // and is a simple object or reference
		{
			UniqueTypeIds::Pointer::Value value = UniqueTypeIds::Pointer::Value(UniqueTypeIds::Node(type));
			UniqueTypeIds arguments = UniqueTypeIds(TREEALLOCATOR_NULL);
			arguments.head.next = &value;
			OverloadResolver resolver(arguments);

			if(isClass(type))
			{
				SEMANTIC_ASSERT(isComplete(type)); // TODO: non-fatal parse error
				Scope* scope = getObjectType(type.value).declaration->enclosed;
				DeclarationInstanceRef declaration = ::findDeclaration(scope->declarations, scope->bases, id);
				if(declaration != 0)
				{
					resolver.add(declaration);
				}
			}
			// TODO: ignore non-member candidates if no operand has a class type, unless one or more params has enum (ref) type
			DeclarationInstanceRef declaration = findDeclaration(id, IsNonMemberName()); // look up non-member candidates in this context (ignoring members)
			if(declaration != &gUndeclared
				&& !declaration->isTemplate // TODO: template argument deduction for overloaded operator
				&& !declaration->specifiers.isFriend) // TODO: 14.5.3: friend function as member of a template-class, which depends on template arguments
			{
				resolver.add(declaration);
			}
			{
				// TODO: 13.3.1.2: built-in operators for overload resolution
				// These are relevant either when the operand has a user-defined conversion to a non-class type, or is an enum that can be converted to an arithmetic type
				CandidateFunction candidate(&gUnknown);
				candidate.conversions.reserve(1);
				candidate.conversions.push_back(IMPLICITCONVERSION_USERDEFINED);//getIcsRank(???, type)); // TODO: cv-qualified overloads
				resolver.add(candidate); // TODO: ignore built-in overloads that have same signature as a non-member
			}
			return resolver.get();
		}
		return &gUnknown;
	}

	// 5 Expressions
	// paragraph 9: usual arithmetic conversions
	static const UniqueTypeId& binaryOperatorIntegralType(const UniqueTypeId& left, const UniqueTypeId& right)
	{
		if(left.value == UNIQUETYPE_NULL
			|| right.value == UNIQUETYPE_NULL)
		{
			// TODO: assert
			return gUniqueTypeNull;
		}

		if(isEqual(left, gUnsignedLongInt)
			|| isEqual(right, gUnsignedLongInt))
		{
			return gUnsignedLongInt;
		}
		if((isEqual(left, gSignedLongInt)
				&& isEqual(right, gUnsignedInt))
			|| (isEqual(left, gUnsignedInt)
				&& isEqual(right, gSignedLongInt)))
		{
			return gUnsignedLongInt;
		}
		if(isEqual(left, gSignedLongInt)
			|| isEqual(right, gSignedLongInt))
		{
			return gSignedLongInt;
		}
		if(isEqual(left, gUnsignedInt)
			|| isEqual(right, gUnsignedInt))
		{
			return gUnsignedInt;
		}
		return gSignedInt;
	}
	static const UniqueTypeId& binaryOperatorArithmeticType(const UniqueTypeId& left, const UniqueTypeId& right)
	{
		if(left.value == UNIQUETYPE_NULL
			|| right.value == UNIQUETYPE_NULL)
		{
			// TODO: assert
			return gUniqueTypeNull;
		}

		//TODO: SEMANTIC_ASSERT(isArithmetic(left) && isArithmetic(right));
		if(isEqual(left, gLongDouble)
			|| isEqual(right, gLongDouble))
		{
			return gLongDouble;
		}
		if(isEqual(left, gDouble)
			|| isEqual(right, gDouble))
		{
			return gDouble;
		}
		if(isEqual(left, gFloat)
			|| isEqual(right, gFloat))
		{
			return gFloat;
		}
		return binaryOperatorIntegralType(promoteToIntegralType(left), promoteToIntegralType(right));
	}
	static const UniqueTypeId& binaryOperatorAdditiveType(const UniqueTypeId& left, const UniqueTypeId& right)
	{
		if(left.value == UNIQUETYPE_NULL
			|| right.value == UNIQUETYPE_NULL)
		{
			// TODO: assert
			return gUniqueTypeNull;
		}

		if(left.isPointer())
		{
			if(isIntegral(right)
				|| isEnumeration(right))
			{
				return left;
			}
			if(right.isPointer())
			{
				return gSignedLongInt; // TODO: ptrdiff_t
			}
		}
		return binaryOperatorArithmeticType(left, right);
	}
	static UniqueTypeId getBuiltInUnaryOperatorReturnType(cpp::unary_operator* symbol, const UniqueTypeId& type)
	{
		if(symbol->id == cpp::unary_operator::AND) // address-of
		{
			UniqueTypeId result = type;
			result.push_front(DeclaratorPointer()); // produces a non-const pointer
			return result;
		}
		else if(symbol->id == cpp::unary_operator::STAR) // dereference
		{
			UniqueTypeId result = type;
			if(!result.empty()) // TODO: assert
			{
				result.pop_front();
			}
			return result;
		}
		else if(symbol->id == cpp::unary_operator::PLUS
			|| symbol->id == cpp::unary_operator::MINUS)
		{
			if(!isFloating(type))
			{
				// TODO: check type is integral or enumeration
				return promoteToIntegralType(type);
			}
			return type;
		}
		else if(symbol->id == cpp::unary_operator::NOT)
		{
			return gBool;
		}
		else if(symbol->id == cpp::unary_operator::COMPL)
		{
			// TODO: check type is integral or enumeration
			return promoteToIntegralType(type);
		}
		SEMANTIC_ASSERT(symbol->id == cpp::unary_operator::PLUSPLUS || symbol->id == cpp::unary_operator::MINUSMINUS);
		return type;
	}
};

struct WalkerQualified : public WalkerBase
{
	Qualifying qualifying;
	WalkerQualified(const WalkerState& state)
		: WalkerBase(state), qualifying(context)
	{
	}

	void setQualifyingGlobal()
	{
		SEMANTIC_ASSERT(qualifying.empty());
		qualifying_p = context.globalType.get_ref();
		qualifyingScope = qualifying_p->declaration;
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
	void swapQualifying(Qualifying& other, bool isDeclarator = false)
	{
		qualifying.swap(other);
		qualifying_p = qualifying.get_ref();
		qualifyingScope = getQualifying(isDeclarator);
	}
};




#define TREEWALKER_WALK(walker, symbol) SYMBOL_WALK(walker, symbol)
#define TREEWALKER_LEAF(symbol) SYMBOL_WALK(*this, symbol)

#define TREEWALKER_DEFAULT PARSERCONTEXT_DEFAULT


#define TREEWALKER_WALK_CACHED_DATA(walker, symbol, data) \
	if(!parser->cacheLookup((symbol), (data))) \
	{ \
		CachedSymbols::Key key = parser->context.position; \
		TREEWALKER_WALK(walker, symbol); \
		parser->cacheStore(key, (symbol), (data)); \
	} \
	else \
	{ \
		result = symbol; \
	}

#define TREEWALKER_WALK_CACHED(walker, symbol) TREEWALKER_WALK_CACHED_DATA(walker, symbol, walker)
#define TREEWALKER_LEAF_CACHED(symbol) int _dummy; TREEWALKER_WALK_CACHED_DATA(*this, symbol, _dummy)


bool isUnqualified(cpp::elaborated_type_specifier_default* symbol)
{
	return symbol != 0
		&& symbol->isGlobal.value.empty()
		&& symbol->context.p == 0;
}


struct Walker
{


struct IsHiddenNamespaceName
{
	DeclarationPtr hidingType; // valid if the declaration is hidden by a type name

	IsHiddenNamespaceName()
		: hidingType(0)
	{
	}

	bool operator()(const Declaration& declaration)
	{
		if(isNamespaceName(declaration))
		{
			return true;
		}
		if(hidingType == 0
			&& isTypeName(declaration))
		{
			hidingType = const_cast<Declaration*>(&declaration); // TODO: fix const
		}
		return false;
	}
};

struct NamespaceNameWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	DeclarationInstanceRef declaration;
	IsHiddenNamespaceName filter;
	NamespaceNameWalker(const WalkerState& state)
		: WalkerBase(state)
	{
	}
	void visit(cpp::identifier* symbol)
	{
		TREEWALKER_LEAF_CACHED(symbol);
		declaration = findDeclaration(symbol->value, makeLookupFilter(filter));
		if(declaration == &gUndeclared)
		{
			return reportIdentifierMismatch(symbol, symbol->value, declaration, "namespace-name");
		}
		setDecoration(&symbol->value, declaration);
	}
};

struct TemplateArgumentListWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	TemplateArgument argument;
	TemplateArguments arguments;

	TemplateArgumentListWalker(const WalkerState& state)
		: WalkerBase(state), argument(context), arguments(context)
	{
	}
	void visit(cpp::type_id* symbol)
	{
		TypeIdWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		argument.type.swap(walker.type);
		addDependent(argument.dependent, walker.valueDependent);
	}
	void visit(cpp::assignment_expression* symbol)
	{
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		argument.dependent.swap(walker.typeDependent);
		argument.type = &gNonType;
		addDependent(argument.dependent, walker.valueDependent);
	}
	void visit(cpp::template_argument_list* symbol)
	{
		TemplateArgumentListWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		arguments.swap(walker.arguments);
		arguments.push_front(walker.argument); // allocates last element first!
	}
};

struct OverloadableOperatorWalker : public WalkerBase
{
	Name name;
	OverloadableOperatorWalker(const WalkerState& state)
		: WalkerBase(state)
	{
	}
	template<typename T>
	void visit(T* symbol)
	{
		TREEWALKER_LEAF(symbol);
		name = getOverloadableOperatorId(symbol);
	}
	template<LexTokenId id>
	void visit(cpp::terminal<id> symbol)
	{
	}
};

struct OperatorFunctionIdWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	Name name;
	OperatorFunctionIdWalker(const WalkerState& state)
		: WalkerBase(state)
	{
	}
	void visit(cpp::overloadable_operator* symbol)
	{
		OverloadableOperatorWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		name = walker.name;
	}
};

struct TemplateIdWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	IdentifierPtr id;
	TemplateArguments arguments;
	TemplateIdWalker(const WalkerState& state)
		: WalkerBase(state), id(0), arguments(context)
	{
	}
	void visit(cpp::identifier* symbol)
	{
		TREEWALKER_LEAF_CACHED(symbol);
		id = &symbol->value;
	}
	void visit(cpp::operator_function_id* symbol) 
	{
		Source source = parser->get_source();
		FilePosition position = parser->get_position();
		OperatorFunctionIdWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		symbol->value.value = walker.name;
		symbol->value.source = source.absolute;
		symbol->value.position = position;
		id = &symbol->value;
	}
	void visit(cpp::template_argument_clause* symbol)
	{
		clearQualifying();
		TemplateArgumentListWalker walker(getState());
		TREEWALKER_WALK_CACHED(walker, symbol);
		arguments.swap(walker.arguments);
	}
};

struct UnqualifiedIdWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	DeclarationInstanceRef declaration;
	IdentifierPtr id;
	TemplateArguments arguments; // only used if the identifier is a template-name
	bool isIdentifier;
	bool isTemplate;
	UnqualifiedIdWalker(const WalkerState& state, bool isTemplate = false)
		: WalkerBase(state), id(0), arguments(context), isIdentifier(false), isTemplate(isTemplate)
	{
	}
	void visit(cpp::identifier* symbol)
	{
		TREEWALKER_LEAF_CACHED(symbol);
		id = &symbol->value;
		isIdentifier = true;
		if(!isDependent(qualifying_p))
		{
			declaration = findDeclaration(*id);
		}
	}
	void visit(cpp::simple_template_id* symbol)
	{
		TemplateIdWalker walker(getState());
		TREEWALKER_WALK_CACHED(walker, symbol);
		if(!isTemplate
			&& !isDependent(qualifying_p))
		{
			DeclarationInstanceRef declaration = findDeclaration(*walker.id);
			if(declaration == &gUndeclared
				|| !isTemplateName(*declaration))
			{
				return reportIdentifierMismatch(symbol, *walker.id, declaration, "template-name");
			}
			this->declaration = declaration;
		}
		id = walker.id;
		arguments.swap(walker.arguments);
	}
	void visit(cpp::template_id_operator_function* symbol)
	{
		TemplateIdWalker walker(getState());
		TREEWALKER_WALK_CACHED(walker, symbol);
#if 0 // TODO: member lookup
		if(!isTemplate // TODO: is this possible?
			&& !isDependent(qualifying_p))
		{
			DeclarationInstanceRef declaration = findDeclaration(*walker.id);
			if(declaration == &gUndeclared
				|| !isTemplateName(*declaration))
			{
				return reportIdentifierMismatch(symbol, *walker.id, declaration, "template-name");
			}
			this->declaration = findTemplateSpecialization(declaration, walker.arguments);
		}
#endif
		id = walker.id;
		arguments.swap(walker.arguments);
	}
	void visit(cpp::operator_function_id* symbol) 
	{
		Source source = parser->get_source();
		FilePosition position = parser->get_position();
		OperatorFunctionIdWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		symbol->value.value = walker.name;
		symbol->value.source = source.absolute;
		symbol->value.position = position;
		id = &symbol->value;
	}
	void visit(cpp::conversion_function_id* symbol) 
	{
		TypeIdWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		// TODO
		id = &gConversionFunctionId;
	}
	void visit(cpp::destructor_id* symbol)
	{
		// TODO: can destructor-id be dependent?
		TREEWALKER_LEAF(symbol);
		id = &symbol->name->value;
	}
};

struct QualifiedIdWalker : public WalkerQualified
{
	TREEWALKER_DEFAULT;

	DeclarationInstanceRef declaration;
	IdentifierPtr id;
	TemplateArguments arguments; // only used if the identifier is a template-name
	bool isTemplate;
	QualifiedIdWalker(const WalkerState& state)
		: WalkerQualified(state), id(0), arguments(context), isTemplate(false)
	{
	}

	void visit(cpp::terminal<boost::wave::T_COLON_COLON> symbol)
	{
		setQualifyingGlobal();
	}
	void visit(cpp::nested_name_specifier* symbol)
	{
		NestedNameSpecifierWalker walker(getState());
		TREEWALKER_WALK_CACHED(walker, symbol);
		swapQualifying(walker.qualifying);
	}
	void visit(cpp::terminal<boost::wave::T_TEMPLATE> symbol)
	{
		isTemplate = true;
	}
	void visit(cpp::unqualified_id* symbol)
	{
		UnqualifiedIdWalker walker(getState(), isTemplate);
		TREEWALKER_WALK(walker, symbol);
		declaration = walker.declaration;
		id = walker.id;
		arguments.swap(walker.arguments);
	}
	void visit(cpp::qualified_id_suffix* symbol)
	{
		UnqualifiedIdWalker walker(getState(), isTemplate);
		TREEWALKER_WALK(walker, symbol);
		declaration = walker.declaration;
		id = walker.id;
		arguments.swap(walker.arguments);
	}
};

struct IdExpressionWalker : public WalkerQualified
{
	TREEWALKER_DEFAULT;

	/* 14.6.2.2-3
	An id-expression is type-dependent if it contains:
	 an identifier that was declared with a dependent type,
	 a template-id that is dependent,
	 a conversion-function-id that specifies a dependent type,
	 a nested-name-specifier or a qualified-id that names a member of an unknown specialization
	*/
	DeclarationInstanceRef declaration;
	IdentifierPtr id;
	TemplateArguments arguments; // only used if the identifier is a template-name
	bool isIdentifier;
	bool isTemplate;
	IdExpressionWalker(const WalkerState& state, bool isTemplate = false)
		: WalkerQualified(state), id(0), arguments(context), isIdentifier(false), isTemplate(isTemplate)
	{
	}
	void visit(cpp::qualified_id_default* symbol)
	{
		// TODO
		QualifiedIdWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		declaration = walker.declaration;
		id = walker.id;
		arguments.swap(walker.arguments);
		swapQualifying(walker.qualifying);
	}
	void visit(cpp::qualified_id_global* symbol)
	{
		// TODO
		QualifiedIdWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		declaration = walker.declaration;
		id = walker.id;
		arguments.swap(walker.arguments);
		swapQualifying(walker.qualifying);
	}
	void visit(cpp::unqualified_id* symbol)
	{
		// TODO
		UnqualifiedIdWalker walker(getState(), isTemplate);
		TREEWALKER_WALK(walker, symbol);
		declaration = walker.declaration;
		id = walker.id;
		arguments.swap(walker.arguments);
		isIdentifier = walker.isIdentifier;
	}
};

struct ExplicitTypeExpressionWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	TypeId type;
	Dependent typeDependent;
	Dependent valueDependent;
	ExplicitTypeExpressionWalker(const WalkerState& state)
		: WalkerBase(state), type(0, context), typeDependent(context), valueDependent(context)
	{
	}
	void visit(cpp::simple_type_specifier* symbol)
	{
		TypeSpecifierWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		type.swap(walker.type);
		if(type.declaration == 0)
		{
			type = getFundamentalType(walker.fundamental);
		}
		addDependent(typeDependent, type);
	}
	void visit(cpp::typename_specifier* symbol)
	{
		TypenameSpecifierWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		type.swap(walker.type);
		addDependent(typeDependent, type);
	}
	void visit(cpp::type_id* symbol)
	{
		TypeIdWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		type.swap(walker.type);
		addDependent(typeDependent, type);
		addDependent(typeDependent, walker.valueDependent);
	}
	void visit(cpp::new_type* symbol)
	{
		TypeIdWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		type.swap(walker.type);
		addDependent(typeDependent, type);
		addDependent(typeDependent, walker.valueDependent);
	}
	void visit(cpp::assignment_expression* symbol)
	{
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		addDependent(valueDependent, walker.valueDependent);
	}
	void visit(cpp::cast_expression* symbol)
	{
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		addDependent(valueDependent, walker.valueDependent);
	}
};

struct ArgumentListWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	UniqueTypeIds arguments;
	Dependent typeDependent;
	Dependent valueDependent;
	ArgumentListWalker(const WalkerState& state)
		: WalkerBase(state), arguments(context), typeDependent(context), valueDependent(context)
	{
	}
	void visit(cpp::assignment_expression* symbol)
	{
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		arguments.push_front(walker.type);
		addDependent(typeDependent, walker.typeDependent);
		addDependent(valueDependent, walker.valueDependent);
	}
};

struct LiteralWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	UniqueTypeId type;
	LiteralWalker(const WalkerState& state)
		: WalkerBase(state)
	{
	}
	void visit(cpp::numeric_literal* symbol)
	{
		TREEWALKER_LEAF(symbol);
		if(symbol->id == cpp::numeric_literal::UNKNOWN) // workaround for boost::wave issue: T_PP_NUMBER exists in final token stream
		{
			symbol->id = isFloatingLiteral(symbol->value.value.c_str()) ? cpp::numeric_literal::FLOATING : cpp::numeric_literal::INTEGER;
		}
		type = getNumericLiteralType(symbol);
	}
	void visit(cpp::string_literal* symbol)
	{
		TREEWALKER_LEAF(symbol);
		type = getStringLiteralType(symbol);
	}
};

struct DependentPrimaryExpressionWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	DeclarationInstanceRef declaration;
	IdentifierPtr id;
	Dependent typeDependent;
	DependentPrimaryExpressionWalker(const WalkerState& state)
		: WalkerBase(state), id(0), typeDependent(context)
	{
	}
	void visit(cpp::id_expression* symbol)
	{
		/* temp.dep.expr
		An id-expression is type-dependent if it contains:
		 an identifier that was declared with a dependent type,
		 a template-id that is dependent,
		 a conversion-function-id that specifies a dependent type,
		 a nested-name-specifier or a qualified-id that names a member of an unknown specialization.
		*/
		IdExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		declaration = walker.declaration;
		if(walker.isIdentifier // expression is 'identifier'
			&& (declaration == &gUndeclared // identifier was not previously declared
				|| (isObject(*declaration) && declaration->scope->type == SCOPETYPE_NAMESPACE))) // identifier was previously declared in namespace-scope
		{
			// defer name-lookup: this identifier may be a dependent-name.
			id = walker.id;
		}
		else
		{
			if(declaration != 0)
			{
				if(declaration == &gUndeclared
					|| !isObject(*declaration))
				{
					return reportIdentifierMismatch(symbol, *walker.id, declaration, "object-name");
				}
				setDecoration(walker.id, declaration);
				addDependentType(typeDependent, declaration); // an id-expression is type-dependent if it contains an identifier that was declared with a dependent type
			}
			else if(walker.id != 0)
			{
				setDecoration(walker.id, gDependentObjectInstance);
				if(!walker.qualifying.empty())
				{
					addDependent(typeDependent, walker.qualifying.back());
				}
			}
		}
	}
	void visit(cpp::primary_expression_parenthesis* symbol)
	{
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		addDependent(typeDependent, walker.typeDependent);
	}
};

// walks an argument-dependent-lookup function-call expression: postfix-expression ( expression-list. )
struct DependentPostfixExpressionWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	DeclarationInstanceRef declaration;
	IdentifierPtr id;
	Dependent typeDependent;
	DependentPostfixExpressionWalker(const WalkerState& state)
		: WalkerBase(state), id(0), typeDependent(context)
	{
	}
	void visit(cpp::primary_expression* symbol)
	{
		DependentPrimaryExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		declaration = walker.declaration;
		id = walker.id;
		addDependent(typeDependent, walker.typeDependent);
	}
	void visit(cpp::postfix_expression_call* symbol)
	{
		ArgumentListWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		addDependent(typeDependent, walker.typeDependent);
		if(id != 0)
		{
			if(!isDependent(walker.typeDependent))
			{
				if(declaration != 0)
				{
					if(declaration == &gUndeclared
						|| !isObject(*declaration))
					{
						return reportIdentifierMismatch(symbol, *id, declaration, "object-name");
					}
					addDependentType(typeDependent, declaration);
					setDecoration(id, declaration);
				}
			}
			else
			{
				setDecoration(id, gDependentObjectInstance);
			}
		}
	}
};

struct PrimaryExpressionWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	UniqueTypeId type;
	IdentifierPtr id; // only valid when the expression is a (parenthesised) id-expression
	Dependent typeDependent;
	Dependent valueDependent;
	PrimaryExpressionWalker(const WalkerState& state)
		: WalkerBase(state), id(0), typeDependent(context), valueDependent(context)
	{
	}
	void visit(cpp::literal* symbol)
	{
		LiteralWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		SEMANTIC_ASSERT(walker.type.value != UNIQUETYPE_NULL);
		type.swap(walker.type);
	}
	/* temp.dep.constexpr
	An identifier is value-dependent if it is:
	 a name declared with a dependent type,
	 the name of a non-type template parameter,
	 a constant with integral or enumeration type and is initialized with an expression that is value-dependent.
	*/
	void visit(cpp::id_expression* symbol)
	{
		IdExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		id = walker.id;
		DeclarationInstanceRef declaration = walker.declaration;
		if(declaration != 0)
		{
			if(declaration == &gUndeclared
				|| !isObject(*declaration))
			{
				return reportIdentifierMismatch(symbol, *id, declaration, "object-name");
			}
			addDependentType(typeDependent, declaration);
			addDependentType(valueDependent, declaration);
			addDependentName(valueDependent, declaration);
			setDecoration(id, declaration);

			type = gUniqueTypeNull;
			if(!isFunction(*declaration) // if the id-expression refers to a function, the function parameters may be dependent; defer evaluation of type
				&& declaration->type.declaration != &gDependentType
				&& !isDependent(declaration->type)
				&& !isDependent(walker.arguments) // the id-expression may have an explicit template argument list
				&& !isDependent(walker.qualifying.get_ref()))
			{
				type = makeUniqueType(declaration->type, makeUniqueEnclosing(walker.qualifying, enclosingType));
			}
		}
		else
		{
			if(isDependent(walker.qualifying.get_ref()))
			{
				setDecoration(id, gDependentObjectInstance);
			}
			if(!walker.qualifying.empty())
			{
				addDependent(typeDependent, walker.qualifying.back());
			}
		}
	}
	void visit(cpp::primary_expression_parenthesis* symbol)
	{
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		type.swap(walker.type);
		id = walker.id;
		addDependent(typeDependent, walker.typeDependent);
		addDependent(valueDependent, walker.valueDependent);
	}
	void visit(cpp::primary_expression_builtin* symbol)
	{
		TREEWALKER_LEAF(symbol);
		// TODO: cv-qualifiers: change enclosingType to a UniqueType<DeclaratorObject>
		type = (enclosingType != 0) ? UniqueTypeWrapper(pushUniqueType(gUniqueTypes, makeUniqueObjectType(*enclosingType).value, DeclaratorPointer())) : gUniqueTypeNull;
		/* 14.6.2.2-2
		'this' is type-dependent if the class type of the enclosing member function is dependent
		*/
		addDependent(typeDependent, getClassScope()); // TODO: use full type, not just scope
		setExpressionType(symbol, type);
	}
};

struct PostfixExpressionMemberWalker : public WalkerQualified
{
	TREEWALKER_DEFAULT;

	DeclarationInstanceRef declaration;
	IdentifierPtr id; // only valid when the expression is a (parenthesised) id-expression
	bool isTemplate;
	bool isArrow;
	PostfixExpressionMemberWalker(const WalkerState& state)
		: WalkerQualified(state), id(0), isTemplate(false), isArrow(false)
	{
	}
	void visit(cpp::member_operator* symbol)
	{
		TREEWALKER_LEAF(symbol);
		isArrow = true;
	}
	void visit(cpp::terminal<boost::wave::T_TEMPLATE> symbol)
	{
		isTemplate = true;
	}
	void visit(cpp::id_expression* symbol)
	{
		IdExpressionWalker walker(getState(), isTemplate);
		TREEWALKER_WALK(walker, symbol);
		id = walker.id;
		declaration = walker.declaration;
		swapQualifying(walker.qualifying);
	}
};

struct PostfixExpressionWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	UniqueTypeId type;
	IdentifierPtr id; // only valid when the expression is a (parenthesised) id-expression
	Dependent typeDependent;
	Dependent valueDependent;
	PostfixExpressionWalker(const WalkerState& state)
		: WalkerBase(state), id(0), typeDependent(context), valueDependent(context)
	{
	}
	void visit(cpp::primary_expression* symbol)
	{
		PrimaryExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		type.swap(walker.type);
		addDependent(typeDependent, walker.typeDependent);
		addDependent(valueDependent, walker.valueDependent);
		id = walker.id;
		setExpressionType(symbol, type);
	}
	// prefix
	void visit(cpp::postfix_expression_disambiguate* symbol)
	{
		// this is reached only if the lookup of the identifier in the primary-expression failed.
		// TODO: 
		/* 14.6.2-1
		In an expression of the form:
		postfix-expression ( expression-list. )
		where the postfix-expression is an unqualified-id but not a template-id, the unqualified-id denotes a dependent
		name if and only if any of the expressions in the expression-list is a type-dependent expression (
		*/
		DependentPostfixExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		// TODO: type = &gDependent;
		addDependent(typeDependent, walker.typeDependent);
	}
	void visit(cpp::postfix_expression_construct* symbol)
	{
		ExplicitTypeExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		addDependent(typeDependent, walker.typeDependent);
		type = gUniqueTypeNull;
		if(!isDependent(walker.type))
		{
			type = makeUniqueType(walker.type, enclosingType);
		}
		setExpressionType(symbol, type);
	}
	void visit(cpp::postfix_expression_cast* symbol)
	{
		ExplicitTypeExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		type = gUniqueTypeNull;
		if(!isDependent(walker.type))
		{
			type = makeUniqueType(walker.type, enclosingType);
		}
		if(symbol->op->id != cpp::cast_operator::DYNAMIC)
		{
			Dependent tmp(walker.typeDependent);
			addDependent(valueDependent, tmp);
		}
		addDependent(typeDependent, walker.typeDependent);
		addDependent(valueDependent, walker.valueDependent);
		setExpressionType(symbol, type);
	}
	void visit(cpp::postfix_expression_typeid* symbol)
	{
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		// TODO: type = std::type_info
		// not dependent
	}
	void visit(cpp::postfix_expression_typeidtype* symbol)
	{
		TypeIdWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		// TODO: type = std::type_info
		// not dependent
	}
	// suffix
	void visit(cpp::postfix_expression_index* symbol) // TODO: overloaded operator[]
	{
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		type.swap(walker.type);
		addDependent(typeDependent, walker.typeDependent);
		addDependent(valueDependent, walker.valueDependent);
		id = 0; // don't perform overload resolution for a[i](x);
	}
	void visit(cpp::postfix_expression_call* symbol)
	{
		ArgumentListWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		addDependent(typeDependent, walker.typeDependent);
		addDependent(valueDependent, walker.valueDependent);
		if(!isDependent(typeDependent)) // the expression is not dependent
			// TODO: check valueDependent too?
		{
			if(id != 0 // the prefix contains an id-expression
				&& isDecorated(*id) // TODO: assert!
				&& &getDeclaration(*id) != &gDependentObject // the id-expression was not dependent
				&& isFunction(getDeclaration(*id)) // the identifier names an overloadable function
				&& !isMemberOfTemplate(getDeclaration(*id))) // the name of a member function of a template may be dependent: TODO: determine exactly when!
			{
				// TODO: 13.3.1.1.1  Call to named function
				Declaration* declaration = findBestMatch(*id->dec.p, walker.arguments);
				if(declaration != 0)
				{
					DeclarationInstanceRef instance = findLastDeclaration(*id->dec.p, declaration);
					setDecoration(id, instance);
					type = isDependent(declaration->type) ? gUniqueTypeNull : makeUniqueType(declaration->type);
				}
			}
		}
		else
		{
			type = gUniqueTypeNull;
		}
		// TODO: assert
		if(type.isFunction()) // the type of an expression naming a function is T()
		{
			type.pop_front(); // get the return type: T
		}
		// TODO: 13.3.1.1.2  Call to object of class type
		// TODO: set of pointers-to-function
		id = 0; // don't perform overload resolution for a(x)(x);
	}
	void visit(cpp::postfix_expression_member* symbol)
	{
		PostfixExpressionMemberWalker walker(getState());
		UniqueType object = type.isSimple()
			? type.value : type.isSimplePointer() || type.isSimpleReference()
				? getInner(type.value) : UNIQUETYPE_NULL;
		if(object != UNIQUETYPE_NULL) // if the left-hand side is an object type (or pointer/reference-to-object)
		{
			walker.memberObject = getObjectType(object).declaration->enclosed; // TODO: assert that this is a class type
		}
		TREEWALKER_WALK(walker, symbol);
		id = walker.id; // perform overload resolution for a.m(x);
		DeclarationInstanceRef declaration = walker.declaration;
		if(declaration != 0)
		{
			if(declaration == &gUndeclared
				|| !isObject(*declaration))
			{
				// TODO: report non-fatal error
				//reportIdentifierMismatch(symbol, *id, declaration, "object-name");
				return;
			}
			addDependentType(typeDependent, declaration);
			addDependentType(valueDependent, declaration);
			addDependentName(valueDependent, declaration);
			setDecoration(id, declaration);

			if(type.value != UNIQUETYPE_NULL // TODO: dependent member name lookup
				&& !isDependent(declaration->type)
				&& !isDependent(walker.qualifying.get_ref())
				&& !declaration->isTemplate // TODO: member template
				&& !declaration->type.isImplicitTemplateId) // TODO: 
			{
				if(walker.isArrow) // dereference
				{
					// TODO: dependent types
					if(type.isReference()) // T*&
					{
						type.pop_front();
					}
					if(type.isPointer()) // TODO: overloaded operator->
					{
						type.pop_front();
					}
				}

				SEMANTIC_ASSERT(type.isSimple() || type.isSimpleReference()); // TODO: non-fatal error
				const TypeInstance* enclosing = &getObjectType(type.value);
#if 0
				if(!enclosing->declaration->isTemplate) // if the left-hand side is not a template instantiation
				{
					SEMANTIC_ASSERT(enclosing->enclosing != 0);
					enclosing = enclosing->enclosing; // use its enclosing template-instantiation
				}
#endif
				type = makeUniqueType(declaration->type, makeUniqueEnclosing(walker.qualifying, enclosing));
			}
		}
		else
		{
			if(isDependent(walker.qualifying.get_ref()))
			{
				setDecoration(id, gDependentObjectInstance);
			}
			if(!walker.qualifying.empty())
			{
				addDependent(typeDependent, walker.qualifying.back());
			}
		}
	}
	void visit(cpp::postfix_expression_destructor* symbol)
	{
		TREEWALKER_LEAF(symbol);
		type = gVoid; // TODO: should this be null-type?
		id = 0;
		// TODO: name-lookup for member id-expression
	}
};

struct ExpressionWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	IdentifierPtr id; // only valid when the expression is a (parenthesised) id-expression
	UniqueTypeId type;
	/* 14.6.2.2-1
	...an expression is type-dependent if any subexpression is type-dependent.
	*/
	Dependent typeDependent;
	Dependent valueDependent;
	ExpressionWalker(const WalkerState& state)
		: WalkerBase(state), id(0), typeDependent(context), valueDependent(context)
	{
	}
	// this path handles the right-hand side of a binary expression
	// it is assumed that 'type' already contains the type of the left-hand side
	template<typename T, typename Select>
	void walkBinaryExpression(T* symbol, Select select)
	{
		// TODO: SEMANTIC_ASSERT(walker.type.declaration != 0);
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		id = walker.id;
		addDependent(typeDependent, walker.typeDependent);
		addDependent(valueDependent, walker.valueDependent);
		// TODO: SEMANTIC_ASSERT(type.declaration != 0 && walker.type.declaration != 0);
		type = select(type, walker.type);
		ExpressionType<T>::set(symbol, type);
	}
	template<typename T>
	void walkBinaryArithmeticExpression(T* symbol)
	{
		walkBinaryExpression(symbol, binaryOperatorArithmeticType);
		// TODO: overloaded arithmetic operators
	}
	template<typename T>
	void walkBinaryAdditiveExpression(T* symbol)
	{
		walkBinaryExpression(symbol, binaryOperatorAdditiveType);
		// TODO: overloaded arithmetic operators
	}
	template<typename T>
	void walkBinaryIntegralExpression(T* symbol)
	{
		walkBinaryExpression(symbol, binaryOperatorIntegralType);
		// TODO: overloaded shift operators
	}
	template<typename T>
	void walkBinaryBooleanExpression(T* symbol)
	{
		walkBinaryExpression(symbol, binaryOperatorBoolean);
		// TODO: overloaded boolean operators
	}
	void visit(cpp::assignment_expression_suffix* symbol)
	{
		// 5.1.7 Assignment operators
		// the type of an assignment expression is that of its left operand
		walkBinaryExpression(symbol, binaryOperatorAssignment);
	}
	void visit(cpp::conditional_expression_suffix* symbol)
	{
		walkBinaryExpression(symbol, binaryOperatorNull);
		// TODO: determine type of conditional expression, including implicit conversions
	}
	void visit(cpp::logical_or_expression_default* symbol)
	{
		walkBinaryIntegralExpression(symbol);
	}
	void visit(cpp::logical_and_expression_default* symbol)
	{
		walkBinaryBooleanExpression(symbol);
	}
	void visit(cpp::inclusive_or_expression_default* symbol)
	{
		walkBinaryIntegralExpression(symbol);
	}
	void visit(cpp::exclusive_or_expression_default* symbol)
	{
		walkBinaryIntegralExpression(symbol);
	}
	void visit(cpp::and_expression_default* symbol)
	{
		walkBinaryIntegralExpression(symbol);
	}
	void visit(cpp::equality_expression_default* symbol)
	{
		walkBinaryBooleanExpression(symbol);
	}
	void visit(cpp::relational_expression_default* symbol)
	{
		walkBinaryBooleanExpression(symbol);
	}
	void visit(cpp::shift_expression_default* symbol)
	{
		walkBinaryIntegralExpression(symbol);
	}
	void visit(cpp::additive_expression_default* symbol)
	{
		walkBinaryAdditiveExpression(symbol);
	}
	void visit(cpp::multiplicative_expression_default* symbol)
	{
		walkBinaryArithmeticExpression(symbol);
	}
	void visit(cpp::pm_expression_default* symbol)
	{
		walkBinaryExpression(symbol, binaryOperatorNull);
		// TODO: determine type of pm expression
	}
	void visit(cpp::assignment_expression_default* symbol)
	{
		TREEWALKER_LEAF(symbol);
		setExpressionType(symbol, type);
	}
	void visit(cpp::expression* symbol) // RHS of expression-list
	{
		// TODO: this could also be ( expression )
		walkBinaryExpression(symbol, binaryOperatorComma);
	}
	void visit(cpp::expression_list* symbol)
	{
		TREEWALKER_LEAF(symbol);
		setExpressionType(symbol, type);
	}
	void visit(cpp::postfix_expression* symbol)
	{
		PostfixExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		id = walker.id;
		type.swap(walker.type);
		addDependent(typeDependent, walker.typeDependent);
		addDependent(valueDependent, walker.valueDependent);
		//setDependent(dependent, walker.dependent); // TODO:
		setExpressionType(symbol, type);
	}
	void visit(cpp::unary_expression_op* symbol)
	{
		Source source = parser->get_source();
		FilePosition position = parser->get_position();
		TREEWALKER_LEAF(symbol); 
		if(type.value != UNIQUETYPE_NULL) // TODO: assert
		{
			Identifier id;
			id.value = getUnaryOperatorName(symbol->op);
			id.position = position;
			id.source = source.absolute;
			Declaration* declaration = findBestOverloadedOperator(id, type);
			if(declaration == &gUnknown)
			{
				type = getBuiltInUnaryOperatorReturnType(symbol->op, type);
			}
			else
			{
				SEMANTIC_ASSERT(declaration != 0);
				type = makeUniqueType(declaration->type, &getObjectType(type.value));
			}
			// TODO: decorate parse-tree with declaration
		}
		else
		{
			type = gUniqueTypeNull;
		}
		setExpressionType(symbol, type);
	}
	/* 14.6.2.2-3
	Expressions of the following forms are type-dependent only if the type specified by the type-id, simple-type-specifier
	or new-type-id is dependent, even if any subexpression is type-dependent:
	- postfix-expression-construct
	- new-expression
	- postfix-expression-cast
	- cast-expression
	*/
	/* temp.dep.constexpr
	Expressions of the following form are value-dependent if either the type-id or simple-type-specifier is dependent or the
	expression or cast-expression is value-dependent:
	simple-type-specifier ( expression-listopt )
	static_cast < type-id > ( expression )
	const_cast < type-id > ( expression )
	reinterpret_cast < type-id > ( expression )
	( type-id ) cast-expression
	*/
	void visit(cpp::new_expression_placement* symbol)
	{
		ExplicitTypeExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		type = isDependent(walker.type) ? gUniqueTypeNull : makeUniqueType(walker.type, enclosingType);
		type.push_front(DeclaratorPointer());
		addDependent(typeDependent, walker.typeDependent);
		setExpressionType(symbol, type);
	}
	void visit(cpp::new_expression_default* symbol)
	{
		ExplicitTypeExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		type = isDependent(walker.type) ? gUniqueTypeNull : makeUniqueType(walker.type, enclosingType);
		type.push_front(DeclaratorPointer());
		addDependent(typeDependent, walker.typeDependent);
		setExpressionType(symbol, type);
	}
	void visit(cpp::cast_expression_default* symbol)
	{
		ExplicitTypeExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		type = isDependent(walker.type) ? gUniqueTypeNull : makeUniqueType(walker.type, enclosingType);
		Dependent tmp(walker.typeDependent);
		addDependent(valueDependent, tmp);
		addDependent(typeDependent, walker.typeDependent);
		addDependent(valueDependent, walker.valueDependent);
		setExpressionType(symbol, type);
	}
	/* 14.6.2.2-4
	Expressions of the following forms are never type-dependent (because the type of the expression cannot be
	dependent):
	literal
	postfix-expression . pseudo-destructor-name
	postfix-expression -> pseudo-destructor-name
	sizeof unary-expression
	sizeof ( type-id )
	sizeof ... ( identifier )
	alignof ( type-id )
	typeid ( expression )
	typeid ( type-id )
	::opt delete cast-expression
	::opt delete [ ] cast-expression
	throw assignment-expressionopt
	*/
	// TODO: destructor-call is not dependent
	/* temp.dep.constexpr
	Expressions of the following form are value-dependent if the unary-expression is type-dependent or the type-id is dependent
	(even if sizeof unary-expression and sizeof ( type-id ) are not type-dependent):
	sizeof unary-expression
	sizeof ( type-id )
	*/
	void visit(cpp::unary_expression_sizeof* symbol)
	{
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		addDependent(valueDependent, walker.typeDependent);
	}
	void visit(cpp::unary_expression_sizeoftype* symbol)
	{
		TypeIdWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		addDependent(valueDependent, walker.type);
		addDependent(valueDependent, walker.valueDependent);
	}
	void visit(cpp::delete_expression* symbol)
	{
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::throw_expression* symbol)
	{
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
};

struct IsHiddenTypeName
{
	DeclarationPtr nonType; // valid if the declaration is hidden by a non-type name
	DeclarationPtr hidingNamespace; // valid if the declaration is hidden by a namespace name

	IsHiddenTypeName()
		: nonType(0), hidingNamespace(0)
	{
	}

	bool operator()(const Declaration& declaration)
	{
		if(isTypeName(declaration))
		{
			return true;
		}
		if(nonType == 0
			&& isAny(declaration))
		{
			nonType = const_cast<Declaration*>(&declaration); // TODO: fix const
		}
		if(hidingNamespace == 0
			&& isNamespaceName(declaration))
		{
			hidingNamespace = const_cast<Declaration*>(&declaration); // TODO: fix const
		}
		return false;
	}
};


struct TypeNameWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	Type type;
	IsHiddenTypeName filter; // allows type-name to be parsed without knowing whether it is the prefix of a nested-name-specifier (in which case it cannot be hidden by a non-type name)
	bool isTypename; // true if a type is expected in this context; e.g. following 'typename', preceding '::'
	TypeNameWalker(const WalkerState& state, bool isTypename = false)
		: WalkerBase(state), type(0, context), isTypename(isTypename)
	{
	}

	void visit(cpp::identifier* symbol)
	{
		TREEWALKER_LEAF_CACHED(symbol);
		DeclarationInstanceRef declaration = gDependentTypeInstance;
		if(!isDependent(qualifying_p))
		{
			declaration = findDeclaration(symbol->value, makeLookupFilter(filter));
			if(declaration == &gUndeclared)
			{
				return reportIdentifierMismatch(symbol, symbol->value, declaration, "type-name");
			}
 		}
		else if(!isTypename)
		{
			// dependent type, are you missing a 'typename' keyword?
			return reportIdentifierMismatch(symbol, symbol->value, &gUndeclared, "typename");
		}
		type.id = &symbol->value;
		type.declaration = declaration;
		type.isImplicitTemplateId = declaration->isTemplate;
		setDecoration(&symbol->value, declaration);
		setDependent(type);
#if 1 // temp hack, imitate previous isDependent behaviour
		if(declaration->isTemplate
			&& declaration->templateParameter == INDEX_INVALID) // ignore template-template-parameter
		{
			if(declaration->isSpecialization)
			{
				setDependent(type.dependent, declaration->templateArguments);
			}
			else
			{
				SEMANTIC_ASSERT(!declaration->templateParams.empty());
				setDependent(type.dependent, *declaration->templateParams.front().declaration); // depend on first template param
			}
		}
#endif
	}

	void visit(cpp::simple_template_id* symbol)
	{
		//ProfileScope profile(gProfileTemplateId);

		TemplateIdWalker walker(getState());
		TREEWALKER_WALK_CACHED(walker, symbol);
		DeclarationInstanceRef declaration = lookupTemplate(*walker.id, makeLookupFilter(filter));
		if(declaration == &gUndeclared)
		{
			return reportIdentifierMismatch(symbol, *walker.id, &gUndeclared, "type-name");
		}
		if(declaration == &gDependentTemplate
			&& !isTypename)
		{
			// dependent type, are you missing a 'typename' keyword?
			return reportIdentifierMismatch(symbol, *walker.id, &gUndeclared, "typename");
		}

		setDecoration(walker.id, declaration);
		type.id = walker.id;
		type.declaration = declaration;
		type.templateArguments.swap(walker.arguments);
		setDependent(type); // a template-id is dependent if the 'identifier' is a template-parameter
		setDependent(type.dependent, type.templateArguments); // a template-id is dependent if any of its arguments are dependent
	}
};

struct NestedNameSpecifierSuffixWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	Type type;
	bool isDeclarator;
	bool isTemplate;
	NestedNameSpecifierSuffixWalker(const WalkerState& state, bool isDeclarator = false)
		: WalkerBase(state), type(0, context), isDeclarator(isDeclarator), isTemplate(false)
	{
	}
	void visit(cpp::terminal<boost::wave::T_TEMPLATE> symbol)
	{
		isTemplate = true;
	}
	void visit(cpp::identifier* symbol)
	{
		TREEWALKER_LEAF_CACHED(symbol);
		DeclarationInstanceRef declaration = gDependentNestedInstance;
		if(isDeclarator
			|| !isDependent(qualifying_p))
		{
			declaration = findDeclaration(symbol->value, IsNestedName());
			if(declaration == &gUndeclared)
			{
				return reportIdentifierMismatch(symbol, symbol->value, declaration, "nested-name");
			}
		}
		type = declaration;
		type.id = &symbol->value;
		setDecoration(&symbol->value, declaration);
		if(declaration != &gDependentNested)
		{
			setDependent(type); // a template-id is dependent if the 'identifier' is a template-parameter
		}
	}
	void visit(cpp::simple_template_id* symbol)
	{
		TemplateIdWalker walker(getState());
		TREEWALKER_WALK_CACHED(walker, symbol);
		DeclarationInstanceRef declaration = gDependentNestedInstance;
		if(!isTemplate // TODO: should perform name lookup anyway, even if 'qualifying_p' not dependent!
			&& (isDeclarator
				|| !isDependent(qualifying_p)))
		{
			declaration = findDeclaration(*walker.id, IsNestedName());
			if(declaration == &gUndeclared)
			{
				return reportIdentifierMismatch(symbol, *walker.id, declaration, "nested-name");
			}
		}
		type = declaration;
		type.id = walker.id;
		type.templateArguments.swap(walker.arguments);
		if(declaration != &gDependentNested)
		{
			setDependent(type); // a template-id is dependent if the 'identifier' is a template-parameter
		}
		setDependent(type.dependent, type.templateArguments); // a template-id is dependent if any of its arguments are dependent
	}
};

// basic.lookup.qual
// During the lookup for a name preceding the :: scope resolution operator, object, function, and enumerator names are ignored.
struct NestedNameSpecifierPrefixWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	Type type;
	bool isDeclarator;
	NestedNameSpecifierPrefixWalker(const WalkerState& state, bool isDeclarator = false)
		: WalkerBase(state), type(0, context), isDeclarator(isDeclarator)
	{
	}

#if 0 // for debugging parse-tree cache
	void visit(cpp::nested_name* symbol)
	{
		NestedNameSpecifierPrefixWalker walker(getState(), isDeclarator);
		TREEWALKER_WALK_CACHED(walker, symbol);
		type.swap(walker.type);
	}
#endif
	void visit(cpp::namespace_name* symbol)
	{
		NamespaceNameWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		if(walker.filter.hidingType != 0)
		{
			return reportIdentifierMismatch(symbol, walker.filter.hidingType->getName(), walker.filter.hidingType, "namespace-name");
		}
		type.declaration = walker.declaration;
	}
	void visit(cpp::type_name* symbol)
	{
		TypeNameWalker walker(getState(), true);
		TREEWALKER_WALK(walker, symbol);
		if(walker.filter.hidingNamespace != 0)
		{
			return reportIdentifierMismatch(symbol, walker.filter.hidingNamespace->getName(), walker.filter.hidingNamespace, "type-name");
		}
		if(isDeclarator
			&& !isClass(*walker.type.declaration))
		{
			// the prefix of the nested-name-specifier in a qualified declarator-id must be a class-name (not a typedef)
			return reportIdentifierMismatch(symbol, walker.type.declaration->getName(), walker.type.declaration, "class-name");
		}
		type.swap(walker.type);
	}
};

struct NestedNameSpecifierWalker : public WalkerQualified
{
	TREEWALKER_DEFAULT;

	bool isDeclarator;
	NestedNameSpecifierWalker(const WalkerState& state, bool isDeclarator = false)
		: WalkerQualified(state), isDeclarator(isDeclarator)
	{
	}
	void visit(cpp::nested_name_specifier_prefix* symbol)
	{
		NestedNameSpecifierPrefixWalker walker(getState(), isDeclarator);
		TREEWALKER_WALK(walker, symbol);
		SEMANTIC_ASSERT(walker.type.declaration != 0);
		swapQualifying(walker.type, isDeclarator);
	}
	void visit(cpp::nested_name_specifier_suffix_template* symbol)
	{
		NestedNameSpecifierSuffixWalker walker(getState(), isDeclarator);
		TREEWALKER_WALK(walker, symbol);
		SEMANTIC_ASSERT(walker.type.declaration != 0);
		walker.type.qualifying.swap(qualifying);
		swapQualifying(walker.type, isDeclarator);
	}
	void visit(cpp::nested_name_specifier_suffix_default* symbol)
	{
		NestedNameSpecifierSuffixWalker walker(getState(), isDeclarator);
		TREEWALKER_WALK(walker, symbol);
		SEMANTIC_ASSERT(walker.type.declaration != 0);
		walker.type.qualifying.swap(qualifying);
		swapQualifying(walker.type, isDeclarator);
	}
};

struct TypeSpecifierWalker : public WalkerQualified
{
	TREEWALKER_DEFAULT;

	Type type;
	unsigned fundamental;
	TypeSpecifierWalker(const WalkerState& state)
		: WalkerQualified(state), type(0, context), fundamental(0)
	{
	}
	void visit(cpp::simple_type_specifier_name* symbol)
	{
		TypeSpecifierWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		type.swap(walker.type);
		fundamental = walker.fundamental;
	}
	void visit(cpp::simple_type_specifier_template* symbol) // X::template Y<Z>
	{
		TypeSpecifierWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		type.swap(walker.type);
		fundamental = walker.fundamental;
	}
	void visit(cpp::type_name* symbol) // simple_type_specifier_name
	{
		TypeNameWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		if(walker.filter.nonType != 0)
		{
			// 3.3.7: a type-name can be hidden by a non-type name in the same scope (this rule applies to a type-specifier)
			return reportIdentifierMismatch(symbol, walker.filter.nonType->getName(), walker.filter.nonType, "type-name");
		}
		SEMANTIC_ASSERT(walker.type.declaration != 0);
		type.swap(walker.type);
		type.qualifying.swap(qualifying);
		setDependent(type.dependent, type.qualifying);
	}
	void visit(cpp::terminal<boost::wave::T_COLON_COLON> symbol)
	{
		setQualifyingGlobal();
	}
	void visit(cpp::nested_name_specifier* symbol) // simple_type_specifier_name | simple_type_specifier_template
	{
		NestedNameSpecifierWalker walker(getState());
		TREEWALKER_WALK_CACHED(walker, symbol);
		swapQualifying(walker.qualifying);
	}
	void visit(cpp::simple_template_id* symbol) // simple_type_specifier_template
	{
		TemplateIdWalker walker(getState());
		TREEWALKER_WALK_CACHED(walker, symbol);
		IsHiddenTypeName filter;
		DeclarationInstanceRef declaration = lookupTemplate(*walker.id, makeLookupFilter(filter));
		if(declaration == &gUndeclared)
		{
			return reportIdentifierMismatch(symbol, *walker.id, declaration, "type-name");
		}
		if(declaration == &gDependentTemplate)
		{
			// dependent type, are you missing a 'typename' keyword?
			return reportIdentifierMismatch(symbol, *walker.id, &gUndeclared, "typename");
		}
		if(filter.nonType != 0)
		{
			// 3.3.7: a type-name can be hidden by a non-type name in the same scope (this rule applies to a type-specifier)
			return reportIdentifierMismatch(symbol, filter.nonType->getName(), filter.nonType, "type-name");
		}
		setDecoration(walker.id, declaration);
		type.declaration = declaration;
		type.templateArguments.swap(walker.arguments);
		type.qualifying.swap(qualifying);
		setDependent(type); // a template-id is dependent if the template-name is a template-parameter
		setDependent(type.dependent, type.templateArguments); // a template-id is dependent if any of the template arguments are dependent
		setDependent(type.dependent, type.qualifying);
	}
	void visit(cpp::simple_type_specifier_builtin* symbol)
	{
		TREEWALKER_LEAF(symbol);
		fundamental = combineFundamental(0, symbol->id);
	}
};

struct UnqualifiedDeclaratorIdWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	IdentifierPtr id;
	UnqualifiedDeclaratorIdWalker(const WalkerState& state)
		: WalkerBase(state), id(&gAnonymousId)
	{
	}
	void visit(cpp::identifier* symbol)
	{
		TREEWALKER_LEAF_CACHED(symbol);
		id = &symbol->value;
	}
	void visit(cpp::template_id* symbol) 
	{
		TemplateIdWalker walker(getState());
		TREEWALKER_WALK_CACHED(walker, symbol);
		id = walker.id;
	}
	void visit(cpp::operator_function_id* symbol) 
	{
		Source source = parser->get_source();
		FilePosition position = parser->get_position();
		OperatorFunctionIdWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		symbol->value.value = walker.name;
		symbol->value.source = source.absolute;
		symbol->value.position = position;
		id = &symbol->value;
	}
	void visit(cpp::conversion_function_id* symbol) 
	{
		TypeIdWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		// TODO
		id = &gConversionFunctionId;
	}
	void visit(cpp::destructor_id* symbol) 
	{
		TREEWALKER_LEAF(symbol);
		id = &symbol->name->value;
	}
};

struct QualifiedDeclaratorIdWalker : public WalkerQualified
{
	TREEWALKER_DEFAULT;

	IdentifierPtr id;
	QualifiedDeclaratorIdWalker(const WalkerState& state)
		: WalkerQualified(state), id(&gAnonymousId)
	{
	}
	void visit(cpp::terminal<boost::wave::T_COLON_COLON> symbol)
	{
		setQualifyingGlobal();
	}
	void visit(cpp::nested_name_specifier* symbol)
	{
		NestedNameSpecifierWalker walker(getState(), true); // in a template member definition, the qualifying nested-name-specifier may be dependent on a template-parameter
		TREEWALKER_WALK(walker, symbol); // no need to cache: the nested-name-specifier is not a shared-prefix
		swapQualifying(walker.qualifying, true);
	}
	void visit(cpp::unqualified_id* symbol)
	{
		UnqualifiedDeclaratorIdWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		id = walker.id;
	}
};

struct DeclaratorIdWalker : public WalkerQualified
{
	TREEWALKER_DEFAULT;

	IdentifierPtr id;
	DeclaratorIdWalker(const WalkerState& state)
		: WalkerQualified(state), id(&gAnonymousId)
	{
	}
	void visit(cpp::qualified_id_default* symbol)
	{
		QualifiedDeclaratorIdWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		id = walker.id;
		swapQualifying(walker.qualifying, true);
	}
	void visit(cpp::qualified_id_global* symbol)
	{
		QualifiedDeclaratorIdWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		id = walker.id;
		swapQualifying(walker.qualifying, true);
	}
	void visit(cpp::unqualified_id* symbol)
	{
		UnqualifiedDeclaratorIdWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		id = walker.id;
	}
};

struct ParameterDeclarationClauseWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	Parameters parameters;

	ParameterDeclarationClauseWalker(const WalkerState& state)
		: WalkerBase(state)
	{
#if 0 // TODO: this causes error if parse fails after templateParamScope is modified
		pushScope(templateParamScope != 0 ? templateParamScope : newScope(makeIdentifier("$declarator")));
		enclosing->type = SCOPETYPE_PROTOTYPE;
#else
		pushScope(newScope(makeIdentifier("$declarator"), SCOPETYPE_PROTOTYPE));
		if(templateParamScope != 0)
		{
			enclosing->templateDepth = templateParamScope->templateDepth;
#if 0
			enclosing->declarations = templateParamScope->declarations;
			for(Scope::Declarations::iterator i = enclosing->declarations.begin(); i != enclosing->declarations.end(); ++i)
			{
				Declaration* declaration = &(*i).second;
				setDecoration(&declaration->getName(), declaration);
				declaration->scope = enclosing;
				trackDeclaration(declaration);
			}
#else
			// issue: the return-type of the function being declared may point to a declaration in this list
			for(Scope::DeclarationList::iterator i = templateParamScope->declarationList.begin(); i != templateParamScope->declarationList.end(); ++i)
			{
				Declaration* declaration = allocatorNew(context, *(*i));
				const DeclarationInstance& instance = enclosing->declarations.insert(DeclarationInstance(declaration));
				enclosing->declarationList.push_back(declaration);
				if(!isAnonymous(*declaration))
				{
					setDecoration(&declaration->getName(), instance);
				}
				declaration->scope = enclosing;
				SEMANTIC_ASSERT(declaration->enclosed == 0 || declaration->enclosed->type != SCOPETYPE_CLASS); // can't copy class-declaration!
#ifdef ALLOCATOR_DEBUG
				trackDeclaration(instance);
#endif
			}
#endif
		}
#endif	
		clearTemplateParams();
	}

	void visit(cpp::parameter_declaration* symbol)
	{
		ParameterDeclarationWalker walker(getState(), true);
		TREEWALKER_WALK(walker, symbol);
		if(!isVoidParameter(walker.declaration->type))
		{
			parameters.push_back(Parameter(walker.declaration, walker.defaultArgument));
		}
	}
};

struct ExceptionSpecificationWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	ExceptionSpecificationWalker(const WalkerState& state)
		: WalkerBase(state)
	{
	}
	void visit(cpp::type_id* symbol)
	{
		TypeIdWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
};

struct CvQualifierSeqWalker : WalkerBase
{
	TREEWALKER_DEFAULT;

	CvQualifiers qualifiers;
	CvQualifierSeqWalker(const WalkerState& state)
		: WalkerBase(state)
	{
	}
	void visit(cpp::cv_qualifier* symbol)
	{
		TREEWALKER_LEAF(symbol);
		if(symbol->id == cpp::cv_qualifier::CONST)
		{
			qualifiers.isConst = true;
		}
		else if(symbol->id == cpp::cv_qualifier::VOLATILE)
		{
			qualifiers.isVolatile = true;
		}
	}

};

struct PtrOperatorWalker : public WalkerQualified
{
	TREEWALKER_DEFAULT;

	CvQualifiers qualifiers;
	PtrOperatorWalker(const WalkerState& state)
		: WalkerQualified(state)
	{
	}
	void visit(cpp::nested_name_specifier* symbol)
	{
		NestedNameSpecifierWalker walker(getState());
		TREEWALKER_WALK_CACHED(walker, symbol);
		swapQualifying(walker.qualifying);
	}
	void visit(cpp::cv_qualifier_seq* symbol)
	{
		CvQualifierSeqWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		qualifiers = walker.qualifiers;
	}
};

struct DeclaratorFunctionWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	ScopePtr paramScope;
	Parameters parameters;
	CvQualifiers qualifiers;
	DeclaratorFunctionWalker(const WalkerState& state)
		: WalkerBase(state), paramScope(0)
	{
	}

	void visit(cpp::parameter_declaration_clause* symbol)
	{
		ParameterDeclarationClauseWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		paramScope = walker.enclosing; // store reference for later resumption
		parameters = walker.parameters;
	}
	void visit(cpp::exception_specification* symbol)
	{
		ExceptionSpecificationWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::cv_qualifier_seq* symbol)
	{
		CvQualifierSeqWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		qualifiers = walker.qualifiers;
	}
};

struct DeclaratorArrayWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	Dependent valueDependent;
	DeclaratorArrayWalker(const WalkerState& state)
		: WalkerBase(state), valueDependent(context)
	{
	}

	void visit(cpp::constant_expression* symbol)
	{
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		addDependent(valueDependent, walker.valueDependent);
	}
};

struct DeclaratorWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	IdentifierPtr id;
	ScopePtr paramScope;
	Dependent valueDependent;
	TypeSequence typeSequence;
	CvQualifiers qualifiers;
	Qualifying memberPointer;
	DeclaratorWalker(const WalkerState& state)
		: WalkerBase(state), id(&gAnonymousId), paramScope(0), valueDependent(context), typeSequence(context), memberPointer(context)
	{
	}
	void pushPointerType(cpp::ptr_operator* op)
	{
		(op->key->id == cpp::ptr_operator_key::REF)
			? typeSequence.push_front(DeclaratorReference())
			: memberPointer.empty()
				? typeSequence.push_front(DeclaratorPointer(qualifiers))
				: typeSequence.push_front(DeclaratorMemberPointer(*memberPointer.get(), qualifiers));
	}

	void visit(cpp::ptr_operator* symbol)
	{
		PtrOperatorWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		qualifiers = walker.qualifiers;
		memberPointer.swap(walker.qualifying);
	}
	void visit(cpp::declarator_ptr* symbol)
	{
		DeclaratorWalker walker(getState());
		TREEWALKER_WALK(walker, symbol); // if parse fails, state of typeSeqence is not modified.
		id = walker.id;
		enclosing = walker.enclosing;
		paramScope = walker.paramScope;
		addDependent(valueDependent, walker.valueDependent);
		SYMBOLS_ASSERT(typeSequence.empty());
		typeSequence = walker.typeSequence;

		memberPointer.swap(walker.memberPointer);
		pushPointerType(symbol->op);
	}
	void visit(cpp::abstract_declarator_ptr* symbol)
	{
		DeclaratorWalker walker(getState());
		TREEWALKER_WALK(walker, symbol); // if parse fails, state of typeSeqence is not modified.
		id = walker.id;
		enclosing = walker.enclosing;
		paramScope = walker.paramScope;
		addDependent(valueDependent, walker.valueDependent);
		SYMBOLS_ASSERT(typeSequence.empty());
		typeSequence = walker.typeSequence;

		memberPointer.swap(walker.memberPointer);
		pushPointerType(symbol->op);
	}
	void visit(cpp::declarator_id* symbol)
	{
		DeclaratorIdWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		id = walker.id;

		if(walker.getQualifyingScope()
			&& enclosing->type != SCOPETYPE_CLASS) // in 'class C { friend Q::N(X); };' X should be looked up in the scope of C rather than Q
		{
			enclosing = walker.getQualifyingScope(); // names in declarator suffix (array-size, parameter-declaration) are looked up in declarator-id's qualifying scope
		}
	}
	void visit(cpp::declarator_suffix_array* symbol)
	{
		DeclaratorArrayWalker walker(getState());
		TREEWALKER_WALK_CACHED(walker, symbol);
		addDependent(valueDependent, walker.valueDependent);
		typeSequence.push_front(DeclaratorArray(0)); // TODO: how many dimensions, array size
	}
	void visit(cpp::declarator_suffix_function* symbol)
	{
		DeclaratorFunctionWalker walker(getState());
		TREEWALKER_WALK_CACHED(walker, symbol);
		if(paramScope == 0) // only interested in the innermost parameter-list
		{
			paramScope = walker.paramScope;
		}
		typeSequence.push_front(DeclaratorFunction(walker.parameters, walker.qualifiers));
	}
	void visit(cpp::direct_abstract_declarator* symbol)
	{
		DeclaratorWalker walker(getState());
		TREEWALKER_WALK(walker, symbol); // if parse fails, state of typeSeqence is not modified. e.g. type-id: int((int))
		id = walker.id;
		enclosing = walker.enclosing;
		paramScope = walker.paramScope;
		addDependent(valueDependent, walker.valueDependent);
		SYMBOLS_ASSERT(typeSequence.empty());
		typeSequence = walker.typeSequence;
	}
	void visit(cpp::direct_abstract_declarator_parenthesis* symbol)
	{
		DeclaratorWalker walker(getState());
		TREEWALKER_WALK(walker, symbol); // if parse fails, state of typeSeqence is not modified. e.g. function-style-cast type-id followed by parenthesised expression: int(*this)
		id = walker.id;
		enclosing = walker.enclosing;
		paramScope = walker.paramScope;
		addDependent(valueDependent, walker.valueDependent);
		SYMBOLS_ASSERT(typeSequence.empty());
		typeSequence = walker.typeSequence;
	}
	void visit(cpp::declarator* symbol)
	{
		DeclaratorWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		id = walker.id;
		enclosing = walker.enclosing;
		paramScope = walker.paramScope;
		addDependent(valueDependent, walker.valueDependent);
		SYMBOLS_ASSERT(typeSequence.empty());
		typeSequence = walker.typeSequence;
	}
	void visit(cpp::abstract_declarator* symbol)
	{
		DeclaratorWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		id = walker.id;
		enclosing = walker.enclosing;
		paramScope = walker.paramScope;
		addDependent(valueDependent, walker.valueDependent);
		SYMBOLS_ASSERT(typeSequence.empty());
		typeSequence = walker.typeSequence;
	}
};

struct BaseSpecifierWalker : public WalkerQualified
{
	TREEWALKER_DEFAULT;

	Type type;
	BaseSpecifierWalker(const WalkerState& state)
		: WalkerQualified(state), type(0, context)
	{
	}

	void visit(cpp::nested_name_specifier* symbol)
	{
		NestedNameSpecifierWalker walker(getState());
		TREEWALKER_WALK_CACHED(walker, symbol);
		swapQualifying(walker.qualifying);
	}
	void visit(cpp::class_name* symbol)
	{
		/* 10-2
		The class-name in a base-specifier shall not be an incompletely defined class (Clause class); this class is
		called a direct base class for the class being defined. During the lookup for a base class name, non-type
		names are ignored (3.3.10)
		*/
		TypeNameWalker walker(getState(), true);
		TREEWALKER_WALK(walker, symbol);
		type.swap(walker.type);
		type.qualifying.swap(qualifying);
		setDependent(type.dependent, type.qualifying);
	}
};

struct ClassHeadWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	DeclarationPtr declaration;
	IdentifierPtr id;
	TemplateArguments arguments;
	bool isUnion;
	bool isSpecialization;
	ClassHeadWalker(const WalkerState& state)
		: WalkerBase(state), declaration(0), id(0), arguments(context), isUnion(false), isSpecialization(false)
	{
	}

	void visit(cpp::class_key* symbol)
	{
		TREEWALKER_LEAF_CACHED(symbol);
		isUnion = symbol->id == cpp::class_key::UNION;
	}
	void visit(cpp::identifier* symbol) // class_name
	{
		TREEWALKER_LEAF_CACHED(symbol);
		id = &symbol->value;
	}
	void visit(cpp::nested_name_specifier* symbol)
	{
		NestedNameSpecifierWalker walker(getState());
		TREEWALKER_WALK_CACHED(walker, symbol);

		// resolve the (possibly dependent) qualifying scope
		if(walker.getQualifying(true) != 0)
		{
			if(enclosing == templateEnclosing)
			{
				templateEnclosing = walker.getQualifying(true)->enclosed;
			}
			enclosing = walker.getQualifying(true)->enclosed; // names in declaration of nested-class are looked up in scope of enclosing class
		}
	}
	void visit(cpp::simple_template_id* symbol) // class_name
	{
		TemplateIdWalker walker(getState());
		TREEWALKER_WALK_CACHED(walker, symbol);
		// TODO: don't declare anything - this is a template (partial) specialisation
		id = walker.id;
		arguments.swap(walker.arguments);
		isSpecialization = true;
	}
	void visit(cpp::terminal<boost::wave::T_COLON> symbol)
	{
		// defer class declaration until we know this is a class-specifier - it may be an elaborated-type-specifier until ':' is discovered
		// 3.3.1.3 The point of declaration for a class first declared by a class-specifier is immediately after the identifier or simple-template-id (if any) in its class-head
		declaration = declareClass(id, isSpecialization, arguments);
	}
	void visit(cpp::base_specifier* symbol) 
	{
		BaseSpecifierWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		if(walker.type.declaration != 0) // declaration == 0 if base-class is dependent
		{
			SEMANTIC_ASSERT(declaration->enclosed != 0);
			addBase(declaration, walker.type);
		}
	}
};

struct UsingDeclarationWalker : public WalkerQualified
{
	TREEWALKER_DEFAULT;

	bool isTypename;
	UsingDeclarationWalker(const WalkerState& state)
		: WalkerQualified(state), isTypename(false)
	{
	}
	void visit(cpp::terminal<boost::wave::T_COLON_COLON> symbol)
	{
		setQualifyingGlobal();
	}
	void visit(cpp::nested_name_specifier* symbol)
	{
		NestedNameSpecifierWalker walker(getState());
		TREEWALKER_WALK_CACHED(walker, symbol);
		swapQualifying(walker.qualifying);
	}
	void visit(cpp::unqualified_id* symbol)
	{
		UnqualifiedIdWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		if(!isTypename
			&& !isDependent(qualifying_p))
		{
			DeclarationInstanceRef declaration = walker.declaration;
			if(declaration == &gUndeclared
				|| !(isObject(*declaration) || isTypeName(*declaration)))
			{
				return reportIdentifierMismatch(symbol, *walker.id, declaration, "object-name or type-name");
			}

			setDecoration(walker.id, declaration); // refer to the primary declaration of this name, rather than the one declared by this using-declaration
			
			DeclarationInstance instance(declaration);
			instance.name = walker.id;
			instance.overloaded = declaration.p;
			instance.redeclared = declaration.p;
			DeclarationInstanceRef redeclaration = enclosing->declarations.insert(instance);
			enclosing->declarationList.push_back(instance);
#ifdef ALLOCATOR_DEBUG
			trackDeclaration(redeclaration);
#endif

		}
		else
		{
			// TODO: introduce typename into enclosing namespace
			setDecoration(walker.id, gDependentTypeInstance);
		}
	}
	void visit(cpp::terminal<boost::wave::T_TYPENAME> symbol)
	{
		isTypename = true;
	}
};

struct UsingDirectiveWalker : public WalkerQualified
{
	TREEWALKER_DEFAULT;

	UsingDirectiveWalker(const WalkerState& state)
		: WalkerQualified(state)
	{
	}
	void visit(cpp::terminal<boost::wave::T_COLON_COLON> symbol)
	{
		setQualifyingGlobal();
	}
	void visit(cpp::nested_name_specifier* symbol)
	{
		NestedNameSpecifierWalker walker(getState());
		TREEWALKER_WALK_CACHED(walker, symbol);
		swapQualifying(walker.qualifying);
	}
	void visit(cpp::namespace_name* symbol)
	{
		/* basic.lookup.udir
		When looking up a namespace-name in a using-directive or namespace-alias-definition, only namespace
		names are considered.
		*/
		NamespaceNameWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		if(!findScope(enclosing, walker.declaration->enclosed))
		{
			enclosing->usingDirectives.push_back(walker.declaration->enclosed);
		}
	}
};

struct NamespaceAliasDefinitionWalker : public WalkerQualified
{
	TREEWALKER_DEFAULT;

	IdentifierPtr id;
	NamespaceAliasDefinitionWalker(const WalkerState& state)
		: WalkerQualified(state), id(0)
	{
	}
	void visit(cpp::terminal<boost::wave::T_COLON_COLON> symbol)
	{
		setQualifyingGlobal();
	}
	void visit(cpp::nested_name_specifier* symbol)
	{
		NestedNameSpecifierWalker walker(getState());
		TREEWALKER_WALK_CACHED(walker, symbol);
		swapQualifying(walker.qualifying);
	}
	void visit(cpp::identifier* symbol)
	{
		TREEWALKER_LEAF(symbol);
		if(id == 0) // first identifier
		{
			id = &symbol->value;
		}
		else // second identifier
		{
			DeclarationInstanceRef declaration = findDeclaration(symbol->value, IsNamespaceName());
			if(declaration == &gUndeclared)
			{
				return reportIdentifierMismatch(symbol, symbol->value, declaration, "namespace-name");
			}

			// TODO: check for conflicts with earlier declarations
			declaration = pointOfDeclaration(context, enclosing, *id, TYPE_NAMESPACE, declaration->enclosed);
#ifdef ALLOCATOR_DEBUG
			trackDeclaration(declaration);
#endif
			setDecoration(id, declaration);
		}
	}
};

struct MemberDeclarationWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	DeclarationPtr declaration;
	MemberDeclarationWalker(const WalkerState& state)
		: WalkerBase(state), declaration(0)
	{
	}
	void visit(cpp::member_template_declaration* symbol)
	{
		TemplateDeclarationWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		declaration = walker.declaration;
	}
	void visit(cpp::member_declaration_implicit* symbol)
	{
		SimpleDeclarationWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		declaration = walker.declaration;
	}
	void visit(cpp::member_declaration_default* symbol)
	{
		SimpleDeclarationWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		SEMANTIC_ASSERT(walker.type.declaration != 0);
		declaration = walker.declaration;
		if(!isClass(*declaration)
			&& !isEnum(*declaration))
		{
			addDeferredLookupType(*declaration);
		}
	}
	void visit(cpp::member_declaration_nested* symbol)
	{
		QualifiedIdWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::using_declaration* symbol)
	{
		UsingDeclarationWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
};


struct ClassSpecifierWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	DeclarationPtr declaration;
	IdentifierPtr id;
	TemplateArguments arguments;
	DeferredSymbols deferred;
	bool isUnion;
	bool isSpecialization;
	ClassSpecifierWalker(const WalkerState& state)
		: WalkerBase(state), declaration(0), id(0), arguments(context), isUnion(false), isSpecialization(false)
	{
	}

	void visit(cpp::class_head* symbol)
	{
		ClassHeadWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		declaration = walker.declaration;
		id = walker.id;
		isUnion = walker.isUnion;
		isSpecialization = walker.isSpecialization;
		arguments.swap(walker.arguments);
		enclosing = walker.enclosing;
	}
	void visit(cpp::terminal<boost::wave::T_LEFTBRACE> symbol)
	{
		// defer class declaration until we know this is a class-specifier - it may be an elaborated-type-specifier until '{' is discovered
		if(declaration == 0)
		{
			// 3.3.1.3 The point of declaration for a class first declared by a class-specifier is immediately after the identifier or simple-template-id (if any) in its class-head
			declaration = declareClass(id, isSpecialization, arguments);
		}

		/* basic.scope.class-1
		The potential scope of a name declared in a class consists not only of the declarative region following
		the names point of declaration, but also of all function bodies, brace-or-equal-initializers of non-static
		data members, and default arguments in that class (including such things in nested classes).
		*/
		if(declaration->enclosed != 0)
		{
			pushScope(declaration->enclosed);
			if(!declaration->isTemplate)
			{
				Type type(declaration, context);
				enclosingType = &getObjectType(makeUniqueType(type, enclosingType).value);
			}
			else
			{
				enclosingType = 0;
			}
		}
		clearTemplateParams();
	}
	void visit(cpp::member_declaration* symbol)
	{
		MemberDeclarationWalker walker(getState());
		if(WalkerState::deferred == 0)
		{
			walker.deferred = &deferred;
		}
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::terminal<boost::wave::T_RIGHTBRACE> symbol)
	{
		parseDeferred(deferred, *this);
	}
};

struct EnumeratorDefinitionWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	DeclarationPtr declaration;
	EnumeratorDefinitionWalker(const WalkerState& state)
		: WalkerBase(state), declaration(0)
	{
	}

	void visit(cpp::identifier* symbol)
	{
		TREEWALKER_LEAF(symbol);
		/* 3.1-4
		The point of declaration for an enumerator is immediately after its enumerator-definition.
		*/
		// TODO: give enumerators a type
		DeclarationInstanceRef instance = pointOfDeclaration(context, enclosing, symbol->value, TYPE_ENUMERATOR, 0, DeclSpecifiers());
#ifdef ALLOCATOR_DEBUG
		trackDeclaration(instance);
#endif
		setDecoration(&symbol->value, instance);
		declaration = instance;
	}
	void visit(cpp::constant_expression* symbol)
	{
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		addDependent(declaration->valueDependent, walker.valueDependent);
	}
};

struct EnumSpecifierWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	DeclarationPtr declaration;
	IdentifierPtr id;
	EnumSpecifierWalker(const WalkerState& state)
		: WalkerBase(state), declaration(0), id(0)
	{
	}

	void visit(cpp::identifier* symbol)
	{
		TREEWALKER_LEAF(symbol);
		id = &symbol->value;
	}

	void visit(cpp::terminal<boost::wave::T_LEFTBRACE> symbol)
	{
		// defer declaration until '{' resolves ambiguity between enum-specifier and elaborated-type-specifier
		if(id != 0)
		{
			DeclarationInstanceRef instance = pointOfDeclaration(context, enclosing, *id, TYPE_ENUM, 0);
			setDecoration(id, instance);
			declaration = instance;
		}
	}

	void visit(cpp::enumerator_definition* symbol)
	{
		if(declaration == 0)
		{
			// unnamed enum
			DeclarationInstanceRef instance = pointOfDeclaration(context, enclosing, enclosing->getUniqueName(), TYPE_ENUM, 0);
#ifdef ALLOCATOR_DEBUG
			trackDeclaration(instance);
#endif
			declaration = instance;
		}
		EnumeratorDefinitionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
};

struct ElaboratedTypeSpecifierWalker : public WalkerQualified
{
	TREEWALKER_DEFAULT;

	DeclarationPtr key;
	Type type;
	IdentifierPtr id;
	ElaboratedTypeSpecifierWalker(const WalkerState& state)
		: WalkerQualified(state), key(0), type(0, context), id(0)
	{
	}
	void visit(cpp::terminal<boost::wave::T_COLON_COLON> symbol)
	{
		setQualifyingGlobal();
	}
	void visit(cpp::elaborated_type_specifier_default* symbol)
	{
		ElaboratedTypeSpecifierWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		type.swap(walker.type);
		id = walker.id;

		if(!isUnqualified(symbol)
			|| !isClassKey(*type.declaration))
		{
			id = 0;
		}
	}
	void visit(cpp::elaborated_type_specifier_template* symbol)
	{
		ElaboratedTypeSpecifierWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		type.swap(walker.type);
		id = walker.id;
	}
	void visit(cpp::nested_name_specifier* symbol) // elaborated_type_specifier_default | elaborated_type_specifier_template
	{
		NestedNameSpecifierWalker walker(getState());
		TREEWALKER_WALK_CACHED(walker, symbol);
		swapQualifying(walker.qualifying);
	}
	void visit(cpp::class_key* symbol)
	{
		TREEWALKER_LEAF_CACHED(symbol);
		key = &gClass;
	}
	void visit(cpp::enum_key* symbol)
	{
		TREEWALKER_LEAF(symbol);
		key = &gEnum;
	}
	void visit(cpp::simple_template_id* symbol) // elaborated_type_specifier_default | elaborated_type_specifier_template
	{
		SEMANTIC_ASSERT(key == &gClass);
		TemplateIdWalker walker(getState());
		TREEWALKER_WALK_CACHED(walker, symbol);
		// 3.4.4-2: when looking up 'identifier' in elaborated-type-specifier, ignore any non-type names that have been declared. 
		DeclarationInstanceRef declaration = lookupTemplate(*walker.id, IsTypeName());
		if(declaration == &gUndeclared)
		{
			return reportIdentifierMismatch(symbol, *walker.id, &gUndeclared, "type-name");
		}
		if(declaration == &gDependentTemplate)
		{
			// dependent type, are you missing a 'typename' keyword?
			return reportIdentifierMismatch(symbol, *walker.id, &gUndeclared, "typename");
		}
		setDecoration(walker.id, declaration);
		id = walker.id;
		type.declaration = declaration;
		type.templateArguments.swap(walker.arguments);
		type.qualifying.swap(qualifying);
	}
	void visit(cpp::identifier* symbol)
	{
		TREEWALKER_LEAF_CACHED(symbol);
		/* 3.4.4-2
		If the elaborated-type-specifier has no nested-name-specifier ...
		... the identifier is looked up according to 3.4.1 but ignoring any non-type names that have been declared. If
		the elaborated-type-specifier is introduced by the enum keyword and this lookup does not find a previously
		declared type-name, the elaborated-type-specifier is ill-formed. If the elaborated-type-specifier is introduced by
		the class-key and this lookup does not find a previously declared type-name ...
		the elaborated-type-specifier is a declaration that introduces the class-name as described in 3.3.1.
		*/
		id = &symbol->value;
		DeclarationInstanceRef declaration = findDeclaration(symbol->value, IsTypeName());
		if(declaration != &gUndeclared
			&& !isTypedef(*declaration))
		{
			// template<typename T> class C
			if(declaration->isSpecialization) // if the lookup found a template explicit/partial-specialization
			{
				SEMANTIC_ASSERT(declaration->isTemplate);
				declaration = findPrimaryTemplateLastDeclaration(declaration); // the name is a plain identifier, not a template-id, therefore the name refers to the primary template
			}
			setDecoration(&symbol->value, declaration);
			/* 7.1.6.3-2
			3.4.4 describes how name lookup proceeds for the identifier in an elaborated-type-specifier. If the identifier
			resolves to a class-name or enum-name, the elaborated-type-specifier introduces it into the declaration the
			same way a simple-type-specifier introduces its type-name. If the identifier resolves to a typedef-name, the
			elaborated-type-specifier is ill-formed.
			*/
#if 0 // allow hiding a typedef with a forward-declaration
			if(isTypedef(*declaration))
			{
				printPosition(symbol->value.position);
				std::cout << "'" << symbol->value.value.c_str() << "': elaborated-type-specifier refers to a typedef" << std::endl;
				printPosition(declaration->getName().position);
				throw SemanticError();
			}
#endif
			/* 7.1.6.3-3
			The class-key or enum keyword present in the elaborated-type-specifier shall agree in kind with the declaration
			to which the name in the elaborated-type-specifier refers.
			*/
			if(declaration->type.declaration != key)
			{
				printPosition(symbol->value.position);
				std::cout << "'" << symbol->value.value.c_str() << "': elaborated-type-specifier key does not match declaration" << std::endl;
				printPosition(declaration->getName().position);
				throw SemanticError();
			}
			type = declaration;
		}
		else
		{
			/* 3.4.4-2
			... If the elaborated-type-specifier is introduced by the enum keyword and this lookup does not find a previously
			declared type-name, the elaborated-type-specifier is ill-formed. If the elaborated-type-specifier is introduced by
			the class-key and this lookup does not find a previously declared type-name ...
			the elaborated-type-specifier is a declaration that introduces the class-name as described in 3.3.1.
			*/
			if(key != &gClass)
			{
				SEMANTIC_ASSERT(key == &gEnum);
				printPosition(symbol->value.position);
				std::cout << "'" << symbol->value.value.c_str() << "': elaborated-type-specifier refers to undefined enum" << std::endl;
				throw SemanticError();
			}
			type = key;
		}
	}
};

struct TypenameSpecifierWalker : public WalkerQualified
{
	TREEWALKER_DEFAULT;

	Type type;
	TypenameSpecifierWalker(const WalkerState& state)
		: WalkerQualified(state), type(0, context)
	{
	}

	void visit(cpp::terminal<boost::wave::T_COLON_COLON> symbol)
	{
		setQualifyingGlobal();
	}
	void visit(cpp::terminal<boost::wave::T_TEMPLATE> symbol)
	{
		// TODO
	}
	void visit(cpp::nested_name_specifier* symbol)
	{
		NestedNameSpecifierWalker walker(getState());
		TREEWALKER_WALK_CACHED(walker, symbol);
		swapQualifying(walker.qualifying);
	}
	void visit(cpp::type_name* symbol)
	{
		TypeNameWalker walker(getState(), true);
		TREEWALKER_WALK(walker, symbol);
		if(walker.filter.nonType != 0)
		{
			return reportIdentifierMismatch(symbol, walker.filter.nonType->getName(), walker.filter.nonType, "type-name");
		}
		type.swap(walker.type);
		type.qualifying.swap(qualifying);
		setDependent(type.dependent, type.qualifying);
	}
};

struct DeclSpecifierSeqWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	Type type;
	unsigned fundamental;
	DeclSpecifiers specifiers;
	CvQualifiers qualifiers;
	IdentifierPtr forward;
	bool isUnion;
	bool isTemplateParameter;
	DeclSpecifierSeqWalker(const WalkerState& state, bool isTemplateParameter = false)
		: WalkerBase(state), type(0, context), fundamental(0), forward(0), isUnion(false), isTemplateParameter(isTemplateParameter)
	{
	}

	void visit(cpp::simple_type_specifier* symbol)
	{
		TypeSpecifierWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		type.swap(walker.type);
		if(type.declaration == 0)
		{
			fundamental = walker.fundamental;
			type = getFundamentalType(fundamental);
		}
	}
	void visit(cpp::simple_type_specifier_builtin* symbol)
	{
		TREEWALKER_LEAF(symbol);
		fundamental = combineFundamental(fundamental, symbol->id);
		type = getFundamentalType(fundamental);
	}
	void visit(cpp::elaborated_type_specifier* symbol)
	{
		ElaboratedTypeSpecifierWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		forward = walker.id;
		type.swap(walker.type);
	}
	void visit(cpp::typename_specifier* symbol)
	{
		TypenameSpecifierWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		type.swap(walker.type);
	}
	void visit(cpp::class_specifier* symbol)
	{
		ClassSpecifierWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		type = walker.declaration;
		templateParams = walker.templateParams;
		isUnion = walker.isUnion;
	}
	void visit(cpp::enum_specifier* symbol)
	{
		EnumSpecifierWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		type = walker.declaration;
	}
	void visit(cpp::decl_specifier_default* symbol)
	{
		TREEWALKER_LEAF(symbol);
		if(symbol->id == cpp::decl_specifier_default::TYPEDEF)
		{
			specifiers.isTypedef = true;
		}
		else if(symbol->id == cpp::decl_specifier_default::FRIEND)
		{
			specifiers.isFriend = true;
		}
	}
	void visit(cpp::storage_class_specifier* symbol)
	{
		TREEWALKER_LEAF(symbol);
		if(symbol->id == cpp::storage_class_specifier::STATIC)
		{
			specifiers.isStatic = true;
		}
		else if(symbol->id == cpp::storage_class_specifier::EXTERN)
		{
			specifiers.isExtern = true;
		}
	}
	void visit(cpp::cv_qualifier* symbol)
	{
		TREEWALKER_LEAF(symbol);
		if(symbol->id == cpp::cv_qualifier::CONST)
		{
			qualifiers.isConst = true;
		}
		else if(symbol->id == cpp::cv_qualifier::VOLATILE)
		{
			qualifiers.isVolatile = true;
		}
	}
};

struct TryBlockWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	TryBlockWalker(const WalkerState& state)
		: WalkerBase(state)
	{
	}

	void visit(cpp::compound_statement* symbol)
	{
		CompoundStatementWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::handler_seq* symbol)
	{
		HandlerSeqWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
};

struct LabeledStatementWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	LabeledStatementWalker(const WalkerState& state)
		: WalkerBase(state)
	{
	}
	void visit(cpp::identifier* symbol)
	{
		TREEWALKER_LEAF(symbol);
		// TODO: goto label
	}
	void visit(cpp::constant_expression* symbol)
	{
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::statement* symbol)
	{
		StatementWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
};

struct StatementWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	StatementWalker(const WalkerState& state)
		: WalkerBase(state)
	{
	}
	void visit(cpp::simple_declaration* symbol)
	{
		SimpleDeclarationWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		SEMANTIC_ASSERT(walker.type.declaration != 0);
	}
	void visit(cpp::try_block* symbol)
	{
		TryBlockWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::namespace_alias_definition* symbol)
	{
		NamespaceAliasDefinitionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::selection_statement* symbol)
	{
		ControlStatementWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::iteration_statement* symbol)
	{
		ControlStatementWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::compound_statement* symbol)
	{
		CompoundStatementWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::expression_statement* symbol)
	{
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::jump_statement_return* symbol)
	{
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::jump_statement_goto* symbol)
	{
		TREEWALKER_LEAF(symbol);
		// TODO
	}
	void visit(cpp::labeled_statement* symbol)
	{
		LabeledStatementWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::using_declaration* symbol)
	{
		UsingDeclarationWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::using_directive* symbol)
	{
		UsingDirectiveWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
};

struct ControlStatementWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	ControlStatementWalker(const WalkerState& state)
		: WalkerBase(state)
	{
	}
	void visit(cpp::terminal<boost::wave::T_LEFTPAREN> symbol)
	{
		pushScope(newScope(enclosing->getUniqueName(), SCOPETYPE_LOCAL));
	}
	void visit(cpp::condition_init* symbol)
	{
		SimpleDeclarationWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		SEMANTIC_ASSERT(walker.type.declaration != 0);
	}
	void visit(cpp::simple_declaration* symbol)
	{
		SimpleDeclarationWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::statement* symbol)
	{
		StatementWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::expression* symbol)
	{
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
};

struct CompoundStatementWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	CompoundStatementWalker(const WalkerState& state)
		: WalkerBase(state)
	{
	}

	void visit(cpp::terminal<boost::wave::T_LEFTBRACE> symbol)
	{
		pushScope(newScope(enclosing->getUniqueName(), SCOPETYPE_LOCAL));
	}
	void visit(cpp::statement* symbol)
	{
		StatementWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
};

struct HandlerWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	HandlerWalker(const WalkerState& state)
		: WalkerBase(state)
	{
	}
	void visit(cpp::terminal<boost::wave::T_CATCH> symbol)
	{
		pushScope(newScope(enclosing->getUniqueName(), SCOPETYPE_LOCAL));
	}
	void visit(cpp::exception_declaration_default* symbol)
	{
		SimpleDeclarationWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		walker.commit();
	}
	void visit(cpp::compound_statement* symbol)
	{
		CompoundStatementWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
};

struct HandlerSeqWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	HandlerSeqWalker(const WalkerState& state)
		: WalkerBase(state)
	{
	}
	void visit(cpp::handler* symbol)
	{
		HandlerWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
};

struct QualifiedTypeNameWalker : public WalkerQualified
{
	TREEWALKER_DEFAULT;

	QualifiedTypeNameWalker(const WalkerState& state)
		: WalkerQualified(state)
	{
	}
	void visit(cpp::terminal<boost::wave::T_COLON_COLON> symbol)
	{
		setQualifyingGlobal();
	}
	void visit(cpp::nested_name_specifier* symbol)
	{
		NestedNameSpecifierWalker walker(getState());
		TREEWALKER_WALK_CACHED(walker, symbol);
		swapQualifying(walker.qualifying);
	}
	void visit(cpp::class_name* symbol)
	{
		TypeNameWalker walker(getState(), true);
		TREEWALKER_WALK(walker, symbol);
		if(walker.filter.nonType != 0)
		{
			return reportIdentifierMismatch(symbol, walker.filter.nonType->getName(), walker.filter.nonType, "type-name");
		}
	}
};

struct MemInitializerWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	MemInitializerWalker(const WalkerState& state)
		: WalkerBase(state)
	{
	}
	void visit(cpp::mem_initializer_id_base* symbol)
	{
		QualifiedTypeNameWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::identifier* symbol)
	{
		TREEWALKER_LEAF(symbol);
		DeclarationInstanceRef declaration = findDeclaration(symbol->value);
		if(declaration == &gUndeclared
			|| !isObject(*declaration))
		{
			return reportIdentifierMismatch(symbol, symbol->value, declaration, "object-name");
		}
		setDecoration(&symbol->value, declaration);
	}
	void visit(cpp::expression_list* symbol)
	{
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
};

struct MemberDeclaratorBitfieldWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	IdentifierPtr id;
	MemberDeclaratorBitfieldWalker(const WalkerState& state)
		: WalkerBase(state), id(0)
	{
	}
	void visit(cpp::identifier* symbol)
	{
		TREEWALKER_LEAF_CACHED(symbol);
		id = &symbol->value;
	}
	void visit(cpp::constant_expression* symbol)
	{
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
};

struct TypeIdWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	TypeId type;
	Dependent valueDependent;
	TypeIdWalker(const WalkerState& state)
		: WalkerBase(state), type(0, context), valueDependent(context)
	{
	}
	void visit(cpp::terminal<boost::wave::T_OPERATOR> symbol) 
	{
		 // for debugging purposes
	}
	void visit(cpp::type_specifier_seq* symbol)
	{
		DeclSpecifierSeqWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		type.swap(walker.type);
		declareEts(type, walker.forward);
	}
	void visit(cpp::abstract_declarator* symbol)
	{
		DeclaratorWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		addDependent(valueDependent, walker.valueDependent);
		type.typeSequence = walker.typeSequence;
		setDependent(type.dependent, type.typeSequence);
	}
};

struct IsTemplateName
{
	WalkerState& context;
	IsTemplateName(WalkerState& context) : context(context)
	{
	}
	bool operator()(Identifier& id) const
	{
		DeclarationInstanceRef declaration = context.findDeclaration(id);
		return declaration != &gUndeclared && isTemplateName(*declaration);
	}
};

struct InitializerWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	InitializerWalker(const WalkerState& state) : WalkerBase(state)
	{
	}
	void visit(cpp::assignment_expression* symbol)
	{
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
};

struct SimpleDeclarationWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	DeclarationPtr declaration;
	ScopePtr parent;
	IdentifierPtr id;
	TypeId type;
	ScopePtr enclosed;
	DeclSpecifiers specifiers;
	IdentifierPtr forward;

	DeferredSymbols deferred;

	Dependent valueDependent;
	size_t templateParameter;
	bool isParameter;
	bool isUnion;

	SimpleDeclarationWalker(const WalkerState& state, bool isParameter = false, size_t templateParameter = INDEX_INVALID) : WalkerBase(state),
		declaration(0),
		parent(0),
		id(0),
		type(&gCtor, context),
		enclosed(0),
		forward(0),
		valueDependent(context),
		templateParameter(templateParameter),
		isParameter(isParameter),
		isUnion(false)
	{
	}

	// commit the declaration to the enclosing scope.
	// invoked when no further ambiguities remain.
	void commit()
	{
		if(id != 0)
		{
			if(enclosed == 0
				&& templateParamScope != 0)
			{
				templateParamScope->parent = parent;
				enclosed = templateParamScope; // for a static-member-variable definition, store template-params with different names than those in the class definition
			}
			declaration = declareObject(parent, id, type, enclosed, specifiers, templateParameter, valueDependent);

			enclosing = parent;

			if(enclosed != 0) // if the declaration has a parameter-declaration-clause
			{
				enclosed->name = declaration->getName();
				enclosing = enclosed; // subsequent declarations are contained by the parameter-scope - see 3.3.2-1: parameter scope
			}
			clearTemplateParams();

			id = 0;
		}
	}

	void visit(cpp::decl_specifier_seq* symbol)
	{
		DeclSpecifierSeqWalker walker(getState(), templateParameter != INDEX_INVALID);
		TREEWALKER_WALK(walker, symbol);
		type.swap(walker.type);
		type.qualifiers = walker.qualifiers;
		declaration = type.declaration; // if no declarator is specified later, this is probably a class-declaration
		specifiers = walker.specifiers;
		forward = walker.forward;
		templateParams = walker.templateParams;
		isUnion = walker.isUnion;
	}
	void visit(cpp::type_specifier_seq* symbol)
	{
		DeclSpecifierSeqWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		type.swap(walker.type);
		type.qualifiers = walker.qualifiers;
		declaration = type.declaration; // if no declarator is specified later, this is probably a class-declaration
		forward = walker.forward;
		templateParams = walker.templateParams;
		isUnion = walker.isUnion;
	}

	void visit(cpp::declarator* symbol)
	{
		DeclaratorWalker walker(getState());
		if(isParameter)
		{
			walker.declareEts(type, forward);
		}

		if(WalkerState::deferred != 0)
		{
			// while parsing simple-declaration-named declarator which contains deferred default-argument expression,
			// on finding '{', we rewind and try parsing function-definition.
			// In this situation, 'deferred' contains the reference to the deferred expression.
			walker.deferred = &deferred;
		}

		TREEWALKER_WALK_CACHED(walker, symbol);
		parent = walker.enclosing; // if the id-expression in the declarator is a qualified-id, this is the qualifying scope
		id = walker.id;
		enclosed = walker.paramScope;
		type.typeSequence = walker.typeSequence;
		setDependent(type.dependent, type.typeSequence);
		/* temp.dep.constexpr
		An identifier is value-dependent if it is:
			 a name declared with a dependent type,
			 the name of a non-type template parameter,
			 a constant with effective literal type and is initialized with an expression that is value-dependent.
		*/
		addDependent(valueDependent, walker.valueDependent);
	}
	void visit(cpp::abstract_declarator* symbol)
	{
		DeclaratorWalker walker(getState());
		if(isParameter)
		{
			walker.declareEts(type, forward);
		}
		TREEWALKER_WALK(walker, symbol);
		enclosed = walker.paramScope;
		type.typeSequence = walker.typeSequence;
		setDependent(type.dependent, type.typeSequence);
	}
	void visit(cpp::member_declarator_bitfield* symbol)
	{
		MemberDeclaratorBitfieldWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		if(walker.id != 0)
		{
			DeclarationInstanceRef instance = pointOfDeclaration(context, enclosing, *walker.id, type, 0, specifiers); // 3.3.1.1
#ifdef ALLOCATOR_DEBUG
			trackDeclaration(instance);
#endif
			setDecoration(walker.id, instance);
			declaration = instance;
		}
	}
	void visit(cpp::terminal<boost::wave::T_ASSIGN> symbol)
	{
		commit();
	}
	void visit(cpp::terminal<boost::wave::T_TRY> symbol)
	{
		commit();
	}
	void visit(cpp::terminal<boost::wave::T_LEFTBRACE> symbol)
	{
		commit();

		// symbols may be deferred during attempt to parse shared-prefix declarator: f(int i = j)
		// first parsed as member_declaration_named, backtracks on reaching '{'
		if(WalkerState::deferred != 0
			&& !deferred.empty())
		{
			WalkerState::deferred->splice(WalkerState::deferred->end(), deferred);
		}
	}
	void visit(cpp::terminal<boost::wave::T_COMMA> symbol)
	{
		commit();
	}
	void visit(cpp::terminal<boost::wave::T_SEMICOLON> symbol)
	{
		commit();

		// symbols may be deferred during attempt to parse shared-prefix declarator: f(int i = j)
		// first parsed as member_declaration_named, backtracks on reaching '{'
		if(WalkerState::deferred != 0
			&& !deferred.empty())
		{
			WalkerState::deferred->splice(WalkerState::deferred->end(), deferred);
		}
	}
	void visit(cpp::terminal<boost::wave::T_COLON> symbol)
	{
		commit();
	}

	void visit(cpp::default_argument* symbol)
	{
		// We cannot correctly skip a template-id in a default-argument if it refers to a template declared later in the class.
		// This is considered to be correct: http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#325
		if(WalkerState::deferred != 0
			&& templateParameter == INDEX_INVALID) // don't defer parse of default-argument for non-type template-parameter
		{
			result = defer(*WalkerState::deferred, *this, makeSkipDefaultArgument(IsTemplateName(*this)), symbol);
		}
		else
		{
			TREEWALKER_LEAF(symbol);
		}
	}
	// handle assignment-expression(s) in initializer
	void visit(cpp::assignment_expression* symbol)
	{
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	// handle initializer in separate context to avoid ',' confusing recognition of declaration
	void visit(cpp::initializer_clause* symbol)
	{
		InitializerWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	// handle initializer in separate context to avoid ',' confusing recognition of declaration
	void visit(cpp::initializer_parenthesis* symbol)
	{
		InitializerWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::constant_expression* symbol)
	{
		ExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		SEMANTIC_ASSERT(declaration != 0);
		addDependent(declaration->valueDependent, walker.valueDependent);
	}

	void visit(cpp::statement_seq_wrapper* symbol)
	{
		// NOTE: we must ensure that symbol-table modifications within the scope of this function are undone on parse fail
		pushScope(newScope(enclosing->getUniqueName(), SCOPETYPE_LOCAL));
		if(WalkerState::deferred != 0)
		{
			result = defer(*WalkerState::deferred, *this, skipBraced, symbol);
			if(result == 0)
			{
				return;
			}
		}
		else
		{
			TREEWALKER_LEAF(symbol);
		}
	}
	void visit(cpp::statement* symbol)
	{
		StatementWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::mem_initializer* symbol)
	{
		MemInitializerWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::handler_seq* symbol)
	{
		HandlerSeqWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::mem_initializer_clause* symbol)
	{
		if(WalkerState::deferred != 0)
		{
			result = defer(*WalkerState::deferred, *this, skipMemInitializerClause, symbol);
		}
		else // in case of an out-of-line constructor-definition
		{
			TREEWALKER_LEAF(symbol);
		}
	}

	struct DeclareEtsGuard
	{
		Type* p;
		TypeSequence* typeSequence;
		DeclareEtsGuard(SimpleDeclarationWalker& walker)
		{
			p = walker.declareEts(walker.type, walker.forward) ? &walker.type : 0;
			typeSequence = &walker.type.typeSequence;
		}
		~DeclareEtsGuard()
		{
			if(p != 0)
			{
				*p = &gClass;
			}
			if(typeSequence != 0)
			{
				typeSequence->clear(); // if declaration parse failed, don't keep a reference to the (deleted) type-sequence
			}
		}
		void hit()
		{
			p = 0;
			typeSequence = 0;
		}
	};

	void visit(cpp::simple_declaration_named* symbol)
	{
		DeclareEtsGuard guard(*this); // the point of declaration for the ETS in the decl-specifier-seq is just before the declarator
		TREEWALKER_LEAF(symbol);
		guard.hit();
	}
	void visit(cpp::member_declaration_named* symbol)
	{
		DeclareEtsGuard guard(*this);
		TREEWALKER_LEAF(symbol);
		guard.hit();
	}
	void visit(cpp::function_definition* symbol)
	{
		DeclareEtsGuard guard(*this);
		TREEWALKER_LEAF(symbol);
		guard.hit();
	}
	void visit(cpp::type_declaration_suffix* symbol)
	{
		TREEWALKER_LEAF(symbol);
		if(forward != 0)
		{
			bool isSpecialization = !isClassKey(*type.declaration);
			if(isSpecialization
				&& (specifiers.isFriend
					|| isExplicitInstantiation))
			{
				// friend class C<int>; // friend
				// template class C<int>; // explicit instantiation
			}
			else
			{
				if(isSpecialization)
				{
					SEMANTIC_ASSERT(enclosing == templateEnclosing);
				}
				// class C;
				// template<class T> class C;
				// template<> class C<int>;
				// template<class T> class C<T*>;
				// friend class C;
				DeclarationInstanceRef instance = pointOfDeclaration(context, enclosing, *forward, TYPE_CLASS, 0, DeclSpecifiers(), isSpecialization || enclosing == templateEnclosing, getTemplateParams(enclosing), isSpecialization, type.templateArguments);
#ifdef ALLOCATOR_DEBUG
				trackDeclaration(instance);
#endif
				setDecoration(forward, instance);
				declaration = instance;
			}
			type = TypeId(declaration, context); // TODO: is this necessary?
		}
		else if(declaration != 0
			&& declaration->isTemplate
			&& templateParams != 0) // if not an explicit-specialization
		{
			// template<class T> class C;
			SEMANTIC_ASSERT(!declaration->isSpecialization);
			mergeTemplateParamDefaults(*declaration, *templateParams);
		}

		if(isUnion
			&& isAnonymous(*declaration))
		{
			/* class.union-2
			The names of the members of an anonymous union
			shall be distinct from the names of any other entity in the scope in which the anonymous union is declared.
			For the purpose of name lookup, after the anonymous union definition, the members of the anonymous union
			are considered to have been defined in the scope in which the anonymous union is declared.
			*/
			// TODO: verify that member names are distinct
			for(Scope::Declarations::iterator i = declaration->enclosed->declarations.begin(); i != declaration->enclosed->declarations.end(); ++i)
			{
				Declaration& member = *(*i).second;
				if(isAnonymous(member))
				{
					member.setName(enclosing->getUniqueName());
					if(member.enclosed != 0)
					{
						member.enclosed->name = member.getName();
					}
				}
				else
				{
					const DeclarationInstance* holder = ::findDeclaration(enclosing->declarations, member.getName());
					if(holder != 0)
					{
						Declaration* declaration = *holder;
						printPosition(member.getName().position);
						std::cout << "'" << member.getName().value.c_str() << "': anonymous union member already declared" << std::endl;
						printPosition(declaration->getName().position);
						throw SemanticError();
					}
				}
				member.scope = enclosing;
				Identifier* id = &member.getName();
				enclosing->declarations.insert(DeclarationInstance(&member));
				enclosing->declarationList.push_back(&member);
			}
			declaration->enclosed = 0;
		}
	}
};

struct ParameterDeclarationWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	DeclarationPtr declaration;
	size_t templateParameter;
	cpp::default_argument* defaultArgument;
	bool isParameter;

	ParameterDeclarationWalker(const WalkerState& state, bool isParameter = false, size_t templateParameter = INDEX_INVALID)
		: WalkerBase(state), templateParameter(templateParameter), defaultArgument(0), isParameter(isParameter)
	{
	}
	void visit(cpp::parameter_declaration_default* symbol)
	{
		SimpleDeclarationWalker walker(getState(), isParameter, templateParameter);
		TREEWALKER_WALK(walker, symbol);
		walker.commit();
		declaration = walker.declaration;
		defaultArgument = symbol->init;
	}
	void visit(cpp::parameter_declaration_abstract* symbol)
	{
		SimpleDeclarationWalker walker(getState(), isParameter, templateParameter);
		TREEWALKER_WALK(walker, symbol);
		walker.parent = enclosing;
		walker.id = &gAnonymousId;
		walker.commit();
		declaration = walker.declaration;
		defaultArgument = symbol->init;
	}
};

struct TypeParameterWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	DeclarationPtr declaration;
	TypeId argument; // the default argument for this param
	TypeIds arguments; // the default arguments for this param's template-params (if template-template-param)
	size_t templateParameter;
	TypeParameterWalker(const WalkerState& state, size_t templateParameter)
		: WalkerBase(state), declaration(0), argument(0, context), arguments(context), templateParameter(templateParameter)
	{
	}
	void visit(cpp::identifier* symbol)
	{
		TREEWALKER_LEAF(symbol);
		DeclarationInstanceRef instance = pointOfDeclaration(context, enclosing, symbol->value, TYPE_PARAM, 0, DECLSPEC_TYPEDEF, !arguments.empty(), TEMPLATEPARAMETERS_NULL, false, TEMPLATEARGUMENTS_NULL, templateParameter);
#ifdef ALLOCATOR_DEBUG
		trackDeclaration(instance);
#endif
		setDecoration(&symbol->value, instance);
		declaration = instance;
		declaration->templateParams.defaults.swap(arguments);
	}
	void visit(cpp::type_id* symbol)
	{
		SEMANTIC_ASSERT(arguments.empty());
		TypeIdWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		argument.swap(walker.type);
	}
	void visit(cpp::template_parameter_clause* symbol)
	{
		TemplateParameterClauseWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		arguments.swap(walker.params.defaults);
	}
	void visit(cpp::id_expression* symbol)
	{
		IdExpressionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		DeclarationInstanceRef declaration = walker.declaration;
		if(declaration != 0)
		{
			if(declaration == &gUndeclared
				|| !isTemplateName(*declaration))
			{
				return reportIdentifierMismatch(symbol, *walker.id, declaration, "template-name");
			}
			setDecoration(walker.id, declaration);
		}
	}
};

struct TemplateParameterListWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	TemplateParameter param;
	TemplateParameters params;
	size_t count;
	TemplateParameterListWalker(const WalkerState& state, size_t count)
		: WalkerBase(state), param(context), params(context), count(count)
	{
	}
	void visit(cpp::type_parameter_default* symbol)
	{
		TypeParameterWalker walker(getState(), count);
		TREEWALKER_WALK(walker, symbol);
		param = walker.declaration == 0
			? &gUnknown // anonymous type param
			: walker.declaration;
		param.argument.swap(walker.argument);
		++count;
	}
	void visit(cpp::type_parameter_template* symbol)
	{
		TypeParameterWalker walker(getState(), count);
		TREEWALKER_WALK(walker, symbol);
		param = walker.declaration == 0
			? &gUnknown // anonymous type param
			: walker.declaration;
		param.argument.swap(walker.argument);
		++count;
	}
	void visit(cpp::parameter_declaration* symbol)
	{
		ParameterDeclarationWalker walker(getState(), false, count);
		TREEWALKER_WALK(walker, symbol);
		SEMANTIC_ASSERT(walker.declaration != 0);
		param = walker.declaration;
		// TODO: default value for non-type template-param
		if(walker.defaultArgument != 0)
		{
			param.argument = &gNonType;
		}
		++count;
	}
	void visit(cpp::template_parameter_list* symbol)
	{
		TemplateParameterListWalker walker(getState(), count);
		TREEWALKER_WALK(walker, symbol);
		params.swap(walker.params);
		params.push_front(walker.param);
	}
};

struct TemplateParameterClauseWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	TemplateParameters params;
	TemplateParameterClauseWalker(const WalkerState& state)
		: WalkerBase(state), params(context)
	{
		// collect template-params into a new scope
		Scope* scope = templateParamScope != 0 ? static_cast<Scope*>(templateParamScope) : newScope(makeIdentifier("$template"), SCOPETYPE_TEMPLATE);
		clearTemplateParams();
		pushScope(scope);
		enclosing->templateDepth = templateDepth;
	}
	void visit(cpp::template_parameter_list* symbol)
	{
		TemplateParameterListWalker walker(getState(), 0);
		TREEWALKER_WALK(walker, symbol);
		params.swap(walker.params);
		params.push_front(walker.param);
	}
};

struct TemplateDeclarationWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	DeclarationPtr declaration;
	TemplateParameters params;
	TemplateDeclarationWalker(const WalkerState& state)
		: WalkerBase(state), declaration(0), params(context)
	{
		templateEnclosing = enclosing;
		++templateDepth;
	}
	void visit(cpp::template_parameter_clause* symbol)
	{
		TemplateParameterClauseWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		templateParamScope = walker.enclosing;
		enclosing = walker.enclosing->parent;
		params.swap(walker.params);
		templateParams = &params;
	}
	void visit(cpp::declaration* symbol)
	{
		DeclarationWalker walker(getState());
		Source source = parser->get_source();
		TREEWALKER_WALK(walker, symbol);
		symbol->source = source;
		declaration = walker.declaration;
		SEMANTIC_ASSERT(declaration != 0);
	}
	void visit(cpp::member_declaration* symbol)
	{
		MemberDeclarationWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		declaration = walker.declaration;
		SEMANTIC_ASSERT(declaration != 0);
	}
};

struct ExplicitInstantiationWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	DeclarationPtr declaration;
	ExplicitInstantiationWalker(const WalkerState& state)
		: WalkerBase(state), declaration(0)
	{
	}
	void visit(cpp::terminal<boost::wave::T_TEMPLATE> symbol)
	{
		isExplicitInstantiation = true;
	}
	void visit(cpp::declaration* symbol)
	{
		DeclarationWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		declaration = walker.declaration;
	}
};

struct DeclarationWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	DeclarationPtr declaration;
	DeclarationWalker(const WalkerState& state)
		: WalkerBase(state), declaration(0)
	{
	}
	void visit(cpp::linkage_specification* symbol)
	{
		NamespaceWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		declaration = walker.declaration;
	}
	void visit(cpp::namespace_definition* symbol)
	{
		NamespaceWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		declaration = walker.declaration;
	}
	void visit(cpp::namespace_alias_definition* symbol)
	{
		NamespaceAliasDefinitionWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::general_declaration* symbol)
	{
		SimpleDeclarationWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		SEMANTIC_ASSERT(walker.type.declaration != 0);
		declaration = walker.declaration;
	}
	// occurs in for-init-statement
	void visit(cpp::simple_declaration* symbol)
	{
		SimpleDeclarationWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		SEMANTIC_ASSERT(walker.type.declaration != 0);
		declaration = walker.declaration;
	}
	void visit(cpp::constructor_definition* symbol)
	{
		SimpleDeclarationWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		SEMANTIC_ASSERT(walker.type.declaration != 0);
		declaration = walker.declaration;
	}
	void visit(cpp::template_declaration* symbol)
	{
		TemplateDeclarationWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		declaration = walker.declaration;
	}
	void visit(cpp::explicit_instantiation* symbol)
	{
		ExplicitInstantiationWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		declaration = walker.declaration;
	}
	void visit(cpp::explicit_specialization* symbol)
	{
		TemplateDeclarationWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
		declaration = walker.declaration;
	}
	void visit(cpp::using_declaration* symbol)
	{
		UsingDeclarationWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
	void visit(cpp::using_directive* symbol)
	{
		UsingDirectiveWalker walker(getState());
		TREEWALKER_WALK(walker, symbol);
	}
};

struct NamespaceWalker : public WalkerBase
{
	TREEWALKER_DEFAULT;

	DeclarationPtr declaration;
	IdentifierPtr id;
	NamespaceWalker(WalkerContext& context)
		: WalkerBase(context), declaration(0), id(0)
	{
		pushScope(&context.global);
	}

	NamespaceWalker(const WalkerState& state)
		: WalkerBase(state), declaration(0), id(0)
	{
	}

	void visit(cpp::identifier* symbol)
	{
		TREEWALKER_LEAF(symbol);
		id = &symbol->value;
	}
	void visit(cpp::terminal<boost::wave::T_LEFTBRACE> symbol)
	{
		if(id != 0)
		{
			DeclarationInstanceRef instance = pointOfDeclaration(context, enclosing, *id, TYPE_NAMESPACE, 0);
#ifdef ALLOCATOR_DEBUG
			trackDeclaration(instance);
#endif
			setDecoration(id, instance);
			declaration = instance;
			if(declaration->enclosed == 0)
			{
				declaration->enclosed = newScope(*id, SCOPETYPE_NAMESPACE);
			}
			pushScope(declaration->enclosed);
		}
	}
	void visit(cpp::declaration* symbol)
	{
		DeclarationWalker walker(getState());
		IncludeEvents events = parser->get_events();
		Source source = parser->get_source();
		TREEWALKER_WALK(walker, symbol);
		symbol->events = events;
		symbol->source = source;
	}
};

};

#if 0
inline cpp::simple_template_id* parseSymbol(ParserGeneric<Walker::TemplateIdWalker>& parser, cpp::simple_template_id* result)
{
	return parseSymbol(parser, result, False());
}
#endif


TreeAllocator<int> getAllocator(ParserContext& context)
{
#ifdef TREEALLOCATOR_LINEAR
	return context.allocator;
#else
	return DebugAllocator<int>();
#endif
}


cpp::declaration_seq* parseFile(ParserContext& context)
{
	gUniqueNames.clear();
	gUniqueTypes.clear();

	WalkerContext& globals = *new WalkerContext(getAllocator(context));
	Walker::NamespaceWalker& walker = *new Walker::NamespaceWalker(globals);
	ParserGeneric<Walker::NamespaceWalker> parser(context, walker);

	cpp::symbol_sequence<cpp::declaration_seq> result(NULL);
	try
	{
		ProfileScope profile(gProfileParser);
		PARSE_SEQUENCE(parser, result);
	}
	catch(ParseError&)
	{
	}
	catch(SemanticError&)
	{
	}
	if(!context.finished())
	{
		printError(parser);
	}
	dumpProfile(gProfileIo);
	dumpProfile(gProfileWave);
	dumpProfile(gProfileParser);
	dumpProfile(gProfileLookup);
	dumpProfile(gProfileDiagnose);
	dumpProfile(gProfileAllocator);
	dumpProfile(gProfileIdentifier);
	dumpProfile(gProfileTemplateId);

	return result;
}

cpp::statement_seq* parseFunction(ParserContext& context)
{
	gUniqueNames.clear();
	gUniqueTypes.clear();

	WalkerContext& globals = *new WalkerContext(getAllocator(context));
	Walker::CompoundStatementWalker& walker = *new Walker::CompoundStatementWalker(globals);
	ParserGeneric<Walker::CompoundStatementWalker> parser(context, walker);

	cpp::symbol_sequence<cpp::statement_seq> result(NULL);
	try
	{
		ProfileScope profile(gProfileParser);
		PARSE_SEQUENCE(parser, result);
	}
	catch(ParseError&)
	{
	}
	if(!context.finished())
	{
		printError(parser);
	}
	return result;
}


