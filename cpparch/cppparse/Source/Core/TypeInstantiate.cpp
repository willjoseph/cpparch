
#include "TypeInstantiate.h"
#include "Common/Util.h"
#include "TemplateDeduce.h"
#include "TypeUnique.h"


inline bool deduceAndSubstitute(const UniqueTypeArray& parameters, const UniqueTypeArray& arguments, const InstantiationContext& context, SimpleType& enclosing, TemplateArgumentsInstance& substituted)
{
	// deduce the partial-specialization's template arguments from the original argument list
	TemplateArgumentsInstance& deduced = enclosing.deducedArguments;
	if(!deducePairs(parameters, arguments, deduced)
		|| !isValid(deduced))
	{
		return false; // cannot deduce
	}
	try
	{
		// substitute the template-parameters in the partial-specialization's signature with the deduced template-arguments
		substitute(substituted, parameters, setEnclosingTypeSafe(context, &enclosing));
	}
	catch(TypeError&)
	{
		SYMBOLS_ASSERT(isValid(substituted));
		return false; // cannot substitute: SFINAE
	}

	return true;
}


inline bool matchTemplatePartialSpecialization(Declaration* declaration, TemplateArgumentsInstance& deducedArguments, const TemplateArgumentsInstance& specializationArguments, const TemplateArgumentsInstance& arguments, const InstantiationContext& context)
{
	SYMBOLS_ASSERT(!declaration->templateParams.empty());
	TemplateArgumentsInstance deduced(std::distance(declaration->templateParams.begin(), declaration->templateParams.end()), gUniqueTypeNull);
	TemplateArgumentsInstance substituted;
	SimpleType enclosing(declaration, 0);
	enclosing.deducedArguments.swap(deduced);
	enclosing.instantiated = true;
	if(!deduceAndSubstitute(specializationArguments, arguments, context, enclosing, substituted))
	{
		return false; // partial-specialization only matches if template-argument-deduction succeeds
	}
	// TODO: same as comparing deduced arguments with original template parameters?
	// TODO: not necessary unless testing partial ordering?
	if(std::equal(substituted.begin(), substituted.end(), arguments.begin()))
	{
		deducedArguments.swap(enclosing.deducedArguments);
		return true;
	}
	return false;
}

inline bool matchTemplatePartialSpecialization(Declaration* declaration, const TemplateArgumentsInstance& specializationArguments, const TemplateArgumentsInstance& arguments, const InstantiationContext& context)
{
	TemplateArgumentsInstance deducedArguments;
	return matchTemplatePartialSpecialization(declaration, deducedArguments, specializationArguments, arguments, context);
}

inline Declaration* findTemplateSpecialization(Declaration* declaration, TemplateArgumentsInstance& deducedArguments, const TemplateArgumentsInstance& arguments, const InstantiationContext& context, bool allowDependent)
{
	Declaration* best = 0;
	TemplateArgumentsInstance bestArguments;
	for(; declaration != 0; declaration = declaration->overloaded)
	{
		if(!isSpecialization(*declaration))
		{
			continue;
		}

		// TODO: check that all non-defaulted arguments are specified!
		TemplateArgumentsInstance specializationArguments;
		// a partial-specialization may have dependent template-arguments: template<class T> class C<T*>
		makeUniqueTemplateArguments(declaration->templateArguments, specializationArguments, context, true);

		SYMBOLS_ASSERT(specializationArguments.size() <= arguments.size()); // a template-specialization must have no more arguments than the template parameters

		if(declaration->templateParams.empty()) // if this is an explicit specialization
		{
			if(std::equal(specializationArguments.begin(), specializationArguments.end(), arguments.begin()))
			{
				deducedArguments.clear();
				return declaration; // exact match
			}
			continue;
		}

		TemplateArgumentsInstance deduced;
		if(matchTemplatePartialSpecialization(declaration, deduced, specializationArguments, arguments, context)) // if this partial-specialization can be deduced for the specified types
		{
			// consider two specializations: A<T**> and A<T*>
			// when deducing with int**, we deduce against A<T*> and find a match, recording it as 'best'
			// we then try to deduce against A<T**> and also find a match.
			// to break the tie, we attempt to deduce the current P** against the best <T*>: success (T = P*)
			// T** is at least as specialized as T*, because deduction succeeds for P** against T*
			// then we attempt to deduce the best P* against the current <T**>: fail
			// T* is not at least as specialized as T**, because deduction fails for P* against T**
			// therefore T** is more specialized than T*
			// we replace the best <T*> with the current <T**>.
			if(best != 0)
			{
				bool atLeastAsSpecializedCurrent = matchTemplatePartialSpecialization(best, bestArguments, specializationArguments, context); // deduce current against best
				bool atLeastAsSpecializedBest = matchTemplatePartialSpecialization(declaration, specializationArguments, bestArguments, context);

				if(atLeastAsSpecializedCurrent
					&& atLeastAsSpecializedBest)
				{
					// TODO: this may occur if the specializations differ only in non-type arguments
					//SYMBOLS_ASSERT(isNonType(arguments));
					struct AmbiguousSpecialization : TypeError
					{
						void report()
						{
							std::cout << "ambiguous specialization!";
							std::cout << std::endl;
						}
					};
					throw AmbiguousSpecialization();
				}

				if(atLeastAsSpecializedBest)
				{
					continue; // the best specialization is at least as specialized as the current
				}
			}
			best = declaration;
			bestArguments.swap(specializationArguments);
			deducedArguments.swap(deduced);
		}
	}
	return best;
}


#include <fstream>
#include "Common/Util.h"


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

inline void printPosition(const Source& source, FileOutputStream& out)
{
	out << source.absolute.c_str() << "(" << source.line << ", " << source.column << "): ";
}

void printTypeReadable(const SimpleType& type, FileOutputStream& out, bool escape = true)
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
			out << '\n' << '\t';
			printType(*i, out, escape);
			separator = true;
		}
		if(!type.templateArguments.empty())
		{
			out << '\n';
		}
		out << (escape ? "&gt;" : ">");
	}
}

inline void dumpTemplateInstantiations(const SimpleType& instance, bool root = false)
{
	if(instance.dumped)
	{
		return;
	}
	instance.dumped = true;
	SYMBOLS_ASSERT(!instance.visited);
	instance.visited = true;
	FileOutputStream out(Concatenate(makeRange(root ? "debug/!" : "debug/"), makeRange(InstantiationPath(instance).c_str())).c_str());
	SYMBOLS_ASSERT(out.is_open());

	out << "<html>\n"
		"<head>\n"
		"</head>\n"
		"<body>\n"
		"<pre style='color:#000000;background:#ffffff;'>\n";
	printPosition(instance.declaration->getName().source, out);
	out << '\n';
	printTypeReadable(instance, out, true);
	out << '\n' << '\n';

	typedef std::map<const SimpleType*, Location> InstanceMap;
	InstanceMap instanceMap;
	for(ChildInstantiations::const_iterator i = instance.childInstantiations.begin(); i != instance.childInstantiations.end(); ++i)
	{
		instanceMap.insert(InstanceMap::value_type((*i).instance, (*i).source));
	}
	for(InstanceMap::const_iterator i = instanceMap.begin(); i != instanceMap.end(); ++i)
	{
		printPosition((*i).second, out);
		out << '\n';
		out << "<a href='" << InstantiationPath(*(*i).first).c_str() << "'>";
		printTypeReadable(*(*i).first, out, true);
		out << "</a>";
		out << '\n';
		dumpTemplateInstantiations(*(*i).first);
	}
	out << "</pre>\n"
		"</body>\n"
		"</html>\n";
	instance.visited = false;
}

inline TypeLayout addBase(SimpleType& instance, UniqueTypeWrapper base, const InstantiationContext& context)
{
	SYMBOLS_ASSERT(!isDependent(base));
	SYMBOLS_ASSERT(base.isSimple());
	const SimpleType& objectType = getSimpleType(base.value);
	TypeLayout layout = instantiateClass(objectType, setEnclosingTypeSafe(context, &instance));
	SYMBOLS_ASSERT(isClass(*objectType.declaration));
	SYMBOLS_ASSERT(objectType.declaration->enclosed != 0); // this can occur when the primary template is incomplete, and a specialization was not chosen
	instance.bases.push_back(&objectType);
	instance.hasVirtualDestructor |= objectType.hasVirtualDestructor;
	instance.isPolymorphic |= objectType.isPolymorphic;
	instance.isAbstract |= objectType.isAbstract;
	instance.isEmpty &= objectType.isEmpty; // TODO: virtual base
	instance.isPod = false;
	return TypeLayout(evaluateSizeof(layout), layout.align);
}

inline bool isTemplate(const SimpleType& instance)
{
	if(instance.declaration->isTemplate)
	{
		return true;
	}
	return instance.enclosing != 0
		&& isTemplate(*instance.enclosing);
}

// If the class definition does not explicitly declare a copy assignment operator, one is declared implicitly.
// The implicitly-declared copy assignment operator for a class X will have the form
//   X& X::operator=(const X&)
// TODO: correct constness of parameter
inline bool hasCopyAssignmentOperator(const SimpleType& classType, const InstantiationContext& context)
{
	Identifier id;
	id.value = gOperatorAssignId;
	const DeclarationInstance* result = ::findDeclaration(classType.declaration->enclosed->declarations, id);
	if(result == 0)
	{
		return false;
	}
	InstantiationContext memberContext = setEnclosingTypeSafe(context, &classType);
	for(const Declaration* p = findOverloaded(*result); p != 0; p = p->overloaded)
	{
		if(p->isTemplate)
		{
			continue; // TODO: check compliance: copy-assignment-operator cannot be a template?
		}

		UniqueTypeWrapper type = getUniqueType(p->type, memberContext);
		SYMBOLS_ASSERT(type.isFunction());
		const ParameterTypes& parameters = getParameterTypes(type.value);
		SYMBOLS_ASSERT(parameters.size() == 1);
		UniqueTypeWrapper parameterType = removeReference(parameters[0]);
		if(parameterType.isSimple()
			&& &getSimpleType(parameterType.value) == &classType)
		{
			return true;
		}
	}
	return false;
}


TypeLayout instantiateClass(const SimpleType& instanceConst, const InstantiationContext& context, bool allowDependent)
{
	SimpleType& instance = const_cast<SimpleType&>(instanceConst);
	SYMBOLS_ASSERT(isClass(*instance.declaration));

	if(context.enclosingType != 0)
	{
		ChildInstantiations& instantiations = const_cast<SimpleType*>(context.enclosingType)->childInstantiations;
		instantiations.push_back(ChildInstantiation(&instance, context.source));
	}

	if(instance.instantiated)
	{
		return instance.layout;
	}
	try
	{
		instance.instantiated = true; // prevent recursion
		SYMBOLS_ASSERT(!instance.instantiating);
		instance.instantiating = true;
		instance.instantiation = context.source;

		static std::size_t uniqueId = 0;
		instance.uniqueId = ++uniqueId;

		if(!allowDependent
			&& instance.declaration->isTemplate)
		{
			// find the most recently declared specialization
			// TODO: optimise
			const DeclarationInstance* declaration = findDeclaration(instance.declaration->scope->declarations, instance.declaration->getName());
			SYMBOLS_ASSERT(declaration != 0);
			Declaration* specialization = findTemplateSpecialization(
				findOverloaded(*declaration), instance.deducedArguments, instance.templateArguments,
				InstantiationContext(context.source, instance.enclosing, 0, context.enclosingScope), false);
			if(specialization != 0)
			{
				instance.declaration = specialization;
			}
		}

		if(instance.declaration->enclosed == 0)
		{
			std::cout << "instantiateClass failed: ";
			printType(instance);
			std::cout << std::endl;
			return TYPELAYOUT_NONE; // TODO: this can occur when the primary template is incomplete, and a specialization was not chosen
		}

		SYMBOLS_ASSERT(instance.declaration->type.unique != 0);
		// the is the (possibly dependent) unique type of the unspecialized (template) class on which this specialization is based
		const SimpleType& original = getSimpleType(instance.declaration->type.unique);

		SYMBOLS_ASSERT(instance.declaration->enclosed != 0);
		Types& bases = instance.declaration->enclosed->bases;
		instance.bases.reserve(std::distance(bases.begin(), bases.end()));
		for(Types::const_iterator i = bases.begin(); i != bases.end(); ++i)
		{
			// TODO: check compliance: the point of instantiation of a base is the point of declaration of the enclosing (template) class
			// .. along with the point of instantiation of types required when naming the base type. e.g. struct C : A<T>::B {}; struct C : B<A<T>::value> {};
			InstantiationContext baseContext = InstantiationContext(original.instantiation, &instance, 0, context.enclosingScope);
			UniqueTypeId base = getUniqueType(*i, baseContext, allowDependent);
			SYMBOLS_ASSERT((*i).unique != 0);
			SYMBOLS_ASSERT((*i).isDependent || base.value == (*i).unique);
			if(allowDependent && (*i).isDependent)
			{
				// this occurs during 'instantiation' of a template class definition, in which case we postpone instantiation of this dependent base
				continue;
			}
			instance.layout = addMember(instance.layout, addBase(instance, base, baseContext));
		}
		instance.allowLookup = true; // prevent searching bases during lookup within incomplete instantiation
		if(!allowDependent)
		{
			std::size_t dependentTypeCount = instance.declaration->dependentConstructs.typeCount;
			instance.substitutedTypes.reserve(dependentTypeCount); // allocate up front to avoid reallocation

			const Scope::DeclarationList& members = instance.declaration->enclosed->declarationList;
			for(Scope::DeclarationList::const_iterator i = members.begin(); i != members.end(); ++i)
			{
				Declaration& declaration = *(*i);

				// substitute dependent members
				if(declaration.type.isDependent
					&& declaration.type.dependentIndex != INDEX_INVALID)
				{
					if(isUsing(declaration))
					{
						// TODO: substitute type of dependent using-declaration when class is instantiated
					}
					else if(declaration.type.dependentIndex < instance.substitutedTypes.size()) // if the type is already substituted
					{
						// do nothing
					}
					else
					{
						SYMBOLS_ASSERT(declaration.type.dependentIndex == instance.substitutedTypes.size());
						Location childLocation(declaration.location, declaration.location.pointOfInstantiation + 1);
						InstantiationContext childContext(childLocation, &instance, 0, context.enclosingScope);
						UniqueTypeWrapper type = substitute(getUniqueType(declaration.type), childContext);
						SYMBOLS_ASSERT(instance.substitutedTypes.size() != instance.substitutedTypes.capacity());
						instance.substitutedTypes.push_back(type);
					}
				}

				if(!isNonStaticDataMember(declaration))
				{
					continue;
				}
				UniqueTypeWrapper type = getUniqueType(declaration.type);
				if(declaration.type.isDependent)
				{
					// the member declaration should be found by name lookup during its instantation
					Location childLocation(declaration.location, declaration.location.pointOfInstantiation + 1);
					InstantiationContext childContext(childLocation, &instance, 0, context.enclosingScope);
					type = substitute(type, childContext);
					requireCompleteObjectType(type, childContext);
				}
				addNonStaticMember(instance, type);
			}

			SYMBOLS_ASSERT(instance.substitutedTypes.size() == dependentTypeCount);

			const DeferredExpressions& expressions = instance.declaration->dependentConstructs.expressions;
			for(DeferredExpressions::const_iterator i = expressions.begin(); i != expressions.end(); ++i)
			{
				const DeferredExpression& expression = *i;
				InstantiationContext childContext(expression.location, &instance, 0, context.enclosingScope);
#if 1 // TODO: check that the expression is convertible to bool
				ExpressionType type = typeOfExpressionWrapper(expression, childContext);
				SYMBOLS_ASSERT(!isDependent(type));
#endif
				if(expression.message != NAME_NULL)
				{
					evaluateStaticAssert(expression, expression.message.c_str(), childContext);
				}
				else
				{
					SubstitutedExpression substituted = substituteExpression(expression, childContext);
				}
			}	

#if 0
			const DeferredExpressionTypes& expressionTypes = instance.declaration->enclosed->expressionTypes;
			for(DeferredExpressionTypes::const_iterator i = expressionTypes.begin(); i != expressionTypes.end(); ++i)
			{
				const DeferredExpressionType& expression = *i;
				InstantiationContext childContext(expression.location, &instance, 0, context.enclosingScope);
				ExpressionType type = expression.callback(childContext);
				SYMBOLS_ASSERT(!isDependent(type));
			}

			const DeferredExpressionValues& expressionValues = instance.declaration->enclosed->expressionValues;
			for(DeferredExpressionValues::const_iterator i = expressionValues.begin(); i != expressionValues.end(); ++i)
			{
				const DeferredExpressionValue& expression = *i;
				InstantiationContext childContext(expression.location, &instance, 0, context.enclosingScope);
				ExpressionValue value = expression.callback(childContext);
			}
#endif

			instance.hasCopyAssignmentOperator = hasCopyAssignmentOperator(instance, context);
		}
		instance.instantiating = false;
	}
	catch(TypeError&)
	{
		printPosition(context.source);
		std::cout << "while instantiating ";
		printType(instance);
		std::cout << std::endl;
		if(instance.declaration->isTemplate)
		{
			const TemplateArgumentsInstance& templateArguments = instance.declaration->isSpecialization ? instance.deducedArguments : instance.templateArguments;
			TemplateArgumentsInstance::const_iterator a = templateArguments.begin();
			for(TemplateParameters::const_iterator i = instance.declaration->templateParams.begin(); i != instance.declaration->templateParams.end(); ++i)
			{
				SYMBOLS_ASSERT(a != templateArguments.end());
				std::cout << getValue((*i).declaration->getName()) << ": ";
				printType(*a++);
				std::cout << std::endl;
			}
		}

		if(context.enclosingType == 0
			|| !isTemplate(*context.enclosingType))
		{
			dumpTemplateInstantiations(instance, true);
		}
		throw;
	}
	return instance.layout;
}

