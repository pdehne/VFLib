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

#include "queue/vf_CallQueue.cpp"
#include "queue/vf_GuiCallQueue.cpp"
#include "queue/vf_Listeners.cpp"
#include "queue/vf_ManualCallQueue.cpp"
#include "queue/vf_ThreadWorker.cpp"

#include "threads/vf_ReadWriteMutex.cpp"
#include "threads/vf_SharedObject.cpp"
}
