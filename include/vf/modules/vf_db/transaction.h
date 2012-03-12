// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#ifndef __VF_DB_TRANSACTION_VFHEADER__
#define __VF_DB_TRANSACTION_VFHEADER__

namespace db {

class transaction : NonCopyable
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
