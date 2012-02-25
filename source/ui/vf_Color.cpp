// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/ui/vf_Colour.h"

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

XYZColour::XYZColour (Colour const& sRGB)
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

XYZColour const XYZColour::from (Colour const& sRGB)
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

Colour const XYZColour::toRGB () const
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

//------------------------------------------------------------------------------

LabColour::LabColour ()
  : m_L (0)
  , m_a (0)
  , m_b (0)
  , m_alpha (0)
{
}

LabColour::LabColour (float L, float a, float b)
  : m_L (L)
  , m_a (a)
  , m_b (b)
  , m_alpha (1)
{
}

LabColour::LabColour (float L, float a, float b, float alpha)
  : m_L (L)
  , m_a (a)
  , m_b (b)
  , m_alpha (alpha)
{
}

LabColour::LabColour (LabColour const& lab)
  : m_L (lab.m_L)
  , m_a (lab.m_a)
  , m_b (lab.m_b)
  , m_alpha (lab.m_alpha)
{
}

LabColour::LabColour (Colour const& sRGB)
{
  *this = from (sRGB);
}

LabColour::LabColour (XYZColour const& xyz)
{
  *this = from (xyz);
}

XYZColour const LabColour::toXYZ () const
{
  // D65, Observer= 2°
  float const x0 = 95.047f;
  float const y0 = 100.000f;
  float const z0 = 108.883f;

  float y = (m_L + 16) / 116;
  float x = m_a / 500 + y;
  float z = y - m_b / 200;

  float t;

  if ((t = pow (x, 3)) > 0.008856) x = t; else x = (x - 16./116) / 7.787;
  if ((t = pow (y, 3)) > 0.008856) y = t; else y = (y - 16./116) / 7.787;
  if ((t = pow (z, 3)) > 0.008856) z = t; else z = (z - 16./116) / 7.787;

  x *= x0;
  y *= y0;
  z *= z0;

  return XYZColour (x, y, z, m_alpha);
}

Colour const LabColour::toRGB () const
{
  return toXYZ ().toRGB ();
}

LabColour const LabColour::from (XYZColour const& xyz)
{
  // D65, Observer= 2°
  float const x0 = 95.047f;
  float const y0 = 100.000f;
  float const z0 = 108.883f;

  float x = (xyz.getX () / x0);
  float y = (xyz.getY () / y0);
  float z = (xyz.getZ () / z0);

  x = (x > 0.008856) ? pow (x, 1.f/3) : ((7.7787 * x) + 16/116);
  y = (y > 0.008856) ? pow (y, 1.f/3) : ((7.7787 * y) + 16/116);
  z = (z > 0.008856) ? pow (z, 1.f/3) : ((7.7787 * z) + 16/116);

  float const L = (116 * y) - 16;
  float const a = 500 * (x - y);
  float const b = 200 * (y - z);
 
  return LabColour (L, a, b, xyz.getAlpha ());
}

LabColour const LabColour::withLuminance (float L) const
{
  return LabColour (
    jlimit (0.f, 100.f, L),
    getA (),
    getB (),
    getAlpha ());
}

LabColour const LabColour::withAddedLuminance (float amount) const
{
  return LabColour (
    jlimit (0.f, 100.f, getL() + amount * 100),
    getA (),
    getB (),
    getAlpha ());
}

LabColour const LabColour::withMultipliedColour (float amount) const
{
  return LabColour (
    getL (),
    getA () * amount,
    getB () * amount,
    getAlpha ());
}

//------------------------------------------------------------------------------

END_VF_NAMESPACE
