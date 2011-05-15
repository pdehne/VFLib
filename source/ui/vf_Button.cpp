// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/ui/vf_Button.h"

namespace ui {

//------------------------------------------------------------------------------

namespace Model {

Base::~Base ()
{
}

void Base::addListener (Listener* const listener)
{
  m_listeners.add (listener);
}

void Base::removeListener (Listener* const listener)
{
  m_listeners.remove (listener);
}

void Base::destroySharedObject ()
{
  delete this;
}

//------------------------------------------------------------------------------

Button::Button ()
{
}

bool Button::isMomentary ()
{
  return false;
}

void Button::clicked (ModifierKeys const& modifiers)
{
}

void Button::pressed ()
{
}

void Button::released ()
{
}

}

//------------------------------------------------------------------------------

namespace Facade {

Base::~Base ()
{
}

void Base::paint (Graphics& g)
{
}

//------------------------------------------------------------------------------

Button::Button (Control::Button* control)
  : m_control (control)
{
  control->setFacade (this);
}

}

//------------------------------------------------------------------------------

namespace Control {

Button::StateDetector::StateDetector (Button* owner)
: m_owner (*owner)
, m_wasDown (false)
{
}

void Button::StateDetector::buttonClicked (VF_JUCE::Button* button)
{
}

void Button::StateDetector::buttonStateChanged (VF_JUCE::Button* button)
{
  if (button->isDown ())
  {
    if (!m_wasDown)
    {
      m_wasDown = true;
      m_owner.m_model->pressed ();
    }
  }
  else if (m_wasDown)
  {
    m_wasDown = false;
    m_owner.m_model->released ();
  }
}

//------------------------------------------------------------------------------

Button::Button (SharedObjectPtr <Model::Button> model)
  : VF_JUCE::Button (String::empty)
  , m_model (model)
  , m_stateDetector (this)
  , m_bEnabledUnboundedMouseMovement (false)
{
  setTriggeredOnMouseDown (true);

  VF_JUCE::Button::addListener (&m_stateDetector);
}

/*
Button::Button (Facade::Button* facade)
: Base (facade)
, VF_JUCE::Button (String::empty)
, m_stateDetector (this)
, m_facade (facade)
, m_model (&facade->getModel ())
, m_bEnabledUnboundedMouseMovement (false)
{
  jassert (m_model != 0);

  setComponent (this);

  setTriggeredOnMouseDown (true);
  addListener (&m_stateDetector);

  m_model->addView (this);
}

Button::Button (Facade::Button* facade, Model::Button* model)
: Base (facade)
, VF_JUCE::Button (String::empty)
, m_stateDetector (this)
, m_facade (facade)
, m_model (model)
, m_bEnabledUnboundedMouseMovement (false)
{
  jassert (m_model != 0);

  setComponent (this);

  setTriggeredOnMouseDown (true);
  addListener (&m_stateDetector);

  m_model->addView (this);
}
*/

Button::~Button()
{
  m_model->removeListener (this);
}

void Button::paintButton (Graphics& g,
                          bool isMouseOverButton,
                          bool isButtonDown)
{
#if 1
  m_facade->paint (g);
#else
  m_facade->setEnabled (isEnabled());
  m_facade->setToggleState (getToggleState());
  m_facade->setButtonState (isMouseOverButton, isButtonDown);
  m_facade->paint (g, getLocalBounds());
#endif
}

void Button::paintOverChildren (Graphics& g)
{
//  m_facade->paintOverChildren (g, getLocalBounds());
}

void Button::mouseDrag (const MouseEvent& e)
{
  if (isEnabled())
  {
    if (m_model->isMomentary() && !m_bEnabledUnboundedMouseMovement)
    {
      e.source.enableUnboundedMouseMovement (true);
      m_bEnabledUnboundedMouseMovement = true;
    }

    VF_JUCE::Button::mouseDrag (e);
  }
}

void Button::mouseUp (const MouseEvent& e)
{
  if (isEnabled())
  {
    m_bEnabledUnboundedMouseMovement=false;

    VF_JUCE::Button::mouseUp (e);
  }
}

void Button::clicked (const ModifierKeys& modifiers)
{
  m_model->clicked (modifiers);
}

void Button::enablementChanged ()
{
  updateView ();
}

void Button::updateView ()
{
#if 0
  setEnabled (m_model->isEnabled ());
  bool enabled = isEnabled ();
  if (getFacade().isEnabled () != enabled)
    getFacade().setEnabled (enabled);

  setToggleState (m_model->isToggled (), false);

  // make the button look pressed when the model changes without user action
  //setState (m_model->isActive() ? buttonDown :
  //  (isMouseOver() ? buttonOver : buttonNormal));
#endif
  repaint();
}

}

}

END_VF_NAMESPACE

/*

- At least 1 Component always associated with a gui object
- Facade needs to see the Control and the Model

? How does the model signal fine-grained changes?
  Model::Listener
    e.g. onModelEnablement (bool isEnabled),
         onModelToggleState (bool isToggled)

Control, Facade, Model

Facade:
- Should see the model
  * to draw the state
- Needs to see the control
  * controls have a disabled state as well

Control:
- Needs to see the model
  * to determine the disabled state
  * to route user actions

class ui::Facade::Button



*/
