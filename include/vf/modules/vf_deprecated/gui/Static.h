// Copyright (C) 2008 by One Guy Group, Inc., All rights reserved worldwide.

#ifndef VF_UI_STATIC_H
#define VF_UI_STATIC_H

namespace Ui {

//------------------------------------------------------------------------------

// A Static control has appearance but no model and no user input.
// This is useful for things like background panels and what not.

namespace Control {

class Static
  : public Component // must come first
  , public Control::Base
{
public:
  Static (Facade::Base* facade,
          ReferenceCountedObjectPtr <Model::Base> model = nullptr);
  ~Static ();

  void paint (Graphics& g);
  void paintOverChildren (Graphics& g);

protected:
  void mouseDown (const MouseEvent& e);
  void enablementChanged ();
  void updateView ();

private:
};

//------------------------------------------------------------------------------

template<class Facade>
class StaticType : public Static
{
public:
  explicit StaticType (Facade* facade)
    : Static (facade)
    , m_facade (facade)
  {
  }

  Facade& getFacade ()
  {
    return *m_facade;
  }

private:
  Facade* m_facade;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StaticType)
};

}

}

#endif