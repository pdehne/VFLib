// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#ifndef __VF_DB_DETAIL_ONCE_TEMP_TYPE_VFHEADER__
#define __VF_DB_DETAIL_ONCE_TEMP_TYPE_VFHEADER__

#include "vf/db/detail/into_type.h"
#include "vf/db/detail/prepare_temp_type.h"
#include "vf/db/detail/ref_counted_statement.h"

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

#endif
