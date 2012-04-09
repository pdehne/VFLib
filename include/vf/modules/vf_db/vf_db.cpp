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
