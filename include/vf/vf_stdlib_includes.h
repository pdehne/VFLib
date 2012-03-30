// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_STDLIB_INCLUDES_VFHEADER__
#define __VF_STDLIB_INCLUDES_VFHEADER__

// The most commonly used C and C++ Standard library includes

#if defined (_MSC_VER)
#  include <crtdbg.h>
#endif

//
// Standard C++ headers
//

// TRY TO REMOVE AS MANY OF THESE AS POSSIBLE TO REDUCE DEPENDENCIES
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <exception>
#include <functional>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <new>
#include <numeric>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>

//
// Standard C headers
//

// TRY TO REMOVE AS MANY OF THESE AS POSSIBLE TO REDUCE DEPENDENCIES
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <locale.h>
#include <math.h>
#include <memory.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#endif
