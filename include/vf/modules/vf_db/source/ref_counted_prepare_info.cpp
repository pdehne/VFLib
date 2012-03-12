// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.
// Based on ideas from the soci wrapper sqlite back-end.

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
