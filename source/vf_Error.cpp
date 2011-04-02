// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_Error.h"

Error::Error()
  : m_code (success)
  , m_lineNumber (0)
  , m_needsToBeChecked (true)
  , m_szWhat (0)
{
}

Error::Error (const Error& other)
  : m_code (other.m_code)
  , m_reasonText (other.m_reasonText)
  , m_sourceFileName (other.m_sourceFileName)
  , m_lineNumber (other.m_lineNumber)
  , m_needsToBeChecked (true)
  , m_szWhat (0)
{
  other.m_needsToBeChecked = false;
}

Error::~Error()
{
  /* If this goes off it means an error object was created but never tested */
  jassert (!m_needsToBeChecked);
}

Error& Error::operator= (const Error& other)
{
  m_code = other.m_code;
  m_reasonText = other.m_reasonText;
  m_sourceFileName = other.m_sourceFileName;
  m_lineNumber = other.m_lineNumber;
  m_needsToBeChecked = true;
  m_what = String::empty;
  m_szWhat = 0;

  other.m_needsToBeChecked = false;

  return *this;
}

Error::Code Error::code () const
{
  m_needsToBeChecked = false;
  return m_code;
}

bool Error::failed () const
{
  return code () != success;
}

#if 0
Error::operator bool () const
{
  return code () != success;
}
#else
bool Error::asBoolean () const
{
  return code () != success;
}
#endif

const String Error::getReasonText () const
{
  return m_reasonText;
}

const String Error::getSourceFilename () const
{
  return m_sourceFileName;
}

int Error::getLineNumber () const
{
  return m_lineNumber;
}

Error& Error::fail (const char* sourceFileName,
                    int lineNumber,
                    const String reasonText,
                    Code errorCode)
{
  jassert (m_code == success);
  jassert (errorCode != success);

  m_code = errorCode;
  m_reasonText = reasonText;
  m_sourceFileName = Debug::getFileNameFromPath (sourceFileName);
  m_lineNumber = lineNumber;
  m_needsToBeChecked = true;

  return *this;
}

Error& Error::fail (const char* sourceFileName,
                    int lineNumber,
                    Code errorCode)
{
  return fail (sourceFileName,
               lineNumber,
               getReasonTextForCode (errorCode), 
               errorCode);
}

void Error::reset ()
{
  m_code = success;
  m_reasonText = String::empty;
  m_sourceFileName = String::empty;
  m_lineNumber = 0;
  m_needsToBeChecked = true;
  m_what = String::empty;
  m_szWhat = 0;
}

void Error::willBeReported () const
{
  m_needsToBeChecked = false;
}

const char* Error::what () const
{
  if (!m_szWhat)
  {
// The application could not be initialized because sqlite was denied access permission
// The application unexpectedly quit because the exception 'sqlite was denied access permission at file ' was thrown
    m_what <<
      m_reasonText << " " <<
      TRANS("at file") << " '" <<
      m_sourceFileName << "' " <<
      TRANS("line") << " " <<
      String (m_lineNumber) << " " <<
      TRANS("with code") << " = " <<
      String (m_code);

    m_szWhat = (const char*)m_what.toUTF8();
  }

  return m_szWhat;
}

const String Error::getReasonTextForCode (Code code)
{
  String s;

  switch (code)
  {
  case success:         s=TRANS("the operation was successful"); break;

  case general:         s=TRANS("a general error occurred"); break;

  case canceled:        s=TRANS("the operation was canceled"); break;
  case exception:       s=TRANS("an exception was thrown"); break;
  case unexpected:      s=TRANS("an unexpected result was encountered"); break;
  case platform:        s=TRANS("a system exception was signaled"); break;

  case noMemory:        s=TRANS("there was not enough memory"); break;
  case noMoreData:      s=TRANS("the end of data was reached"); break;
  case invalidData:     s=TRANS("the data is corrupt or invalid"); break;
  case bufferSpace:     s=TRANS("the buffer is too small"); break;
  case badParameter:    s=TRANS("one or more parameters were invalid"); break;
  case assertFailed:    s=TRANS("an assertion failed"); break;

  case fileInUse:       s=TRANS("the file is in use"); break;
  case fileExists:      s=TRANS("the file exists"); break;
  case fileNoPerm:      s=TRANS("permission was denied"); break;
  case fileIOError:     s=TRANS("an I/O or device error occurred"); break;
  case fileNoSpace:     s=TRANS("there is no space left on the device"); break;
  case fileNotFound:    s=TRANS("the file was not found"); break;
  case fileNameInvalid: s=TRANS("the file name was illegal or malformed"); break;

  default:              s=TRANS("an unknown error code was received"); break;
  }

  return s;
}

END_VF_NAMESPACE

/*
class vf::Worker        // thread queue interface
class vf::GuiWorker     // worker that operates on the ui, using Juce if available
class vf::ManualWorker  // polling style worker, good for audio mixer

class vf::Thread        // thread interface

class vf::ThreadWorker  // base that uses a vf::Thread
class vf::BoostThread ? // implementation using boost threads and interruption point exceptions
class vf::JuceThread  ? // implementation using juce threads and polling

template <>
class vf::Listeners     // set of listeners, each registered to a Worker

*/
