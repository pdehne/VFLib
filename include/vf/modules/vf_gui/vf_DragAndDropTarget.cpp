// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/modules/vf_gui/vf_DragAndDropTarget.h"

bool DragAndDropTarget::isInterestedInDragSource (const SourceDetails& dragSourceDetails)
{
  DragAndDropData* data = dragSourceDetails.description.isObject () ?
    dynamic_cast <DragAndDropData*> (dragSourceDetails.description.getObject ()) :
    0;
  if (data)
    return this->isInterestedInDragSource (*data, dragSourceDetails);
  else
    return false;
}

void DragAndDropTarget::itemDragEnter (const SourceDetails& dragSourceDetails)
{
  DragAndDropData* data = dragSourceDetails.description.isObject () ?
    dynamic_cast <DragAndDropData*> (dragSourceDetails.description.getObject ()) :
    0;
  if (data)
    this->itemDragEnter (*data, dragSourceDetails);
}

void DragAndDropTarget::itemDragMove (const SourceDetails& dragSourceDetails)
{
  DragAndDropData* data = dragSourceDetails.description.isObject () ?
    dynamic_cast <DragAndDropData*> (dragSourceDetails.description.getObject ()) :
    0;
  if (data)
    this->itemDragMove (*data, dragSourceDetails);
}

void DragAndDropTarget::itemDragExit (const SourceDetails& dragSourceDetails)
{
  DragAndDropData* data = dragSourceDetails.description.isObject () ?
    dynamic_cast <DragAndDropData*> (dragSourceDetails.description.getObject ()) :
    0;
  if (data)
    this->itemDragExit (*data, dragSourceDetails);
}

void DragAndDropTarget::itemDropped (const SourceDetails& dragSourceDetails)
{
  DragAndDropData* data = dragSourceDetails.description.isObject () ?
    dynamic_cast <DragAndDropData*> (dragSourceDetails.description.getObject ()) :
    0;
  if (data)
    this->itemDropped (*data, dragSourceDetails);
}

END_VF_NAMESPACE
