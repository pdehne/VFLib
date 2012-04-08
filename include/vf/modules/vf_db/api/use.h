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

#ifndef VF_DB_USE_VFHEADER
#define VF_DB_USE_VFHEADER

namespace db {

inline detail::use_type_ptr use_null()
{
  return detail::do_use_null();
}

template <typename T>
detail::use_type_ptr use(T& t)
{
  return detail::do_use(t,
                        typename detail::exchange_traits<T>::type_family());
}

template <typename T>
detail::use_type_ptr use(T const& t)
{
  return detail::do_use(t,
                        typename detail::exchange_traits<T>::type_family());
}

template <typename T>
detail::use_type_ptr use(T& t, indicator& ind)
{
  return detail::do_use(t, ind,
                        typename detail::exchange_traits<T>::type_family());
}

}

#endif