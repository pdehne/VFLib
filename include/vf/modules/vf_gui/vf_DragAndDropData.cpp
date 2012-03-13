// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/modules/vf_gui/vf_DragAndDropData.h"

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

END_VF_NAMESPACE
