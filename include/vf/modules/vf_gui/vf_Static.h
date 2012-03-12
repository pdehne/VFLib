// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_STATIC_VFHEADER__
#define __VF_STATIC_VFHEADER__

#include "vf/modules/vf_gui/vf_UiBase.h"
#include "vf/modules/vf_gui/vf_ResizableLayout.h"

// A Static control has no interaction, just drawing.
// Child components can be added to the ResizableLayout.
//

namespace Facade {

class Static : public ConnectedEdges
{
public:
};

class SolidColour : public Static
{
public:
  SolidColour ();
  void setSolidColour (Colour const& colour);
  void paint (Graphics& g, Rectangle <int> const& bounds);

private:
  Colour m_colour;
};

}

namespace Control {

class Static
  : public Component
  , public ResizableLayout
  , public Base
{
public:
  Static ();

  int getConnectedEdgeFlags () const
    { return m_connectedEdgeFlags; }

  void setConnectedEdges (int flags);

  void setConnectedEdgeFlag (int flag, bool connected)
  {
    if (connected)
      setConnectedEdges (getConnectedEdgeFlags () | flag);
    else
      setConnectedEdges (getConnectedEdgeFlags () & ~flag);
  }

  void setConnectedOnLeft (bool connected)
    { setConnectedEdgeFlag (Button::ConnectedOnLeft, connected); }

  void setConnectedOnRight (bool connected)
    { setConnectedEdgeFlag (Button::ConnectedOnRight, connected); }

  void setConnectedOnTop (bool connected)
    { setConnectedEdgeFlag (Button::ConnectedOnTop, connected); }

  void setConnectedOnBottom (bool connected)
    { setConnectedEdgeFlag (Button::ConnectedOnBottom, connected); }

  void paint (Graphics& g);
  void paintOverChildren (Graphics& g);

protected:
  virtual void updateFacade ();

private:
  int m_connectedEdgeFlags;
};

template <class FacadeType>
class StaticType : public Static, public FacadeType
{
public:
};

}

#endif
