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

#ifndef VF_DB_DETAIL_INTO_TYPE_VFHEADER
#define VF_DB_DETAIL_INTO_TYPE_VFHEADER

namespace db {

namespace detail {

// base class for defining output data
class into_type_base
{
public:
  virtual ~into_type_base() {}
  virtual void bind (statement_imp& st, int& iCol)=0;
  virtual void do_into()=0;
};

typedef type_ptr<into_type_base> into_type_ptr;

// standard output types
class standard_into_type : public into_type_base
{
public:
  standard_into_type(void* data, exchange_type type)
    : m_data(data), m_type(type), m_ind(0) {}
  standard_into_type(void* data, exchange_type type, indicator& ind)
    : m_data(data), m_type(type), m_ind(&ind) {}
  virtual ~standard_into_type();
  virtual void bind(statement_imp& st, int& iCol );
  virtual void do_into();

private:
  virtual void convert_from_base() {}

private:
  void* m_data;
  exchange_type m_type;
  indicator* m_ind;
  statement_imp* m_st;
  int m_iCol;
};

// implementation for built-in types without conversions
template<typename T>
class into_type : public standard_into_type
{
public:
  into_type(T& t) : standard_into_type (&t,
    static_cast <exchange_type> (exchange_traits<T>::x_type)) {}

  into_type(T& t, indicator& ind) : standard_into_type(&t,
        static_cast<exchange_type>(exchange_traits<T>::x_type), ind) {}
};

template<typename T>
into_type_ptr do_into(T& t, basic_type_tag)
{
  return into_type_ptr(new into_type<T>(t));
}

template<typename T>
into_type_ptr do_into(T& t, indicator& ind, basic_type_tag)
{
  return into_type_ptr(new into_type<T>(t,ind));
}

}

}

#endif
