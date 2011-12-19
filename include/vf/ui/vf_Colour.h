// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_COLOUR_VFHEADER__
#define __VF_COLOUR_VFHEADER__

class XYZColour
{
public:
  XYZColour ();
  XYZColour (float x, float y, float z);
  XYZColour (float x, float y, float z, float alpha);
  XYZColour (XYZColour const& xyz);
  XYZColour (Colour const& sRGB);

  XYZColour& operator= (XYZColour const& other);

  float getX () const { return m_x; }
  float getY () const { return m_y; }
  float getZ () const { return m_z; }
  float getAlpha () const { return m_alpha; }

  Colour const toRGB () const;

private:
  static XYZColour const from (Colour const& sRGB);

private:
  float m_x;
  float m_y;
  float m_z;
  float m_alpha;
};

//------------------------------------------------------------------------------

class LabColour
{
public:
  LabColour ();
  LabColour (float L, float a, float b);
  LabColour (float L, float a, float b, float alpha);
  LabColour (LabColour const& lab);
  LabColour (Colour const& sRGB);
  LabColour (XYZColour const& xyz);

  float getL () const { return m_L; }
  float getA () const { return m_a; }
  float getB () const { return m_b; }
  float getAlpha () const { return m_alpha; }

  XYZColour const toXYZ () const;
  Colour const toRGB () const;

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
