// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_CONFIG_VFHEADER__
#define __VF_CONFIG_VFHEADER__

// Turns vflib specific debugging support on or off
//
#ifndef VF_DEBUG
#define VF_DEBUG JUCE_DEBUG // get the flag from Juce
#endif

// Enables leak checking
//
#ifndef VF_CHECK_LEAKS
#define VF_CHECK_LEAKS JUCE_CHECK_MEMORY_LEAKS
#endif

#endif
