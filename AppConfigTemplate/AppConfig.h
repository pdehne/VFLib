// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

#ifndef VF_APPCONFIGTEMPLATE_VFHEADER
#define VF_APPCONFIGTEMPLATE_VFHEADER

//==============================================================================
/** @file AppConfig.h
    @ingroup vf_core
    
    Additional configuration options.

    Append these additional conditional compilation directives to your already
    existing AppConfig.hto control the compilations options avilable in VFLib.
*/

/** Activate features that use Boost.

    Your project will need to be configured to find the Boost includes
    and libraries.
*/
#ifndef VF_USE_BOOST
#define VF_USE_BOOST 0
#endif

/** Activate features that use BZip2.

    Your project will require the @ref vf_bzip2 external module.
*/
#ifndef VF_USE_BZIP2
#define VF_USE_BZIP2 1
#endif

/** Activate features that use FreeType.

    Your project will require the @ref vf_freetype external module.
*/
#ifndef VF_USE_FREETYPE
#define VF_USE_FREETYPE 1
#endif

//==============================================================================

// Ignore this
#define JUCE_CHECK_MEMORY_LEAKS 1

#endif
