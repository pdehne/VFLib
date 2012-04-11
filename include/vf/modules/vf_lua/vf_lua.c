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

#include "AppConfig.h"

#ifdef _MSC_VER
#pragma push_macro("_CRT_SECURE_NO_WARNINGS")
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

// luaconf.h only declares some things if it is being included
// from certain source files. We define all the relevant macros
// and include luaconf.h once so we get all the declarations.
//
#define lobject_c
#define lvm_c
#define LUA_CORE
#define LUA_LIB
#include "lua/luaconf.h"
#undef lobject_c
#undef lvm_c
#undef LUA_CORE
#undef LUA_LIB

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4244)
#pragma warning (disable: 4702)
#endif

#include "lua/lauxlib.c"
#include "lua/lbaselib.c"

#include "lua/lbitlib.c"
#include "lua/lcorolib.c"
#include "lua/ldblib.c"
#include "lua/linit.c"
#include "lua/liolib.c"
#include "lua/lmathlib.c"
#include "lua/loslib.c"
#include "lua/lstrlib.c"
#include "lua/ltablib.c"

// Must come before any include of llimits.h, for luai_hashnum
#include "lua/ltable.c"

#include "lua/lapi.c"
#include "lua/lcode.c"
#include "lua/lctype.c"
#include "lua/ldebug.c"
#include "lua/ldo.c"
#include "lua/ldump.c"
#include "lua/lfunc.c"
#include "lua/lgc.c"
#include "lua/llex.c"
#include "lua/lmem.c"
#include "lua/lobject.c"
#include "lua/lopcodes.c"
#include "lua/lparser.c"
#include "lua/lstate.c"
#include "lua/lstring.c"
#include "lua/ltm.c"
#include "lua/lua.c"
#include "lua/lundump.c"
#include "lua/lvm.c"
#include "lua/lzio.c"

// loadlib.c includes Windows.h, which defines the LoadString macro,
// so include it last to prevent errors.
#include "lua/loadlib.c"

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#ifdef __cplusplus
}
#endif

#ifdef _MSC_VER
#pragma pop_macro("_CRT_SECURE_NO_WARNINGS")
#endif
