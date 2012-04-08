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

#ifndef VF_DB_STATEMENT_VFHEADER
#define VF_DB_STATEMENT_VFHEADER

namespace db {

//
// Lightweight copyable handle to a statement. This
// is just a wrapper around the actual statement implementation.
//
class statement
{
public:
  statement (session& s);
  statement (detail::prepare_temp_type const& prep);
  ~statement ();

  statement (statement const& o);
  void operator= (statement const& o);

  void exchange (detail::into_type_ptr const& i);
  void exchange (detail::use_type_ptr const& u);

  void prepare (std::string const& query, bool bRepeatable=true);

  Error execute ();
  bool execute_and_fetch (Error& error);
  bool fetch (Error& error);

  bool got_data() const;
  rowid last_insert_rowid();

private:
  friend class detail::ref_counted_statement;

  void clean_up();

private:
  ReferenceCountedObjectPtr <detail::statement_imp> m_imp;
};

}

#endif
