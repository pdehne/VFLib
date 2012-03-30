// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_SHAREDOBJECTPTR_VFHEADER
#define VF_SHAREDOBJECTPTR_VFHEADER

// Smart pointer which points to a SharedObject.
// Ideas from Juce.
//

template <class SharedObjectClass>
class SharedObjectPtr
{
public:
    /** The class being referenced by this pointer. */
    typedef SharedObjectClass SharedType;

    //==============================================================================
    /** Creates a pointer to a null object. */
    inline SharedObjectPtr() noexcept
        : referencedObject (nullptr)
    {
    }

    /** Creates a pointer to an object.

        This will increment the object's reference-count if it is non-null.
    */
    inline SharedObjectPtr (SharedObjectClass* const refCountedObject) noexcept
        : referencedObject (refCountedObject)
    {
        if (refCountedObject != nullptr)
            refCountedObject->incReferenceCount();
    }

    /** Copies another pointer.
        This will increment the object's reference-count (if it is non-null).
    */
    inline SharedObjectPtr (const SharedObjectPtr& other) noexcept
        : referencedObject (other.referencedObject)
    {
        if (referencedObject != nullptr)
            referencedObject->incReferenceCount();
    }

   #if JUCE_COMPILER_SUPPORTS_MOVE_SEMANTICS
    /** Takes-over the object from another pointer. */
    inline SharedObjectPtr (SharedObjectPtr&& other) noexcept
        : referencedObject (other.referencedObject)
    {
        other.referencedObject = nullptr;
    }
   #endif

    /** Copies another pointer.
        This will increment the object's reference-count (if it is non-null).
    */
    template <class DerivedClass>
    inline SharedObjectPtr (const SharedObjectPtr<DerivedClass>& other) noexcept
        : referencedObject (static_cast <SharedObjectClass*> (other.getObject()))
    {
        if (referencedObject != nullptr)
            referencedObject->incReferenceCount();
    }

    /** Changes this pointer to point at a different object.

        The reference count of the old object is decremented, and it might be
        deleted if it hits zero. The new object's count is incremented.
    */
    SharedObjectPtr& operator= (const SharedObjectPtr& other)
    {
        return operator= (other.referencedObject);
    }

    /** Changes this pointer to point at a different object.

        The reference count of the old object is decremented, and it might be
        deleted if it hits zero. The new object's count is incremented.
    */
    template <class DerivedClass>
    SharedObjectPtr& operator= (const SharedObjectPtr<DerivedClass>& other)
    {
        return operator= (static_cast <SharedObjectClass*> (other.getObject()));
    }

   #if JUCE_COMPILER_SUPPORTS_MOVE_SEMANTICS
    /** Takes-over the object from another pointer. */
    SharedObjectPtr& operator= (SharedObjectPtr&& other)
    {
        std::swap (referencedObject, other.referencedObject);
        return *this;
    }
   #endif

    /** Changes this pointer to point at a different object.

        The reference count of the old object is decremented, and it might be
        deleted if it hits zero. The new object's count is incremented.
    */
    SharedObjectPtr& operator= (SharedObjectClass* const newObject)
    {
        if (referencedObject != newObject)
        {
            if (newObject != nullptr)
                newObject->incReferenceCount();

            SharedObjectClass* const oldObject = referencedObject;
            referencedObject = newObject;

            if (oldObject != nullptr)
                oldObject->decReferenceCount();
        }

        return *this;
    }

    /** Destructor.

        This will decrement the object's reference-count, and may delete it if it
        gets to zero.
    */
    inline ~SharedObjectPtr()
    {
        if (referencedObject != nullptr)
            referencedObject->decReferenceCount();
    }

    /** Returns the object that this pointer references.
        The pointer returned may be zero, of course.
    */
    inline operator SharedObjectClass*() const noexcept
    {
        return referencedObject;
    }

    // the -> operator is called on the referenced object
    inline SharedObjectClass* operator->() const noexcept
    {
        return referencedObject;
    }

    /** Returns the object that this pointer references.
        The pointer returned may be zero, of course.
    */
    inline SharedObjectClass* getObject() const noexcept
    {
        return referencedObject;
    }

private:
    //==============================================================================
    SharedObjectClass* referencedObject;
};


/** Compares two SharedObjectPointers. */
template <class SharedObjectClass>
bool operator== (const SharedObjectPtr<SharedObjectClass>& object1, SharedObjectClass* const object2) noexcept
{
    return object1.getObject() == object2;
}

/** Compares two SharedObjectPointers. */
template <class SharedObjectClass>
bool operator== (const SharedObjectPtr<SharedObjectClass>& object1, const SharedObjectPtr<SharedObjectClass>& object2) noexcept
{
    return object1.getObject() == object2.getObject();
}

/** Compares two SharedObjectPointers. */
template <class SharedObjectClass>
bool operator== (SharedObjectClass* object1, SharedObjectPtr<SharedObjectClass>& object2) noexcept
{
    return object1 == object2.getObject();
}

/** Compares two SharedObjectPointers. */
template <class SharedObjectClass>
bool operator!= (const SharedObjectPtr<SharedObjectClass>& object1, const SharedObjectClass* object2) noexcept
{
    return object1.getObject() != object2;
}

/** Compares two SharedObjectPointers. */
template <class SharedObjectClass>
bool operator!= (const SharedObjectPtr<SharedObjectClass>& object1, SharedObjectPtr<SharedObjectClass>& object2) noexcept
{
    return object1.getObject() != object2.getObject();
}

/** Compares two SharedObjectPointers. */
template <class SharedObjectClass>
bool operator!= (SharedObjectClass* object1, SharedObjectPtr<SharedObjectClass>& object2) noexcept
{
    return object1 != object2.getObject();
}

#endif