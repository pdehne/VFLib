// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#include "vf_audio.h"

#if JUCE_MSVC
#pragma warning (push)
#pragma warning (disable: 4100) // unreferenced formal parmaeter
#endif

namespace vf
{
#include "buffers/vf_AudioBufferPool.cpp"
#include "sources/vf_NoiseAudioSource.cpp"
}

#if JUCE_MSVC
#pragma warning (pop)
#endif
