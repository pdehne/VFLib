// Copyright (C) 2008 by One Guy Group, Inc., All rights reserved worldwide.

#ifndef VF_UI_METER_H
#define VF_UI_METER_H

namespace Ui {

// A Meter displays a single value from a Model, without user interaction

//------------------------------------------------------------------------------

namespace Model {

class Meter : virtual public Model::Base
{
public:
  virtual double getValue () = 0;
  virtual const String getValueAsText(); // default: empty
};

//------------------------------------------------------------------------------

class DummyMeter : public Meter
{
public:
  DummyMeter (double value = 0, const String name = String::empty);

  const String getName ();
  double getValue ();
  const String getValueAsText ();

private:
  String m_name;
  double m_value;
};

}

//------------------------------------------------------------------------------

namespace Facade {
  
class Meter : public Facade::Base
{
public:
  Meter();
  void setValue (double value);

  double getValue ();

private:
  double m_value;
};

}

//------------------------------------------------------------------------------

namespace Control {

class Meter
  : public Component
  , public Control::Base
{
public:
  explicit Meter (Facade::Meter* facade, Model::Meter* model);
  ~Meter();

  Model::Meter& getModel();
  Facade::Meter& getFacade();

  void paint (Graphics& g);
  void paintOverChildren (Graphics& g);

protected:
  void enablementChanged ();
  void updateView ();

private:
  Facade::Meter* m_facade;
  ReferenceCountedObjectPtr<Model::Meter> m_model;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Meter)
};

template<class FacadeType, class ModelType>
class MeterType : public Meter
{
public:
  MeterType (FacadeType* facade, ModelType* model)
    : Meter (facade, model)
    , m_facade (facade)
    , m_model (model)
  {
  }

  ModelType& getModel() { return *m_model; }
  FacadeType& getFacade() { return *m_facade; }

private:
  ModelType* m_model;
  FacadeType* m_facade;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MeterType)
};

}

}

#endif