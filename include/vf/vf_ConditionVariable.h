// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_CONDITIONVARIABLE_VFHEADER__
#define __VF_CONDITIONVARIABLE_VFHEADER__

//
// WARNING:: UNFINISHED AND UNTESTED, DO NOT USE!!!
//

// Unfinished condition variable designed for one reader one writer

// UNFINISHED
class UnfinishedConditionVariable
{
public:
  UnfinishedConditionVariable ();

  void signal ();

  template <class CriticalSectionToUse>
  bool wait (CriticalSectionToUse& lock,
             int timeOutMilliseconds = -1)
  {
    bool signaled;

    // Avoid race conditions.
    m_mutex.enter();

    // Increment count of waiters.
    ++m_waiters_count;

    // Store current generation in our activation record.
    int my_generation = m_wait_generation_count;

    m_mutex.exit();

    lock.exit();

    for (;;)
    {
      // Wait until the event is signaled.
      signaled = m_event.wait (timeOutMilliseconds);

      m_mutex.enter();

      if (signaled)
      {
        // Exit the loop when event is signaled and
        // there are still waiting threads from this wait_generation
        // that haven't been released from this wait yet.
        const bool wait_done = m_release_count > 0 &&
                               m_wait_generation_count != my_generation;

        m_mutex.exit();

        if (wait_done)
          break;
      }
      else
      {
        --m_waiters_count;

        m_mutex.exit();

        break;
      }
    }

    if (signaled)
    {
      lock.enter();

      m_mutex.enter();

      --m_waiters_count;
      --m_release_count;
      
      bool last_waiter = m_release_count == 0;

      m_mutex.exit();

      // If we're the last waiter, reset the manual event.
      if (last_waiter)
        m_event.reset();
    }
    
    return signaled;
  }

private:
  int m_waiters_count;
  int m_release_count;
  int m_wait_generation_count;
  VF_JUCE::WaitableEvent m_event;
  VF_JUCE::CriticalSection m_mutex;
};

#endif
