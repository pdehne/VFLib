// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_APPCONFIGTEMPLATE_VFHEADER
#define VF_APPCONFIGTEMPLATE_VFHEADER

//==============================================================================
/** AppConfig.h template

    Append this section to your existing AppConfig.h to control VFLib options.
*/

/* If boost is available, and your project is configured to find the includes
   and libraries, this can be turned on.
*/
#ifndef VF_HAVE_BOOST
#define VF_HAVE_BOOST 0
#endif

//==============================================================================

// Ignore this
#define JUCE_CHECK_MEMORY_LEAKS 1

#endif
