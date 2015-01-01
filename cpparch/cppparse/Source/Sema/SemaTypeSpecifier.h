
#ifndef INCLUDED_CPPPARSE_SEMA_SEMATYPESPECIFIER_H
#define INCLUDED_CPPPARSE_SEMA_SEMATYPESPECIFIER_H

#include "SemaCommon.h"
#include "SemaTypeName.h"

struct SemaTypeSpecifier : public SemaQualified, SemaTypeSpecifierResult
{
	SEMA_BOILERPLATE;

	SemaTypeSpecifier(const SemaState& state)
		: SemaQualified(state), SemaTypeSpecifierResult(context)
	{
	}
	SEMA_POLICY(cpp::simple_type_specifier_name, SemaPolicyPush<struct SemaTypeSpecifier>)
	void action(cpp::simple_type_specifier_name* symbol, const SemaTypeSpecifierResult& walker)
	{
		type = walker.type;
		fundamental = walker.fundamental;
	}
	SEMA_POLICY(cpp::simple_type_specifier_template, SemaPolicyPush<struct SemaTypeSpecifier>)
	void action(cpp::simple_type_specifier_template* symbol, const SemaTypeSpecifierResult& walker) // X::template Y<Z>
	{
		type = walker.type;
		fundamental = walker.fundamental;
	}
	SEMA_POLICY(cpp::type_name, SemaPolicyPushChecked<struct SemaTypeName>)
	bool action(cpp::type_name* symbol, const SemaTypeName& walker) // simple_type_specifier_name
	{
		if(walker.filter.nonType != 0)
		{
			// 3.3.7: a type-name can be hidden by a non-type name in the same scope (this rule applies to a type-specifier)
			return reportIdentifierMismatch(symbol, walker.filter.nonType->getName(), walker.filter.nonType, "type-name");
		}
		SEMANTIC_ASSERT(walker.type.declaration != 0);
		type = walker.type;
		type.qualifying.swap(qualifying);
		return true;
	}
	void action(cpp::terminal<boost::wave::T_COLON_COLON> symbol)
	{
		setQualifyingGlobal();
	}
	SEMA_POLICY(cpp::nested_name_specifier, SemaPolicyPushCached<struct SemaNestedNameSpecifier>)
	void action(cpp::nested_name_specifier* symbol, const SemaQualifyingResult& walker) // simple_type_specifier_name | simple_type_specifier_template
	{
		swapQualifying(walker.qualifying);
	}
	SEMA_POLICY(cpp::simple_template_id, SemaPolicyPushCachedChecked<struct SemaTemplateId>)
	bool action(cpp::simple_template_id* symbol, const SemaTemplateIdResult& walker) // simple_type_specifier_template
	{
		// [temp]
		// A class template shall not have the same name as any other template, class, function, variable, enumeration,
		// enumerator, namespace, or type in the same scope
		LookupResultRef declaration = lookupTemplate(*walker.id, IsAny());
		if(declaration == &gUndeclared
			|| !isTypeName(*declaration)
			|| !isTemplateName(*declaration))
		{
			return reportIdentifierMismatch(symbol, *walker.id, declaration, "class-template-name");
		}
		if(declaration == &gDependentTemplate)
		{
			// dependent type, are you missing a 'typename' keyword?
			return reportIdentifierMismatch(symbol, *walker.id, &gUndeclared, "typename");
		}
		setDecoration(walker.id, declaration);
		type.declaration = declaration;
		type.templateArguments = walker.arguments;
		type.qualifying.swap(qualifying);
		return true;
	}
	SEMA_POLICY(cpp::simple_type_specifier_builtin, SemaPolicyIdentity)
	void action(cpp::simple_type_specifier_builtin* symbol)
	{
		fundamental = combineFundamental(0, symbol->id);
	}
	SEMA_POLICY(cpp::decltype_specifier, SemaPolicyPush<struct SemaDecltypeSpecifier>)
	void action(cpp::decltype_specifier* symbol, const SemaDecltypeSpecifierResult& walker)
	{
		type = walker.type;
	}
};

struct SemaDecltypeSpecifier : public SemaBase, SemaDecltypeSpecifierResult
{
	SEMA_BOILERPLATE;

	SemaDecltypeSpecifier(const SemaState& state)
		: SemaBase(state), SemaDecltypeSpecifierResult(context)
	{
	}
	SEMA_POLICY(cpp::expression, SemaPolicyPush<struct SemaExpression>)
	void action(cpp::expression* symbol, const SemaExpressionResult& walker)
	{
		type.declaration = &gUnknown;
		type.expression = walker.expression;
		addDeferredExpression(type.expression);
	}
};



inline bool isUnqualified(cpp::elaborated_type_specifier_default* symbol)
{
	return symbol != 0
		&& symbol->isGlobal.value.empty()
		&& symbol->context.get() == 0;
}


struct SemaElaboratedTypeSpecifier : public SemaQualified, SemaElaboratedTypeSpecifierResult
{
	SEMA_BOILERPLATE;

	DeclarationPtr key;
	SemaElaboratedTypeSpecifier(const SemaState& state)
		: SemaQualified(state), SemaElaboratedTypeSpecifierResult(context), key(0)
	{
	}
	void action(cpp::terminal<boost::wave::T_COLON_COLON> symbol)
	{
		setQualifyingGlobal();
	}
	SEMA_POLICY(cpp::elaborated_type_specifier_default, SemaPolicyPush<struct SemaElaboratedTypeSpecifier>)
	void action(cpp::elaborated_type_specifier_default* symbol, const SemaElaboratedTypeSpecifierResult& walker)
	{
		type = walker.type;
		id = walker.id;

		if(!isUnqualified(symbol))
		{
			id = 0;
		}
	}
	SEMA_POLICY(cpp::elaborated_type_specifier_template, SemaPolicyPush<struct SemaElaboratedTypeSpecifier>)
	void action(cpp::elaborated_type_specifier_template* symbol, const SemaElaboratedTypeSpecifierResult& walker)
	{
		type = walker.type;
		id = walker.id;
	}
	SEMA_POLICY(cpp::nested_name_specifier, SemaPolicyPushCached<struct SemaNestedNameSpecifier>)
	void action(cpp::nested_name_specifier* symbol, const SemaQualifyingResult& walker) // elaborated_type_specifier_default | elaborated_type_specifier_template
	{
		swapQualifying(walker.qualifying);
	}
	SEMA_POLICY(cpp::class_key, SemaPolicyIdentityCached)
	void action(cpp::class_key* symbol)
	{
		key = &gClass;
	}
	SEMA_POLICY(cpp::enum_key, SemaPolicyIdentity)
	void action(cpp::enum_key* symbol)
	{
		key = &gEnum;
	}
	SEMA_POLICY(cpp::simple_template_id, SemaPolicyPushCachedChecked<struct SemaTemplateId>)
	bool action(cpp::simple_template_id* symbol, const SemaTemplateIdResult& walker) // elaborated_type_specifier_default | elaborated_type_specifier_template
	{
		SEMANTIC_ASSERT(key == &gClass);
		// 3.4.4-2: when looking up 'identifier' in elaborated-type-specifier, ignore any non-type names that have been declared. 
		LookupResultRef declaration = lookupTemplate(*walker.id, IsTypeName());
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
		type.templateArguments = walker.arguments;
		type.qualifying.swap(qualifying);
		return true;
	}
	SEMA_POLICY(cpp::identifier, SemaPolicyIdentityCached)
	void action(cpp::identifier* symbol)
	{
		id = &symbol->value;
		type = key;
	}
};


#endif
