// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

#ifndef VF_APPCONFIGTEMPLATE_VFHEADER
#define VF_APPCONFIGTEMPLATE_VFHEADER

//==============================================================================
/** AppConfig.h template

    Append this section to your existing AppConfig.h to control VFLib options.
*/

/* Turns on Boost features. Your project will need to be
   configured to find the Boost includes and libraries.
*/
#ifndef VF_USE_BOOST
#define VF_USE_BOOST 0
#endif

/* Turns on SQLite features */
#ifndef VF_USE_SQLITE
#define VF_USE_SQLITE 1
#endif

/* Turns on FreeType features */
#ifndef VF_USE_FREETYPE
#define VF_USE_FREETYPE 1
#endif

/* Turns on Lua features */
#ifndef VF_USE_LUA
#define VF_USE_LUA 1
#endif

/* Turns on BZip2 features */
#ifndef VF_USE_BZIP2
#define VF_USE_BZIP2 1
#endif

//==============================================================================

// Ignore this
#define JUCE_CHECK_MEMORY_LEAKS 1

#endif
