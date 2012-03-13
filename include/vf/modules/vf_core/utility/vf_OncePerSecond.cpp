// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

static char const* const threadName = "Once Per Second";

//------------------------------------------------------------------------------

class OncePerSecond::TimerSingleton
  : public SharedSingleton <OncePerSecond::TimerSingleton>
{
private:
  TimerSingleton ()
    : SharedSingleton <OncePerSecond::TimerSingleton> (SingletonLifetime::persistAfterCreation)
    , m_thread (threadName)
  {
    m_thread.start (boost::bind (&TimerSingleton::run, this));
  }

  ~TimerSingleton ()
  {
    m_thread.join ();

    vfassert (m_list.empty ());
  }

  void run ()
  {
    VF_JUCE::Thread::setCurrentThreadName (threadName);

    try
    {
      for(;;)
      {
        const bool interrupted = m_thread.wait (1000);

        if (interrupted)
          break;

        notify();
      }
    }
    catch (Thread::Interrupted&)
    {
    }
  }

  void notify ()
  {
    Mutex::ScopedLockType lock (m_mutex);

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
    Mutex::ScopedLockType lock (m_mutex);

    m_list.push_back (*elem);
  }

  void remove (Elem* elem)
  {
    Mutex::ScopedLockType lock (m_mutex);

    m_list.erase (m_list.iterator_to (*elem));
  }

  static TimerSingleton* createInstance ()
  {
    return new TimerSingleton;
  }

private:
  JuceThreadType <JuceThread::PollingBased> m_thread;
  Mutex m_mutex;
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
