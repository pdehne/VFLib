// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

// Original source code links in .cpp file

#ifndef __VF_MURMURHASH_VFHEADER__
#define __VF_MURMURHASH_VFHEADER__

// This file depends on some Juce declarations and defines

namespace Murmur {

extern void MurmurHash3_x86_32  (const void *key, int len, uint32 seed, void* out);
extern void MurmurHash3_x86_128 (const void *key, int len, uint32 seed, void* out);
extern void MurmurHash3_x64_128 (const void *key, int len, uint32 seed, void* out);

// Uses Juce to choose an appropriate routine

// This handy template deduces which size hash is desired
template <typename HashType>
inline void Hash (const void* key, int len, uint32 seed, HashType* out)
{
  switch (8*sizeof(HashType))
  {
  case 32:  MurmurHash3_x86_32 (key, len, seed, out); break;

#if JUCE_64BIT
  case 128: MurmurHash3_x64_128 (key, len, seed, out); break;
#else
  case 128: MurmurHash3_x86_128 (key, len, seed, out); break;
#endif

  default:
    Throw (std::runtime_error ("invalid key size in MurmurHash"));
    break;
  };
}

}

#endif
