// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#include "vf/vf_StandardHeader.h"

#include "sqlite/sqlite3.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_db.h"
#include "vf/db/detail/statement_imp.h"

namespace db {

statement::statement(session& s)
  : m_imp(new detail::statement_imp(s))
{
}

statement::statement(detail::prepare_temp_type const& prep )
{
  m_imp.reset(new detail::statement_imp(prep));
}

statement::~statement()
{
}

statement::statement(statement const& o)
  : m_imp(o.m_imp)
{
}

void statement::operator=(statement const& o)
{
  m_imp = o.m_imp;
}

void statement::exchange(detail::into_type_ptr const& i)
{
  m_imp->exchange(i);
}

void statement::exchange(detail::use_type_ptr const& u)
{
  m_imp->exchange(u);
}

void statement::prepare(std::string const& query, bool bRepeatable)
{
  m_imp->prepare(query, bRepeatable);
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

END_VF_NAMESPACE


}
