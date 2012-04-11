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

#ifndef VF_SQLITE_VFHEADER
#define VF_SQLITE_VFHEADER

//==============================================================================
/** A self-contained, serverless, zero configuration, transactional SQL engine.

    This external module provides the SQLite embedded database library.

    SQLite is public domain software, visit
    http://sqlite.org

    @todo Use native SQLite libraries if available (like iOS)

    @todo Allow a user provided SQLite library through AppConfig.h configuration.

    @defgroup vf_sqlite vf_sqlite
    @ingroup external
*/

//==============================================================================
/** Include file for the @ref vf_sqlite external module.

    @file vf_sqlite.h
    @ingroup vf_sqlite
    @ingroup external
*/

#include "AppConfig.h"

#if VF_USE_SQLITE

#include "sqlite/sqlite3.h"

#endif

#endif
