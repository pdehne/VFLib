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

#ifndef VF_DB_DETAIL_USE_TYPE_VFHEADER
#define VF_DB_DETAIL_USE_TYPE_VFHEADER

namespace db {

namespace detail {

// base class for defining input data
class use_type_base
{
public:
  virtual ~use_type_base () {}
  virtual void bind (statement_imp& st, int& iParam) = 0;
  virtual void do_use () = 0;
  virtual void post_use () = 0;
  virtual void clean_up () = 0;
};

typedef type_ptr<use_type_base> use_type_ptr;

// handles builtin types
class standard_use_type : public use_type_base
{
public:
  standard_use_type(void* data, exchange_type type, bool bReadOnly)
    : m_data(data), m_type(type), m_ind(0), m_bReadOnly(bReadOnly) {}
  standard_use_type(void* data, exchange_type type, indicator& ind, bool bReadOnly)
    : m_data(data), m_type(type), m_ind(&ind), m_bReadOnly(bReadOnly) {}
  virtual ~standard_use_type();
  virtual void bind(statement_imp& st, int& iParam);
  virtual void do_use();
  virtual void post_use();
  virtual void clean_up();

  virtual void convert_to_base() {}
  virtual void convert_from_base() {}

private:
  void clean_up_backend();

private:
  void* m_data;
  exchange_type m_type;
  indicator* m_ind;
  bool m_bReadOnly;
  statement_imp* m_st;
  int m_iParam;
};

// worker for use
template<typename T>
class use_type : public standard_use_type
{
public:
  use_type(T& t)
    : standard_use_type(&t, static_cast<exchange_type>
                        (exchange_traits<T>::x_type), false) {}

  use_type(T const& t)
    : standard_use_type(const_cast<T*>(&t), static_cast<exchange_type>
                        (exchange_traits<T>::x_type), true) {}

  use_type(T& t, indicator& ind)
    : standard_use_type(&t, static_cast<exchange_type>
                        (exchange_traits<T>::x_type), ind, false) {}

  use_type(T const& t, indicator& ind)
    : standard_use_type(const_cast<T*>(&t), static_cast<exchange_type>
                        (exchange_traits<T>::x_type), ind, true) {}
};

// helpers for builtins
inline use_type_ptr do_use_null()
{
  return use_type_ptr(new standard_use_type(0,x_null,true));
}

template<typename T>
use_type_ptr do_use(T& t, basic_type_tag)
{
  return use_type_ptr(new use_type<T>(t));
}

template<typename T>
use_type_ptr do_use(T const& t, basic_type_tag)
{
  return use_type_ptr(new use_type<T>(t));
}

template<typename T>
use_type_ptr do_use(T& t, indicator& ind, basic_type_tag)
{
  return use_type_ptr(new use_type<T>(t, ind));
}

template<typename T>
use_type_ptr do_use(T const& t, indicator& ind, basic_type_tag)
{
  return use_type_ptr(new use_type<T>(t, ind));
}

}

}

#endif
