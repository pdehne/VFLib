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

/** \defgroup vf_concurrent Concurrent Programming (module vf_concurrent)

    A suite of classes useful for building concurrent systems.

    This module provides a concurrency model.
*/

#include "../vf_core/vf_core.h"

namespace vf
{
#include "memory/vf_AllocatedBy.h"
#include "memory/vf_FifoFreeStore.h"
#include "memory/vf_FifoFreeStoreWithoutTLS.h"
#include "memory/vf_FifoFreeStoreWithTLS.h"
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

