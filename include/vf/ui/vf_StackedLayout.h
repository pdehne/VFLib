// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_STACKEDLAYOUT_VFHEADER__
#define __VF_STACKEDLAYOUT_VFHEADER__

#include "vf/ui/vf_ResizableLayout.h"

class StackedLayout
  : public Component
  , public ResizableChild
  , private ComponentListener
  , private AsyncUpdater
{
public:
  class Band
  {
  public:
    Band ();

  private:
    friend class StackedLayout;

    bool m_resizable;
    Component* m_component;
  };

public:
  StackedLayout (bool vertical,
                 BorderSize <int> borderSize,
                 int gapSize);
  ~StackedLayout ();

  void addBand (Component* component);

  void setBandVisible (int index, bool visible);

  void recalculateLayout ();

  void activateStackedLayout ();

  void resized ();

private:
  void resizeStart ();

  void componentMovedOrResized (Component& component,
                                bool wasMoved,
                                bool wasResized);

  void componentVisibilityChanged (Component& component);

  void componentBeingDeleted (Component& component);

  void handleAsyncUpdate ();

private:
  struct Item
  {
    Component* component;
    ResizableChild* resizableChild;
  };

  bool const m_vertical;
  bool m_active;
  const BorderSize<int> m_borderSize;
  const int m_gapSize;
  std::vector <Item> m_bands; // yuck
};

#endif
