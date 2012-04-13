// Copyright (C) 2008 by One Guy Group, Inc., All rights reserved worldwide.

namespace Ui {

//------------------------------------------------------------------------------

namespace Model {

const String Meter::getValueAsText ()
{
  return String::empty;
}

//------------------------------------------------------------------------------

DummyMeter::DummyMeter (double value, const String name)
: m_name (name)
, m_value (value)
{
}

const String DummyMeter::getName ()
{
  return m_name;
}

double DummyMeter::getValue ()
{
  return m_value;
}

const String DummyMeter::getValueAsText ()
{
  return String (m_value, 3);
}

}

//------------------------------------------------------------------------------

namespace Facade {

Meter::Meter()
  : m_value (0)
{
}

void Meter::setValue (double value)
{
  m_value = value;
}

double Meter::getValue ()
{
  return m_value;
}

}

//------------------------------------------------------------------------------

namespace Control {

Meter::Meter (Facade::Meter* facade, Model::Meter* model)
: Base (this, facade, model)
, m_facade (facade)
, m_model(model)
{
  getModel().addView (this);
}

Meter::~Meter()
{
  getModel().removeView (this);
}

Model::Meter& Meter::getModel()
{
  return *m_model;
}

Facade::Meter& Meter::getFacade()
{
  return *m_facade;
}

void Meter::paint (Graphics& g)
{
  getFacade().paint (g, getLocalBounds());
}

void Meter::paintOverChildren (Graphics& g)
{
  getFacade().paintOverChildren (g, getLocalBounds());
}

void Meter::enablementChanged ()
{
  updateView ();
}

void Meter::updateView ()
{
  setEnabled (getModel().isEnabled());
  getFacade().setEnabled (isEnabled ());
  getFacade().setValue (getModel().getValue ());
  repaint();
}

}

}
