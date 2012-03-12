// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#ifndef __VF_DB_BLOB_VFHEADER__
#define __VF_DB_BLOB_VFHEADER__

#include "vf/modules/vf_db/backend.h"

namespace db {

// BLOB interface
class blob
{
public:
  blob();
  ~blob();

  Error select (session& s,
                char const* zTable,
                char const* zColumn,
                rowid id,
                bool readAndWrite = false) noexcept;

  std::size_t get_len();
  Error read (std::size_t offset, void* buf, std::size_t toRead);
  Error write (std::size_t offset, void const* buf, std::size_t toWrite);
  void close ();

private:
  session* m_session;
  sqlite3_blob* m_blob;
};

}

#endif
