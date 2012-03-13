// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/modules/vf_gui/vf_Button.h"

//------------------------------------------------------------------------------

namespace Model {

Button::Button ()
  : m_checked (false)
{
}

bool Button::isChecked ()
{
  return m_checked;
}

void Button::setChecked (bool checked)
{
  if (m_checked != checked)
  {
    m_checked = checked;
    getListeners().call (&Listener::onModelChecked, this);
  }
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

//------------------------------------------------------------------------------

DummyButton::DummyButton (bool enabled, bool checkButton)
  : m_checkButton (checkButton)
{
  setEnabled (enabled);
}

void DummyButton::clicked (ModifierKeys const& modifiers)
{
  if (m_checkButton)
    setChecked (!isChecked ());
}

}

//------------------------------------------------------------------------------

namespace Facade {

Button::Button ()
  : m_isMouseOverButton (false)
  , m_isButtonDown (false)
  , m_isEnabled (true)
  , m_isChecked (false)
{
}

bool Button::isMouseOverButton () const
{
  return m_isMouseOverButton;
}

bool Button::isButtonDown () const
{
  return m_isButtonDown;
}

bool Button::isEnabled () const
{
  return m_isEnabled;
}

bool Button::isChecked () const
{
  return m_isChecked;
}

void Button::setMouseOverButton (bool isMouseOverButton)
{
  m_isMouseOverButton = isMouseOverButton;
}

void Button::setButtonDown (bool isButtonDown)
{
  m_isButtonDown = isButtonDown;
}

void Button::setEnabled (bool isEnabled)
{
  m_isEnabled = isEnabled;
}

void Button::setChecked (bool isChecked)
{
  m_isChecked = isChecked;
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
  // ignore
}

void Button::StateDetector::buttonStateChanged (VF_JUCE::Button* button)
{
  if (button->isDown ())
  {
    if (!m_wasDown)
    {
      m_wasDown = true;
      m_owner.pressed ();
    }
  }
  else if (m_wasDown)
  {
    m_wasDown = false;
    m_owner.released ();
  }
}

//------------------------------------------------------------------------------

Button::Button (Model::Button* model)
: Base (model)
, VF_JUCE::Button (String::empty)
, m_listener (this)
, m_bEnabledUnboundedMouseMovement (false)
{
  addListener (&m_listener);

  if (model != nullptr)
    setToggleState (model->isChecked (), false);
}

Button::~Button ()
{
}

bool Button::isMomentary ()
{
  bool momentary;

  vf::Model::Button* model = dynamic_cast <vf::Model::Button*> (getModel ());
  
  if (model != nullptr)
    momentary = model->isMomentary ();
  else
    momentary = false;

  return momentary;
}

void Button::clicked (ModifierKeys const& modifiers)
{
  vf::Model::Button* model = dynamic_cast <vf::Model::Button*> (getModel ());

  if (model != nullptr)
  {
    model->clicked (modifiers);
  }
}

void Button::pressed ()
{
  vf::Model::Button* model = dynamic_cast <vf::Model::Button*> (getModel ());

  if (model != nullptr)
  {
    model->pressed ();
  }
}

void Button::released ()
{
  vf::Model::Button* model = dynamic_cast <vf::Model::Button*> (getModel ());

  if (model != nullptr)
  {
    model->released ();
  }
}

void Button::paintButton (
  Graphics& g,
  bool isMouseOverButton,
  bool isButtonDown)
{
  vf::Facade::Button* facade = dynamic_cast <vf::Facade::Button*> (this);

  if (facade != nullptr)
  {
    updateFacade ();

    facade->paint (g, getLocalBounds ());
  }
}

void Button::paintOverChildren (Graphics& g)
{
  vf::Facade::Button* facade = dynamic_cast <vf::Facade::Button*> (this);

  if (facade != nullptr)
  {
    facade->paintOverChildren (g, getLocalBounds ());
  }
}

void Button::updateFacade ()
{
  vf::Facade::Button* facade = dynamic_cast <vf::Facade::Button*> (this);

  if (facade != nullptr)
  {
    facade->setMouseOverButton (isOver ());
    facade->setButtonDown (isDown ());
    facade->setEnabled (isEnabled ());
    facade->setChecked (getToggleState ());
    facade->setConnectedEdgeFlags (getConnectedEdgeFlags ());
  }
}

void Button::enablementChanged ()
{
  if (isEnabled ())
  {
    setAlpha (1.f);
  }
  else
  {
    setAlpha (.5f);
  }
}

void Button::mouseDrag (MouseEvent const& e)
{
  if (VF_JUCE::Button::isEnabled ())
  {
    if (!m_bEnabledUnboundedMouseMovement && isMomentary ())
    {
      e.source.enableUnboundedMouseMovement (true);

      m_bEnabledUnboundedMouseMovement = true;
    }

    VF_JUCE::Button::mouseDrag (e);
  }
}

void Button::mouseUp (MouseEvent const& e)
{
  if (VF_JUCE::Button::isEnabled ())
  {
    m_bEnabledUnboundedMouseMovement = false;

    VF_JUCE::Button::mouseUp (e);
  }
}

void Button::onModelChecked (Model::Button*)
{
  vf::Model::Button& model = dynamic_cast <vf::Model::Button&> (*getModel ());

  setToggleState (model.isChecked (), false);
}

}

END_VF_NAMESPACE
