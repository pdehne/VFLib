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

#ifndef VF_DB_DETAIL_REF_COUNTED_STATEMENT_VFHEADER
#define VF_DB_DETAIL_REF_COUNTED_STATEMENT_VFHEADER

namespace db {

namespace detail {

// used for "once" and "prepare"
class ref_counted_statement_base : Uncopyable
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
