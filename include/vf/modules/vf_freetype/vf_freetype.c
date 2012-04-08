// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

#include "AppConfig.h"

#if VF_USE_FREETYPE

int z_verbose = 0;

void z_error (char* message)
{
}

#include "freetype/FreeTypeAmalgam.c"

#endif
