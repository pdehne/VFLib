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

namespace detail {

const Error sqliteError (const char* sourceFileName,
                         int lineNumber,
                         int sqliteErrorCode)
{
  /*
  jassert (sqliteErrorCode != SQLITE_ROW &&
           sqliteErrorCode != SQLITE_DONE);
 */

  Error e;
  String s;
  Error::Code c;

  switch (sqliteErrorCode)
  {
  // should never get these two here but include them just in case
  case SQLITE_ROW:
  case SQLITE_DONE:

  case SQLITE_OK:
    // this is fine, and lets us avoid having to explicitly
    // check for SQLITE_OK and just deal in terms of the Error object.
    c = Error::success;
    break;

  case SQLITE_ERROR:      s=TRANS("an sqlite error or missing database was encountered"); c = Error::general; break;
  case SQLITE_INTERNAL:   s=TRANS("sqlite encountered an internal logic error"); c = Error::unexpected; break;
  case SQLITE_PERM:       s=TRANS("sqlite was denied file access permission"); c = Error::fileNoPerm; break;
  case SQLITE_ABORT:      s=TRANS("the sqlite operation was canceled due to a callback"); c = Error::canceled; break;
  case SQLITE_BUSY:       s=TRANS("the sqlite database file is locked"); c = Error::fileInUse; break;
  case SQLITE_LOCKED:     s=TRANS("the sqlite database table was locked"); c = Error::fileInUse; break;
  case SQLITE_NOMEM:      s=TRANS("sqlite ran out of memory"); c = Error::noMemory; break;
  case SQLITE_READONLY:   s=TRANS("sqlite tried to write to a read-only database"); c = Error::fileNoPerm; break;
  case SQLITE_INTERRUPT:  s=TRANS("the sqlite operation was interrupted"); c = Error::canceled; break;
  case SQLITE_IOERR:      s=TRANS("sqlite encountered a device I/O error"); c = Error::fileIOError; break;
  case SQLITE_CORRUPT:    s=TRANS("the sqlite database is corrupt"); c = Error::invalidData; break;
  case SQLITE_FULL:       s=TRANS("the sqlite database is full"); c = Error::fileNoSpace; break;
  case SQLITE_CANTOPEN:   s=TRANS("the sqlite database could not be opened"); c = Error::fileNotFound; break;
  case SQLITE_PROTOCOL:   s=TRANS("sqlite encountered a lock protocol error"); c = Error::badParameter; break;
  case SQLITE_EMPTY:      s=TRANS("the sqlite database is empty"); c = Error::noMoreData; break;
  case SQLITE_SCHEMA:     s=TRANS("the sqlite database scheme was changed"); c = Error::invalidData; break;
  case SQLITE_TOOBIG:     s=TRANS("the sqlite string or blob was too large"); c = Error::fileNoSpace; break;
  case SQLITE_CONSTRAINT: s=TRANS("the sqlite operation was aborted due to a constraint violation"); c = Error::badParameter; break;
  case SQLITE_MISMATCH:   s=TRANS("the sqlite data was mismatched"); c = Error::badParameter; break;
  case SQLITE_MISUSE:     s=TRANS("the sqlite library parameter was invalid"); c = Error::badParameter; break;
  case SQLITE_NOLFS:      s=TRANS("the sqlite platform feature is unavailable"); c = Error::badParameter; break;
  case SQLITE_AUTH:       s=TRANS("sqlite authorization was denied"); c = Error::fileNoPerm; break;
  case SQLITE_FORMAT:     s=TRANS("the auxiliary sqlite database has an invalid format"); c = Error::invalidData; break;
  case SQLITE_RANGE:      s=TRANS("the sqlite parameter was invalid"); c = Error::badParameter; break;
  case SQLITE_NOTADB:     s=TRANS("the file is not a sqlite database"); c = Error::invalidData; break;
  default:
    s << TRANS("an unknown sqlite3 error code #")
      << String(sqliteErrorCode)
      << TRANS("was returned");
    c = Error::general;
    break;
  }

  if (c != Error::success)
    e.fail (sourceFileName, lineNumber, s, c);

  return e;
}

}

}
