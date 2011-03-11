// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_BOOST_INCLUDES_VFHEADER__
#define __VF_BOOST_INCLUDES_VFHEADER__

// Includes essential boost headers

// TODO: TRY TO REMOVE AS MANY OF THESE AS POSSIBLE TO REDUCE DEPENDENCIES

#include <boost/config.hpp>

// This thing is so big it blows up visual studio
//#include <boost/spirit/include/phoenix1.hpp>

#include <boost/bind.hpp>
#include <boost/bind/protect.hpp>
#include <boost/thread.hpp>
#include <boost/ref.hpp>

#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>
#include <boost/noncopyable.hpp>

#include <boost/filesystem.hpp>        // DEPRECATED, REMOVE ASAP
#include <boost/intrusive/list.hpp>    // DEPRECATED, REMOVE ASAP
#include <boost/intrusive/avl_set.hpp> // DEPRECATED, REMOVE ASAP
#include <boost/intrusive/list.hpp>    // DEPRECATED, REMOVE ASAP

#include <boost/throw_exception.hpp>

// HOLY FUCK GET THESE OUT OF HERE!!!
#include <boost/cstdint.hpp>
#include <boost/mpl/and.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/detail/atomic_count.hpp>

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable:4244) // conversion from X to Y: possible loss of data
#  pragma warning(disable:4706) // assignment within conditional expression
#endif
// singleton isn't part of boost yet,
// there's a copy floating around on the net
#include <boost/utility/singleton.hpp> // DEPRECATED, REMOVE ASAP, USE JUCE
#ifdef _MSC_VER
#  pragma warning(pop)
#endif


#endif
