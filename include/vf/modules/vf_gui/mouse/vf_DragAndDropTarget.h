//==============================================================================
//
// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file GNU_GPL_v2.txt for full licensing terms.
// 
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your option)
// any later version.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
// 
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 51
// Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
// 
//==============================================================================

#ifndef VF_DRAGANDDROPTARGET_VFHEADER
#define VF_DRAGANDDROPTARGET_VFHEADER

#include "vf_DragAndDropData.h"

//==============================================================================
/**
    DragAndDropTarget wrapper supporting the DragAndDropData container.

    \ingroup vf_gui
*/
class DragAndDropTarget : public juce::DragAndDropTarget
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

