// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

class OncePerSecond::TimerSingleton
  : public ReferenceCountedSingleton <OncePerSecond::TimerSingleton>
{
private:
  TimerSingleton ()
    : ReferenceCountedSingleton <OncePerSecond::TimerSingleton> (
		SingletonLifetime::persistAfterCreation)
    , m_thread ("Once Per Second")
  {
    m_thread.start (vf::bind (&TimerSingleton::run, this));
  }

  ~TimerSingleton ()
  {
    m_thread.join ();

    vfassert (m_list.empty ());
  }

  void run ()
  {
    for(;;)
    {
      const bool interrupted = m_thread.wait (1000);

      if (interrupted)
        break;

      notify();
    }
  }

  void notify ()
  {
	VF_JUCE::CriticalSection::ScopedLockType lock (m_mutex);

    for (List::iterator iter = m_list.begin(); iter != m_list.end();)
    {
      OncePerSecond* object = iter->object;
      ++iter;
      object->doOncePerSecond ();
    }
  }

public:
  void insert (Elem* elem)
  {
	VF_JUCE::CriticalSection::ScopedLockType lock (m_mutex);

    m_list.push_back (*elem);
  }

  void remove (Elem* elem)
  {
	VF_JUCE::CriticalSection::ScopedLockType lock (m_mutex);

    m_list.erase (m_list.iterator_to (*elem));
  }

  static TimerSingleton* createInstance ()
  {
    return new TimerSingleton;
  }

private:
  InterruptibleThread m_thread;
  VF_JUCE::CriticalSection m_mutex;
  List m_list;
};

//------------------------------------------------------------------------------

OncePerSecond::OncePerSecond ()
{
  m_elem.instance = TimerSingleton::getInstance ();
  m_elem.object = this;
}

OncePerSecond::~OncePerSecond ()
{
}

void OncePerSecond::startOncePerSecond ()
{
  m_elem.instance->insert (&m_elem);
}

void OncePerSecond::endOncePerSecond ()
{
  m_elem.instance->remove (&m_elem);
}
