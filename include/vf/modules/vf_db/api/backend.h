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

#ifndef VF_DB_BACKEND_VFHEADER
#define VF_DB_BACKEND_VFHEADER

namespace db {

namespace detail {

// supported standard types for intos and uses
enum exchange_type
{
  x_null, // only for input variables

  x_bool,

  x_char,  x_uchar,
  x_short, x_ushort,
  x_int,   x_uint,
  x_long,  x_ulong,
  x_int64, x_uint64,
  x_float, x_double,

  x_cstring, // null terminated char*, only valid for use_type
  x_cwstring, // null terminated wchar*, only valid for use_type
  x_stdstring,
  x_stdwstring,
  x_juceString,

  x_stdtm,

  x_blob
};

}

// the enum type for indicator variables
enum indicator
{
  i_ok,
  i_null,
  i_truncated
};

// sqlite row id

//typedef sqlite3_int64 rowid;
typedef VF_JUCE::int64 rowid;

}

#endif
