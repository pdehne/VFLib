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

#ifndef VF_DB_DETAIL_ONCE_TEMP_TYPE_VFHEADER
#define VF_DB_DETAIL_ONCE_TEMP_TYPE_VFHEADER

namespace db {

namespace detail {

class once_temp_type
{
public:
  once_temp_type(session& s, Error& error);
  once_temp_type(once_temp_type const& o);
  once_temp_type& operator=(once_temp_type const& o);
  ~once_temp_type();

  once_temp_type& operator,(into_type_ptr const& i);
  once_temp_type& operator,(use_type_ptr const& u);

  template<typename T>
  once_temp_type& operator<<(T const& t)
  {
    m_rcst->accumulate(t);
    return *this;
  }

private:
  ref_counted_statement* m_rcst;
};

class once_type
{
public:
  once_type (const once_type& other);
  once_type (session* s, Error& error);

  template<typename T>
  once_temp_type operator<<(T const& t)
  {
    once_temp_type o(m_session, m_error);
    o << t;
    return o;
  }

private:
  once_type& operator= (const once_type&);

  session& m_session;
  Error& m_error;
};

}

}

#endif
