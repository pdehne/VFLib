// Copyright (C) 2008 by One Guy Group, Inc., All rights reserved worldwide.

#ifndef VF_UI_COLOUREDIMAGEMASKFACADE_H
#define VF_UI_COLOUREDIMAGEMASKFACADE_H

namespace Ui {

namespace Facade {

// Takes a single channel grayscale image and uses it as a
// mask for drawing with a solid color.

class ColouredImageMask : public Facade::Base
{
public:
  ColouredImageMask (Colour colour,
                     const Image& image,
                     Justification justification = Justification::centred,
                     Colour dropShadowColour = Colour::fromRGBA(0, 0, 0, 64));

  void paint (Graphics& g, const Rectangle<int>& bounds);

protected:
  void colourImageAt (Graphics& g, int x, int y, const Rectangle<int>& clip);

private:
  Image m_image;
  Colour m_colour;
  Colour m_dropShadowColour;
  Justification m_justification;

  JUCE_LEAK_DETECTOR(ColouredImageMask)
};

}

}

#endif