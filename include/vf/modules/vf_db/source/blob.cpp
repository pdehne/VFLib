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
