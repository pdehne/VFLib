// Copyright (C) 2008 by One Guy Group, Inc., All rights reserved worldwide.

#ifndef VF_UI_RESIZABLEPANEL_H
#define VF_UI_RESIZABLEPANEL_H

#include "Static.h"

namespace Ui {

namespace Control {

// A static panel which includes a layout for resizing
class ResizablePanel
  : public Static
  , public ResizableLayout
{
public:
  explicit ResizablePanel (Facade::Base* facade);

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResizablePanel)
};

}

}

#endif