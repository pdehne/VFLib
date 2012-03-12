// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.
// Based on ideas from the soci wrapper sqlite back-end.

#ifndef __VF_DB_DETAIL_REF_COUNTED_STATEMENT_VFHEADER__
#define __VF_DB_DETAIL_REF_COUNTED_STATEMENT_VFHEADER__

#include "vf/modules/vf_db/statement.h"

namespace db {

namespace detail {

// used for "once" and "prepare"
class ref_counted_statement_base : NonCopyable
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

}

#endif
