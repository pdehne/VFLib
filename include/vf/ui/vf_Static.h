// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_STATIC_VFHEADER__
#define __VF_STATIC_VFHEADER__

#include "vf/ui/vf_UiBase.h"
#include "vf/ui/vf_ResizableLayout.h"

// A Static control has no interaction, just drawing.
// Child components can be added to the ResizableLayout.
//

namespace Facade {

class Static : public Base
{
public:
};

}

namespace Control {

class Static
  : public Base
  , public ResizableLayout
  , public Component
{
public:
  Static ();

  void paint (Graphics& g);
  void paintOverChildren (Graphics& g);
};

}

#endif
