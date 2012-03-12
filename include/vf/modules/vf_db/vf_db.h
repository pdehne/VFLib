// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_DB_VFHEADER
#define VF_DB_VFHEADER

#include "../vf_core/vf_core.h"

namespace vf
{

namespace db
{

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

}

#endif
