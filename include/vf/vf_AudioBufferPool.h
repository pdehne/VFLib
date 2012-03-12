// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_AUDIOBUFFERPOOL_VFHEADER__
#define __VF_AUDIOBUFFERPOOL_VFHEADER__

#include "vf/modules/vf_core/diagnostic/vf_LeakChecked.h"
#include "vf/modules/vf_core/threads/vf_Mutex.h"

class AudioBufferPoolBase
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
};

// Manages a collection of AudioSampleBuffer that
// persist in memory and never decrease in size.
template <class BufferClass>
class AudioBufferPoolTemplate
: public AudioBufferPoolBase
, public LeakChecked <AudioBufferPoolTemplate<BufferClass> >
{
public:
  AudioBufferPoolTemplate ()
  {
    m_buffers.ensureStorageAllocated (10);
  }
    
  ~AudioBufferPoolTemplate ()
  {
    for (int i = 0; i < m_buffers.size(); ++i)
      delete m_buffers[i];
  }    

  BufferClass* acquireBuffer (int numChannels, int numSamples)
  {
    BufferClass* buffer = 0;
    int samplesNeeded = numChannels * numSamples;
    
    int index = -1;
    
    {
        m_mutex.enter ();
        
        for (int i = 0; i < m_buffers.size(); ++i)
        {
            BufferClass* cur = m_buffers[i];
            
            if (!buffer)
            {
                buffer = cur;
                index = i;
            }
            else
            {
                int numSamplesAvailable = cur->getNumSamplesAllocated();
                
                // Use the smallest buffer which is equal or bigger than what
                // we need. If no buffer is large enough, then we will use the
                // largest available and increase it, to minimize memory usage.
                if ( (numSamplesAvailable >= samplesNeeded &&
                      numSamplesAvailable < buffer->getNumSamplesAllocated()) ||
                    (numSamplesAvailable < samplesNeeded &&
                     numSamplesAvailable > buffer->getNumSamplesAllocated()))
                {
                    buffer = cur;
                    index = i;
                }
            }
        }
        
        if (buffer)
            m_buffers.remove (index);
        
        m_mutex.exit();
    }
    
    if (buffer)
        buffer->resize (numChannels, numSamples);
    else
        buffer = new BufferClass (numChannels, numSamples);
    
    return buffer;
  }

  void releaseBuffer (BufferClass* buffer)
  {
    if (buffer)
    {
        m_mutex.enter ();
        
        m_buffers.add (buffer);
        
        m_mutex.exit ();
    }
  }    

private:
  Mutex m_mutex;
  VF_JUCE::Array <BufferClass*> m_buffers;
};

typedef AudioBufferPoolTemplate<AudioBufferPoolBase::Buffer> AudioBufferPool;

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
