// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_BIND_VFHEADER__
#define __VF_BIND_VFHEADER__

#ifdef VF_HAVE_BIND
#error "VF_HAVE_BIND already set"
#endif

#if VF_USE_BOOST

//
// Boost
//
using ::boost::bind;
using ::boost::ref;
using ::boost::cref;
//using   ::boost::protect;
using ::_1;
using ::_2;
using ::_3;
using ::_4;
using ::_5;
using ::_6;
using ::_7;
using ::_8;
using ::_9; // boost limit as of 1.45
#define VF_HAVE_BIND 1

#elif defined (_MSC_VER) && defined (_HAS_TR1)

//
// std::tr1
//
using std::tr1::bind;
using std::tr1::ref;
using std::tr1::cref;
using std::tr1::placeholders::_1;
using std::tr1::placeholders::_2;
using std::tr1::placeholders::_3;
using std::tr1::placeholders::_4;
using std::tr1::placeholders::_5;
using std::tr1::placeholders::_6;
using std::tr1::placeholders::_7;
using std::tr1::placeholders::_8;
using std::tr1::placeholders::_9;
#define VF_HAVE_BIND 1

#else

#define VF_HAVE_BIND 0

#endif

#endif
