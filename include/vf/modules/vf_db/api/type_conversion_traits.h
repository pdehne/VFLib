// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.
// Based on ideas from the soci wrapper sqlite back-end.

#ifndef VF_DB_TYPE_CONVERSION_TRAITS_VFHEADER
#define VF_DB_TYPE_CONVERSION_TRAITS_VFHEADER

namespace db {

// default conversion (copy in to out)
template<typename T>
struct type_conversion
{
  typedef T base_type;

  static void from_base(base_type const& in, indicator ind, T& out )
  {
    // null not allowed
    if( ind == i_null )
       Throw (Error().fail (__FILE__, __LINE__));
    out = in;
  }

  static void to_base(T const& in, base_type& out, indicator& ind )
  {
    out = in;
    ind = i_ok;
  }
};

// VF_JUCE::Time
template<>
struct type_conversion <VF_JUCE::Time>
{
  //typedef sqlite3_int64 base_type;
  typedef int64 base_type;

  static void from_base (const base_type v, indicator ind, VF_JUCE::Time& result)
  {
    if (ind == i_null)
    {
      // jassertfalse
      result = VF_JUCE::Time (0);
    }
    else
    {
      result = VF_JUCE::Time (v);
    }
  }

  static void to_base (const VF_JUCE::Time& v, base_type& result, indicator& ind)
  {
    result = v.toMilliseconds ();
    ind = i_ok;
  }
};

}

#endif
