// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.
// Based on ideas from the soci wrapper sqlite back-end.

#ifndef __VF_DB_BACKEND_VFHEADER__
#define __VF_DB_BACKEND_VFHEADER__

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
