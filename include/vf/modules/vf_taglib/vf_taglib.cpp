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

/** Add this to get the @ref vf_taglib external module.

    @file vf_taglib.cpp
    @ingroup vf_taglib external
*/

#include "AppConfig.h"

#include "modules/juce_core/system/juce_TargetPlatform.h"

#if JUCE_MSVC
#pragma warning (push)
#pragma warning (disable: 4018) // signed/unsigned comparison mismatch
#pragma warning (disable: 4019) // signed/unsigned comparison mismatch
#pragma warning (disable: 4127) // conditional expression is constant
#pragma warning (disable: 4244) // conversion, possible loss of data
#pragma warning (disable: 4309) // assignment truncates constant value
#pragma warning (disable: 4310) // cast truncates constant value
#pragma warning (disable: 4390) // empty controlled statement
#pragma warning (disable: 4512) // assignment operator could not be generated
#pragma warning (disable: 4996) // deprecated POSIX name
#endif

//#define HAVE_ZLIB 0

#include "taglib/TagLibAmalgam.cpp"

#if JUCE_MSVC
#pragma warning (pop)
#endif
