// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file BOOST_LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// This file incorporates work covered by the following copyright
// and permission notice:
//
//   Copyright (C) 2004-2008 Maciej Sobczak, Stephen Hutton,
//   Mateusz Loskot, Pawel Aleksander Fedorynski, David Courtney,
//   Rafal Bobrowski, Julian Taylor, Henning Basold, Ilia Barahovski,
//   Denis Arnaud, Daniel Lidström, Matthieu Kermagoret, Artyom Beilis,
//   Cory Bennett, Chris Weed, Michael Davidsaver, Jakub Stachowski,
//   Alex Ott, Rainer Bauer, Martin Muenstermann, Philip Pemberton,
//   Eli Green, Frederic Chateau, Artyom Tonkikh, Roger Orr,
//   Robert Massaioli, Sergey Nikulov, Shridhar Daithankar,
//   Sören Meyer-Eppler, Mario Valesco
//
//   Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file BOOST_LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//

#include "AppConfig.h"

#if VF_USE_SQLITE

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

#endif
