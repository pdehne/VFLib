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
