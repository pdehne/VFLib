// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

#ifndef VF_MATH_VFHEADER
#define VF_MATH_VFHEADER

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
