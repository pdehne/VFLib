// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/modules/vf_gui/vf_Static.h"

namespace Facade {

SolidColour::SolidColour ()
  : m_colour (Colours::black)
{
}

void SolidColour::setSolidColour (Colour const& colour)
{
  m_colour = colour;
}

void SolidColour::paint (Graphics& g, Rectangle <int> const& bounds)
{
  g.setColour (m_colour);
  g.fillRect (bounds);
}

}

//------------------------------------------------------------------------------

namespace Control {

Static::Static ()
  : ResizableLayout (this)
  , m_connectedEdgeFlags (0)
{
}

void Static::setConnectedEdges (int flags)
{
  if (m_connectedEdgeFlags != flags)
  {
    m_connectedEdgeFlags = flags;
    repaint ();
  }
}

void Static::paint (Graphics& g)
{
  vf::Facade::Static* facade = dynamic_cast <vf::Facade::Static*> (this);

  if (facade != nullptr)
  {
    updateFacade ();

    facade->paint (g, getLocalBounds ());
  }
}

void Static::paintOverChildren (Graphics& g)
{
  vf::Facade::Static* facade = dynamic_cast <vf::Facade::Static*> (this);

  if (facade != nullptr)
  {
    facade->paintOverChildren (g, getLocalBounds ());
  }
}

void Static::updateFacade ()
{
  vf::Facade::Static* facade = dynamic_cast <vf::Facade::Static*> (this);

  if (facade != nullptr)
    facade->setConnectedEdgeFlags (m_connectedEdgeFlags);
}

}

END_VF_NAMESPACE
