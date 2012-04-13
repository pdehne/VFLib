// Copyright (C) 2008 by One Guy Group, Inc., All rights reserved worldwide.

#ifndef VF_UI_SOLIDCOLOURFACADE_H
#define VF_UI_SOLIDCOLOURFACADE_H

namespace Ui {

namespace Facade {

class SolidColour : public Facade::Base
{
public:
  explicit SolidColour (Colour colour);

  BorderSize<int> getTransparency ();

  void paint (Graphics& g, const Rectangle<int>& bounds);

private:
  Colour m_colour;

  JUCE_LEAK_DETECTOR (SolidColour)
};

}

}

#endif