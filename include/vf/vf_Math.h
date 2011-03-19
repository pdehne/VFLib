// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_MATH_VFHEADER__
#define __VF_MATH_VFHEADER__

//
// Miscellaneous mathematical calculations
//

// Calculate the bin for a value given the bin size.
// This correctly handles negative numbers. For example
// if value == -1 then calc_bin returns -1.
template <typename Ty>
inline Ty calc_bin (Ty value, int size)
{
  if (value >= 0)
    return value / size;
  else
    return (value - size + 1) / size;
}

// Given a number and a bin size, this returns the first
// corresponding value of the bin associated with the given number.
// It correctly handles negative numbers. For example,
// if value == -1 then calc_bin always returns -size
template <typename Ty>
inline Ty calc_bin_start (Ty value, int size)
{
  return calc_bin (value, size) * size;
}

#endif
