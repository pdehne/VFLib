// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_GUI_VFHEADER
#define VF_GUI_VFHEADER

#include "../vf_core/vf_core.h"

#if JUCE_MSVC
#pragma warning (push)
#pragma warning (disable: 4100) // unreferenced formal parameter
#pragma warning (disable: 4355) // 'this' : used in base member initializer list
#endif

namespace vf
{

#include "components/vf_ComponentBroadcast.h"
#include "components/vf_ConnectedEdges.h"
#include "components/vf_TransparentBorder.h"

#include "graphics/vf_LabColour.h"
#include "graphics/vf_XYZColour.h"
#include "graphics/vf_RadialImageConvolutionKernel.h"
#include "graphics/vf_VerticalGradient.h"

#include "layout/vf_ContentComponentConstrainer.h"
#include "layout/vf_ResizableLayout.h"
#include "layout/vf_StackedLayout.h"

#include "mouse/vf_DragAndDropData.h"
#include "mouse/vf_DragAndDropTarget.h"
#include "mouse/vf_MouseEnterEditable.h"
#include "mouse/vf_MouseEnterGroup.h"

}

#if JUCE_MSVC
#pragma warning (pop)
#endif

#endif

