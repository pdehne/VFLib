// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_AUDIO_VFHEADER
#define VF_AUDIO_VFHEADER

#include "../vf_core/vf_core.h"

namespace vf
{

#ifndef VF_AUDIOBUFFERPOOL_VFHEADER
#include "buffers/vf_AudioBufferPool.h"
#endif

#ifndef VF_AUDIOSAMPLEBUFFERARRAY_VFHEADER
#include "buffers/vf_AudioSampleBufferArray.h"
#endif

#ifndef VF_NOISEAUDIOSOURCE_VFHEADER
#include "sources/vf_NoiseAudioSource.h"
#endif

}

#endif

