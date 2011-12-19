// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_AUDIOSAMPLEBUFFERARRAY_VFHEADER__
#define __VF_AUDIOSAMPLEBUFFERARRAY_VFHEADER__

// lightweight object to track an adjustable array of pointers to audio data.
template <int Channels = 2>
class AudioSampleBufferArray
{
public:
  typedef float Sample;

  AudioSampleBufferArray ()
    : m_numSamples (0)
  {
  }

  AudioSampleBufferArray (int numSamples, Sample* const* channels)
  {
    setFrom (numSamples, channels);
  }

  AudioSampleBufferArray (const AudioSampleBufferArray& other)
  {
    setFrom (other);
  }

  void setFrom (int numSamples, Sample* const* channels)
  {
    m_numSamples = numSamples;
    for (int i=0; i<Channels; i++)
      m_channels[i]=channels[i];
  }

  void setFrom (const AudioSampleBufferArray& other)
  {
    m_numSamples = other.m_numSamples;
    for (int i=0; i<Channels; i++)
      m_channels[i] = other.m_channels[i];
  }

  AudioSampleBufferArray& operator= (const AudioSampleBufferArray& other)
  {
    setFrom (other);
    return *this;
  }

  AudioSampleBufferArray operator+ (int numSamples)
  {
    jassert (numSamples <= m_numSamples);
    Sample* channels[Channels];
    for (int i=0; i<Channels; i++)
      channels[i] = m_channels[i] + numSamples;
    return AudioSampleBufferArray (m_numSamples - numSamples, channels);
  }

  AudioSampleBufferArray operator- (int numSamples)
  {
    return operator+ (-numSamples);
  }

  AudioSampleBufferArray& operator+= (int numSamples)
  {
    *this = *this + numSamples;
    return *this;
  }

  AudioSampleBufferArray& operator-= (int numSamples)
  {
    return operator+= (-numSamples);
  }

  bool isEmpty () const
  {
    return m_numSamples <= 0;
  }

  Sample* const* getArrayOfChannels() const
  {
    return m_channels;
  }

  operator Sample* const* () const
  {
    return m_channels;
  }

  Sample* const* operator()() const
  {
    return m_channels;
  }

  Sample*& operator[] (int index)
  {
    jassert (index >= 0 && index < Channels);
    return m_channels[index];
  }

  const Sample* operator[] (int index) const
  {
    jassert (index >= 0 && index < Channels);
    return m_channels[index];
  }

  int getNumSamples () const
  {
    return m_numSamples;
  }

public:
  // if AudioSampleBuffer ever becomes a template, we can
  // move the Sample typedef into the template parameters.

  AudioSampleBufferArray (const VF_JUCE::AudioSampleBuffer& buffer,
                          int offset = 0,
                          int numSamples = -1)
  {
    setFrom (buffer, offset, numSamples);
  }

  AudioSampleBufferArray (const VF_JUCE::AudioSourceChannelInfo& bufferToFill)
  {
    setFrom (*bufferToFill.buffer, bufferToFill.startSample, bufferToFill.numSamples);
  }

  operator VF_JUCE::AudioSampleBuffer()
  {
    return VF_JUCE::AudioSampleBuffer (m_channels, Channels, m_numSamples);
  }

  operator const AudioSampleBuffer() const
  {
    return VF_JUCE::AudioSampleBuffer (m_channels, Channels, m_numSamples);
  }

  void setFrom (const VF_JUCE::AudioSampleBuffer& buffer, int offset = 0, int numSamples = -1)
  {
    jassert (buffer.getNumChannels() == Channels);
    jassert (offset >= 0 && offset <= buffer.getNumSamples());
    if (numSamples == -1)
      numSamples = buffer.getNumSamples() - offset;
    jassert (numSamples <= buffer.getNumSamples() - offset);
    m_numSamples = numSamples;
    for (int i = 0; i < Channels; ++i)
      m_channels[i] = buffer.getArrayOfChannels()[i] + offset;
  }

  AudioSampleBufferArray& operator= (const VF_JUCE::AudioSampleBuffer& buffer)
  {
    setFrom (buffer);
    return *this;
  }

  AudioSampleBufferArray& operator= (const VF_JUCE::AudioSourceChannelInfo& bufferToFill)
  {
    setFrom (bufferToFill);
    return *this;
  }

private:
  int m_numSamples;
  Sample* m_channels[Channels];
};

typedef AudioSampleBufferArray <2> StereoSampleBuffer;

#endif
