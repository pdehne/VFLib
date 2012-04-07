// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

class ConcurrentObject::Deleter
  : public ReferenceCountedSingleton <Deleter>
  , LeakChecked <Deleter>
{
private:
  typedef VF_JUCE::SpinLock LockType;

  Deleter ()
    : ReferenceCountedSingleton (SingletonLifetime::persistAfterCreation)
    , m_fifo ("Deleter")
  {
	m_fifo.start ();
  }

  ~Deleter ()
  {
    m_fifo.stop_and_wait ();
  }

private:
  static void doDelete (ConcurrentObject* sharedObject)
  {
    delete sharedObject;
  }

public:
  void Delete (ConcurrentObject* sharedObject)
  {
    if (m_fifo.isAssociatedWithCurrentThread ())
      delete sharedObject;
    else
      m_fifo.call (&Deleter::doDelete, sharedObject);
  }

  static Deleter* createInstance ()
  {
    return new Deleter;
  }

private:
  AtomicCounter m_refs;

  ThreadWorker m_fifo;
};

//------------------------------------------------------------------------------

ConcurrentObject::ConcurrentObject()
{
  Deleter::getInstance()->incReferenceCount();
}

ConcurrentObject::~ConcurrentObject()
{
  Deleter::getInstance()->decReferenceCount ();
}

void ConcurrentObject::destroyConcurrentObject ()
{
  Deleter::getInstance()->Delete (this);
}
