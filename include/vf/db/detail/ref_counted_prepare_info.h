// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#ifndef __VF_DB_DETAIL_REF_COUNTED_PREPARE_INFO_VFHEADER__
#define __VF_DB_DETAIL_REF_COUNTED_PREPARE_INFO_VFHEADER__

#include "vf/db/detail/ref_counted_statement.h"

namespace db {

namespace detail {

// helper that collects statement text and bindings to create the statement
class ref_counted_prepare_info : public ref_counted_statement_base
{
public:
  ref_counted_prepare_info (session& s);
  void exchange (into_type_ptr const& i);
  void exchange (use_type_ptr const& u);
  virtual void final_action();

public:
  std::string get_query() const;

  std::vector <into_type_base*> m_intos;
  std::vector <use_type_base*> m_uses;
};

}

}

#endif
