// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_FPUFlags.h"

//------------------------------------------------------------------------------

void FPUFlags::clearUnsetFlagsFrom (const FPUFlags& flags)
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

//
// Platform specific implementations
//

//------------------------------------------------------------------------------

#ifdef _MSC_VER

//
// Microsoft Visual Studio
//

FPUFlags FPUFlags::getCurrent ()
{
  unsigned int currentControl;
  const unsigned int newControl = 0;
  const unsigned int mask = 0;

  errno_t result = _controlfp_s (&currentControl, newControl, mask);

  if (result != 0)
    Throw (std::runtime_error ("error in _controlfp_s"));

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
    Throw (std::runtime_error ("unknown rounding in _controlfp_s"));
  };
  flags.setRounding (rounding);

  Precision precision;
  switch (currentControl & _MCW_PC )
  {
  case _PC_64: precision = bits64; break;
  case _PC_53: precision = bits53; break;
  case _PC_24: precision = bits24; break;
  default:
    Throw (std::runtime_error ("unknown precision in _controlfp_s"));
  };
  flags.setPrecision (precision);

  return flags;
}

static void setControl (const FPUFlags::Flag& flag,
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

void FPUFlags::setCurrent (const FPUFlags& flags)
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
    Throw (std::runtime_error ("error in _controlfp_s"));
}

//------------------------------------------------------------------------------

#else

//
// Unimplemented version that does nothing
//

#pragma message(VF_LOC_"Missing platform-specific implementation")

FPUFlags FPUFlags::getCurrent ()
{
  return FPUFlags();
}

void FPUFlags::setCurrent (const FPUFlags& flags)
{
}

#endif

END_VF_NAMESPACE
