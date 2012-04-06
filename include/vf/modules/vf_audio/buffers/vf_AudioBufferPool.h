// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_AUDIOBUFFERPOOL_VFHEADER
#define VF_AUDIOBUFFERPOOL_VFHEADER

//==============================================================================
/**
	A pool of multiple AudioSampleBuffer that get re-used.

	This is useful when you need to allocate temporary buffers in your
	audioDeviceIOCallback routines, but don't want to incur the penalty of
	allocating and freeing every time. This class intelligently recycles
	buffers to match your needs.

	Example:

	static AudioBufferPool pool;

	void audioDeviceIOCallback ()
	{
	  // Request a stereo buffer with room for 1024 samples.
	  AudioBufferPool::BufferType* buffer = pool.requestBuffer (2, 1024);

	  // (Process buffer)

	  // Release the buffer to be re-used later.
	  pool.releaseBuffer (buffer);
	}
*/

/* Factored base */
namespace detail {

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

}

// Normally you will just use AudioBufferPool
//
template <class BufferClass = detail::AudioBufferPoolBase::Buffer,
		  class MutexType = CriticalSection>
class AudioBufferPoolTemplate
  : public detail::AudioBufferPoolBase
  , public LeakChecked <AudioBufferPoolTemplate <BufferClass> >
{
public:
  typedef typename BufferClass BufferType;

  /** Create a new empty pool with enough slots for 10 buffers. If
	  more slots are needed they will automatically be allocated.
  */
  AudioBufferPoolTemplate ()
  {
	m_buffers.ensureStorageAllocated (10);
  }

  /** Destructor.

	  All allocated buffers are freed.
	  Any previously requested buffers must already be released.
  */
  ~AudioBufferPoolTemplate ()
  {
	for (int i = 0; i < m_buffers.size(); ++i)
	  delete m_buffers[i];
  }    

  /** Requests a buffer with the specified number of channels and
	  enough room for at least numSamples, from the pool. The buffer
	  is owned by the caller until it is released using releaseBuffer().

	  @see releaseBuffer
  */
  BufferClass* acquireBuffer (int numChannels, int numSamples)
  {
	BufferClass* buffer = 0;
	int samplesNeeded = numChannels * numSamples;

	int index = -1;

	{
	  MutexType::ScopedLockType lock (m_mutex);

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
	}

	if (buffer)
	  buffer->resize (numChannels, numSamples);
	else
	  buffer = new BufferClass (numChannels, numSamples);

	return buffer;
  }

  /** Releases a previously requested buffer back into the pool.

	  @see requestBuffer
  */
  void releaseBuffer (BufferClass* buffer)
  {
	if (buffer)
	{
	  MutexType::ScopedLockType lock (m_mutex);

	  m_buffers.add (buffer);
	}
  }    

private:
  MutexType m_mutex;
  VF_JUCE::Array <BufferClass*> m_buffers;
};

typedef AudioBufferPoolTemplate <detail::AudioBufferPoolBase::Buffer> AudioBufferPool;

//------------------------------------------------------------------------------

// scoped lifetime management for a temporary audio buffer
class ScopedAudioSampleBuffer
  // NO IDEA why the leak checking fails
  // : LeakChecked <ScopedAudioSampleBuffer>, Uncopyable
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

#endif
