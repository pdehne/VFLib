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

#ifndef VF_DB_VFHEADER
#define VF_DB_VFHEADER

#include "../vf_core/vf_core.h"

#if VF_USE_SQLITE

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

#if JUCE_MSVC
#pragma warning (push)
#pragma warning (disable: 4355) // 'this' : used in base member initializer list
#endif

namespace vf
{

#include "api/backend.h"
#include "api/transaction.h"
#include "detail/type_ptr.h"

#include "api/blob.h"
#include "api/type_conversion_traits.h"

#include "detail/exchange_traits.h"

#include "detail/into_type.h"
#include "detail/use_type.h"

#include "api/statement.h"
#include "detail/type_conversion.h"

#include "detail/ref_counted_statement.h"

#include "api/into.h"
#include "api/use.h"
#include "detail/ref_counted_prepare_info.h"

#include "detail/prepare_temp_type.h"

#include "detail/once_temp_type.h"

#include "api/session.h"

}

#if JUCE_MSVC
#pragma warning (pop)
#endif

#endif

#endif
