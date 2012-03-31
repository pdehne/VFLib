// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.
// Based on ideas from the soci wrapper sqlite back-end.

namespace db {

class session::Sqlite3 : public ReferenceCountedSingleton <Sqlite3>
{
private:
  friend class ReferenceCountedSingleton <Sqlite3>;

  Sqlite3 () : ReferenceCountedSingleton (SingletonLifetime::persistAfterCreation)
	{
    int threadSafe = sqlite3_threadsafe ();
    if (threadSafe != 1)
      Throw (Error().fail (__FILE__, __LINE__, Error::assertFailed));

		int result = sqlite3_config (SQLITE_CONFIG_MULTITHREAD);
    if (result != SQLITE_OK)
      Throw (Error().fail (__FILE__, __LINE__, Error::assertFailed));

  	sqlite3_initialize ();
	}

	~Sqlite3()
	{
		sqlite3_shutdown ();
	}

  static Sqlite3* createInstance ()
  {
    return new Sqlite3;
  }
};

//------------------------------------------------------------------------------

session::session ()
  : prepare (this)
  , m_instance (Sqlite3::getInstance ())
  , m_bInTransaction (false)
  , m_connection (0)
{
}

session::session (const session& deferredClone)
  : prepare (this)
  , m_bInTransaction (false)
  , m_connection (0)
  , m_fileName (deferredClone.m_fileName)
  , m_connectString (deferredClone.m_connectString)
{
  // shouldn't be needed since deferredClone did it
  //Sqlite::initialize();
}

session::~session()
{
  close();
}

Error session::clone ()
{
  if (m_connection)
    Throw (Error ().fail (__FILE__, __LINE__)); // already open

  return open (m_fileName, m_connectString);
}

/*
static int infiniteBusyHandler (void* data, int tries)
{
  boost::this_thread::sleep (boost::posix_time::seconds(1));
  return 1; // try again
}
*/

Error session::open (String fileName, std::string options)
{
  Error err;

  // can't open twice
  if (m_connection)
    Throw (err.fail (__FILE__, __LINE__, Error::fileInUse));

  int mode = 0;
  int flags = 0;
  int timeout = 0;
  
  std::stringstream ssconn (options);

  while (!err && !ssconn.eof() && ssconn.str().find('=') >= 0)
  {
    std::string key, val;
    std::getline(ssconn, key, '=');
    std::getline(ssconn, val, '|');

    if ("timeout" == key)
    {
      if( "infinite" == val )
      {
        //timeout = -1;
        timeout = 0x7fffffff;
      }
      else
      {
        std::istringstream converter(val);
        converter >> timeout;

        if( timeout < 1 )
          timeout = 1;
      }
    }
    else if( "mode" == key )
    {
      if( ! ( mode & ( SQLITE_OPEN_READONLY | SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE ) ) )
      {
        if( "read" == val )
        {
          mode = SQLITE_OPEN_READONLY;
        }
        else if( "write" == val )
        {
          mode = SQLITE_OPEN_READWRITE;
        }
        else if( "create" == val )
        {
          mode = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
        }
        else
        {
          Throw (err.fail (__FILE__, __LINE__, Error::badParameter));
        }
      }
      else
      {
        // duplicate
        Throw (err.fail (__FILE__, __LINE__, Error::badParameter));
      }
    }
    else if( "cache" == key )
    {
      if( ! ( flags & ( SQLITE_OPEN_SHAREDCACHE | SQLITE_OPEN_PRIVATECACHE ) ) )
      {
        if( "shared" == val )
        {
          flags |= SQLITE_OPEN_SHAREDCACHE;
        }
        else if( "private" == val )
        {
          flags |= SQLITE_OPEN_PRIVATECACHE;
        }
        else
        {
          Throw (err.fail (__FILE__, __LINE__, Error::badParameter));
        }
      }
      else
      {
        // duplicate
        Throw (err.fail (__FILE__, __LINE__, Error::badParameter));
      }
    }
    else if( "threads" == key )
    {
      if( ! ( flags & ( SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_FULLMUTEX ) ) )
      {
        if( "single" == val )
        {
          flags |= SQLITE_OPEN_FULLMUTEX;
        }
        else if( "multi" == val )
        {
          flags |= SQLITE_OPEN_NOMUTEX;
        }
        else
        {
          Throw (err.fail (__FILE__, __LINE__, Error::badParameter));
        }
      }
      else
      {
        // duplicate
        Throw (err.fail (__FILE__, __LINE__, Error::badParameter));
      }
    }
    else
    {
      // unknown option
      Throw (err.fail (__FILE__, __LINE__, Error::badParameter));
    }
  }

  if (!err)
  {
    if( ! mode )
      mode = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;

    flags |= mode;

    err = detail::sqliteError (__FILE__, __LINE__,
      sqlite3_open_v2 (fileName.toUTF8(), &m_connection, flags, 0));

    if (!err)
    {
      if (timeout > 0)
      {
        err = detail::sqliteError (__FILE__, __LINE__,
          sqlite3_busy_timeout (m_connection, timeout));
      }
      /*
      else
      {
        err = detail::sqliteError (__FILE__, __LINE__,
          sqlite3_busy_handler(m_connection, infiniteBusyHandler, 0));
      }
      */
    }

    if (!err)
    {
      m_fileName = fileName;
      m_connectString = options;
    }

    if (err)
    {
      close ();
    }
  }

  return err;
}

void session::close()
{
  if (m_connection)
  {
    sqlite3_close (m_connection);
    m_connection = 0;
    m_fileName = String::empty;
    m_connectString = "";
  }
}

void session::begin()
{
  vfassert( !m_bInTransaction );
  m_bInTransaction = true;
  
  //Error error = hard_exec("BEGIN EXCLUSIVE");
  Error error = hard_exec("BEGIN");
  if (error)
    Throw (error);
}

Error session::commit()
{
  vfassert( m_bInTransaction );
  m_bInTransaction = false;
  return hard_exec("COMMIT");
}

void session::rollback()
{
  vfassert (m_bInTransaction);
  m_bInTransaction = false;
  Error error = hard_exec("ROLLBACK");
  if (error)
    Throw (error);
}

detail::once_type session::once (Error& error)
{
  return detail::once_type(this, error);
}

rowid session::last_insert_rowid()
{
  return sqlite3_last_insert_rowid( m_connection );
}

std::ostringstream& session::get_query_stream()
{
  return m_query_stream;
}

void session::log_query(std::string const& query)
{
  // TODO
}

void session::set_got_data(bool bGotData)
{
  m_bGotData = bGotData;
}

bool session::got_data() const
{
  return m_bGotData;
}

Error session::hard_exec (std::string const& query)
{
  Error error;
  sqlite3_stmt* stmt;
  char const* tail=0;

  int result = sqlite3_prepare_v2(
    m_connection,
    query.c_str(),
    static_cast<int>(query.size()),
    &stmt,
    &tail );

  if (result == SQLITE_OK)
  {
    result = sqlite3_step (stmt);

    sqlite3_finalize (stmt);
  }

  if (result != SQLITE_DONE)
    error = detail::sqliteError (__FILE__, __LINE__, result);

  return error;
}

}
