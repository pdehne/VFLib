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

  // Retrieve the current flags fron the FPU
  static inline FPUFlags getCurrent ();

  // Change the current FPU flags based on what is set in flags
  static inline void setCurrent (const FPUFlags& flags);

  // Clears our flags if they are not set in another object
  void clearUnsetFlagsFrom (const FPUFlags& flags)
  {
    if (!flags.getMaskNaNs().is_set ())         m_maskNaNs.clear ();
    if (!flags.getMaskDenormals().is_set ())    m_maskDenormals.clear ();
    if (!flags.getMaskZeroDivides().is_set ())  m_maskZeroDivides.clear ();
    if (!flags.getMaskOverflows().is_set ())    m_maskOverflows.clear ();
    if (!flags.getMaskUnderflows().is_set ())   m_maskUnderflows.clear ();
  //if (!flags.getMaskInexacts().is_set ())     m_maskInexacts.clear ();
    if (!flags.getFlushDenormals().is_set ())   m_flushDenormals.clear ();
    if (!flags.getInfinitySigned().is_set ())   m_infinitySigned.clear ();
    if (!flags.getRounding().is_set ())         m_rounding.clear ();
    if (!flags.getPrecision().is_set ())        m_precision.clear ();
  }

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

//
// Platform specific implementations
//

#ifdef _MSC_VER

/*
 * Microsoft Visual Studio
 *
 */
class FPUFlagsImplementation : private FPUFlags
{
public:
  static FPUFlags getCurrent ()
  {
    unsigned int currentControl;
    const unsigned int newControl = 0;
    const unsigned int mask = 0;

    errno_t result = _controlfp_s (&currentControl, newControl, mask);

    if (result != 0)
      throw std::runtime_error ("error in _controlfp_s");

    FPUFlags flags;

    flags.setMaskNaNs        ((currentControl & _EM_INVALID)    == _EM_INVALID);
    flags.setMaskDenormals   ((currentControl & _EM_DENORMAL)   == _EM_DENORMAL);
    flags.setMaskZeroDivides ((currentControl & _EM_ZERODIVIDE) == _EM_ZERODIVIDE);
    flags.setMaskOverflows   ((currentControl & _EM_OVERFLOW)   == _EM_OVERFLOW);
    flags.setMaskUnderflows  ((currentControl & _EM_UNDERFLOW)  == _EM_UNDERFLOW);
  //flags.setMaskInexacts    ((currentControl & _EM_INEXACT)    == _EM_INEXACT);
    flags.setFlushDenormals  ((currentControl & _DN_FLUSH)      == _DN_FLUSH);
    flags.setInfinitySigned  ((currentControl & _IC_AFFINE)     == _IC_AFFINE);

    Rounding rounding;
    switch (currentControl & _MCW_RC)
    {
    case _RC_CHOP: rounding = roundChop; break;
    case _RC_UP:   rounding = roundUp;   break;
    case _RC_DOWN: rounding = roundDown; break;
    case _RC_NEAR: rounding = roundNear; break;
    default:
      throw std::runtime_error ("unknown rounding in _controlfp_s");
    };
    flags.setRounding (rounding);

    Precision precision;
    switch (currentControl & _MCW_PC )
    {
    case _PC_64: precision = bits64; break;
    case _PC_53: precision = bits53; break;
    case _PC_24: precision = bits24; break;
    default:
      throw std::runtime_error ("unknown precision in _controlfp_s");
    };
    flags.setPrecision (precision);

    return flags;
  }

  static void setCurrent (const FPUFlags& flags)
  {
    unsigned int newControl = 0;
    unsigned int mask = 0;

    setControl (flags.getMaskNaNs(), newControl, mask,        _EM_INVALID);
    setControl (flags.getMaskDenormals(), newControl, mask,   _EM_DENORMAL);
    setControl (flags.getMaskZeroDivides(), newControl, mask, _EM_ZERODIVIDE);
    setControl (flags.getMaskOverflows(), newControl, mask,   _EM_OVERFLOW);
    setControl (flags.getMaskUnderflows(), newControl, mask,  _EM_UNDERFLOW);
  //setControl (flags.getMaskInexacts(), newControl, mask,    _EM_INEXACT);
    setControl (flags.getFlushDenormals(), newControl, mask,  _DN_FLUSH);
    setControl (flags.getInfinitySigned(), newControl, mask,  _IC_AFFINE);

    if (flags.getRounding().is_set ())
    {
      Rounding rounding = flags.getRounding().value ();

      switch (rounding)
      {
      case roundChop: mask |= _MCW_RC; newControl |= _RC_CHOP; break;
      case roundUp:   mask |= _MCW_RC; newControl |= _RC_UP;   break;
      case roundDown: mask |= _MCW_RC; newControl |= _RC_DOWN; break;
      case roundNear: mask |= _MCW_RC; newControl |= _RC_NEAR; break;
      }
    }

    if (flags.getPrecision().is_set ())
    {
      switch (flags.getPrecision().value ())
      {
      case bits64: mask |= _MCW_PC; newControl |= _PC_64; break;
      case bits53: mask |= _MCW_PC; newControl |= _PC_53; break;
      case bits24: mask |= _MCW_PC; newControl |= _PC_24; break;
      }
    }

    unsigned int currentControl;

    errno_t result = _controlfp_s (&currentControl, newControl, mask);

    if (result != 0)
      throw std::runtime_error ("error in _controlfp_s");
  }

private:
  static void setControl (const Flag& flag,
                          unsigned int& newControl,
                          unsigned int& mask,
                          unsigned int constant)
  {
    if (flag.is_set ())
    {
      mask |= constant;
      if (flag.value ())
        newControl |= constant;
    }
  }
};

#else
/*
 * Unsupported platform
 *
 */

#pragma message(VF_LOC_"Missing platform-specific implementation")

class FPUFlagsImplementation
{
public:
  static FPUFlags getCurrent ()
  {
    return FPUFlags();
  }

  static void setCurrent (const FPUFlags& flags)
  {
  }

#endif

//------------------------------------------------------------------------------

inline FPUFlags FPUFlags::getCurrent ()
{
  return FPUFlagsImplementation::getCurrent ();
}

inline void FPUFlags::setCurrent (const FPUFlags& flags)
{
  FPUFlagsImplementation::setCurrent (flags);
}

//------------------------------------------------------------------------------

// Adjusts the state of FPU flags for the lifetime of the
// object and restores them later. Flags that are not defined
// in flagsToSet are unaffected.
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

