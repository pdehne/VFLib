// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_CONFIG_VFHEADER__
#define __VF_CONFIG_VFHEADER__

//
// Master settings file
//

// Controls the namespace
//
#ifndef VF_NAMESPACE
#define VF_NAMESPACE vf
#endif

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
