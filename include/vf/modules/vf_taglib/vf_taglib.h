// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
//
// This file incorporates work covered by the following copyright
// and permission notice:
//
//   This library is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Lesser General Public License version
//   2.1 as published by the Free Software Foundation.
//
//   This library is distributed in the hope that it will be useful, but
//   WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.
//
//   You should have received a copy of the GNU Lesser General Public
//   License along with this library; if not, write to the Free Software
//   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
//   02110-1301  USA
//
//   Alternatively, this file is available under the Mozilla Public
//   License Version 1.1.  You may obtain a copy of the License at
//   http://www.mozilla.org/MPL/
//

#ifndef VF_TAGLIB_VFHEADER
#define VF_TAGLIB_VFHEADER

//==============================================================================
/** TagLib: Meta data extraction and output to multimedia files.

    This external module provides the
    <a href="http://developer.kde.org/~wheeler/taglib.html">TagLib Audio Meta-Data Library</a>.

    @defgroup vf_taglib vf_taglib
    @ingroup external
*/

/** External module header file.

    Include this file to use the TagLib external library.

    @file vf_taglib.h
    @ingroup vf_taglib
    @ingroup external
*/

#include "AppConfig.h"

#if VF_USE_TAGLIB

#include "taglib/TagLibAmalgam.h"

#endif

#endif
