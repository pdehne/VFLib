// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#ifndef __VF_DB_BACKEND_VFHEADER__
#define __VF_DB_BACKEND_VFHEADER__

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

#endif
