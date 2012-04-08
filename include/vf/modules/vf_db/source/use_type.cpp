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

standard_use_type::~standard_use_type()
{
}

void standard_use_type::bind(statement_imp& st, int& iParam)
{
  m_st = &st;
  m_iParam = iParam++;
}

namespace {

template<typename T>
inline T const& as (void const* v)
{
  return *static_cast <T const*> (v);
}

template<typename T, typename L>
inline T const& as(void const* v)
{
  T const& val = *static_cast <T const*> (v);

  if (val > T (std::numeric_limits<L>::max()))
    Throw (Error().fail (__FILE__, __LINE__));

  return val;
}

}

void standard_use_type::do_use()
{
  int result=SQLITE_OK;

  convert_to_base();

  switch (m_type)
  {
  case x_null:
    result = sqlite3_bind_null (m_st->m_stmt, m_iParam);
    break;

  case x_bool:
    result = sqlite3_bind_int (m_st->m_stmt, m_iParam, as <bool> (m_data)?1:0 );
    break;

  case x_char:
    result = sqlite3_bind_int (m_st->m_stmt, m_iParam, as <char> (m_data));
    break;

  case x_short:
    result = sqlite3_bind_int (m_st->m_stmt, m_iParam, as <short> (m_data));
    break;

  case x_int:
    result = sqlite3_bind_int (m_st->m_stmt, m_iParam, as <int> (m_data));
    break;

  case x_long:
    result = sqlite3_bind_int (m_st->m_stmt, m_iParam, as <long> (m_data));
    break;

  case x_int64:
    result = sqlite3_bind_int64 (m_st->m_stmt, m_iParam, as <int64> (m_data));
    break;

  case x_uchar:
    result = sqlite3_bind_int (m_st->m_stmt, m_iParam, as <unsigned char> (m_data));
    break;

  case x_ushort:
    result = sqlite3_bind_int (m_st->m_stmt, m_iParam, as <unsigned short> (m_data));
    break;

  case x_uint:
    result = sqlite3_bind_int64 (m_st->m_stmt, m_iParam, as <unsigned int> (m_data));
    break;

  case x_ulong:
    result = sqlite3_bind_int64 (m_st->m_stmt, m_iParam, as <unsigned long> (m_data));
    break;

  case x_uint64:
    result = sqlite3_bind_int64 (m_st->m_stmt, m_iParam, as <sqlite3_uint64, sqlite3_int64> (m_data));
    break;

  case x_float:
    result = sqlite3_bind_double (m_st->m_stmt, m_iParam, as <float> (m_data));
    break;

  case x_double:
    result = sqlite3_bind_double (m_st->m_stmt, m_iParam, as <double> (m_data));
    break;

  case x_cstring:
    result = sqlite3_bind_text (m_st->m_stmt, m_iParam, as <char*> (m_data), -1, SQLITE_STATIC);
    break;

  case x_cwstring:
    result = sqlite3_bind_text16 (m_st->m_stmt, m_iParam, as <wchar_t*> (m_data), -1, SQLITE_STATIC);
    break;

  case x_stdstring:
    {
      std::string const& s = as <std::string> (m_data);
      result = sqlite3_bind_text (m_st->m_stmt,
                                  m_iParam,
                                  s.c_str(),
                                  s.size() * sizeof(s[0]), 
                                  SQLITE_STATIC);
    }
    break;

  case x_stdwstring:
    {
      std::wstring const& s = as <std::wstring> (m_data);
      result = sqlite3_bind_text16 (m_st->m_stmt,
                                    m_iParam,
                                    s.c_str(),
                                    s.size() * sizeof(s[0]),
                                    SQLITE_STATIC);
    }
    break;

  case x_juceString:
    {
      VF_JUCE::String const& s = as <VF_JUCE::String> (m_data);
      result = sqlite3_bind_text (m_st->m_stmt,
                                  m_iParam,
                                  s.toUTF8(),
                                  -1,
                                  SQLITE_STATIC);
    }
    break;

  case x_stdtm:
  case x_blob:
  default:
    Throw (Error().fail (__FILE__, __LINE__, Error::badParameter));
  }

  if (result != SQLITE_OK)
    Throw (detail::sqliteError(__FILE__, __LINE__, result));
}

void standard_use_type::post_use()
{
  convert_from_base();
}

void standard_use_type::clean_up()
{
}

}

}
