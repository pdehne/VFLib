/*============================================================================*/
/*
  Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
  See the file GNU_GPL_v2.txt for full licensing terms.

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 2 of the License, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 51
  Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
/*============================================================================*/

ThreadGroup::Worker::Worker (ThreadGroup& group, String name)
  : Thread (name)
  , m_group (group)
{
  startThread ();
}

ThreadGroup::Worker::~Worker ()
{
  stopThread (-1);
}

void ThreadGroup::Worker::run ()
{
  while (!threadShouldExit ())
  {
    m_group.m_semaphore.wait ();

    Work* w = m_group.m_queue.pop_front ();

    w->operator() ();

    delete w;
  }
}

//==============================================================================

ThreadGroup::ThreadGroup ()
{
  setNumberOfThreads (SystemStats::getNumCpus ());
}

ThreadGroup::ThreadGroup (int numberOfThreads)
{
  setNumberOfThreads (numberOfThreads);
}

ThreadGroup::~ThreadGroup ()
{
  setNumberOfThreads (0);
}

void ThreadGroup::setNumberOfThreads (int numberOfThreads)
{
  jassert (numberOfThreads >= 0);

  ScopedLock lock (m_mutex);

  int previousSize = m_workers.size ();

  if (numberOfThreads > previousSize)
  {
    while (m_workers.size () < numberOfThreads)
    {
      String s;
      s << "ThreadGroup (" << (m_workers.size () + 1) << ")";

      Worker* worker = new Worker (*this, s);

      m_workers.push_back (*worker);
    }
  }
  else
  {
    jassertfalse;
  }
}

void ThreadGroup::callw (Work* w)
{
  m_queue.push_front (w);

  m_semaphore.signal ();
}
