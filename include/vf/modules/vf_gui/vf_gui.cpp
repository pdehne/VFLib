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
#include "vf_TransparentBorder.cpp"

#include "vf_LabColour.cpp"
#include "vf_XYZColour.cpp"
#include "vf_RadialImageConvolutionKernel.cpp"

#include "vf_ResizableLayout.cpp"
#include "vf_StackedLayout.cpp"

#include "vf_DragAndDropData.cpp"
#include "vf_DragAndDropTarget.cpp"
#include "vf_MouseEnterEditable.cpp"
}

#if JUCE_MSVC
#pragma warning (pop)
#endif
