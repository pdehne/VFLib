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
