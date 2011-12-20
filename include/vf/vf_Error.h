// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_ERROR_VFHEADER__
#define __VF_ERROR_VFHEADER__

#include "vf/vf_SafeBool.h"
#include "vf/vf_String.h"

// Yet another error reporting class. This one records the file/line
// where the error occurred along with some human readable text. It
// can be freely copied and passed around as a return value and all that.

class Error
  : public std::exception
  , public SafeBool <Error>
{
public:
  // These are for when the caller wants to distinguish
  // results, for example, knowing that a file is locked.
  enum Code
  {
    success,        // "the operation was successful"

    general,			  // "a general error occurred"

    canceled,			  // "the operation was canceled"
    exception,		  // "an exception was thrown"
    unexpected,     // "an unexpected result was encountered"
    platform,       // "a system exception was signaled"

    noMemory,       // "there was not enough memory"
    noMoreData,			// "the end of data was reached"
    invalidData,		// "the data is corrupt or invalid"
    bufferSpace,		// "the buffer is too small"
    badParameter,		// "one or more parameters were invalid"
    assertFailed,		// "an assertion failed"

    fileInUse,			// "the file is in use"
    fileExists,			// "the file exists"
    fileNoPerm,			// "permission was denied" (file attributes conflict)
    fileIOError,		// "an I/O or device error occurred"
    fileNoSpace,		// "there is no space left on the device"
    fileNotFound,		// "the file was not found"
    fileNameInvalid	// "the file name was illegal or malformed"
  };

  Error ();
  Error (const Error& other);
  Error& operator= (const Error& other);

  virtual ~Error() noexcept;

  Code code () const;
  bool failed () const;

  bool asBoolean () const;

  const String getReasonText () const;
  const String getSourceFilename () const;
  int getLineNumber () const;

  Error& fail (const char* sourceFileName,
               int lineNumber,
               const String reasonText,
               Code errorCode = general);

  Error& fail (const char* sourceFileName,
               int lineNumber,
               Code errorCode = general);

  // A function that is capable of recovering from an error (for
  // example, by performing a different action) can reset the
  // object so it can be passed up.
  void reset ();

  // Call this when reporting the error to clear the "checked" flag
  void willBeReported () const;

  // for std::exception. This lets you throw an Error that should
  // terminate the application. The what() message will be less
  // descriptive so ideally you should catch the Error object instead.
  const char* what() const noexcept;

  static const String getReasonTextForCode (Code code);

private:
  Code m_code;
  String m_reasonText;
  String m_sourceFileName;
  int m_lineNumber;
  mutable bool m_needsToBeChecked;

  mutable String m_what; // created on demand
  mutable const char* m_szWhat;
};

#endif
