// Copyright (C) 2008 by One Guy Group, Inc., All rights reserved worldwide.

#ifndef VF_UI_SLIDER_H
#define VF_UI_SLIDER_H

#include "Value.h"

namespace Ui {

// A slider is a bar or knob that can take on values from 0..1 inclusive

//------------------------------------------------------------------------------

namespace Model {

class Slider : public Model::Value
{
};

//------------------------------------------------------------------------------

class DummySlider : public Model::DummyValue
{
};

}

//------------------------------------------------------------------------------

namespace Facade {

class Slider : public Facade::Base
{
public:
  enum Style
  {
    rotary,
    topToBottom,
    bottomToTop,
    leftToRight,
  };

  explicit Slider (Style style);

  void setValue (double value);
  Style getStyle ();
  double getValue ();

private:
  Style m_style;
  double m_value;
};

}

//------------------------------------------------------------------------------

namespace Control {

class Slider
  : public Component
  , public Control::Base
{
public:
  Slider (Facade::Slider* facade, Model::Slider* model);
  ~Slider ();

  Model::Slider& getModel();
  Facade::Slider& getFacade();

  virtual int getStepCount();

  void paint (Graphics& g);
  void paintOverChildren (Graphics& g);

protected:
  void mouseDown (const MouseEvent& e);
  void mouseDrag (const MouseEvent& e);
  void mouseDoubleClick (const MouseEvent& e);
  void mouseWheelMove (const MouseEvent& e, float wheelIncrementX, float wheelIncrementY);
  void enablementChanged ();
  void updateView ();

  double toFacadeValue (double modelValue);

private:
  Facade::Slider* m_facade;
  ReferenceCountedObjectPtr <Model::Slider> m_model;
  bool m_bEnableUnboundedMouseMovement;
  double m_startValue;
};

}

}

#endif