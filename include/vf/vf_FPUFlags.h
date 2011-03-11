// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_FPUFLAGS_VFHEADER__
#define __VF_FPUFLAGS_VFHEADER__

//
// Cross platform interface for inspecting and changing IEEE FPU flags
//

class FPUFlags
{
public:
  struct Flag
  {
    Flag () : m_set (false) { }
    Flag (const Flag& flag) : m_set (flag.m_set), m_value (flag.m_value) { }
    Flag& operator= (const Flag& flag)
      { m_set = flag.m_set; m_value = flag.m_value; return *this; }
    bool is_set () const { return m_set; }
    bool value () const { assert (m_set); return m_value; }
    void set_value (bool value) { m_set = true; m_value = value; }
    void clear () { m_set = false; }

  private:
    bool m_set : 1;
    bool m_value : 1;
  };

  template <typename Constants>
  struct Enum
  {
    Enum () : m_set (false) { }
    Enum (const Enum& value) : m_set (value.m_set), m_value (value.m_value) { }
    Enum& operator= (const Enum& value)
      { m_set = value.m_set; m_value = value.m_value; return *this; }
    bool is_set () const { return m_set; }
    Constants value () const { return m_value; }
    void set_value (Constants value) { m_set = true; m_value = value; }
    void clear () { m_set = false; }

  private:
    bool m_set : 1;
    Constants m_value;
  };

public:
  //
  // Exception masks
  //

  void setMaskNaNs        (bool mask = true) { m_maskNaNs.set_value (mask); }
  void setMaskDenormals   (bool mask = true) { m_maskDenormals.set_value (mask); }
  void setMaskZeroDivides (bool mask = true) { m_maskZeroDivides.set_value (mask); }
  void setMaskOverflows   (bool mask = true) { m_maskOverflows.set_value (mask); }
  void setMaskUnderflows  (bool mask = true) { m_maskUnderflows.set_value (mask); }
//void setMaskInexacts    (bool mask = true) { m_maskInexacts.set_value (mask); }

  void setUnmaskAllExceptions (bool unmask = true)
  {
    setMaskNaNs (!unmask);
    setMaskDenormals (!unmask);
    setMaskZeroDivides (!unmask);
    setMaskOverflows (!unmask);
    setMaskUnderflows (!unmask);
  //setMaskInexacts (!unmask);
  }

  //
  // Denormal control
  //

  void setFlushDenormals (bool flush = true) { m_flushDenormals.set_value (flush); }

  //
  // Infinity control
  //

  void setInfinitySigned (bool is_signed = true) { m_infinitySigned.set_value (is_signed); }

  //
  // Rounding control
  //

  enum Rounding
  {
    roundChop,
    roundUp,
    roundDown,
    roundNear
  };

  void setRounding (Rounding rounding) { m_rounding.set_value (rounding); }

  //
  // Precision control
  //

  enum Precision
  {
    bits24,
    bits53,
    bits64
  };

  void setPrecision (Precision precision) { m_precision.set_value (precision); }

  //
  // Retrieval
  //

  const Flag getMaskNaNs () const               { return m_maskNaNs; }
  const Flag getMaskDenormals () const          { return m_maskDenormals; }
  const Flag getMaskZeroDivides () const        { return m_maskZeroDivides; }
  const Flag getMaskOverflows () const          { return m_maskOverflows; }
  const Flag getMaskUnderflows () const         { return m_maskUnderflows; }
//const Flag getMaskInexacts () const           { return m_maskInexacts; }
  const Flag getFlushDenormals () const         { return m_flushDenormals; }
  const Flag getInfinitySigned() const          { return m_infinitySigned; }
  const Enum <Rounding> getRounding () const    { return m_rounding; }
  const Enum <Precision> getPrecision () const  { return m_precision; }

  Flag& getMaskNaNs ()                          { return m_maskNaNs; }
  Flag& getMaskDenormals ()                     { return m_maskDenormals; }
  Flag& getMaskZeroDivides ()                   { return m_maskZeroDivides; }
  Flag& getMaskOverflows ()                     { return m_maskOverflows; }
  Flag& getMaskUnderflows ()                    { return m_maskUnderflows; }
//Flag& getMaskInexacts ()                      { return m_maskInexacts; }
  Flag& getFlushDenormals ()                    { return m_flushDenormals; }
  Flag& getInfinitySigned()                     { return m_infinitySigned; }
  Enum <Rounding>& getRounding ()               { return m_rounding; }
  Enum <Precision>& getPrecision ()             { return m_precision; }

  // Clears our flags if they are not set in another object
  void clearUnsetFlagsFrom (const FPUFlags& flags);

  // Retrieve the current flags fron the FPU
  static FPUFlags getCurrent ();

  // Change the current FPU flags based on what is set in flags
  static void setCurrent (const FPUFlags& flags);

private:
  Flag m_maskNaNs;
  Flag m_maskDenormals;
  Flag m_maskZeroDivides;
  Flag m_maskOverflows;
  Flag m_maskUnderflows;
//Flag m_maskInexacts;
  Flag m_flushDenormals;
  Flag m_infinitySigned;
  Enum <Rounding> m_rounding;
  Enum <Precision> m_precision;
};

//------------------------------------------------------------------------------

//
// Allows the FPU flags to be set and restored in a scope.
//
class ScopedFPUFlags
{
public:
  ScopedFPUFlags (const FPUFlags& flagsToSet)
  {
    m_savedFlags = FPUFlags::getCurrent ();
    m_savedFlags.clearUnsetFlagsFrom (flagsToSet);
    FPUFlags::setCurrent (flagsToSet);
  }

  ~ScopedFPUFlags ()
  {
    FPUFlags::setCurrent (m_savedFlags);
  }

private:
  FPUFlags m_savedFlags;
};

#if 0

// Example for unmasking FPU exceptions
inline void usageExample ()
{
  FPUFlags flags;
  flags.setUnmaskAllExceptions ();

  {
    ScopedFPUFlags fpu (flags); 

    // Perform floating point calculations
  }

  // FPU flags are back to what they were now
}

#endif

#endif

