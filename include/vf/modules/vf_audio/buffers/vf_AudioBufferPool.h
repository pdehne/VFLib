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

#ifndef VF_AUDIOBUFFERPOOL_VFHEADER
#define VF_AUDIOBUFFERPOOL_VFHEADER

//==============================================================================
/** 
    A pool of audio buffers for temporary calculations.

    The container provides a pool of audio buffers that grow to match the
    working set requirements based on actual usage. Since the buffers never
    shrink or get deleted, there are almost no calls to the system to allocate
    or free memory.

    This is ideal for audioDeviceIOCallback implementations which process
    audio buffers and require temporary storage for intermediate calculations.
    The usage style is to request a temporary buffer of the desired size,
    perform calculations, and then release the buffer when finished.

    The container will intelligently resize and recycle these buffers to
    consume the smallest amount of memory possible based on usage patterns,
    with no effort required by the programmer.

    It's easy to use:

    @code
	
    AudioBufferPoolType <DummyCriticalSection> pool;

    // Request a stereo buffer with room for 1024 samples.
    /
    AudioBufferPool::Buffer* buffer = pool.requestBuffer (2, 1024);

    // (Process buffer)

    // Release the buffer to be re-used later.
    //
    pool.releaseBuffer (buffer);

    @endcode

    Since Buffer is derived from AudioSampleBuffer, it can be used anywhere
    an AudioSampleBuffer is expected. This example requests a temporary buffer
    and stores it in an AudioSourceChannelInfo:

    @code

    // Request a stereo buffer with room for 1024 samples.
    //
    AudioBufferPool::Buffer* buffer = pool.requestBuffer (2, 1024);

    // Fill out the AudioSourceChannelInfo structure with the buffer
    //
    AudioSourceChannelInfo info;
    info.buffer = buffer;         // allowed, since AudioBufferPool::Buffer *
                                  // is-a AudioSampleBuffer *
    info.startSample = 0;
    info.numSamples = 1024;

    // Clear out the range of samples
    //
    info.clearActiveBufferRegion ();
    
    @endcode

    The thread safety of AudioBufferPoolType is determined by the LockType
    template parameter:

    @param LockType  The type of lock to use. To share the pool between threads
                      a CriticalSection is needed. To use the pool without any
                      locking, a DummyCriticalSection may be used.

    @see ScopedAudioSampleBuffer

    @ingroup vf_audio
*/

class AudioBufferPool
{
public:
  class Buffer : public juce::AudioSampleBuffer
  {
  public:
	Buffer (int numChannels, int numSamples);

	void resize (int newNumChannels, int newNumSamples);

	int getNumSamplesAllocated () const;

  private:
	int m_samplesAllocated;
  };

  /** Create a new, empty pool.
  
      The pool starts with enough slots for 10 buffers. More slots are
      automatically allocated if needed.
  */
  AudioBufferPool ();

  /** Destroy a pool.

	  All allocated buffers are freed. Any previously requested buffers must
      already be released.
  */
  ~AudioBufferPool ();

  /** Request a temporary buffer.

      A buffer is returned from the pool with the specified number of channels
      and enough room for at least numSamples. The buffer is taken out of the
      pool, until it is released with a matching call to releaseBuffer().

      @param numChannels  The number of channels requested.

      @param numSamples   The number of samples per channel requested.

      @return             The resulting buffer.
  */
  virtual Buffer* requestBuffer (int numChannels, int numSamples) = 0;

  /** Releases a previously requested buffer back into the pool.

      @param buffer The buffer to release. This must have been provided by an
                    earlier call to requestBuffer()
  */
  virtual void releaseBuffer (Buffer* buffer) = 0;

protected:
  Buffer* requestBufferInternal (int numChannels, int numSamples);
  void releaseBufferInternal (Buffer* buffer);

private:
  juce::Array <Buffer*> m_buffers;
};

//==============================================================================

template <class LockType = CriticalSection>
class AudioBufferPoolType
  : public AudioBufferPool
  , public LeakChecked <AudioBufferPoolType <LockType> >
{
public:
  Buffer* requestBuffer (int numChannels, int numSamples)
  {
    LockType::ScopedLockType lock (m_mutex);

    return requestBufferInternal (numChannels, numSamples);
  }

  void releaseBuffer (Buffer* buffer)
  {
    LockType::ScopedLockType lock (m_mutex);

    releaseBufferInternal (buffer);
  }    

private:
  LockType m_mutex;
};

#endif
