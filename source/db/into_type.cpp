// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#include "vf/vf_StandardHeader.h"

#include "sqlite/sqlite3.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_db.h"
#include "vf/db/detail/into_type.h"
#include "vf/db/detail/statement_imp.h"

namespace db {

namespace detail {

namespace {

template <typename T>
inline T& as(void* data)
{
  return *static_cast<T*>(data);
}

template <typename T>
static void integer_into(void* data, sqlite3_int64 v)
{
  *(static_cast<T*>(data)) = T(v);
}

}

standard_into_type::~standard_into_type()
{
}

void standard_into_type::bind (statement_imp& st, int& iCol)
{
  m_st = &st;
  m_iCol = iCol++;
}

void standard_into_type::do_into()
{
  int colType = sqlite3_column_type( m_st->m_stmt, m_iCol );

  if (colType == SQLITE_NULL)
  {
    // null encountered with no indicator
    if (!m_ind)
      Throw (Error().fail (__FILE__, __LINE__));

    *m_ind = i_null;
  }
  else
  {
    if (m_ind)
      *m_ind = i_ok;

    switch (colType)
    {
    case SQLITE_INTEGER:
      {
        sqlite3_int64 v = sqlite3_column_int64 (m_st->m_stmt, m_iCol);
        switch (m_type)
        {
        case x_bool:
          as<bool>(m_data) = v != 0;
          break;

        case x_char:  integer_into<char>(m_data,v); break;
        case x_short: integer_into<short>(m_data,v); break; 
        case x_int:   integer_into<int>(m_data,v); break;
        case x_long:  integer_into<long>(m_data,v); break;
        case x_int64: integer_into<int64>(m_data,v); break;

        case x_uchar:  integer_into<unsigned char>(m_data,v); break;
        case x_ushort: integer_into<unsigned short>(m_data,v); break; 
        case x_uint:   integer_into<unsigned int>(m_data,v); break;
        case x_ulong:  integer_into<unsigned long>(m_data,v); break;
        case x_uint64: integer_into<uint64>(m_data,v); break;

        case x_juceString:
          as <VF_JUCE::String> (m_data) = String (v);
          break;

        default:
          Throw (Error().fail (__FILE__, __LINE__));
        }
      }
      break;

    case SQLITE_FLOAT:
      {
        double v = sqlite3_column_double (m_st->m_stmt, m_iCol);

        switch (m_type)
        {
        case x_float:
          as<float>(m_data) = static_cast<float>(v);
          break;

        case x_double:
          as<double>(m_data) = v;
          break;

        case x_juceString:
          as <VF_JUCE::String> (m_data) = String (v);
          break;

        default:
          Throw (Error().fail (__FILE__, __LINE__));
        };
      }
      break;

    case SQLITE_TEXT:
      {
        switch (m_type)
        {
        case x_stdstring:
          {
            // excludes terminator
            int bytes = sqlite3_column_bytes( m_st->m_stmt, m_iCol );
            unsigned char const* v = sqlite3_column_text( m_st->m_stmt, m_iCol );
            std::string& result = as<std::string>(m_data);
            result.assign( reinterpret_cast<char const*>(v), bytes );
          }
          break;

        case x_stdwstring:
          Throw (Error().fail (__FILE__, __LINE__));
          break;

        case x_juceString:
          {
            // excludes terminator
            int bytes = sqlite3_column_bytes (m_st->m_stmt, m_iCol);

            const VF_JUCE::CharPointer_UTF8::CharType* c = reinterpret_cast
              <const VF_JUCE::CharPointer_UTF8::CharType*>
              (sqlite3_column_text (m_st->m_stmt, m_iCol));

            VF_JUCE::String& s = as <VF_JUCE::String> (m_data);
            s = VF_JUCE::String (VF_JUCE::CharPointer_UTF8 (c), VF_JUCE::CharPointer_UTF8 (c + bytes));
          }
          break;

        default:
          {
            sqlite3_int64 v = sqlite3_column_int64( m_st->m_stmt, m_iCol );
            switch( m_type )
            {
            case x_bool:
              as<bool>(m_data) = v != 0;
              break;

            case x_char:  integer_into<char>(m_data,v); break;
            case x_short: integer_into<short>(m_data,v); break; 
            case x_int:   integer_into<int>(m_data,v); break;
            case x_long:  integer_into<long>(m_data,v); break;
            case x_int64: integer_into<int64>(m_data,v); break;

            case x_uchar:  integer_into<unsigned char>(m_data,v); break;
            case x_ushort: integer_into<unsigned short>(m_data,v); break; 
            case x_uint:   integer_into<unsigned int>(m_data,v); break;
            case x_ulong:  integer_into<unsigned long>(m_data,v); break;
            case x_uint64: integer_into<uint64>(m_data,v); break;

            default:
              Throw (Error().fail (__FILE__, __LINE__));
            }

          }
          break;
        };
      }
      break;

    case SQLITE_BLOB:
       Throw (Error().fail (__FILE__, __LINE__));

    default:
      Throw (Error().fail (__FILE__, __LINE__));
    };
  }

  convert_from_base();
}

}

}

END_VF_NAMESPACE

