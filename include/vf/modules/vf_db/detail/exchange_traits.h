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

#ifndef VF_DB_DETAIL_EXCHANGE_TRAITS_VFHEADER
#define VF_DB_DETAIL_EXCHANGE_TRAITS_VFHEADER

namespace db {

namespace detail {

struct basic_type_tag {};
struct user_type_tag {};

// converts a type to exchange_type
template<typename T>
struct exchange_traits
{
  typedef user_type_tag type_family;

  enum
  {
    x_type = exchange_traits<typename type_conversion<T>::base_type>::x_type
  };
};

template<> struct exchange_traits<bool>
{
  typedef basic_type_tag type_family;
  enum { x_type = x_bool };
};

template<> struct exchange_traits<char>
{
  typedef basic_type_tag type_family;
  enum { x_type = x_char };
};

template<> struct exchange_traits<short>
{
  typedef basic_type_tag type_family;
  enum { x_type = x_short };
};

template<> struct exchange_traits<int>
{
  typedef basic_type_tag type_family;
  enum { x_type = x_int };
};

template<> struct exchange_traits<long>
{
  typedef basic_type_tag type_family;
  enum { x_type = x_long };
};

//template<> struct exchange_traits<boost::int64>
//template<> struct exchange_traits<sqlite3_int64>
template<> struct exchange_traits<int64>
{
  typedef basic_type_tag type_family;
  enum { x_type = x_int64 };
};

template<> struct exchange_traits<unsigned char>
{
  typedef basic_type_tag type_family;
  enum { x_type = x_uchar };
};

template<> struct exchange_traits<unsigned short>
{
  typedef basic_type_tag type_family;
  enum { x_type = x_ushort };
};

template<> struct exchange_traits<unsigned int>
{
  typedef basic_type_tag type_family;
  enum { x_type = x_uint };
};

template<> struct exchange_traits<unsigned long>
{
  typedef basic_type_tag type_family;
  enum { x_type = x_ulong };
};

//template<> struct exchange_traits<boost::uint64>
//template<> struct exchange_traits<sqlite3_uint64>
template<> struct exchange_traits<uint64>
{
  typedef basic_type_tag type_family;
  enum { x_type = x_uint64 };
};

template<> struct exchange_traits<float>
{
  typedef basic_type_tag type_family;
  enum { x_type = x_float };
};

template<> struct exchange_traits<double>
{
  typedef basic_type_tag type_family;
  enum { x_type = x_double };
};

template<> struct exchange_traits<char*>
{
  typedef basic_type_tag type_family;
  enum { x_type = x_cstring };
};

template<> struct exchange_traits<char const*>
{
  typedef basic_type_tag type_family;
  enum { x_type = x_cstring };
};

template<> struct exchange_traits<wchar_t*>
{
  typedef basic_type_tag type_family;
  enum { x_type = x_cwstring };
};

template<> struct exchange_traits<wchar_t const*>
{
  typedef basic_type_tag type_family;
  enum { x_type = x_cwstring };
};

template<> struct exchange_traits<std::string>
{
  typedef basic_type_tag type_family;
  enum { x_type = x_stdstring };
};

template<> struct exchange_traits<std::wstring>
{
  typedef basic_type_tag type_family;
  enum { x_type = x_stdwstring };
};

template<> struct exchange_traits<VF_JUCE::String>
{
  typedef basic_type_tag type_family;
  enum { x_type = x_juceString };
};

template<> struct exchange_traits<std::tm>
{
  typedef basic_type_tag type_family;
  enum { x_type = x_stdtm };
};

template<> struct exchange_traits<blob>
{
  typedef basic_type_tag type_family;
  enum { x_type = x_blob };
};

}

}

#endif
