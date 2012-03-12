// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#ifndef __VF_DB_VFHEADER__
#define __VF_DB_VFHEADER__

namespace db {

// forward declarations to prevent circular dependencies

namespace detail {

class prepare_temp_type;
class ref_counted_statement;
class ref_counted_statement_base;
class standard_into_type;
class statement_imp;

}

class blob;
class session;
class statement;

}

#include "vf/modules/vf_db/blob.h"
#include "vf/modules/vf_db/into.h"
#include "vf/modules/vf_db/session.h"
#include "vf/modules/vf_db/statement.h"
#include "vf/modules/vf_db/transaction.h"
#include "vf/modules/vf_db/use.h"

#endif
