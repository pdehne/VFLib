// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

#ifndef VF_MANUALCALLQUEUE_VFHEADER
#define VF_MANUALCALLQUEUE_VFHEADER

#include "vf_CallQueue.h"

/** A CallQueue that requires periodic manual synchronization.

    To use this, declare an instance and then place calls into it as usual.
    Every so often, you must call synchronize() from the thread you want to
    associate with the queue. Typically this is done with the
    audioDeviceIOCallback:

    @code

    ManualCallQueue fifo ("Audio CallQueue");

    void audioDeviceIOCallback (const float** inputChannelData,
							    int numInputChannels,
							    float** outputChannelData,
							    int numOutputChannels,
							    int numSamples)
    {
	  fifo.synchronize ();

	  // do audio i/o
    }

    @endcode

	The close() function is provided for diagnostics. Call it as early as
	possible based on the exit or shutdown logic of your application. If calls
    are put into the queue after it is closed, it will generate an exception so
    you can track it down.

    @see CallQueue
*/
class ManualCallQueue : public CallQueue
{
public:
  /** Create a ManualCallQueue.

	  @param name           A string used to help identify the associated
                            thread for debugging.
  */
  explicit ManualCallQueue (String name);

  /** Close the queue. If calls are placed into a closed queue, an exception
	  is thrown.
  */
  void close ();

  /** Synchronize the queue by calling all pending functors. */
  bool synchronize ();

private:
  void signal ();
  void reset ();
};

#endif
