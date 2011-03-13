// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_STRING_VFHEADER__
#define __VF_STRING_VFHEADER__

#if VF_HAVE_JUCE

using JUCE_NAMESPACE::String;

#else

typedef std::string String;

template <typename S>
inline S const& TRANS (S const& s)
{
  return s;
}

#endif

#endif

