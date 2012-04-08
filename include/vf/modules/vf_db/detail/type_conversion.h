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

#ifndef VF_DB_DETAIL_TYPE_CONVERSION_VFHEADER
#define VF_DB_DETAIL_TYPE_CONVERSION_VFHEADER

namespace db {

namespace detail {

// used to order into and uses that need conversions
template<typename T>
struct base_value_holder
{
  typename type_conversion<T>::base_type m_value;
};

// create into_type from type_conversion
template <typename T>
class conversion_into_type
  : private base_value_holder<T>
  , public into_type<typename type_conversion<T>::base_type>
  , vf::Uncopyable
{
public:
  typedef typename type_conversion<T>::base_type BASE_TYPE;

  conversion_into_type(T& value)
    : into_type<BASE_TYPE>(detail::base_value_holder<T>::m_value, m_own_ind)
    , m_value(value), m_ind(m_own_ind) {}

  conversion_into_type(T& value, indicator& ind)
    : into_type<BASE_TYPE>(detail::base_value_holder<T>::m_value, ind)
    , m_value(value), m_ind(ind) {}

private:
  void convert_from_base()
  {
    type_conversion<T>::from_base(
      detail::base_value_holder<T>::m_value, m_ind, m_value);
  }

  T& m_value;

  indicator m_own_ind;
  indicator& m_ind;
};

// used to create a use_type from a type_conversion
template<typename T>
class conversion_use_type
  : private base_value_holder<T>
  , public use_type<typename type_conversion<T>::base_type>
{
public:
  typedef typename type_conversion<T>::base_type BASE_TYPE;

  conversion_use_type(T& value)
    : use_type<BASE_TYPE>(detail::base_value_holder<T>::m_value, m_own_ind)
    , m_value(value), m_ind(m_own_ind), m_bReadOnly(false) {}

  conversion_use_type(T const& value)
    : use_type<BASE_TYPE>(detail::base_value_holder<T>::m_value, m_own_ind)
    , m_value(const_cast<T&>(value)), m_ind(m_own_ind), m_bReadOnly(true) {}

  conversion_use_type(T& value, indicator& ind)
    : use_type<BASE_TYPE>(detail::base_value_holder<T>::m_value, ind)
    , m_value(value), m_ind(ind), m_bReadOnly(false) {}

  conversion_use_type(T const& value, indicator& ind)
    : use_type<BASE_TYPE>(detail::base_value_holder<T>::m_value, ind)
    , m_value(const_cast<T&>(value)), m_ind(ind), m_bReadOnly(true) {}

  void convert_from_base()
  {
    if( !m_bReadOnly )
    {
      type_conversion<T>::from_base(
        detail::base_value_holder<T>::m_value, m_ind, m_value);
    }
  }

  void convert_to_base()
  {
    type_conversion<T>::to_base(m_value,
                                detail::base_value_holder<T>::m_value, m_ind);
  }

private:
  T& m_value;
  indicator m_own_ind;
  indicator& m_ind;
  bool m_bReadOnly;

  JUCE_DECLARE_NON_COPYABLE (conversion_use_type);
};

template <typename T>
into_type_ptr do_into(T& t, user_type_tag)
{
  return into_type_ptr(new conversion_into_type<T>(t));
}

template <typename T>
into_type_ptr do_into(T& t, indicator& ind, user_type_tag)
{
  return into_type_ptr(new conversion_into_type<T>(t, ind));
}

template <typename T>
use_type_ptr do_use(T& t, user_type_tag)
{
  return use_type_ptr(new conversion_use_type<T>(t));
}

template <typename T>
use_type_ptr do_use(T const& t, user_type_tag)
{
  return use_type_ptr(new conversion_use_type<T>(t));
}

template <typename T>
use_type_ptr do_use(T& t, indicator& ind, user_type_tag)
{
  return use_type_ptr(new conversion_use_type<T>(t, ind));
}

template <typename T>
use_type_ptr do_use(T const& t, indicator& ind, user_type_tag)
{
  return use_type_ptr(new conversion_use_type<T>(t, ind));
}

}

}

#endif
