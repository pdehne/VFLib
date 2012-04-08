// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

#ifndef VF_XYZCOLOUR_VFHEADER
#define VF_XYZCOLOUR_VFHEADER

class XYZColour
{
public:
  XYZColour ();
  XYZColour (float x, float y, float z);
  XYZColour (float x, float y, float z, float alpha);
  XYZColour (XYZColour const& xyz);
  XYZColour (VF_JUCE::Colour const& sRGB);

  XYZColour& operator= (XYZColour const& other);

  float getX () const { return m_x; }
  float getY () const { return m_y; }
  float getZ () const { return m_z; }
  float getAlpha () const { return m_alpha; }

  VF_JUCE::Colour const toRGB () const;

private:
  static XYZColour const from (VF_JUCE::Colour const& sRGB);

private:
  float m_x;
  float m_y;
  float m_z;
  float m_alpha;
};

#endif
