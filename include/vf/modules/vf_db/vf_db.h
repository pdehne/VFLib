// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_DB_VFHEADER
#define VF_DB_VFHEADER

#include "../vf_core/vf_core.h"

// forward declares
struct sqlite3;
struct sqlite3_blob;
struct sqlite3_stmt;
namespace vf {
namespace db {
namespace detail {
class prepare_temp_type;
class ref_counted_statement;		// statement.h
class statement_imp;				// into_type.h, use_type.h
}
class blob;							// exchange_traits.h
class session;						// statement.h
}
}

namespace vf
{

#include "backend.h"
#include "error_codes.h"
#include "detail/type_ptr.h"
#include "transaction.h"

#include "blob.h"
#include "type_conversion_traits.h"

#include "detail/exchange_traits.h"

#include "detail/into_type.h"
#include "detail/use_type.h"

#include "statement.h"
#include "detail/statement_imp.h"
#include "detail/type_conversion.h"

#include "detail/ref_counted_statement.h"

#include "into.h"
#include "use.h"
#include "detail/ref_counted_prepare_info.h"

#include "detail/prepare_temp_type.h"

#include "detail/once_temp_type.h"

#include "session.h"

}

#endif
