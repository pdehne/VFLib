// Copyright (C) 2008-2011 by One Guy Group, Inc., All rights reserved worldwide.

#ifndef __VF_AUDIOBUFFERPOOL_VFHEADER__
#define __VF_AUDIOBUFFERPOOL_VFHEADER__

#if !VF_USE_JUCE
#error "Requires Juce"
#endif

#include "vf/vf_Mutex.h"

// Manages a collection of AudioSampleBuffer that
// persist in memory and never decrease in size.
class AudioBufferPool : LeakChecked <AudioBufferPool>
{
public:
  class Buffer : public AudioSampleBuffer
  {
  public:
    Buffer (int numChannels,
            int numSamples);

    void resize (int newNumChannels, int newNumSamples);

    int getNumSamplesAllocated () const;

  private:
    int m_samplesAllocated;
  };

public:
  AudioBufferPool ();
  ~AudioBufferPool ();

  Buffer* acquireBuffer (int numChannels,
                         int numSamples);

  void releaseBuffer (Buffer* buffer);

private:
  Mutex m_mutex;
  Array <Buffer*> m_buffers;
};

//------------------------------------------------------------------------------

// scoped lifetime management for a temporary audio buffer
class ScopedAudioSampleBuffer : LeakChecked <ScopedAudioSampleBuffer>,
                                NonCopyable
{
public:
  ScopedAudioSampleBuffer (AudioBufferPool& pool,
                           int numChannels,
                           int numSamples)
      : m_pool (pool)
      , m_buffer (pool.acquireBuffer (numChannels, numSamples))
  {
  }
  
  ~ScopedAudioSampleBuffer ()
  {
    m_pool.releaseBuffer (m_buffer);
  }

  AudioSampleBuffer* getBuffer ()
  {
    return m_buffer;
  }

  AudioSampleBuffer* operator-> ()
  {
    return getBuffer();
  }

  AudioSampleBuffer& operator* ()
  {
    return *getBuffer();
  }

  operator AudioSampleBuffer* ()
  {
    return getBuffer();
  }

private:
  AudioBufferPool& m_pool;
  AudioBufferPool::Buffer* m_buffer;
};

#endif
