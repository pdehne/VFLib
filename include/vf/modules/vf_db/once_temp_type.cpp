// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#include "vf/vf_StandardHeader.h"

#include "sqlite/sqlite3.h"

BEGIN_VF_NAMESPACE

#include "vf/modules/vf_db/vf_db.h"
#include "vf/modules/vf_db/detail/once_temp_type.h"

namespace db {

namespace detail {

once_type::once_type (const once_type& other)
  : m_session (other.m_session)
  , m_error (other.m_error)
{
}

once_temp_type::once_temp_type(session& s, Error& error)
  : m_rcst (new ref_counted_statement(s, error))
{
  // new query
  s.get_query_stream().str("");
}

once_temp_type::once_temp_type(once_temp_type const& o)
  : m_rcst (o.m_rcst)
{
  m_rcst->addref();
}

once_temp_type& once_temp_type::operator= (once_temp_type const& o)
{
  o.m_rcst->addref();
  m_rcst->release();
  m_rcst = o.m_rcst;
  return *this;
}

once_temp_type::~once_temp_type()
{
  m_rcst->release();
}

once_temp_type& once_temp_type::operator,(into_type_ptr const& i)
{
  m_rcst->exchange(i);
  return *this;
}

once_temp_type& once_temp_type::operator,(use_type_ptr const& u)
{
  m_rcst->exchange(u);
  return *this;
}

//------------------------------------------------------------------------------

once_type::once_type(session* s, Error& error)
: m_session (*s)
, m_error (error)
{
}

}

}

END_VF_NAMESPACE
