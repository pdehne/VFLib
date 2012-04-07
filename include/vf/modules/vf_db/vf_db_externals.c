// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.
// Based on ideas from the soci wrapper sqlite back-end.

#if _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4127) // conditional expression is constant
#pragma warning (disable: 4232) // nonstandard extension used: dllimport address 
#pragma warning (disable: 4244) // conversion from 'int': possible loss of data
#pragma warning (disable: 4701) // potentially uninitialized variable
#pragma warning (disable: 4706) // assignment within conditional expression
#endif

/* When compiled with SQLITE_THREADSAFE=2, SQLite can be used in a
   multithreaded program so long as no two threads attempt to use the
   same database connection at the same time.
*/

#define SQLITE_THREADSAFE 2

#include "sqlite/sqlite3.c"
#include "sqlite/sqlite3async.c"

#if _MSC_VER
#pragma warning (pop)
#endif
