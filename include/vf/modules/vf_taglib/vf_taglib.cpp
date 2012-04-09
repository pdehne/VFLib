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

#include "AppConfig.h"

#if VF_USE_TAGLIB

#ifdef _MSC_VER
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

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#endif
