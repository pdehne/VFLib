//==============================================================================
//
// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file GNU_GPL_v2.txt for full licensing terms.
// 
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your option)
// any later version.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
// 
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 51
// Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
// 
//==============================================================================

#ifndef VF_AUDIOSAMPLEBUFFERARRAY_VFHEADER
#define VF_AUDIOSAMPLEBUFFERARRAY_VFHEADER

//==============================================================================
/** A wrapper for an array of pointers to audio data.

    This simple class wraps an array of pointers to multi-channel audio data.
    Provided functions can move the pointers back and forth as a group,
    making certain forms of code that manipulate audio buffers more concise.

    Conversion to and from AudioSampleBuffer allow seamless usage.

    \ingroup vf_audio
*/
template <int Channels = 2>
class AudioSampleBufferArray
{
public:
  typedef float Sample;

  /** Create a new array of pointers with no data. */
  AudioSampleBufferArray ()
    : m_numSamples (0)
  {
  }

  /** Create an array of pointers from existing data pointers.
  
      @param numSamples The number of samples in the resulting array. The
                        passed array of channels must have sufficient space.

      @param channels   */
  AudioSampleBufferArray (int numSamples, Sample* const* channels)
  {
    setFrom (numSamples, channels);
  }

  /** Copy constructor. */
  AudioSampleBufferArray (const AudioSampleBufferArray& other)
  {
    setFrom (other);
  }

  // if AudioSampleBuffer ever becomes a template, we can
  // move the Sample typedef into the template parameters.

  /** Create a new array from an existing AudioSampleBuffer.

      @param startingSampleIndex Zero based index of the first sample to use.
      @param numSamples          The number of samples in the desired range.
  */
  AudioSampleBufferArray (const VF_JUCE::AudioSampleBuffer& buffer,
                          int startSample = 0,
                          int numSamples = -1)
  {
    setFrom (buffer, startSample, numSamples);
  }

  /** Create a new array from a AudioSourceChannelInfo

      This provides a convenient conversion.
  */
  AudioSampleBufferArray (const VF_JUCE::AudioSourceChannelInfo& bufferToFill)
  {
    setFrom (*bufferToFill.buffer,
             bufferToFill.startSample,
             bufferToFill.numSamples);
  }

  int getNumSamples () const
  {
    return m_numSamples;
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

  operator VF_JUCE::AudioSampleBuffer()
  {
    return VF_JUCE::AudioSampleBuffer (m_channels, Channels, m_numSamples);
  }

  operator const AudioSampleBuffer() const
  {
    return VF_JUCE::AudioSampleBuffer (m_channels, Channels, m_numSamples);
  }

  /** Manually assign a range of samples from a set of pointers. */
  void setFrom (int numSamples, Sample* const* channels)
  {
    m_numSamples = numSamples;
    for (int i=0; i<Channels; i++)
      m_channels[i]=channels[i];
  }

  /** Assign the array from another array. */
  void setFrom (const AudioSampleBufferArray& other)
  {
    m_numSamples = other.m_numSamples;
    for (int i=0; i<Channels; i++)
      m_channels[i] = other.m_channels[i];
  }

  void setFrom (VF_JUCE::AudioSampleBuffer const& buffer,
                int startSample = 0,
                int numSamples = -1)
  {
    jassert (buffer.getNumChannels() == Channels);
    jassert (startSample >= 0 && startSample <= buffer.getNumSamples());
    if (numSamples == -1)
      numSamples = buffer.getNumSamples() - startSample;
    jassert (numSamples <= buffer.getNumSamples() - startSample);
    m_numSamples = numSamples;
    for (int i = 0; i < Channels; ++i)
      m_channels[i] = buffer.getArrayOfChannels()[i] + startSample;
  }

  /** Assign the array from another array. */
  AudioSampleBufferArray& operator= (const AudioSampleBufferArray& other)
  {
    setFrom (other);
    return *this;
  }

  /** Advance all channels by the specified number of samples.

      Advancing by more than the number of samples remaining is undefined.

      @param numSamples The number of samples to advance by.

      @return An array that points to the new range of samples.
  */
  AudioSampleBufferArray operator+ (int numSamples)
  {
    jassert (numSamples <= m_numSamples);
    Sample* channels[Channels];
    for (int i=0; i<Channels; i++)
      channels[i] = m_channels[i] + numSamples;
    return AudioSampleBufferArray (m_numSamples - numSamples, channels);
  }

  AudioSampleBufferArray& operator+= (int numSamples)
  {
    *this = *this + numSamples;
    return *this;
  }

  /** Rewind all channels by the specified number of samples.

      Rewinding past the original beginning of the buffer is undefined.

      @param numSamples The number of samples to rewind by.

      @return An array that points to the new range of samples.
  */
  AudioSampleBufferArray operator- (int numSamples)
  {
    return operator+ (-numSamples);
  }

  AudioSampleBufferArray& operator-= (int numSamples)
  {
    return operator+= (-numSamples);
  }

  /** Determine if the range of samples if empty.

      @return `true` if there are no more samples in the buffer.
  */
  bool isEmpty () const
  {
    return m_numSamples <= 0;
  }

  /** Retrieve the raw array of channel pointers.

       @return An array of pointers to sample data.
  */
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

private:
  int m_numSamples;
  Sample* m_channels[Channels];
};

typedef AudioSampleBufferArray <2> StereoSampleBuffer;

#endif
