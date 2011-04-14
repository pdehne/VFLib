// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_BOOST_INCLUDES_VFHEADER__
#define __VF_BOOST_INCLUDES_VFHEADER__

// Includes essential boost headers

// We actually need these
#include <boost/bind.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/tss.hpp>
//#include <boost/spirit/include/phoenix1.hpp> // blows up visual studio

// THESE ARE NOT STRICTLY NEEDED
#include <boost/foreach.hpp>          // various places
#include <boost/shared_ptr.hpp>       // because of db
#include <boost/intrusive/list.hpp>   // because of Ui::Event and DJ library

#endif
