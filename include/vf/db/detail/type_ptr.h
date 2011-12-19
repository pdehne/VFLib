// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#ifndef __VF_DB_DETAIL_TYPE_PTR_VFHEADER__
#define __VF_DB_DETAIL_TYPE_PTR_VFHEADER__

namespace db {

namespace detail {

// helper for managing the lifetime of into and use objects
template <typename T>
class type_ptr
{
public:
  type_ptr(T* p) : m_p(p) { }
  ~type_ptr()
  {
    delete m_p;
  }

  T* get() const
  {
    return m_p;
  }
  void release() const
  {
    m_p = 0;
  }

private:
  mutable T* m_p;
};

}

}

#endif
