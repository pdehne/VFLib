// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_AudioBufferPool.h"

AudioBufferPool::Buffer::Buffer (int numChannels,
                                 int numSamples)
  : AudioSampleBuffer (numChannels, numSamples)
{
}

void AudioBufferPool::Buffer::resize (int newNumChannels, int newNumSamples)
{
  int samplesAllocated = newNumChannels * newNumSamples;

  if (m_samplesAllocated < samplesAllocated)
    m_samplesAllocated = samplesAllocated;

  setSize (newNumChannels, newNumSamples, false, false, true);
}

int AudioBufferPool::Buffer::getNumSamplesAllocated () const
{
  return m_samplesAllocated;
}

//------------------------------------------------------------------------------

AudioBufferPool::AudioBufferPool()
{
  m_buffers.ensureStorageAllocated (10);
}

AudioBufferPool::~AudioBufferPool()
{
  for (int i = 0; i < m_buffers.size(); ++i)
    delete m_buffers[i];
}

AudioBufferPool::Buffer* AudioBufferPool::acquireBuffer (int numChannels, int numSamples)
{
  AudioBufferPool::Buffer* buffer = 0;
  int samplesNeeded = numChannels * numSamples;

  int index = -1;

  {
    m_mutex.enter ();

    for (int i = 0; i < m_buffers.size(); ++i)
    {
      Buffer* cur = m_buffers[i];

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
    buffer = new Buffer (numChannels, numSamples);

  return buffer;
}

void AudioBufferPool::releaseBuffer (Buffer* buffer)
{
  if (buffer)
  {
    m_mutex.enter ();

    m_buffers.add (buffer);

    m_mutex.exit ();
  }
}

END_VF_NAMESPACE
