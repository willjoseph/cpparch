
#ifndef INCLUDED_CPPPARSE_SEMA_SEMAENUM_H
#define INCLUDED_CPPPARSE_SEMA_SEMAENUM_H

#include "SemaCommon.h"


struct SemaEnumeratorDefinition : public SemaBase
{
	SEMA_BOILERPLATE;

	DeclarationPtr declaration; // result
	SemaEnumeratorDefinition(const SemaState& state)
		: SemaBase(state), declaration(0)
	{
	}

	SEMA_POLICY(cpp::identifier, SemaPolicyIdentity)
	void action(cpp::identifier* symbol)
	{
		/* 3.1-4
		The point of declaration for an enumerator is immediately after its enumerator-definition.
		*/
		// TODO: give enumerators a type
		DeclarationInstanceRef instance = pointOfDeclaration(context, enclosingScope, symbol->value, TYPE_ENUMERATOR, 0, false, DeclSpecifiers());
#ifdef ALLOCATOR_DEBUG
		trackDeclaration(instance);
#endif
		setDecoration(&symbol->value, instance);
		declaration = instance;
		declaration->isEnumerator = true;
	}
	SEMA_POLICY(cpp::constant_expression, SemaPolicyPush<struct SemaExpression>)
	void action(cpp::constant_expression* symbol, const SemaExpressionResult& walker)
	{
		SEMANTIC_ASSERT(walker.expression.isValueDependent || walker.expression.value.isConstant); // TODO: non-fatal error: expected constant expression
		declaration->initializer = walker.expression;
	}
};

struct SemaEnumSpecifier : public SemaBase, SemaEnumSpecifierResult
{
	SEMA_BOILERPLATE;

	IdentifierPtr id; // internal state
	ExpressionWrapper value;
	SemaEnumSpecifier(const SemaState& state)
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
		// defer declaration until '{' resolves ambiguity between enum-specifier and elaborated-type-specifier
		if(id != 0)
		{
			DeclarationInstanceRef instance = pointOfDeclaration(context, enclosingScope, *id, TYPE_ENUM, 0, true);
			setDecoration(id, instance);
			declaration = instance;
		}
		if(declaration == 0)
		{
			// unnamed enum
			DeclarationInstanceRef instance = pointOfDeclaration(context, enclosingScope, enclosingScope->getUniqueName(), TYPE_ENUM, 0, true);
#ifdef ALLOCATOR_DEBUG
			trackDeclaration(instance);
#endif
			declaration = instance;
		}
	}

	SEMA_POLICY(cpp::enumerator_definition, SemaPolicyPush<struct SemaEnumeratorDefinition>)
	void action(cpp::enumerator_definition* symbol, const SemaEnumeratorDefinition& walker)
	{
		Declaration& enumerator = *walker.declaration;
		enumerator.type = declaration; // give the enumerator the type of its enumeration
		enumerator.type.qualifiers = CvQualifiers(true, false); // an enumerator may be used in an integral constant expression
		makeUniqueTypeSafe(enumerator.type);
		enumerator.isTypeDependent = enumerator.type.isDependent;
		enumerator.typeDependent = isDependent2(getUniqueType(enumerator.type));
		if(enumerator.initializer.p != 0)
		{
			SEMANTIC_ASSERT(enumerator.initializer.isValueDependent || enumerator.initializer.value.isConstant);
			value = enumerator.initializer;
			addDeferredPersistentExpression(enumerator.initializer);
		}
		else
		{
			if(value.p == 0)
			{
				// [dcl.enum] If the first enumerator has no initializer, the value of the corresponding constant is zero.
				value = makeConstantExpressionZero(); // TODO: [dcl.enum] underlying type of enumerator
			}
			else
			{
				// [dcl.enum] An enumerator-definition without an initializer gives the enumerator the value obtained by increasing the value of the previous enumerator by one.
				ExpressionWrapper one = makeConstantExpressionOne();
				value = makeExpression(
					BinaryExpression(gOperatorPlusId, operator+, typeOfBinaryExpression<binaryOperatorAdditiveType>, value, one) // TODO: type of enumerator
				);
			}
			enumerator.initializer = value;
		}

		addDeferredDeclarationType(enumerator);
	}
};

#endif
