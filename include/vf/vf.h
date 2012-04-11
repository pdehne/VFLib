//==============================================================================
//
// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file GNU_GPL_v2.txt for full licensing terms.
// 
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your option)
// any later version.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
// 
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 51
// Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
// 
//==============================================================================

#ifndef __VF_VFHEADER__
#define __VF_VFHEADER__

/*! \mainpage VFLib: A multipurpose class library built on JUCE.

  VFLib is a source code collection of individual modules containing
  functionality for a variety of applications. 

  VFLib requires JUCE (Jules' Utility Class Extensions) available from
  http://rawmaterialsoftware.com/juce.php. JUCE is available under both the
  GNU General Public License and a commercial, closed source license. Visit
  http://rawmaterialsoftware.com/jucelicense.php for more details.

  Other than JUCE, VFLib has no external dependencies.

  \section Platforms

  All platforms supported by JUCE are also supported by VFLib. Currently these
  platforms include:
  
  - Windows: Applications and VST/RTAS/NPAPI/ActiveX plugins can be built 
    using MS Visual Studio. The results are all fully compatible with Windows
    XP, Vista or Win7

  - Mac OS X: Applications and VST/AudioUnit/RTAS/NPAPI plugins with Xcode

  - GNU/Linux: Applications and plugins can be built for any kernel 2.6 or later.

  - iOS: Native iPhone and iPad apps

  - Android

  \section Prerequisites

  The documentation for VFLib assumes that the reader has a working knowledge
  of JUCE. Some modules built on external libraries assume that the reader
  understands the operation of those external libraries. Certain modules assume
  that the reader understands additional domain-specific information. Modules
  with additional prerequisites are marked in the documentation.

  \section External Modules

  Some modules bring in functionality provided by external libraries. For
  example, the \ref vf_bzip2 module provides the compression and decompression
  algorithms in bZip2. Usage of these external library modules is optional.
  They come with complete source code, as well as options for using either
  system or user provided variants of the external libraries: it is not
  necessary to download additional files to use these modules.

  External code incorporated into VFLib is covered by a separate license.
  See the licensing information and notes in the corresponding source files,
  for copyright information and terms of use.

  \section Integration

  VFLib requires recent versions of JUCE. It won't work with JUCE versions
  1.53 or earlier. To use the library it is necessary to first download JUCE to
  a location where VFLib can find it. Or, you can use your existing installation
  of JUCE. 

  This library uses the same modularized organizational structure as JUCE. To
  use a module, first add a path to the list of includes searched by your
  development environment or project, which points to the VFLib directory
  named "include". Then, add the single corresponding .c or .cpp file
  to your existing project which already uses JUCE. For example, to use the
  \ref vf_core module, add the file vf_core.cpp to your project. Some modules
  depend on other modules. All modules depend on \ref vf_core.

  To use a module, include the appropriate header from within your source code.
  For example, to access classes in the \ref vf_concurrent module, use this:

  @code
  #include "vf/modules/vf_concurrent/vf_concurrent.h"
  @endcode

  \section AppConfig

  \section References

*/

#include "AppConfig.h"

/** Include this header to bring in all the modules,
    or manually include only the modules that you want
*/

#include "modules/vf_audio/vf_audio.h"
#include "modules/vf_concurrent/vf_concurrent.h"
#include "modules/vf_core/vf_core.h"
#include "modules/vf_db/vf_db.h"
#include "modules/vf_gui/vf_gui.h"

#endif
