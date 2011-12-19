// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#include "vf/vf_StandardHeader.h"

#include "sqlite/sqlite3.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_db.h"
#include "vf/db/detail/prepare_temp_type.h"

namespace db {

namespace detail {

prepare_temp_type::prepare_temp_type(session& s)
  : m_rcpi(new ref_counted_prepare_info(s))
{
  // new query
  s.get_query_stream().str("");
}

prepare_temp_type::prepare_temp_type(prepare_temp_type const& o)
  : m_rcpi(o.m_rcpi)
{
  m_rcpi->addref();
}

prepare_temp_type& prepare_temp_type::operator=(prepare_temp_type const& o)
{
  o.m_rcpi->addref();
  m_rcpi->release();
  m_rcpi = o.m_rcpi;
  return *this;
}

prepare_temp_type::~prepare_temp_type()
{
  m_rcpi->release();
}

prepare_temp_type& prepare_temp_type::operator,(into_type_ptr const& i)
{
  m_rcpi->exchange(i);
  return *this;
}

prepare_temp_type& prepare_temp_type::operator,(use_type_ptr const& u)
{
  m_rcpi->exchange(u);
  return *this;
}

}

}

END_VF_NAMESPACE
