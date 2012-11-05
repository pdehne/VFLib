/*============================================================================*/
/*
  VFLib: https://github.com/vinniefalco/VFLib

  Copyright (C) 2008 by Vinnie Falco <vinnie.falco@gmail.com>

  This library contains portions of other open source products covered by
  separate licenses. Please see the corresponding source files for specific
  terms.
  
  VFLib is provided under the terms of The MIT License (MIT):

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  IN THE SOFTWARE.
*/
/*============================================================================*/

#ifndef VF_SHAREDOBJECT_VFHEADER
#define VF_SHAREDOBJECT_VFHEADER

/*============================================================================*/
/**
    A reference counted object with overridable destroy behavior.

    This is a reference counted object compatible with
    ReferenceCountedObjectPtr or {name}. When the last reference is removed,
    the scope provided at the time of construction is used to destroy the
    object. This is typically used to perform the delete on a separate thread.
    If no scope is specified, the object is deleted directly.

    @ingroup vf_concurrent
*/
class SharedObject : Uncopyable
{
public:
  /** Abstract SharedObject scope.

      The scope is invoked to destroy the object.
  */
  class Scope
  {
  public:
    virtual ~Scope () { }

    virtual void destroySharedObject (SharedObject* const object) = 0;
  };

public:
  /** Separate thread for a SharedObject scope.

      This Scope deletes the shared object on a separate provided thread.
  */
  class ThreadedScope : public Scope, public ThreadWithCallQueue
  {
  public:
    explicit ThreadedScope (char const* name);

    void destroySharedObject (SharedObject* const object);

    template <class Object>
    void deleteAsync (Object* const object)
    {
      callf (Delete (object));
    }

  private:
    // Simple functor to delete an object.
    //
    template <class Object>
    struct Delete
    {
      Delete (Object* const object) : m_object (object)
      {
      }

      void operator() ()
      {
        delete m_object;
      }

    private:
      Delete& operator= (Delete const& other);

      Object* const m_object;
    };
  };

protected:
  /** Construct a shared object.

      The specified scope will be used to destroy the object when its
      reference count drops to zero.
  */
  explicit SharedObject (Scope& scope);

  /** Construct a shared object.

      With no scope specified, the default destroy behavior will call
      operator delete directly unless overridden.
  */
  SharedObject ();

  virtual ~SharedObject ();

  /** Delete the object.

      This function is called when the reference count drops to zero.
      The default implementation uses the scope to delete the object if
      one was provided, or uses operator delete if no scope was specified.
  */
  virtual void destroySharedObject ();

public:
  /** Increment the reference count.

      It should not be necessary to call this function directly. Use one of
      the RAII containers that manages the reference count to hold the
      object instead.
  */
  inline void incReferenceCount() noexcept
  {
    m_refs.addref ();
  }

  /** Decrement the reference count.

      It should not be necessary to call this function directly. Use one of
      the RAII containers that manages the reference count to hold the
      object instead.
  */
  inline void decReferenceCount() noexcept
  {
    if (m_refs.release ())
      destroySharedObject ();
  }

private:
  Scope* const m_scope;
  AtomicCounter m_refs;
};

#endif
