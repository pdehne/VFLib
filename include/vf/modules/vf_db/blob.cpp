// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#include "vf/vf_StandardHeader.h"

#include "sqlite/sqlite3.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_db.h"
#include "error_codes.h"

namespace db {

blob::blob()
  : m_blob(0)
{
}

blob::~blob()
{
  close();
}

Error blob::select (session& s,
                    char const* zTable,
                    char const* zColumn,
                    rowid id,
                    bool readAndWrite) noexcept
{
  close();

  m_session = &s;

  int result = sqlite3_blob_open (s.get_connection(),
                                  "main",
                                  zTable,
                                  zColumn,
                                  id,
                                  readAndWrite ? 1 : 0,
                                  &m_blob );

  return detail::sqliteError (__FILE__, __LINE__, result);
}

std::size_t blob::get_len()
{
  return sqlite3_blob_bytes (m_blob);
}

Error blob::read (std::size_t offset, void* buf, std::size_t toRead)
{
  return detail::sqliteError (__FILE__, __LINE__,
    sqlite3_blob_read(m_blob, static_cast<char*>(buf), toRead, offset));
}

Error blob::write(std::size_t offset, void const* buf, std::size_t toWrite)
{
  return detail::sqliteError (__FILE__, __LINE__,
    sqlite3_blob_write(m_blob, buf, toWrite, offset));
}

void blob::close()
{
  if (m_blob)
  {
    sqlite3_blob_close (m_blob);
    m_blob = 0;
  }
}

}

END_VF_NAMESPACE
