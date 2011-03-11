// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_VFHEADER__
#define __VF_VFHEADER__

//
// This is the only header file that should be included by outside applications.
//
// SOURCE AND HEADER FILES IN THE VF LIBRARY SHOULD NEVER INCLUDE THIS!!!!
//

// Catches errors in files included previously.
namespace vf_DummyNamespace {}

// This tells the standard header that it is being included for public use.
#define VF_PUBLIC_INCLUDES 1
#include "vf/vf_StandardHeader.h"

// Add outside headers, and our own headers which
// need to be added outside of our namespace.
#include "vf/vf_ext_includes.h"

// Bring in everything in the vf library within our namespace.
BEGIN_VF_NAMESPACE

#ifdef HAVE_VF_PRIVATE
  #include "vf/vf_lib_includes.h"
  #include "vf/shared_data.h"
  #include "vf/Array.h"
  #include "vf/Debug.h"
  #include "vf/Err.h"
  #include "vf/Memory.h"
  #include "vf/MurmurHash.h"
  #include "vf/Printf.h"
  #include "vf/Range.h"
  #include "vf/Str.h"
  #include "vf/Types.h"
  #include "vf/Utf.h"
  #include "vf/vf_Math.h"
  #include "vf/Ui.h"
#endif

#ifdef HAVE_VF_PRIVATE
#  include "vf/vf_private_includes.h"
#endif

#ifdef VF_DB_H
#  error "Can't include Db.h within the namespace yet"
#endif

END_VF_NAMESPACE

#ifdef HAVE_VF_PRIVATE
  // move this after we fix all the db headers
  #include "vf/Db.h"
  // REMOVE ASAP
  #include "vf/string.h"
  // hack until we get rid of err
  typedef vf::Err Err;
#endif

#endif
