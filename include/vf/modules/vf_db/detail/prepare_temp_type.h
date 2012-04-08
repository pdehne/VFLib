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

#ifndef VF_DB_DETAIL_PREPARE_TEMP_TYPE_VFHEADER
#define VF_DB_DETAIL_PREPARE_TEMP_TYPE_VFHEADER

namespace db {

namespace detail {

// lightweight copyable wrapper around the prepare info
class prepare_temp_type
{
public:
  prepare_temp_type(session& s);
  prepare_temp_type(prepare_temp_type const& o);
  prepare_temp_type& operator=(prepare_temp_type const& o);

  ~prepare_temp_type();

  template<typename T>
  prepare_temp_type& operator<<(T const& t)
  {
    m_rcpi->accumulate(t);
    return *this;
  }

  prepare_temp_type& operator,(into_type_ptr const& i);
  prepare_temp_type& operator,(use_type_ptr const& u);

  ref_counted_prepare_info& get_prepare_info() const
  {
    return *m_rcpi;
  }

private:
  ref_counted_prepare_info* m_rcpi;
};

class prepare_type
{
public:
  prepare_type() : m_session(NULL) {}
  prepare_type(session* s) : m_session(s) {}

  template<typename T> prepare_temp_type operator<<(T const& t)
  {
    prepare_temp_type p(*m_session);
    p << t;
    return p;
  }

private:
  session* m_session;
};

}

}

#endif
