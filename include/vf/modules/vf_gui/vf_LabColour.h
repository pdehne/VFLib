// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_LABCOLOUR_VFHEADER
#define VF_LABCOLOUR_VFHEADER

#include "vf_XYZColour.h"

class LabColour
{
public:
  LabColour ();
  LabColour (float L, float a, float b);
  LabColour (float L, float a, float b, float alpha);
  LabColour (LabColour const& lab);
  LabColour (VF_JUCE::Colour const& sRGB);
  LabColour (XYZColour const& xyz);

  float getL () const { return m_L; }
  float getA () const { return m_a; }
  float getB () const { return m_b; }
  float getAlpha () const { return m_alpha; }

  XYZColour const toXYZ () const;
  VF_JUCE::Colour const toRGB () const;

  LabColour const withLuminance (float L) const; // L [0,1]
  LabColour const withAddedLuminance (float amount) const; // amount [0,1]
  LabColour const withMultipliedColour (float amount) const;

private:
  static LabColour const from (XYZColour const& xyz);

private:
  float m_L;
  float m_a;
  float m_b;
  float m_alpha;
};

#endif
