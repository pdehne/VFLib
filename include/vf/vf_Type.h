// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_TYPE_VFHEADER__
#define __VF_TYPE_VFHEADER__

// Template ideas from Juce

/** The Param struct is used to find the best type to use when passing some kind
    of object as a parameter.

    Of course, this is only likely to be useful in certain esoteric template situations.

    Because "typename TypeHelpers::Param<SomeClass>::type" is a bit of a mouthful, there's
    a PARAMETER_TYPE(SomeClass) macro that you can use to get the same effect.

    E.g. "myFunction (PARAMETER_TYPE (int), PARAMETER_TYPE (MyObject))"
    would evaluate to "myfunction (int, const MyObject&)", keeping any primitive types as
    pass-by-value, but passing objects as a const reference, to avoid copying.
*/

#if 0

namespace Type {

template <typename T> struct Param                  { typedef T t; };

template <typename T> struct Param <T&>             { typedef T& t; };
template <typename T> struct Param <T*>             { typedef T* t; };
template <>           struct Param <char>           { typedef char t; };
template <>           struct Param <unsigned char>  { typedef unsigned char t; };
template <>           struct Param <short>          { typedef short t; };
template <>           struct Param <unsigned short> { typedef unsigned short t; };
template <>           struct Param <int>            { typedef int t; };
template <>           struct Param <unsigned int>   { typedef unsigned int t; };
template <>           struct Param <long>           { typedef long t; };
template <>           struct Param <unsigned long>  { typedef unsigned long t; };
template <>           struct Param <bool>           { typedef bool t; };
template <>           struct Param <float>          { typedef float t; };
template <>           struct Param <double>         { typedef double t; };

template <>           struct Param <VF_JUCE::int64>  { typedef VF_JUCE::int64 t; };
template <>           struct Param <VF_JUCE::uint64> { typedef VF_JUCE::uint64 t; };

//#define PARAMETER_TYPE(a)    typename T::Param<a>::t

}

#endif

#endif
