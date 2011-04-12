// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

#include <iostream>

BEGIN_VF_NAMESPACE

#include "vf/vf_CatchAny.h"

END_VF_NAMESPACE

//------------------------------------------------------------------------------
//
// Windows structured exception handling
//
#if _MSC_VER

#include <windows.h>

BEGIN_VF_NAMESPACE

#include "vf/vf_Mutex.h"

namespace {

//
// While this object is in scope, any Windows SEH
// exceptions will be caught and re-thrown as an Error object.
//
class ScopedPlatformExceptionCatcher : NonCopyable
{
public:
  ScopedPlatformExceptionCatcher ()
  {
    s_mutex.enter ();

    if (++s_count == 1)
      s_sehPrev = ::SetUnhandledExceptionFilter (sehFilter);

    s_mutex.exit ();
  }

  ~ScopedPlatformExceptionCatcher ()
  {
    s_mutex.enter ();

    if (--s_count == 0)
      SetUnhandledExceptionFilter (s_sehPrev);

    s_mutex.exit ();
  }

  static LONG WINAPI sehFilter (_EXCEPTION_POINTERS* ei)
  {
    EXCEPTION_RECORD* er = ei->ExceptionRecord;

    if (er->ExceptionCode == EXCEPTION_BREAKPOINT ||
        er->ExceptionCode == EXCEPTION_SINGLE_STEP)
    {
      // pass through
    }
    else
    {
      String s;

      switch (er->ExceptionCode)
      {
      case EXCEPTION_ACCESS_VIOLATION: s=TRANS("an access violation occurred"); break;
      case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: s=TRANS("array bounds were exceeded"); break;
      case EXCEPTION_DATATYPE_MISALIGNMENT: s=TRANS("memory access was unaligned"); break;
      case EXCEPTION_FLT_DENORMAL_OPERAND: s=TRANS("a floating point operation produced a denormal"); break;
      case EXCEPTION_FLT_DIVIDE_BY_ZERO: s=TRANS("a floating point divide by zero was attempted"); break;
      case EXCEPTION_FLT_INEXACT_RESULT: s=TRANS("the floating point operation was unrepresentable"); break;
      case EXCEPTION_FLT_INVALID_OPERATION: s=TRANS("the floating point operation was invalid"); break;
      case EXCEPTION_FLT_OVERFLOW: s=TRANS("the floating point operation overflowed"); break;
      case EXCEPTION_FLT_STACK_CHECK: s=TRANS("a stack check resulted from a floating point operation"); break;
      case EXCEPTION_FLT_UNDERFLOW: s=TRANS("the floating point operation underflowed"); break;
      case EXCEPTION_ILLEGAL_INSTRUCTION: s=TRANS("an invalid instruction was received"); break;
      case EXCEPTION_IN_PAGE_ERROR: s=TRANS("a virtual paging error occurred"); break;
      case EXCEPTION_INT_DIVIDE_BY_ZERO: s=TRANS("an integer divide by zero was attempted"); break;
      case EXCEPTION_INT_OVERFLOW: s=TRANS("an integer operation overflowed"); break;
      case EXCEPTION_INVALID_DISPOSITION: s=TRANS("the exception handler returned an invalid disposition"); break;
      case EXCEPTION_NONCONTINUABLE_EXCEPTION: s=TRANS("a non-continuable exception occurred"); break;
      case EXCEPTION_PRIV_INSTRUCTION: s=TRANS("a privileged instruction was attempted"); break;  
      case EXCEPTION_STACK_OVERFLOW: s=TRANS("the stack overflowed"); break;
      default:
        s=TRANS("an unknown system exception of code ");
        s << String((unsigned int)er->ExceptionCode);
        s << " " << TRANS("occurred");
        break;
     }

      Throw (Error().fail (__FILE__, __LINE__, s, Error::platform));
    }

    return s_sehPrev (ei);
  }

private:
  static int s_count;
  static Mutex s_mutex;
  static LPTOP_LEVEL_EXCEPTION_FILTER s_sehPrev;
};

Mutex ScopedPlatformExceptionCatcher::s_mutex;
int ScopedPlatformExceptionCatcher::s_count = 0;
LPTOP_LEVEL_EXCEPTION_FILTER ScopedPlatformExceptionCatcher::s_sehPrev = 0;

}

END_VF_NAMESPACE

//------------------------------------------------------------------------------

#else

// TODO: POSIX SIGNAL HANDLER

#pragma message(VF_LOC_"Missing class ScopedPlatformExceptionCatcher")

BEGIN_VF_NAMESPACE

namespace {

class ScopedPlatformExceptionCatcher
{
public:
  // Missing
}

}

END_VF_NAMESPACE

#endif

//------------------------------------------------------------------------------

BEGIN_VF_NAMESPACE

bool CatchAny (Function <void (void)> f, bool returnFromException)
{
  bool caughtException = true; // assume the worst

#if 0
  if (Debug::isDebuggerAttached ())
  {
    f ();
    caughtException = false;
  }
  else
#endif
  {
    try
    {
      ScopedPlatformExceptionCatcher platformExceptionCatcher;
    
      f ();

      caughtException = false;
    }
#if VF_HAVE_JUCE
    catch (Error& e)
    {
      if (!returnFromException)
      {
        VF_JUCE::JUCEApplication* app = VF_JUCE::JUCEApplication::getInstance();

        if (app)
        {
          app->unhandledException (
            &e,
            e.getSourceFilename(),
            e.getLineNumber());
        }
        else
        {
          std::cout << e.what ();
          std::unexpected ();
        }
      }
    }
    catch (std::exception& e)
    {
      if (!returnFromException)
      {
        VF_JUCE::JUCEApplication* app = VF_JUCE::JUCEApplication::getInstance();

        if (app)
        {
          app->unhandledException (&e, __FILE__, __LINE__);
        }
        else
        {
          std::cout << e.what ();
          std::unexpected ();
        }
      }
    }
    catch (...)
    {
      if (!returnFromException)
      {
        VF_JUCE::JUCEApplication* app = VF_JUCE::JUCEApplication::getInstance();

        if (app)
        {
          app->unhandledException (0, __FILE__, __LINE__);
        }
        else
        {
          std::unexpected ();
        }
      }
    }
#else
    catch (std::exception& e)
    {
      if (!returnFromException)
      {
        std::cout << e.what ();
        std::unexpected ();
      }
    }
    catch (...)
    {
      if (!returnFromException)
        std::unexpected ();
    }
#endif
  }

  return caughtException;
}

END_VF_NAMESPACE
