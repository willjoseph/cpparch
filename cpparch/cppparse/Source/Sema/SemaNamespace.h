
#ifndef INCLUDED_CPPPARSE_SEMA_SEMANAMESPACE_H
#define INCLUDED_CPPPARSE_SEMA_SEMANAMESPACE_H

#include "SemaCommon.h"
#include "SemaIdExpression.h"
#include "SemaNamespaceName.h"

struct SemaUsingDeclaration : public SemaQualified, SemaDeclarationResult
{
	SEMA_BOILERPLATE;

	bool isTypename;
	SemaUsingDeclaration(const SemaState& state)
		: SemaQualified(state), isTypename(false)
	{
	}
	void action(cpp::terminal<boost::wave::T_COLON_COLON> symbol)
	{
		setQualifyingGlobal();
	}
	SEMA_POLICY(cpp::nested_name_specifier, SemaPolicyPushCached<struct SemaNestedNameSpecifier>)
	void action(cpp::nested_name_specifier* symbol, const SemaQualifyingResult& walker)
	{
		swapQualifying(walker.qualifying);
	}
	SEMA_POLICY(cpp::unqualified_id, SemaPolicyPushChecked<struct SemaUnqualifiedId>)
	bool action(cpp::unqualified_id* symbol, const SemaUnqualifiedId& walker)
	{
		SYMBOLS_ASSERT(qualifying_p != TypePtr(0)); // TODO: non-fatal error: expected qualified-id
		UniqueTypeWrapper qualifyingType = isNamespace(*qualifying_p->declaration) // if the nested name specifier names a namespace
			? gUniqueTypeNull
			: getUniqueType(*qualifying_p);

		LookupResultRef existingDeclaration = isTypename ? gDependentTypeInstance : gDependentObjectInstance;

		// [namespace.udecl]
		// If a using-declaration uses the keyword typename and specifies a dependent name, the name introduced
		// by the using-declaration is treated as a typedef-name
		bool isType = isTypename;
		bool isTemplate = false; // dependent names cannot be templates

		if(!isTypename
			&& !isDependentSafe(qualifying_p))
		{
			existingDeclaration = walker.declaration;
			if(existingDeclaration == &gUndeclared
				|| !(isObject(*existingDeclaration) || isTypeName(*existingDeclaration)))
			{
				return reportIdentifierMismatch(symbol, *walker.id, existingDeclaration, "object-name or type-name");
			}

			isType = isTypeName(*existingDeclaration);
			isTemplate = isTemplateName(*existingDeclaration);
		}
		else
		{
			SYMBOLS_ASSERT(qualifyingType != gUniqueTypeNull); // TODO: non-fatal error: cannot use 'typename' with a member of a namespace
		}

		declaration = declareUsing(enclosingScope, walker.id, qualifyingType, existingDeclaration, isType, isTemplate);
		if(isType
			&& !isTemplate)
		{
			declaration->specifiers.isTypedef = true;
			Type& type = declaration->type;
			type.id = walker.id;
			type.declaration = existingDeclaration;
			type.qualifying.swap(qualifying);
			setDependent(type.dependent, type.qualifying);
			makeUniqueTypeSafe(type);
			declaration->isTypeDependent = declaration->type.isDependent;
		}
		else if(qualifying_p != TypePtr(0))
		{
			addDependent(declaration->type.dependent, qualifying_p->dependent);
			declaration->type.isDependent = isDependentSafe(qualifying_p);
			declaration->isTypeDependent = isDependentSafe(qualifying_p);
		}

		return true;
	}
	void action(cpp::terminal<boost::wave::T_TYPENAME> symbol)
	{
		isTypename = true;
	}
};

struct SemaUsingDirective : public SemaQualified
{
	SEMA_BOILERPLATE;

	SemaUsingDirective(const SemaState& state)
		: SemaQualified(state)
	{
	}
	void action(cpp::terminal<boost::wave::T_COLON_COLON> symbol)
	{
		setQualifyingGlobal();
	}
	SEMA_POLICY(cpp::nested_name_specifier, SemaPolicyPushCached<struct SemaNestedNameSpecifier>)
	void action(cpp::nested_name_specifier* symbol, const SemaQualifyingResult& walker)
	{
		swapQualifying(walker.qualifying);
	}
	// [basic.lookup.udir]
	// When looking up a namespace-name in a using-directive or namespace-alias-definition, only namespace
	// names are considered.
	SEMA_POLICY(cpp::namespace_name, SemaPolicyPush<struct SemaNamespaceName>)
	void action(cpp::namespace_name* symbol, const SemaNamespaceName& walker)
	{
		if(!findScope(enclosingScope, walker.declaration->enclosed))
		{
			enclosingScope->usingDirectives.push_back(walker.declaration->enclosed);
		}
	}
};

struct SemaNamespaceAliasDefinition : public SemaQualified
{
	SEMA_BOILERPLATE;

	IdentifierPtr id;
	SemaNamespaceAliasDefinition(const SemaState& state)
		: SemaQualified(state), id(0)
	{
	}
	void action(cpp::terminal<boost::wave::T_COLON_COLON> symbol)
	{
		setQualifyingGlobal();
	}
	SEMA_POLICY(cpp::nested_name_specifier, SemaPolicyPushCached<struct SemaNestedNameSpecifier>)
	void action(cpp::nested_name_specifier* symbol, const SemaQualifyingResult& walker)
	{
		swapQualifying(walker.qualifying);
	}
	SEMA_POLICY(cpp::identifier, SemaPolicyIdentityChecked)
	bool action(cpp::identifier* symbol)
	{
		if(id == 0) // first identifier
		{
			id = &symbol->value;
		}
		else // second identifier
		{
			LookupResultRef declaration = findDeclaration(symbol->value, IsNamespaceName());
			if(declaration == &gUndeclared)
			{
				return reportIdentifierMismatch(symbol, symbol->value, declaration, "namespace-name");
			}

			// TODO: check for conflicts with earlier declarations
			declaration = pointOfDeclaration(context, enclosingScope, *id, TYPE_NAMESPACE, declaration->enclosed, false);
#ifdef ALLOCATOR_DEBUG
			trackDeclaration(declaration);
#endif
			setDecoration(id, declaration);
		}
		return true;
	}
};

struct SemaNamespace : public SemaBase, SemaNamespaceResult
{
	SEMA_BOILERPLATE;

	IdentifierPtr id;
	SemaNamespace(SemaContext& context)
		: SemaBase(context), id(0)
	{
		pushScope(&context.global);
	}

	SemaNamespace(const SemaState& state)
		: SemaBase(state), id(0)
	{
	}

	SEMA_POLICY(cpp::identifier, SemaPolicyIdentity)
	void action(cpp::identifier* symbol)
	{
		id = &symbol->value;
	}
	void action(cpp::terminal<boost::wave::T_LEFTBRACE> symbol)
	{
		if(id != 0)
		{
			DeclarationInstanceRef instance = pointOfDeclaration(context, enclosingScope, *id, TYPE_NAMESPACE, 0, false);
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
	SEMA_POLICY(cpp::declaration, SemaPolicyPushTop<struct SemaDeclaration>)
	void action(cpp::declaration* symbol, const SemaDeclarationResult& walker)
	{
	}
};

#endif
