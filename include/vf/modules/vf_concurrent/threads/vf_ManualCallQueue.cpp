// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

ManualCallQueue::ManualCallQueue (String name)
  : CallQueue (name)
{
}

void ManualCallQueue::close ()
{
  CallQueue::close ();
}

bool ManualCallQueue::synchronize ()
{
  return CallQueue::synchronize ();
}

void ManualCallQueue::signal ()
{
}

void ManualCallQueue::reset ()
{
}
