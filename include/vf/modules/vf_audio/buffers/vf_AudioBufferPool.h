// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_AUDIOBUFFERPOOL_VFHEADER
#define VF_AUDIOBUFFERPOOL_VFHEADER

//==============================================================================
/**
    A pool of reusable AudioSampleBuffer.

    This is useful when you need to allocate temporary buffers in your
    audioDeviceIOCallback routines, but don't want to incur the penalty of
    allocating and freeing every time. This class intelligently recycles
    buffers to match your needs. It is easy to use:

    @code
	
    AudioBufferPoolType <DummyCriticalSection> pool;

    void audioDeviceIOCallback (const float** inputChannelData,
							    int numInputChannels,
							    float** outputChannelData,
							    int numOutputChannels,
							    int numSamples)
    {
      // Request a stereo buffer with room for 1024 samples.
      AudioBufferPool::Buffer* buffer = pool.requestBuffer (2, 1024);

      // (Process buffer)

      // Release the buffer to be re-used later.
      pool.releaseBuffer (buffer);
    }

    @endcode

    Since Buffer is derived from AudioSampleBuffer, it can be used anywhere
    an AudioSampleBuffer is expected. This example requests a temporary buffer
    and stores it in an AudioSourceChannelInfo:

    @code

    {
      // Request a stereo buffer with room for 1024 samples.
      AudioBufferPool::Buffer* buffer = pool.requestBuffer (2, 1024);

      AudioSourceChannelInfo info;
      info.buffer = buffer;         // allowed, since AudioBufferPool::Buffer *
                                    // is-a AudioSampleBuffer *
      info.startSample = 0;
      info.numSamples = 1024;
      info.clearActiveBufferRegion ();

    }
    
    @endcode

    @param LockType  The type of lock to use. To share the pool between threads
                     a CriticalSection is needed. To use the pool without any
                     locking, a DummyCriticalSection may be used.
*/

class AudioBufferPool
{
public:
#ifndef DOXYGEN
  class Buffer : public VF_JUCE::AudioSampleBuffer
  {
  public:
	Buffer (int numChannels, int numSamples);

	void resize (int newNumChannels, int newNumSamples);

	int getNumSamplesAllocated () const;

  private:
	int m_samplesAllocated;
  };
#endif

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

      @see releaseBuffer
  */
  virtual Buffer* acquireBuffer (int numChannels, int numSamples) = 0;

  /** Releases a previously requested buffer back into the pool.

	  @see requestBuffer
  */
  virtual void releaseBuffer (Buffer* buffer) = 0;

protected:
  Buffer* acquireBufferInternal (int numChannels, int numSamples);
  void releaseBufferInternal (Buffer* buffer);

private:
  VF_JUCE::Array <Buffer*> m_buffers;
};

//==============================================================================

#ifndef DOXYGEN
template <class LockType = CriticalSection>
class AudioBufferPoolType
  : public AudioBufferPool
  , public LeakChecked <AudioBufferPoolType <LockType> >
{
public:
  Buffer* acquireBuffer (int numChannels, int numSamples)
  {
    LockType::ScopedLockType lock (m_mutex);

    return acquireBufferInternal (numChannels, numSamples);
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

//------------------------------------------------------------------------------

/** Scoped lifetime temporary audio buffer.

    This utility class allows scoped lifetime management for acquiring temporary
    audio buffers during processing. It is easy to use:

    @code

    AudioBufferPoolType <CriticalSection> pool;

    {
      // Request a stereo buffer with room for 1024 samples.
      ScopedAudioSampleBuffer buffer (pool, 2, 1024);

      // `buffer` is released when it goes out of scope.
    }

    @endcode

    ScopedAudioSampleBuffer is freely convertible to AudioSampleBuffer* so it
    can be used anywhere a pointer to AudioSampleBuffer is expected. The
    dereference and pointer to member operators are similarly overloaded to
    support transparent usage of the underlying AudioSampleBuffer:

    @code

    AudioBufferPoolType <CriticalSection> pool;

    {
      ScopedAudioSampleBuffer buffer (pool, 2, 1024);

      // Call a member of AudioSampleBuffer
      buffer->clear ();
    }

    @endcode

    Note that changing the size of a buffer is undefined.
*/
class ScopedAudioSampleBuffer
  // NO IDEA why the leak checking fails
  // : LeakChecked <ScopedAudioSampleBuffer>, Uncopyable
{
public:
  /** Acquire a ScopedAudioSampleBuffer from a pool.

      @param numChannels  The number of channels requested.
      @param numSamples   The number of samples per channel requested.
  */
  ScopedAudioSampleBuffer (AudioBufferPool& pool,
	int numChannels,
	int numSamples)
	: m_pool (pool)
	, m_buffer (pool.acquireBuffer (numChannels, numSamples))
  {
  }

  /** Destroy the ScopedAudioSampleBuffer, releasing the buffer */
  ~ScopedAudioSampleBuffer ()
  {
	m_pool.releaseBuffer (m_buffer);
  }

  /** Return the object as a AudioSampleBuffer pointer */
  VF_JUCE::AudioSampleBuffer* getBuffer ()
  {
	return m_buffer;
  }

  /** Return the object as a AudioSampleBuffer pointer */
  VF_JUCE::AudioSampleBuffer* operator-> ()
  {
	return getBuffer();
  }

  /** Return the object as a reference to AudioSampleBuffer */
  VF_JUCE::AudioSampleBuffer& operator* ()
  {
	return *getBuffer();
  }

  /** Conversion operator for pointer to AudioSampleBuffer */
  operator VF_JUCE::AudioSampleBuffer* ()
  {
	return getBuffer();
  }

private:
  AudioBufferPool& m_pool;
  AudioBufferPool::Buffer* const m_buffer;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScopedAudioSampleBuffer);
};

#endif
