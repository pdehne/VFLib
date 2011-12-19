// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_VERTICALGRADIENT_VFHEADER__
#define __VF_VERTICALGRADIENT_VFHEADER__

// Simple class to facilitate setting vertical blends
//
class VerticalGradient : public VF_JUCE::ColourGradient
{
public:
  VerticalGradient (
    Colour const& startColour,
    Colour const& endColour,
    Rectangle <int> const& bounds)
    : ColourGradient (
        startColour, bounds.getX (), bounds.getY () + .5f,
        endColour, bounds.getX (), bounds.getBottom () -.5f,
        false)
  {
  }
};

#endif
