// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#ifndef __VF_DB_DETAIL_PREPARE_TEMP_TYPE_VFHEADER__
#define __VF_DB_DETAIL_PREPARE_TEMP_TYPE_VFHEADER__

#include "vf/modules/vf_db/detail/ref_counted_prepare_info.h"

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
