// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

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
