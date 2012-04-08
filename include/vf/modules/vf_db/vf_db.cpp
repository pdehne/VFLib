// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.
// Based on ideas from the soci wrapper sqlite back-end.

#include "vf_db.h"

#if VF_USE_SQLITE

#include "../vf_sqlite/vf_sqlite.h"

#if JUCE_MSVC
#pragma warning (push)
#pragma warning (disable: 4100) // unreferenced formal parmaeter
#pragma warning (disable: 4355) // 'this' used in base member
#endif

namespace vf
{
// implementation headers
#include "detail/error_codes.h"
#include "detail/statement_imp.h"
}

namespace vf
{
#include "source/blob.cpp"
#include "source/error_codes.cpp"
#include "source/into_type.cpp"
#include "source/once_temp_type.cpp"
#include "source/prepare_temp_type.cpp"
#include "source/ref_counted_prepare_info.cpp"
#include "source/ref_counted_statement.cpp"
#include "source/session.cpp"
#include "source/statement.cpp"
#include "source/statement_imp.cpp"
#include "source/transaction.cpp"
#include "source/use_type.cpp"
}

#if JUCE_MSVC
#pragma warning (pop)
#endif

#endif
