// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/ui/vf_Static.h"

namespace Control {

Static::Static ()
  : ResizableLayout (this)
{
}

void Static::paint (Graphics& g)
{
  vf::Facade::Static* facade = dynamic_cast <vf::Facade::Static*> (this);

  if (facade != nullptr)
  {
    //updateFacade ();

    facade->paint (g, getLocalBounds ());
  }
}

void Static::paintOverChildren (Graphics& g)
{
  vf::Facade::Static* facade = dynamic_cast <vf::Facade::Static*> (this);

  if (facade != nullptr)
  {
    facade->paintOverChildren (g, getLocalBounds ());
  }
}

}

END_VF_NAMESPACE
