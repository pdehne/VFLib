// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_BOOST_INCLUDES_VFHEADER__
#define __VF_BOOST_INCLUDES_VFHEADER__

// Includes essential boost headers

// We actually need these
#include <boost/iterator/iterator_facade.hpp>
#include <boost/thread/tss.hpp>
//#include <boost/spirit/include/phoenix1.hpp> // blows up visual studio

// THESE ARE NOT STRICTLY NEEDED
#include <boost/foreach.hpp>          // various places
#include <boost/shared_ptr.hpp>       // because of db

#endif
