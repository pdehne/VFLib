// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.
// Based on ideas from the soci wrapper sqlite back-end.

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
