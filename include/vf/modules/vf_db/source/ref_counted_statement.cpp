// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file BOOST_LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// This file incorporates work covered by the following copyright
// and permission notice:
//
//   Copyright (C) 2004-2008 Maciej Sobczak, Stephen Hutton,
//   Mateusz Loskot, Pawel Aleksander Fedorynski, David Courtney,
//   Rafal Bobrowski, Julian Taylor, Henning Basold, Ilia Barahovski,
//   Denis Arnaud, Daniel Lidström, Matthieu Kermagoret, Artyom Beilis,
//   Cory Bennett, Chris Weed, Michael Davidsaver, Jakub Stachowski,
//   Alex Ott, Rainer Bauer, Martin Muenstermann, Philip Pemberton,
//   Eli Green, Frederic Chateau, Artyom Tonkikh, Roger Orr,
//   Robert Massaioli, Sergey Nikulov, Shridhar Daithankar,
//   Sören Meyer-Eppler, Mario Valesco
//
//   Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file BOOST_LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//

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
