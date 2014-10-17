
#ifndef INCLUDED_CPPPARSE_CORE_TYPEINSTANTIATE_H
#define INCLUDED_CPPPARSE_CORE_TYPEINSTANTIATE_H

#include "Ast/Type.h"
#include "NameLookup.h"
#include "Ast/Print.h"
#include "Fundamental.h"

TypeLayout instantiateClass(const SimpleType& instanceConst, const InstantiationContext& context, bool allowDependent = false);


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
		const SimpleType& objectType = getSimpleType(type.value);
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
		const SimpleType& objectType = getSimpleType(type.value);
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

inline bool findBase(const SimpleType& other, const SimpleType& type)
{
	SYMBOLS_ASSERT(other.declaration->enclosed != 0);
	SYMBOLS_ASSERT(isClass(*type.declaration));
	for(UniqueBases::const_iterator i = other.bases.begin(); i != other.bases.end(); ++i)
	{
		const SimpleType& base = *(*i);
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
inline bool isBaseOf(const SimpleType& type, const SimpleType& other, const InstantiationContext& context)
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
	const SimpleType& baseType = getSimpleType(base.value);
	const SimpleType& derivedType = getSimpleType(derived.value);
	if(&baseType == &derivedType)
	{
		return true;
	}
	SYMBOLS_ASSERT(!isClass(*derivedType.declaration) || !isIncomplete(*derivedType.declaration)); // TODO: does SFINAE apply?
	return isBaseOf(baseType, derivedType, context);
}


inline bool hasVirtualDestructor(const SimpleType& classType)
{
	SYMBOLS_ASSERT(isClass(classType));
	SYMBOLS_ASSERT(isComplete(classType));
	SYMBOLS_ASSERT(classType.instantiated);
	// [class.dtor]
	// A destructor can be declared virtual (10.3) or pure virtual (10.4); if any objects of that class or any
	// derived class are created in the program, the destructor shall be defined.If a class has a base class with a
	// virtual destructor, its destructor(whether user- or implicitly-declared) is virtual.
	return classType.hasVirtualDestructor;
}

// return true if class (or any base thereof) has a virtual destructor.
inline bool hasVirtualDestructor(UniqueTypeWrapper type, const InstantiationContext& context)
{
	if(!isClass(type))
	{
		return false;
	}
	const SimpleType& classType = getSimpleType(type.value);
	instantiateClass(classType, context); // requires a complete type
	return hasVirtualDestructor(classType);
}

inline bool isEmpty(const SimpleType& classType)
{
	SYMBOLS_ASSERT(isClass(classType));
	SYMBOLS_ASSERT(isComplete(classType));
	SYMBOLS_ASSERT(classType.instantiated);
	if(isUnion(classType))
	{
		return false; // union cannot be empty
	}
	// TODO: if any member is not a zero-sized bitfield, return false
	return classType.isEmpty;
}

const bool TYPETRAITS_ISEMPTY_NONCLASS = false;

// returns true if type is a non-union class with no members (other than bitfield size zero), no virtual functions, no virtual base classes, no non-empty base classes
inline bool isEmpty(UniqueTypeWrapper type, const InstantiationContext& context)
{
	if(!isClass(type))
	{
		return TYPETRAITS_ISEMPTY_NONCLASS;
	}
	const SimpleType& classType = getSimpleType(type.value);
	instantiateClass(classType, context); // requires a complete type
	return isEmpty(classType);
}

inline bool isPod(const SimpleType& classType)
{
	SYMBOLS_ASSERT(isClass(classType));
	SYMBOLS_ASSERT(isComplete(classType));
	SYMBOLS_ASSERT(classType.instantiated);
	if(isUnion(classType))
	{
		return true;
	}
	return classType.isPod;
}

const bool TYPETRAITS_ISPOD_NONCLASS = true; // TODO: MSVC returns false for non-class non-union types, GCC returns true

// returns true if type is a class or union with no non-pod members, no base classes, no virtual functions, no constructor or destructor
inline bool isPod(UniqueTypeWrapper type, const InstantiationContext& context)
{
	if(!isClass(type))
	{
		return TYPETRAITS_ISPOD_NONCLASS;
	}
	const SimpleType& classType = getSimpleType(type.value);
	instantiateClass(classType, context); // requires a complete type
	return isPod(classType);
}

inline bool isAnonymousUnion(const SimpleType& classType)
{
	return !classType.isCStyle // 'typedef union { } U' is not anonymous!
		&& isUnion(classType)
		&& classType.declaration->getName().value.c_str()[0] == '$';
}

inline bool isNonStaticDataMember(const Declaration& declaration)
{
	return isMember(declaration) // just members, for now
		&& !isClass(declaration)
		&& !isEnum(declaration)
		&& !isEnumerator(declaration)
		&& !isFunction(declaration) // member functions are not instantiated when class is implicitly instantiated
		&& !isStatic(declaration) // static members are not instantiated when class is implicitly instantiated
		&& !isTypedef(declaration); // member typedefs are not instantiated when class is implicitly instantiated
}



inline void addNonStaticMember(const SimpleType& classType, bool isPod, bool isEmpty)
{
	const_cast<SimpleType&>(classType).isPod &= isPod;
	const_cast<SimpleType&>(classType).isEmpty &= isEmpty;
	if(isAnonymousUnion(classType))
	{
		addNonStaticMember(*classType.enclosing, isPod, isEmpty);
	}
}

inline void addNonStaticMember(const SimpleType& classType, UniqueTypeWrapper type)
{
	TypeLayout layout = getTypeLayout(type);
	const_cast<SimpleType&>(classType).layout = addMember(const_cast<SimpleType&>(classType).layout, layout, isUnion(classType));
	addNonStaticMember(classType,
		isClass(type) ? isPod(getSimpleType(type.value)) : TYPETRAITS_ISPOD_NONCLASS,
		isClass(type) ? isEmpty(getSimpleType(type.value)) : TYPETRAITS_ISEMPTY_NONCLASS); // TODO: ignore bitfield with size zero!
}

inline bool isPolymorphic(const SimpleType& classType)
{
	SYMBOLS_ASSERT(isClass(classType));
	SYMBOLS_ASSERT(isComplete(classType));
	SYMBOLS_ASSERT(classType.instantiated);
	return classType.isPolymorphic;
}

// return true if class (or any base thereof) has at least one virtual function.
inline bool isPolymorphic(UniqueTypeWrapper type, const InstantiationContext& context)
{
	if(!isClass(type))
	{
		return false;
	}
	const SimpleType& classType = getSimpleType(type.value);
	instantiateClass(classType, context); // requires a complete type
	return isPolymorphic(classType);
}

inline bool isAbstract(const SimpleType& classType, const InstantiationContext& context)
{
	SYMBOLS_ASSERT(isClass(classType));
	SYMBOLS_ASSERT(isComplete(classType));
	SYMBOLS_ASSERT(classType.instantiated);
	return classType.isAbstract;
}

// return true if class (or any base thereof) has at least one pure virtual function.
inline bool isAbstract(UniqueTypeWrapper type, const InstantiationContext& context)
{
	if(!isClass(type))
	{
		return false;
	}
	const SimpleType& classType = getSimpleType(type.value);
	instantiateClass(classType, context); // requires a complete type
	return isAbstract(classType, context);
}

#endif
