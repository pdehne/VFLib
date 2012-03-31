// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

namespace Ui {

namespace Control {

Base::Base (Component* component,
            Facade::Base* facade,
            ReferenceCountedObjectPtr <Model::Base> model)
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
