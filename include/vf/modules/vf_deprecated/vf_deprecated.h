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

/** This contains deprecated code.

    @file vf_deprecated.h
    @ingroup vf_deprecated
    @deprecated
*/

#ifndef VF_DEPRECATED_VFHEADER
#define VF_DEPRECATED_VFHEADER

//==============================================================================
/**
    Deprecated classes.

    These classes are here for compatibility and should not be used.

    @defgroup vf_deprecated vf_deprecated
    @deprecated
*/

#include "vf/modules/vf_core/vf_core.h"
#include "vf/modules/vf_gui/vf_gui.h"

namespace vf
{
#include "containers/binary_tree.h"
#include "containers/Array.h"
#include "containers/Range.h"

#include "memory/block_pool.h"
#include "memory/destroy_n.h"
#include "memory/fixed_pool.h"
#include "memory/page_pool.h"
#include "memory/Pipe.h"

#include "gui/vf_Button.h"
#include "gui/vf_Control.h"
#include "gui/vf_ConnectedEdges.h"
#include "gui/vf_Facade.h"
#include "gui/vf_Model.h"
#include "gui/vf_Static.h"
#include "gui/vf_UiBase.h"

#include "gui/Button.h"
#include "gui/ColouredImageMaskFacade.h"
#include "gui/Label.h"
#include "gui/Meter.h"
#include "gui/PopupButton.h"
#include "gui/ResizablePanel.h"
#include "gui/ResizingPanes.h"
#include "gui/Slider.h"
#include "gui/SolidColourFacade.h"
#include "gui/Static.h"
#include "gui/Value.h"
}

#endif
