// Copyright (C) 2008 by One Guy Group, Inc., All rights reserved worldwide.

namespace Ui {

namespace Facade {

SolidColour::SolidColour (Colour colour)
  : m_colour (colour)
{
}

BorderSize<int> SolidColour::getTransparency ()
{
  return m_colour.isOpaque() ? fullyOpaque : fullyTransparent;
}

void SolidColour::paint (Graphics& g, const Rectangle<int>& bounds)
{
  g.setColour (m_colour);
  g.fillRect (bounds);
}

}

}
