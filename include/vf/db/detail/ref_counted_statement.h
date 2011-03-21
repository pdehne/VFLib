// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#ifndef __VF_DB_DETAIL_REF_COUNTED_STATEMENT_VFHEADER__
#define __VF_DB_DETAIL_REF_COUNTED_STATEMENT_VFHEADER__

#include "vf/db/statement.h"

namespace detail {

// used for "once" and "prepare"
class ref_counted_statement_base : vf::NonCopyable
{
public:
  ref_counted_statement_base (session& s);
  virtual ~ref_counted_statement_base ();

  virtual void final_action () = 0;

  void addref();
  void release();

  template<typename T> void accumulate(T const& t)
  {
    get_query_stream() << t;
  }

public:
  // break circular header dependency
  std::ostringstream& get_query_stream();

  Error* m_perror;
  int m_refs;
  session& m_session;
};

// used for "once"
class ref_counted_statement : public ref_counted_statement_base
{
public:
  ref_counted_statement(session& s, Error& error);
  void exchange(into_type_ptr const& i);
  void exchange(use_type_ptr const& u);

  void final_action();

private:
  Error& m_error;
  statement m_st;
};

}

#endif
