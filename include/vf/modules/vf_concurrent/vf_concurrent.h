//==============================================================================
//
// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file GNU_GPL_v2.txt for full licensing terms.
// 
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your option)
// any later version.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
// 
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 51
// Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
// 
//==============================================================================

#ifndef VF_CONCURRENT_VFHEADER
#define VF_CONCURRENT_VFHEADER

//==============================================================================
/**
    Classes for building a concurrent system.

    This object is an alternative to traditional locking techniques used to
    implement concurrent systems. Instead of acquiring a mutex to change shared
    data, a functor is queued for later execution (usually on another thread). The
    execution of the functor applies the transformation to the shared state that
    was formerly performed within a lock (i.e. CriticalSection).

    For read operations on shared data, instead of acquiring a mutex and
    accessing the data directly, copies are made (one for each thread), and the
    thread accesses its copy without acquiring a lock. One thread owns the master
    copy of the shared state. Requests for changing shared state are made by other
    threads by posting functors to the master thread's CallQueue. The master
    thread notifies other threads of changes by posting functors to their
    respective associated CallQueue, using the Listeners interface.

    The purpose of the functor is to encapsulate one mutation of shared state to
    guarantee progress towards a consensus of the concurrent data among
    participating threads. Functors should execute quickly, ideally in constant
    time. Dynamically allocated objects of class type passed as functor parameters
    should, in general, be reference counted. The ConcurrentObject class is ideal
    for meeting this requirement, and has the additional benefit that the workload
    of deletion is performed on a separate, provided thread. This queue is not a
    replacement for a thread pool or job queue type system.

    A CallQueue is considered signaled when one or more functors are present.
    Functors are executed during a call to synchronize(). The operation of
    executing functors via the call to synchronize() is called synchronizing
    the queue. It can more generally be thought of as synchronizing multiple
    copies of shared data between threads.

    Although there is some extra work required to set up and maintain this
    system, the benefits are significant. Since shared data is only synchronized
    at well defined times, the programmer can reason and make strong statements
    about the correctness of the concurrent system. For example, if the
    audioDeviceIOCallback synchronizes the CallQueue only at the beginning of its
    execution, it is guaranteed that shared data will remain the same throughout
    the remainder of the function.

    Because shared data is accessed for reading without a lock, upper bounds
    on the run time performance can easily be calculated and assured. Compare
    this with the use of a mutex - the run time performance experiences a
    combinatorial explosion of possibilities depending on the complex interaction
    of multiple threads.

    \defgroup vf_concurrent vf_concurrent
 */
/// \todo discuss limit on the size of the functor

#include "../vf_core/vf_core.h"

namespace vf
{
#include "memory/vf_AllocatedBy.h"
#include "memory/vf_FifoFreeStore.h"
#if VF_USE_BOOST
#include "memory/vf_FifoFreeStoreWithTLS.h"
#else
#include "memory/vf_FifoFreeStoreWithoutTLS.h"
#endif
#include "memory/vf_GlobalFifoFreeStore.h"
#include "memory/vf_GlobalPagedFreeStore.h"
#include "memory/vf_PagedFreeStore.h"

#include "threads/vf_CallQueue.h"
#include "threads/vf_ConcurrentObject.h"
#include "threads/vf_ConcurrentState.h"
#include "threads/vf_GuiCallQueue.h"
#include "threads/vf_Listeners.h"
#include "threads/vf_ManualCallQueue.h"
#include "threads/vf_ReadWriteMutex.h"
#include "threads/vf_ThreadWithCallQueue.h"
}

#endif

