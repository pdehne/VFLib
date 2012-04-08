// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

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
        startColour, float (bounds.getX ()), float (bounds.getY ()) + .5f,
        endColour, float (bounds.getX ()), float (bounds.getBottom ()) -.5f,
        false)
  {
  }
};

#endif
