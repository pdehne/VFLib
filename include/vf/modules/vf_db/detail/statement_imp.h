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

#ifndef VF_DB_DETAIL_STATEMENT_IMP_VFHEADER
#define VF_DB_DETAIL_STATEMENT_IMP_VFHEADER

namespace db {

namespace detail {

// statement implementation
class statement_imp
  : public ReferenceCountedObject
  , Uncopyable
{
public:
  explicit statement_imp (session& s);
  explicit statement_imp (prepare_temp_type const& prep);
  ~statement_imp ();

  void erase_bindings ();
  void exchange (detail::into_type_ptr const& i);
  void exchange (detail::use_type_ptr const& u);
  void clean_up ();

  void prepare (std::string const& query, bool bRepeatable = true);
  Error execute ();
  bool fetch (Error& error);
  bool got_data () const;

public:
  void do_intos ();
  void pre_use ();
  void do_uses ();
  void post_use ();
  void release_resources ();
  rowid last_insert_rowid ();

public:
  session& m_session;
  sqlite3_stmt* m_stmt;
  std::string m_query;
  bool m_bReady;
  bool m_bGotData;
  bool m_bFirstTime;
  rowid m_last_insert_rowid;

  typedef std::vector <detail::into_type_base*> intos_t;
  typedef std::vector <detail::use_type_base*> uses_t;

  intos_t m_intos;
  uses_t m_uses;
};

}

}

#endif
