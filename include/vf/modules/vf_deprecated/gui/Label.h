// Copyright (C) 2008 by One Guy Group, Inc., All rights reserved worldwide.

#ifndef VF_UI_LABEL_H
#define VF_UI_LABEL_H

namespace Ui {

// A Label is simply a line of formatted text that comes from a Model

//------------------------------------------------------------------------------

namespace Model {

class Label : virtual public Model::Base
{
public:
  virtual const String getText () = 0;
};

//------------------------------------------------------------------------------

// This is for prototyping when you don't have a real Model

class DummyLabel : public Label
{
public:
  DummyLabel (const String text);
  const String getText ();

private:
  String m_text;

  JUCE_LEAK_DETECTOR(DummyLabel)
};

}

//------------------------------------------------------------------------------

namespace Facade {

class LabelBase : public Facade::Base
{
public:
  LabelBase();
  void setText (const String text);

protected:
  const String getText ();

private:
  String m_text;

  JUCE_LEAK_DETECTOR(LabelBase)
};

//------------------------------------------------------------------------------

// A simple implementation of a Label Facade that should satisfy most uses

class Label : public LabelBase
{
public:
  Label();
  
  void setFont (Font font);
  void setColour (Colour colour);
  void setJustification (Justification just);
  
  void paint (Graphics& g, const Rectangle<int>& bounds);

protected:
  Font getFont();
  Colour getColour();
  Justification getJustification();

private:
  Font m_font;
  Colour m_colour;
  Justification m_just;

  JUCE_LEAK_DETECTOR(Label)
};

}

//------------------------------------------------------------------------------

namespace Control {

class Label
  : public Component
  , public Base
{
public:
  Label (Facade::Label* facade, Model::Label* model);
  ~Label();

  Model::Label& getModel();
  Facade::Label& getFacade();

  void paint (Graphics& g);
  void paintOverChildren (Graphics& g);
  void enablementChanged ();
  void updateView();

private:
  Facade::Label* m_facade;
  ReferenceCountedObjectPtr<Model::Label> m_model;
};

  //  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Static)

}

}

#endif