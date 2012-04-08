// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

#if VF_CHECK_LEAKS

//==============================================================================
// Type-independent portion of Counter
class LeakCheckedBase::CounterBase::Singleton
{
public:
  void push_back (CounterBase* counter)
  {
    m_list.push_front (counter);
  }

  void detectAllLeaks ()
  {
    CounterBase* counter = m_list.pop_front ();
    
    while (counter != nullptr)
    {
      counter->detectLeaks ();
      
      counter = m_list.pop_front ();
    }
  }

  static Singleton& getInstance ()
  {
    static Singleton instance;

    return instance;
  }

private:
  LockFreeStack <CounterBase> m_list;
};

//------------------------------------------------------------------------------

LeakCheckedBase::CounterBase::CounterBase ()
{
  Singleton::getInstance().push_back (this);
}

void LeakCheckedBase::CounterBase::detectAllLeaks ()
{
  Singleton::getInstance().detectAllLeaks ();
}

//------------------------------------------------------------------------------

void LeakCheckedBase::detectAllLeaks ()
{
  CounterBase::detectAllLeaks ();
}

#endif
