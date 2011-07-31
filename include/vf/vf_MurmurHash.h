// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// From http://code.google.com/p/smhasher/

#ifdef _MSC_VER

#ifndef __VF_MURMURHASH_VFHEADER__
#define __VF_MURMURHASH_VFHEADER__

// This file depends on some Juce declarations and defines

namespace Murmur {

// http://code.google.com/p/smhasher/
// http://sites.google.com/site/murmurhash/

// Murmur Hash
extern void Hash3_x86_32  ( const void * key, int len, uint32 seed, void * out );
extern void Hash3_x86_64  ( const void * key, int len, uint32 seed, void * out );
extern void Hash3_x86_128 ( const void * key, int len, uint32 seed, void * out );

extern void Hash3_x64_32  ( const void * key, int len, uint32 seed, void * out );
extern void Hash3_x64_64  ( const void * key, int len, uint32 seed, void * out );
extern void Hash3_x64_128 ( const void * key, int len, uint32 seed, void * out );

// Uses Juce to choose an appropriate routine

#if JUCE_64BIT

inline void Hash3_32 ( const void * key, int len, uint32 seed, void * out )
{
  Hash3_x64_32 (key, len, seed, out);
}

inline void Hash3_64 ( const void * key, int len, uint32 seed, void * out )
{
  Hash3_x64_64 (key, len, seed, out);
}

inline void Hash3_128 ( const void * key, int len, uint32 seed, void * out )
{
  Hash3_x64_64 (key, len, seed, out);
}

#else

inline void Hash3_32 ( const void * key, int len, uint32 seed, void * out )
{
  Hash3_x86_32 (key, len, seed, out);
}

inline void Hash3_64 ( const void * key, int len, uint32 seed, void * out )
{
  Hash3_x86_64 (key, len, seed, out);
}

inline void Hash3_128 ( const void * key, int len, uint32 seed, void * out )
{
  Hash3_x86_64 (key, len, seed, out);
}

#endif

// This handy template deduces which size hash is desired
template <typename HashType>
inline void Hash (const void* key, int len, uint32 seed, HashType* out)
{
  switch (8*sizeof(HashType))
  {
  case 32:  Hash3_32 (key, len, seed, out); break;
  case 64:  Hash3_64 (key, len, seed, out); break;
  case 128: Hash3_128 (key, len, seed, out); break;
  default:
    Throw (std::runtime_error ("invalid key size in MurmurHash"));
    break;
  };
}

}

#endif

#endif
