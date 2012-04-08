// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

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
