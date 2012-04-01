// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.
// Based on ideas from the soci wrapper sqlite back-end.

namespace db {

statement::statement (session& s)
  : m_imp (new detail::statement_imp(s))
{
}

statement::statement (detail::prepare_temp_type const& prep)
  : m_imp (new detail::statement_imp(prep))
{
}

statement::~statement()
{
}

statement::statement (statement const& o)
  : m_imp (o.m_imp)
{
}

void statement::operator= (statement const& o)
{
  m_imp = o.m_imp;
}

void statement::exchange (detail::into_type_ptr const& i)
{
  m_imp->exchange (i);
}

void statement::exchange (detail::use_type_ptr const& u)
{
  m_imp->exchange (u);
}

void statement::prepare(std::string const& query, bool bRepeatable)
{
  m_imp->prepare (query, bRepeatable);
}

Error statement::execute ()
{
  return m_imp->execute();
}

bool statement::execute_and_fetch (Error& error)
{
  bool gotData = false;

  error = m_imp->execute();

  if (!error)
    gotData = fetch (error);

  return gotData;
}

bool statement::fetch (Error& error)
{
  return m_imp->fetch (error);
}

bool statement::got_data() const
{
  return m_imp->got_data();
}

rowid statement::last_insert_rowid()
{
  return m_imp->last_insert_rowid();
}

void statement::clean_up()
{
  m_imp->clean_up();
}

}
