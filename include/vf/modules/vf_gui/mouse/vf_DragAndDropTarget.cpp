// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

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
