// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_DRAGANDDROPTARGET_VFHEADER__
#define __VF_DRAGANDDROPTARGET_VFHEADER__

#include "vf/modules/vf_gui/vf_DragAndDropData.h"

// Puts a front-end on the juce DragAndDropTarget to make it work with
// DragAndDropData.
//

class DragAndDropTarget : public VF_JUCE::DragAndDropTarget
{
public:
  virtual bool isInterestedInDragSource (DragAndDropData const& data,
                                         const SourceDetails& dragSourceDetails)
    { return false; }

  virtual void itemDragEnter (DragAndDropData const& data,
                              const SourceDetails& dragSourceDetails) { }

  virtual void itemDragMove (DragAndDropData const& data,
                             const SourceDetails& dragSourceDetails) { }

  virtual void itemDragExit (DragAndDropData const& data,
                             const SourceDetails& dragSourceDetails) { }

  virtual void itemDropped (DragAndDropData const& data,
                            const SourceDetails& dragSourceDetails) { }

private:
  bool isInterestedInDragSource (const SourceDetails& dragSourceDetails);
  void itemDragEnter (const SourceDetails& dragSourceDetails);
  void itemDragMove (const SourceDetails& dragSourceDetails);
  void itemDragExit (const SourceDetails& dragSourceDetails);
  void itemDropped (const SourceDetails& dragSourceDetails);
};

#endif

