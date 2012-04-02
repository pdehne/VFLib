// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.
// Based on ideas from the soci wrapper sqlite back-end.

#ifndef VF_DB_BLOB_VFHEADER
#define VF_DB_BLOB_VFHEADER

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
