// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_AudioBufferPool.h"

AudioBufferPoolBase::Buffer::Buffer (int numChannels,
                                 int numSamples)
  : AudioSampleBuffer (numChannels, numSamples)
  , m_samplesAllocated (numChannels * numSamples)
{
}

void AudioBufferPoolBase::Buffer::resize (int newNumChannels, int newNumSamples)
{
  int samplesAllocated = newNumChannels * newNumSamples;

  if (m_samplesAllocated < samplesAllocated)
    m_samplesAllocated = samplesAllocated;

  setSize (newNumChannels, newNumSamples, false, false, true);
}

int AudioBufferPoolBase::Buffer::getNumSamplesAllocated () const
{
  return m_samplesAllocated;
}

END_VF_NAMESPACE
