// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_UI_TRANSPARENTBORDER_VFHEADER__
#define __VF_UI_TRANSPARENTBORDER_VFHEADER__

namespace ui {

// Allows a Component to draw with setOpaque in a rectangular
// subsection of its client area, with the border being transparent.
  //
class TransparentBorder
  : private ComponentListener
  , LeakChecked <TransparentBorder>
{
public:
  TransparentBorder ();
  ~TransparentBorder ();

  enum
  {
    maxBorderSize = 0x1fffffff
  };

  static const BorderSize <int> fullyOpaque;
  static const BorderSize <int> fullyTransparent;

  void setComponent (Component *component,
                     BorderSize <int> borderSize = BorderSize<int>());

private:
  bool isAttached ();
  void updateBounds ();
  void updateZOrder ();
  void componentMovedOrResized (Component& component,
                                bool wasMoved,
                                bool wasResized);
  void componentVisibilityChanged (Component& component);
  void componentChildrenChanged (Component& component);
  void componentParentHierarchyChanged (Component& component);
  void componentBeingDeleted (Component& component);

private:
  class OpaqueComponent : public Component
  {
  public:
    OpaqueComponent ();
    void paint (Graphics& g);
  };

  Component* m_component;
  Component* m_parent;
  ScopedPointer <OpaqueComponent> m_opaque;
  BorderSize <int> m_borderSize;
};

}

#endif