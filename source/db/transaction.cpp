// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#include "vf/vf_StandardHeader.h"

#include "sqlite/sqlite3.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_db.h"
#include "vf/db/transaction.h"

namespace db {

transaction::transaction(session& s)
: m_session(s), m_bHandled(false)
{
  m_session.begin();
}

transaction::~transaction()
{
  if( !m_bHandled )
    rollback();
}

Error transaction::commit()
{
  Error error;

  // already handled
  if( m_bHandled )
    Throw (Error().fail (__FILE__, __LINE__));

  // if commit() throws, m_bHandled will remain false
  // and the destructor will attempt a rollback.

  m_bHandled=true;
  
  error = m_session.commit();

  return error;
}

void transaction::rollback()
{
  // already handled
  if( m_bHandled )
    Throw (Error().fail (__FILE__, __LINE__));

  m_session.rollback();
  m_bHandled=true;
}

}

END_VF_NAMESPACE
