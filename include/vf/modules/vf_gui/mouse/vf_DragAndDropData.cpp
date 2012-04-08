// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

DragAndDropData::DragAndDropData ()
{
}

DragAndDropData::~DragAndDropData ()
{
  for (Items::iterator iter = m_items.begin (); iter != m_items.end ();)
  {
    Item* item = &(*iter++);
    delete item;
  }
}
