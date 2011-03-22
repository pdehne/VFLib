// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#include "vf/vf_StandardHeader.h"

#include "sqlite/sqlite3.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_db.h"

namespace db {

#include "vf/db/detail/statement_imp.h"
#include "error_codes.h"

namespace detail {

/*

Running a new prepared statement:

sqlite3_prepare()   // create prepared statement
sqlite3_bind()      // define input variables
sqlite3_step()      // retrieve next row
sqlite3_column()    // extract output value
sqlite3_finalize()  // destroy prepared statement

Re-using an existing prepared statement

sqlite3_reset()     // reset the statement (input bindings are not cleared)
sqlite3_clear_bindings()  // set all input variables to NULL
  AND/OR
sqlite3_bind()      // define input variables
sqlite3_step()      // retrieve next row
sqlite3_column()    // extract output value

*/

statement_imp::statement_imp (session& s)
  : m_session (s)
  , m_stmt (0)
  , m_bReady (false)
  , m_bGotData (false)
  , m_last_insert_rowid (0)
{
}

statement_imp::statement_imp (prepare_temp_type const& prep)
  : m_session (prep.get_prepare_info().m_session)
  , m_stmt (0)
  , m_bReady (false)
  , m_bGotData (false)
{
  ref_counted_prepare_info& rcpi = prep.get_prepare_info();

  m_intos.swap (rcpi.m_intos);
  m_uses.swap (rcpi.m_uses);

  prepare (rcpi.get_query());
}

statement_imp::~statement_imp()
{
  clean_up();
}

void statement_imp::erase_bindings()
{
  // delete intos
  for (std::size_t i = m_intos.size(); i != 0; --i)
  {
    into_type_base* p = m_intos[i-1];
    delete p;
    m_intos.resize (i-1);
  }

  // delete uses
  for (std::size_t i = m_uses.size(); i != 0; --i)
  {
    use_type_base* p = m_uses[i-1];
    p->clean_up();
    delete p;
    m_uses.resize (i-1);
  }
}

void statement_imp::exchange (detail::into_type_ptr const& i)
{
  m_intos.push_back (i.get());
  i.release();
}

void statement_imp::exchange (detail::use_type_ptr const& u)
{
  m_uses.push_back(u.get());
  u.release();
}

void statement_imp::clean_up()
{
  erase_bindings();
  release_resources();
}

void statement_imp::prepare (std::string const& query, bool bRepeatable)
{
  m_query = query;
  m_session.log_query(query);
  m_last_insert_rowid = 0;

  release_resources();

  char const* tail = 0;
  int result = sqlite3_prepare_v2(
    m_session.get_connection(),
    query.c_str(),
    static_cast<int>(query.size()),
    &m_stmt,
    &tail );

  if (result == SQLITE_OK)
  {
    m_bReady = true;
  }
  else
  {
    throw detail::sqliteError (__FILE__, __LINE__, result);
  }
}

Error statement_imp::execute()
{
  Error error;

  if( !m_stmt )
    throw Error().fail (__FILE__, __LINE__, Error::badParameter);

  // ???
  m_bGotData = false;
  m_session.set_got_data (m_bGotData);

  // binds

  int iCol = 0;
  BOOST_FOREACH(detail::into_type_base* i, m_intos)
    i->bind(*this, iCol);

  int iParam = 1;
  BOOST_FOREACH(detail::use_type_base* u, m_uses)
    u->bind(*this, iParam);

  // reset
  error = detail::sqliteError (__FILE__, __LINE__, sqlite3_reset (m_stmt));

  if (!error)
  {
    // set input variables
    do_uses();

    m_bReady = true;
    m_bFirstTime = true;
  }

  return error;
}

bool statement_imp::fetch (Error& error)
{
  int result = sqlite3_step (m_stmt);

  if (result == SQLITE_ROW ||
      result == SQLITE_DONE)
  {
    if (m_bFirstTime)
    {
      m_last_insert_rowid = m_session.last_insert_rowid();
      m_bFirstTime = false;
    }

    if (result == SQLITE_ROW)
    {
      m_bGotData = true;
      m_session.set_got_data (m_bGotData);

      do_intos();
    }
    else
    {
      m_bGotData = false;
      m_session.set_got_data (m_bGotData);

      if (result == SQLITE_DONE)
      {
        m_bReady = false;
      }
    }
  }
  else if (result != SQLITE_OK)
  {
    m_bGotData = false;
    error = detail::sqliteError (__FILE__, __LINE__, result);
  }
  else
  {
    // should never get SQLITE_OK here
    throw Error().fail (__FILE__, __LINE__, Error::assertFailed);
  }

  return m_bGotData;
}

bool statement_imp::got_data() const
{
  return m_bGotData;
}

void statement_imp::do_intos()
{
  BOOST_FOREACH( into_type_base* i, m_intos )
    i->do_into();
}

void statement_imp::do_uses()
{
  vf::Error error = detail::sqliteError(__FILE__, __LINE__,
    sqlite3_clear_bindings (m_stmt));
 
 if (error)
   throw error;

  BOOST_FOREACH(detail::use_type_base* u, m_uses)
    u->do_use();
}

void statement_imp::post_use()
{
  // reverse order
  BOOST_REVERSE_FOREACH(detail::use_type_base* u, m_uses)
    u->post_use();
}

void statement_imp::release_resources()
{
  if( m_stmt )
  {
    sqlite3_finalize( m_stmt );
    m_stmt = 0;
  }

  m_bReady = false;
  m_bGotData = false;
}

rowid statement_imp::last_insert_rowid()
{
  return m_last_insert_rowid;
}

}

}

END_VF_NAMESPACE