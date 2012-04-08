// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file BOOST_LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// This file incorporates work covered by the following copyright
// and permission notice:
//
//   Copyright (C) 2004-2008 Maciej Sobczak, Stephen Hutton,
//   Mateusz Loskot, Pawel Aleksander Fedorynski, David Courtney,
//   Rafal Bobrowski, Julian Taylor, Henning Basold, Ilia Barahovski,
//   Denis Arnaud, Daniel Lidström, Matthieu Kermagoret, Artyom Beilis,
//   Cory Bennett, Chris Weed, Michael Davidsaver, Jakub Stachowski,
//   Alex Ott, Rainer Bauer, Martin Muenstermann, Philip Pemberton,
//   Eli Green, Frederic Chateau, Artyom Tonkikh, Roger Orr,
//   Robert Massaioli, Sergey Nikulov, Shridhar Daithankar,
//   Sören Meyer-Eppler, Mario Valesco
//
//   Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file BOOST_LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//

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
