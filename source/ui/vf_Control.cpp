// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/ui/vf_Control.h"

namespace Ui {

namespace Control {

Base::Base (Component* component,
            Facade::Base* facade,
            SharedObjectPtr <Model::Base> model)
  : m_component (*component)
  , m_facade (facade)
  , m_model (model)
{
  m_facade->attach (model, this);
  m_model->addView (this);
  m_model->addListener (this);
}

Base::~Base ()
{
  m_model->removeListener (this);
  m_model->removeView (this);
}

Model::Base& Base::getModel ()
{
  return *m_model;
}

Facade::Base& Base::getFacade ()
{
  return *m_facade;
}
 
Component& Base::getComponent ()
{
  return m_component;
}

void Base::updateView ()
{
  m_component.repaint ();
}

void Base::onModelEnablement (Model::Base* model)
{
  m_component.setEnabled (model->isEnabled ());
}

}

}

END_VF_NAMESPACE
