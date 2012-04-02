// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.
// Based on ideas from the soci wrapper sqlite back-end.

#ifndef VF_DB_TRANSACTION_VFHEADER
#define VF_DB_TRANSACTION_VFHEADER

namespace db {

class transaction : Uncopyable
{
public:
  explicit transaction (session& s);
  ~transaction();

  Error commit();
  void rollback();

private:
  session& m_session;
  bool m_bHandled;
};

}

#endif
