
#ifndef INCLUDED_CPPPARSE_CORE_TYPEINSTANTIATE_H
#define INCLUDED_CPPPARSE_CORE_TYPEINSTANTIATE_H

#include "Ast/Type.h"
#include "NameLookup.h"
#include "Ast/Print.h"
#include "Fundamental.h"

TypeLayout instantiateClass(const Instance& instanceConst, const InstantiationContext& context, bool allowDependent = false);

inline std::size_t getPointOfInstantiation(const Instance& instance)
{
	return instance.instantiation.pointOfInstantiation;
}

inline void requireCompleteObjectType(UniqueTypeWrapper type, const InstantiationContext& context)
{
	SYMBOLS_ASSERT(!isDependent(type));
	if(type.isArray()
		&& getArrayType(type.value).size != 0)
	{
		type.pop_front(); // arrays of known size are complete object types
		requireCompleteObjectType(type, context);
	}
	else if(type.isSimple())
	{
		const Instance& objectType = getInstance(type.value);
		if(isClass(*objectType.declaration))
		{
			instantiateClass(objectType, context);
		}
	}
}

inline TypeLayout getTypeLayout(UniqueTypeWrapper type)
{
	SYMBOLS_ASSERT(!isDependent(type));
	if(type.isArray()
		&& getArrayType(type.value).size != 0)
	{
		std::size_t count = getArrayType(type.value).size;
		type.pop_front();
		return makeArray(getTypeLayout(type), count);
	}
	else if(type.isPointer())
	{
		return TypeLayout(4, 4); // TODO: x64
	}
	else if(type.isMemberPointer())
	{
		return TypeLayout(4, 4); // TODO: x64, size depends on class
	}
	else if(type.isSimple())
	{
		const Instance& objectType = getInstance(type.value);
		if(isEnum(*objectType.declaration))
		{
			return TypeLayout(4, 4); // TODO: x64, variable enum size
		}
		if(isClass(*objectType.declaration))
		{
			SYMBOLS_ASSERT(objectType.instantiated);
			return TypeLayout(evaluateSizeof(objectType.layout), objectType.layout.align);
		}
		return objectType.layout; // built-in type
	}
	return TYPELAYOUT_NONE; // this type has no meaningful layout (e.g. incomplete array, reference, function)
}

inline bool findBase(const Instance& other, const Instance& type)
{
	SYMBOLS_ASSERT(other.declaration->enclosed != 0);
	SYMBOLS_ASSERT(isClass(*type.declaration));
	for(UniqueBases::const_iterator i = other.bases.begin(); i != other.bases.end(); ++i)
	{
		const Instance& base = *(*i);
		SYMBOLS_ASSERT(isClass(*base.declaration));
		if(&base == &type)
		{
			return true;
		}
		if(findBase(base, type))
		{
			return true;
		}
	}
	return false;
}

// Returns true if 'type' is a base of 'other'
inline bool isBaseOf(const Instance& type, const Instance& other, const InstantiationContext& context)
{
	if(!isClass(*type.declaration)
		|| !isClass(*other.declaration))
	{
		return false;
	}
	if(isIncomplete(*type.declaration)
		|| isIncomplete(*other.declaration))
	{
		return false;
	}
	instantiateClass(other, context);
	return findBase(other, type);
}


inline bool isBaseOf(UniqueTypeWrapper base, UniqueTypeWrapper derived, const InstantiationContext& context)
{
	if(!base.isSimple()
		|| !derived.isSimple())
	{
		return false;
	}
	const Instance& baseType = getInstance(base.value);
	const Instance& derivedType = getInstance(derived.value);
	if(&baseType == &derivedType)
	{
		return true;
	}
	SYMBOLS_ASSERT(!isClass(*derivedType.declaration) || !isIncomplete(*derivedType.declaration)); // TODO: does SFINAE apply?
	return isBaseOf(baseType, derivedType, context);
}


inline bool hasVirtualDestructor(const Instance& classInstance)
{
	SYMBOLS_ASSERT(isClass(classInstance));
	SYMBOLS_ASSERT(isComplete(classInstance));
	SYMBOLS_ASSERT(classInstance.instantiated);
	// [class.dtor]
	// A destructor can be declared virtual (10.3) or pure virtual (10.4); if any objects of that class or any
	// derived class are created in the program, the destructor shall be defined.If a class has a base class with a
	// virtual destructor, its destructor(whether user- or implicitly-declared) is virtual.
	return classInstance.hasVirtualDestructor;
}

// return true if class (or any base thereof) has a virtual destructor.
inline bool hasVirtualDestructor(UniqueTypeWrapper type, const InstantiationContext& context)
{
	if(!isClass(type))
	{
		return false;
	}
	const Instance& classInstance = getInstance(type.value);
	instantiateClass(classInstance, context); // requires a complete type
	return hasVirtualDestructor(classInstance);
}

inline bool isEmpty(const Instance& classInstance)
{
	SYMBOLS_ASSERT(isClass(classInstance));
	SYMBOLS_ASSERT(isComplete(classInstance));
	SYMBOLS_ASSERT(classInstance.instantiated);
	if(isUnion(classInstance))
	{
		return false; // union cannot be empty
	}
	// TODO: if any member is not a zero-sized bitfield, return false
	return classInstance.isEmpty;
}

const bool TYPETRAITS_ISEMPTY_NONCLASS = false;

// returns true if type is a non-union class with no members (other than bitfield size zero), no virtual functions, no virtual base classes, no non-empty base classes
inline bool isEmpty(UniqueTypeWrapper type, const InstantiationContext& context)
{
	if(!isClass(type))
	{
		return TYPETRAITS_ISEMPTY_NONCLASS;
	}
	const Instance& classInstance = getInstance(type.value);
	instantiateClass(classInstance, context); // requires a complete type
	return isEmpty(classInstance);
}

inline bool isPod(const Instance& classInstance)
{
	SYMBOLS_ASSERT(isClass(classInstance));
	SYMBOLS_ASSERT(isComplete(classInstance));
	SYMBOLS_ASSERT(classInstance.instantiated);
	if(isUnion(classInstance))
	{
		return true;
	}
	return classInstance.isPod;
}

const bool TYPETRAITS_ISPOD_NONCLASS = true; // TODO: MSVC returns false for non-class non-union types, GCC returns true

// returns true if type is a class or union with no non-pod members, no base classes, no virtual functions, no constructor or destructor
inline bool isPod(UniqueTypeWrapper type, const InstantiationContext& context)
{
	if(!isClass(type))
	{
		return TYPETRAITS_ISPOD_NONCLASS;
	}
	const Instance& classInstance = getInstance(type.value);
	instantiateClass(classInstance, context); // requires a complete type
	return isPod(classInstance);
}

inline bool isAnonymousUnion(const Declaration& declaration)
{
	return !declaration.isCStyle // 'typedef union { } U' is not anonymous!
		&& declaration.isUnion
		&& declaration.getName().value.c_str()[0] == '$';
}

inline bool isAnonymousUnion(const Instance& classInstance)
{
	return isAnonymousUnion(*classInstance.declaration);
}

// returns true if this declaration requires a complete type at the point of declaration
inline bool isCompleteTypeRequired(const Declaration& declaration)
{
	if(isMember(declaration)
		&& isStatic(declaration))
	{
		return false;
	}
	if(isType(declaration)
		|| isEnumerator(declaration)
		|| isFunction(declaration)
		|| isUsing(declaration)
		|| isFunctionParameter(declaration)
		|| isExtern(declaration))
	{
		return false;
	}
	return true;
}

// returns true if this declaration is a member that contributes to the layout of the class
inline bool isNonStaticDataMember(const Declaration& declaration)
{
	return isMember(declaration)
		&& !isType(declaration) // member class/enum/typedef
		&& !isUsing(declaration)
		&& !isEnumerator(declaration)
		&& !isFunction(declaration)
		&& !isStatic(declaration);
}



inline void addNonStaticMember(const Instance& classInstance, bool isPod, bool isEmpty)
{
	const_cast<Instance&>(classInstance).isPod &= isPod;
	const_cast<Instance&>(classInstance).isEmpty &= isEmpty;
	if(isAnonymousUnion(classInstance))
	{
		addNonStaticMember(*classInstance.enclosing, isPod, isEmpty);
	}
}

inline void addNonStaticMember(const Instance& classInstance, UniqueTypeWrapper type)
{
	TypeLayout layout = getTypeLayout(type);
	const_cast<Instance&>(classInstance).layout = addMember(const_cast<Instance&>(classInstance).layout, layout, isUnion(classInstance));
	addNonStaticMember(classInstance,
		isClass(type) ? isPod(getInstance(type.value)) : TYPETRAITS_ISPOD_NONCLASS,
		isClass(type) ? isEmpty(getInstance(type.value)) : TYPETRAITS_ISEMPTY_NONCLASS); // TODO: ignore bitfield with size zero!
}

inline bool isPolymorphic(const Instance& classInstance)
{
	SYMBOLS_ASSERT(isClass(classInstance));
	SYMBOLS_ASSERT(isComplete(classInstance));
	SYMBOLS_ASSERT(classInstance.instantiated);
	return classInstance.isPolymorphic;
}

// return true if class (or any base thereof) has at least one virtual function.
inline bool isPolymorphic(UniqueTypeWrapper type, const InstantiationContext& context)
{
	if(!isClass(type))
	{
		return false;
	}
	const Instance& classInstance = getInstance(type.value);
	instantiateClass(classInstance, context); // requires a complete type
	return isPolymorphic(classInstance);
}

inline bool isAbstract(const Instance& classInstance, const InstantiationContext& context)
{
	SYMBOLS_ASSERT(isClass(classInstance));
	SYMBOLS_ASSERT(isComplete(classInstance));
	SYMBOLS_ASSERT(classInstance.instantiated);
	return classInstance.isAbstract;
}

// return true if class (or any base thereof) has at least one pure virtual function.
inline bool isAbstract(UniqueTypeWrapper type, const InstantiationContext& context)
{
	if(!isClass(type))
	{
		return false;
	}
	const Instance& classInstance = getInstance(type.value);
	instantiateClass(classInstance, context); // requires a complete type
	return isAbstract(classInstance, context);
}



//-----------------------------------------------------------------------------
struct DeferredInstantiation
{
	InstantiationContext context;
	const Instance* instance;
	DeferredInstantiation(const InstantiationContext& context, const Instance* instance)
		: context(context), instance(instance)
	{
	}
};
typedef std::list<DeferredInstantiation> DeferredInstantiations;

inline void instantiateDeferred(DeferredInstantiations& deferred)
{
	for(DeferredInstantiations::const_iterator i = deferred.begin(); i != deferred.end(); ++i)
	{
		const DeferredInstantiation& instantiation = *i;
		const_cast<Instance*>(instantiation.instance)->instantiated = false;
		instantiateClass(*instantiation.instance, instantiation.context);
	}
	deferred.clear();
}

extern DeferredInstantiations gDeferredInstantiations;


#endif
