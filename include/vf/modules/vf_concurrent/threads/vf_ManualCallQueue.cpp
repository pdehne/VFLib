// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

ManualCallQueue::ManualCallQueue (String name)
  : CallQueue (name)
{
}

void ManualCallQueue::close ()
{
  CallQueue::close ();
}

bool ManualCallQueue::process ()
{
  return CallQueue::process ();
}

void ManualCallQueue::signal ()
{
}

void ManualCallQueue::reset ()
{
}
