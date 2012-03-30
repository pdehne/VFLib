// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

XYZColour::XYZColour ()
  : m_x (0)
  , m_y (0)
  , m_z (0)
  , m_alpha (0)
{
}

XYZColour::XYZColour (float x, float y, float z)
  : m_x (x)
  , m_y (y)
  , m_z (z)
  , m_alpha (1)
{
}

XYZColour::XYZColour (float x, float y, float z, float alpha)
  : m_x (x)
  , m_y (y)
  , m_z (z)
  , m_alpha (alpha)
{
}

XYZColour::XYZColour (XYZColour const& xyz)
  : m_x (xyz.m_x)
  , m_y (xyz.m_y)
  , m_z (xyz.m_z)
  , m_alpha (xyz.m_alpha)
{
}

XYZColour::XYZColour (VF_JUCE::Colour const& sRGB)
{
  *this = from (sRGB);
}

XYZColour& XYZColour::operator= (XYZColour const& other)
{
  m_x = other.m_x;
  m_y = other.m_y;
  m_z = other.m_z;
  m_alpha = other.m_alpha;

  return *this;
}

XYZColour const XYZColour::from (VF_JUCE::Colour const& sRGB)
{
  float r = sRGB.getRed   () / 255.f;
  float g = sRGB.getGreen () / 255.f;
  float b = sRGB.getBlue  () / 255.f;

  if (r > 0.04045f) r = 100 * pow ((r + 0.055) / 1.055, 2.4); else r = r / 12.92;
  if (g > 0.04045f) g = 100 * pow ((g + 0.055) / 1.055, 2.4); else g = g / 12.92;
  if (b > 0.04045f) b = 100 * pow ((b + 0.055) / 1.055, 2.4); else b = b / 12.92;

  // D65
  float x = r * 0.4124 + g * 0.3576 + b * 0.1805;
  float y = r * 0.2126 + g * 0.7152 + b * 0.0722;
  float z = r * 0.0193 + g * 0.1192 + b * 0.9505;

  return XYZColour (x, y, z, sRGB.getAlpha() / 255.f);
}

VF_JUCE::Colour const XYZColour::toRGB () const
{
  float x = m_x / 100;
  float y = m_y / 100;
  float z = m_z / 100;

  float r = x *  3.2406 + y * -1.5372 + z * -0.4986;
  float g = x * -0.9689 + y *  1.8758 + z *  0.0415;
  float b = x *  0.0557 + y * -0.2040 + z *  1.0570;

  if (r > 0.0031308) r = 1.055 * pow (double (r), 1/2.4) - 0.055; else r = 12.92 * r;
  if (g > 0.0031308) g = 1.055 * pow (double (g), 1/2.4) - 0.055; else g = 12.92 * g;
  if (b > 0.0031308) b = 1.055 * pow (double (b), 1/2.4) - 0.055; else b = 12.92 * b;

  return Colour::fromFloatRGBA  (r, g, b, m_alpha);
}
