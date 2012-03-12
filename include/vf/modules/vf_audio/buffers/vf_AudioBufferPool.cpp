// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

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
