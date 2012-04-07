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

#include "vf_TransparentBorder.h"

#include "vf_LabColour.h"
#include "vf_XYZColour.h"
#include "vf_RadialImageConvolutionKernel.h"
#include "vf_VerticalGradient.h"

#include "vf_ResizableLayout.h"
#include "vf_StackedLayout.h"

#include "vf_DragAndDropData.h"
#include "vf_DragAndDropTarget.h"
#include "vf_MouseEnterEditable.h"
#include "vf_MouseEnterGroup.h"

#include "vf_ComponentBroadcast.h"
#include "vf_ConnectedEdges.h"

}

#if JUCE_MSVC
#pragma warning (pop)
#endif

#endif

