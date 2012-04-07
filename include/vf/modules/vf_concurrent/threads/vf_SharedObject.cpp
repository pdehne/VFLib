// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

class SharedObject::Deleter
  : public ReferenceCountedSingleton <Deleter>
  , LeakChecked <Deleter>
{
private:
  typedef VF_JUCE::SpinLock LockType;

  Deleter ()
    : ReferenceCountedSingleton (SingletonLifetime::persistAfterCreation)
    , m_queue ("Deleter")
  {
	m_queue.start ();
  }

  ~Deleter ()
  {
    m_queue.stop_and_wait ();
  }

private:
  static void doDelete (SharedObject* sharedObject)
  {
    delete sharedObject;
  }

public:
  void Delete (SharedObject* sharedObject)
  {
    if (m_queue.isAssociatedWithCurrentThread ())
      delete sharedObject;
    else
      m_queue.call (&Deleter::doDelete, sharedObject);
  }

  static Deleter* createInstance ()
  {
    return new Deleter;
  }

private:
  AtomicCounter m_refs;

  ThreadWorker m_queue;
};

//------------------------------------------------------------------------------

SharedObject::SharedObject()
{
  Deleter::getInstance()->incReferenceCount();
}

SharedObject::~SharedObject()
{
  Deleter::getInstance()->decReferenceCount ();
}

void SharedObject::destroySharedObject ()
{
  Deleter::getInstance()->Delete (this);
}
