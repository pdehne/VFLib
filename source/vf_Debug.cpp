// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

#ifdef _MSC_VER
#include <windows.h>
#endif

BEGIN_VF_NAMESPACE

#include "vf/vf_Debug.h"

namespace Debug {

#if VF_DEBUG

//------------------------------------------------------------------------------
//
// Debug::breakPoint
//
#if VF_HAVE_JUCE && JUCE_DEBUG && defined (juce_breakDebugger)
  void breakPoint () { juce_breakDebugger; }

#elif defined (_MSC_VER)
  #pragma intrinsic (__debugbreak)
  void breakPoint () { __debugbreak(); }

#else
  #pragma message(VF_LOC_"Missing Debug::breakPoint()")
  void breakPoint () { }

#endif

//------------------------------------------------------------------------------
//
// Debug::isDebuggerAttached
//
#if VF_HAVE_JUCE
  bool isDebuggerAttached () { return VF_JUCE::juce_isRunningUnderDebugger (); }

#elif defined (_WINDOWS)
  bool isDebuggerAttached () { return IsDebuggerPresent (); }

#else
  #pragma message(VF_LOC_"Missing Debug::isDebuggerAttached()")
  bool isDebuggerAttached () { return false; }

#endif

#endif

//------------------------------------------------------------------------------

const String getFileNameFromPath (const char *sourceFileName)
{
  return String (boost::filesystem::path (sourceFileName).filename().c_str());
}

// Returns a String with double quotes escaped
const String withEscapedQuotes (const String& string)
{
  String escaped;

  int i0 = 0;
  int i;
  do
  {
    i = string.indexOfChar (i0, '"');

    if (i == -1)
    {
      escaped << string.substring (i0, string.length());
    }
    else
    {
      escaped << string.substring (i0, i) << "\\\"";
      i0 = i + 1;
    }
  }
  while (i != -1);

  return escaped;
}

// Converts escaped quotes back into regular quotes
const String withUnescapedQuotes (const String& string)
{
  String unescaped;

  int i0 = 0;
  int i;
  do
  {
    i = string.indexOfChar (i0, '\\');

    if (i == -1)
    {
      unescaped << string.substring (i0, string.length());
    }
    else
    {
      // peek
      if (string.length() > i && string[i+1] == '\"')
      {
        unescaped << string.substring (i0, i) << '"';
        i0 = i + 2;
      }
      else
      {
        unescaped << string.substring (i0, i + 1);
        i0 = i + 1;
      }
    }
  }
  while (i != -1);

  return unescaped;
}


// Converts a String that may contain newlines, into a
// command line where each line is delimited with quotes.
// Any quotes in the actual string will be escaped via \".
String stringToCommandLine (const String& string)
{
  String commandLine;

  int i0 = 0;
  int i;
  for (i = 0; i < string.length(); i++)
  {
    juce_wchar c = string[i];

    if (c == '\n')
    {
      if (i0 != 0)
        commandLine << ' ';
      commandLine << '"' << withEscapedQuotes (string.substring (i0, i)) << '"';
      i0 = i+1;
    }
  }

  if (i0 < i)
  {
    if (i0 != 0)
      commandLine << ' ';
    commandLine << '"' << withEscapedQuotes (string.substring (i0, i)) << '"';
  }

  return commandLine;
}

// Converts a command line consisting of multiple quoted strings
// back into a single string with newlines delimiting each quoted
// string. Escaped quotes \" are turned into real quotes.
String commandLineToString (const String& commandLine)
{
  String string;

  bool quoting = false;
  int i0 = 0;
  int i;
  for (i = 0; i < commandLine.length(); i++)
  {
    juce_wchar c = commandLine[i];

    if (c == '\\')
    {
      // peek
      if (commandLine.length() > i && commandLine[i+1] == '\"')
      {
        i++;
      }
    }
    else if (c == '"')
    {
      if (!quoting)
      {
        i0 = i + 1;
        quoting = true;
      }
      else
      {
        if (!string.isEmpty ())
          string << '\n';
        string << withUnescapedQuotes (commandLine.substring (i0, i));
        quoting = false;
      }
    }
  }

  return string;
}

}

END_VF_NAMESPACE
