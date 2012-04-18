// Copyright (C) 2008 by One Guy Group, Inc., All rights reserved worldwide.

namespace Ui {

//------------------------------------------------------------------------------

namespace Model {

DummyValue::DummyValue (double value, const String name)
: m_name (name)
, m_value (value)
{
}

const String DummyValue::getName ()
{
  return m_name;
}

double DummyValue::getValue ()
{
  return m_value;
}

const String DummyValue::getValueAsText ()
{
  return String (m_value, 3);
}

double DummyValue::getDefaultValue ()
{
  return 0;
}

void DummyValue::setValue (double newValue)
{
  newValue = jlimit (0., 1., newValue);

  if (m_value != newValue)
  {
    m_value = newValue;
    updateAllViews ();
  }
}

}

}
