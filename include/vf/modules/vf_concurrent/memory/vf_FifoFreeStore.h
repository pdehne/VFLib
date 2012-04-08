// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

#ifndef VF_FIFOFREESTORE_VFHEADER
#define VF_FIFOFREESTORE_VFHEADER

#include "vf_FifoFreeStoreWithTLS.h"
#include "vf_FifoFreeStoreWithoutTLS.h"

#if VF_USE_BOOST
typedef FifoFreeStoreWithTLS FifoFreeStoreType;
#else
typedef FifoFreeStoreWithoutTLS FifoFreeStoreType;
#endif

#endif
