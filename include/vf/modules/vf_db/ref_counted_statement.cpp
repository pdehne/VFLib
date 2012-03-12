// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.
// Based on ideas from the soci wrapper sqlite back-end.

namespace db {

namespace detail {

ref_counted_statement_base::ref_counted_statement_base (session& s)
  : m_refs (1)
  , m_session (s)
{
}

ref_counted_statement_base ::~ref_counted_statement_base ()
{
}

void ref_counted_statement_base::addref()
{
  ++m_refs;
}

void ref_counted_statement_base::release()
{
  if (--m_refs == 0)
  {
    final_action();
    delete this;
  }
}

std::ostringstream& ref_counted_statement_base::get_query_stream()
{
  return m_session.get_query_stream();
}

//------------------------------------------------------------------------------

ref_counted_statement::ref_counted_statement(session& s, Error& error)
  : ref_counted_statement_base(s)
  , m_error (error)
  , m_st(s)
{
}

void ref_counted_statement::exchange (into_type_ptr const& i)
{
  m_st.exchange(i);
}

void ref_counted_statement::exchange(use_type_ptr const& u)
{
  m_st.exchange(u);
}

void ref_counted_statement::final_action()
{
  m_st.prepare (m_session.get_query_stream().str(), false);

  Error error;
  m_st.execute_and_fetch (error);
  m_error = error;

  m_st.clean_up();
}

}

}
