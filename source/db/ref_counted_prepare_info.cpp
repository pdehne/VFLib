// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#include "vf/vf_StandardHeader.h"

#include "sqlite/sqlite3.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_db.h"
#include "vf/db/detail/ref_counted_prepare_info.h"

namespace db {

namespace detail {

ref_counted_prepare_info::ref_counted_prepare_info (session& s)
: ref_counted_statement_base (s)
{
}

void ref_counted_prepare_info::exchange(into_type_ptr const& i)
{
  m_intos.push_back(i.get());
  i.release();
}

void ref_counted_prepare_info::exchange(use_type_ptr const& u)
{
  m_uses.push_back(u.get());
  u.release();
}

void ref_counted_prepare_info::final_action()
{
  for (std::size_t i = m_intos.size(); i > 0; --i)
  {
    delete m_intos[i - 1];
    m_intos.resize(i - 1);
  }

  for (std::size_t i = m_uses.size(); i > 0; --i)
  {
    delete m_uses[i - 1];
    m_uses.resize(i - 1);
  }
}

std::string ref_counted_prepare_info::get_query() const
{
  return m_session.get_query_stream().str();
}

}

}

END_VF_NAMESPACE
