// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

#include "AppConfig.h"

#if VF_USE_FREETYPE

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4100)
#endif

int z_verbose = 0;

void z_error (char* message)
{
  (message);
}

#include "freetype/FreeTypeAmalgam.c"

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#endif
