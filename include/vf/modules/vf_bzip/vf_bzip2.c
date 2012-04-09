// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
//
// This file incorporates work covered by the following copyright
// and permission notice:
//
//   This program, "bzip2", the associated library "libbzip2", and all
//   documentation, are copyright (C) 1996-2010 Julian R Seward.  All
//   rights reserved.
//   
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
//   
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//   
//   2. The origin of this software must not be misrepresented; you must 
//      not claim that you wrote the original software.  If you use this 
//      software in a product, an acknowledgment in the product 
//      documentation would be appreciated but is not required.
//   
//   3. Altered source versions must be plainly marked as such, and must
//      not be misrepresented as being the original software.
//   
//   4. The name of the author may not be used to endorse or promote 
//      products derived from this software without specific prior written 
//      permission.
//
//   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
//   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
//   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
//   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
//   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//   Julian Seward, jseward@bzip.org
//   bzip2/libbzip2 version 1.0.6 of 6 September 2010
//

#include "AppConfig.h"

#if VF_USE_BZIP2

#if _MSC_VER
#pragma push_macro("_CRT_SECURE_NO_WARNINGS")
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#pragma warning (push)
#pragma warning (disable: 4100) // unreferenced formal parameter
#pragma warning (disable: 4127) // conditional expression is constant
#pragma warning (disable: 4244) // conversion, possible loss of data
#pragma warning (disable: 4996) // deprecated POSIX name
#endif

#include "bzip2/blocksort.c"
#include "bzip2/bzlib.c"
#include "bzip2/compress.c"
#include "bzip2/crctable.c"
#include "bzip2/decompress.c"
#include "bzip2/huffman.c"
#include "bzip2/randtable.c"

#if _MSC_VER
#pragma warning (pop)
#pragma pop_macro("_CRT_SECURE_NO_WARNINGS")
#endif

#endif
