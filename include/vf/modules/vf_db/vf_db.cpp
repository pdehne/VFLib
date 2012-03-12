// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.
// Based on ideas from the soci wrapper sqlite back-end.

#include "vf/vf_StandardHeader.h"

#include "sqlite/sqlite3.h"

#include "vf_db.h"

namespace vf
{
// implementation headers
#include "error_codes.h"
#include "detail/statement_imp.h"
}

namespace vf
{
#include "blob.cpp"
#include "error_codes.cpp"
#include "into_type.cpp"
#include "once_temp_type.cpp"
#include "prepare_temp_type.cpp"
#include "ref_counted_prepare_info.cpp"
#include "ref_counted_statement.cpp"
#include "session.cpp"
#include "statement.cpp"
#include "statement_imp.cpp"
#include "transaction.cpp"
#include "use_type.cpp"
}
