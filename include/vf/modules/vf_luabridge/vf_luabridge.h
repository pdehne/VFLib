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

#ifndef VF_LUABRIDGE_VFHEADER
#define VF_LUABRIDGE_VFHEADER

//==============================================================================
/** 
    LuaBridge: A lightweight, dependency-free library for binding Lua to C++.

    This is an external module that provides LuaBridge. It requires the
    \ref vf_lua module.

    @defgroup vf_luabridge vf_luabridge
*/

#include "AppConfig.h"

#if VF_USE_LUABRIDGE

#if VF_USE_LUA
#  include "../vf_lua/vf_lua.h"
#else
#  error vf_luabridge requires vf_lua
#endif

#include "luabridge/luabridge.hpp"

#endif

#endif
