// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_AUDIOBUFFERPOOL_VFHEADER__
#define __VF_AUDIOBUFFERPOOL_VFHEADER__

#include "vf/vf_LeakChecked.h"
#include "vf/vf_Mutex.h"

// Manages a collection of AudioSampleBuffer that
// persist in memory and never decrease in size.
class AudioBufferPool : LeakChecked <AudioBufferPool>
{
public:
  class Buffer : public VF_JUCE::AudioSampleBuffer
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
  VF_JUCE::Array <Buffer*> m_buffers;
};

//------------------------------------------------------------------------------

// scoped lifetime management for a temporary audio buffer
#if 1
class ScopedAudioSampleBuffer
  // NO IDEA why the leak checking fails
  // : LeakChecked <ScopedAudioSampleBuffer>, NonCopyable
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

  VF_JUCE::AudioSampleBuffer* getBuffer ()
  {
    return m_buffer;
  }

  VF_JUCE::AudioSampleBuffer* operator-> ()
  {
    return getBuffer();
  }

  VF_JUCE::AudioSampleBuffer& operator* ()
  {
    return *getBuffer();
  }

  operator VF_JUCE::AudioSampleBuffer* ()
  {
    return getBuffer();
  }

private:
  AudioBufferPool& m_pool;
  AudioBufferPool::Buffer* m_buffer;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScopedAudioSampleBuffer);
};
#else

class ScopedAudioSampleBuffer
{
public:
  ScopedAudioSampleBuffer (AudioBufferPool& pool,
                           int numChannels,
                           int numSamples)
      : m_buffer (numChannels, numSamples)
  {
  }
  
  ~ScopedAudioSampleBuffer ()
  {
  }

  VF_JUCE::AudioSampleBuffer* getBuffer ()
  {
    return &m_buffer;
  }

  VF_JUCE::AudioSampleBuffer* operator-> ()
  {
    return getBuffer();
  }

  VF_JUCE::AudioSampleBuffer& operator* ()
  {
    return *getBuffer();
  }

  operator VF_JUCE::AudioSampleBuffer* ()
  {
    return getBuffer();
  }

private:
  AudioBufferPool::Buffer m_buffer;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScopedAudioSampleBuffer);
};

#endif

#endif
