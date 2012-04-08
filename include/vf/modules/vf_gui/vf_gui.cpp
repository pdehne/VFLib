// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#include "vf_gui.h"

#if JUCE_MSVC
#pragma warning (push)
#pragma warning (disable: 4100) // unreferenced formal parmaeter
#pragma warning (disable: 4355) // 'this' used in base member

#endif

namespace vf
{
#include "components/vf_TransparentBorder.cpp"

#include "graphics/vf_LabColour.cpp"
#include "graphics/vf_XYZColour.cpp"
#include "graphics/vf_RadialImageConvolutionKernel.cpp"

#include "layout/vf_ResizableLayout.cpp"
#include "layout/vf_StackedLayout.cpp"

#include "mouse/vf_DragAndDropData.cpp"
#include "mouse/vf_DragAndDropTarget.cpp"
#include "mouse/vf_MouseEnterEditable.cpp"
}

#if JUCE_MSVC
#pragma warning (pop)
#endif
