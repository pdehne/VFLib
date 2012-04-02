// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#include "vf/vf_StandardHeader.h"

#include "vf_concurrent.h"

namespace vf
{
#include "memory/vf_FifoFreeStoreWithoutTLS.cpp"
#include "memory/vf_FifoFreeStoreWithTLS.cpp"
#include "memory/vf_GlobalPagedFreeStore.cpp"
#include "memory/vf_PagedFreeStore.cpp"

#include "threads/vf_CallQueue.cpp"
#include "threads/vf_GuiCallQueue.cpp"
#include "threads/vf_Listeners.cpp"
#include "threads/vf_ManualCallQueue.cpp"
#include "threads/vf_ReadWriteMutex.cpp"
#include "threads/vf_SharedObject.cpp"
#include "threads/vf_ThreadWorker.cpp"
}
