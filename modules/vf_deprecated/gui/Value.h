// Copyright (C) 2008 by One Guy Group, Inc., All rights reserved worldwide.

#ifndef VF_UI_VALUE_H
#define VF_UI_VALUE_H

#include "Meter.h"

namespace Ui {

// A Value is a Meter which can be changed through user interaction

//------------------------------------------------------------------------------

namespace Model {

class Value : public Model::Meter
{
public:
  virtual double getDefaultValue () = 0;
  virtual void setValue (double newValue) = 0;
};

//------------------------------------------------------------------------------

class DummyValue : public Value
{
public:
  DummyValue (double value = 0, const String name = String::empty);
  const String getName ();
  double getValue ();
  const String getValueAsText ();
  double getDefaultValue ();
  void setValue (double newValue);

private:
  String m_name;
  double m_value;
};

}

//------------------------------------------------------------------------------

namespace Facade {

}

//------------------------------------------------------------------------------

namespace Control {

}

}

#endif