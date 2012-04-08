// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_APPCONFIGTEMPLATE_VFHEADER
#define VF_APPCONFIGTEMPLATE_VFHEADER

//==============================================================================
/** AppConfig.h template

    Append this section to your existing AppConfig.h to control VFLib options.
*/

/* Activate features that depend on Boost. Your project will need to be
   configured to find the Boost includes and libraries.
*/
#ifndef VF_USE_BOOST
#define VF_USE_BOOST 0
#endif

/* Activate features that depend on SQLite.
*/
#ifndef VF_USE_SQLITE
#define VF_USE_SQLITE 1
#endif

/* This turns on features that depend on FreeType */
#ifndef VF_USE_FREETYPE
#define VF_USE_FREETYPE 1
#endif

//==============================================================================

// Ignore this
#define JUCE_CHECK_MEMORY_LEAKS 1

#endif
