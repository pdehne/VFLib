// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_UI_BUTTON_VFHEADER__
#define __VF_UI_BUTTON_VFHEADER__

#include "vf/vf_SharedObject.h"

namespace ui {

//------------------------------------------------------------------------------

namespace Model {

template <class ListenerClass>
class Listeners
{
public:
  Listeners ()
  {
    m_array.ensureStorageAllocated (2);
  }

  void add (ListenerClass* const listener)
  {
    m_array.addIfNotAlreadyThere (listener);
  }

  void remove (ListenerClass* const listener)
  {
    m_array.removeValue (listener);
  }

  template <class C>
  void call (void (C::*f)())
  { callf <C> (boost::bind (f, _1)); }

  template <class C, class T1>
  void call (void (C::*f)(), T1 t1)
  { callf <C> (boost::bind (f, _1, t1)); }

  template <class C, class T1, class T2>
  void call (void (C::*f)(), T1 t1, T2 t2)
  { callf <C> (boost::bind (f, _1, t1, t2)); }

  template <class C, class T1, class T2, class T3>
  void call (void (C::*f)(), T1 t1, T2 t2, T3 t3)
  { callf <C> (boost::bind (f, _1, t1, t2, t3)); }

  template <class C, class T1, class T2, class T3, class T4>
  void call (void (C::*f)(), T1 t1, T2 t2, T3 t3, T4 t4)
  { callf <C> (boost::bind (f, _1, t1, t2, t3, t4)); }

  template <class C, class T1, class T2, class T3, class T4, class T5>
  void call (void (C::*f)(), T1 t1, T2 t2, T3 t3, T4 t4, T5 t5)
  { callf <C> (boost::bind (f, _1, t1, t2, t3, t4, t5)); }

  template <class C, class T1, class T2, class T3, class T4, class T5, class T6>
  void call (void (C::*f)(), T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6)
  { callf <C> (boost::bind (f, _1, t1, t2, t3, t4, t5, t6)); }

  template <class C, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
  void call (void (C::*f)(), T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7)
  { callf <C> (boost::bind (f, _1, t1, t2, t3, t4, t5, t6, t7)); }

  template <class C, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
  void call (void (C::*f)(), T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8)
  { callf <C> (boost::bind (f, _1, t1, t2, t3, t4, t5, t6, t7, t8)); }

private:
  template <class ListenerSubClass, class Functor>
  void callf (Functor f)
  {
    for (int i = 0; i < m_array.size(); ++i)
    {
      ListenerSubClass* const listener =
        dynamic_cast <ListenerSubClass*> (m_array[i]);

      // If this goes off it means that the listener did not
      // derive from the interface containing the member function.
      //
      if (listener == nullptr)
        Throw (Error().fail (__FILE__, __LINE__));

      f (listener);
    }
  }

private:
  Array <ListenerClass*> m_array;
};

//------------------------------------------------------------------------------

class Base : public SharedObject
{
public:
  class Listener
  {
  public:
    virtual void onModelChanged (Base* model) { }
  };

public:
  virtual ~Base ();

  void addListener (Listener* const listener);
  void removeListener (Listener* const listener);

private:
  void destroySharedObject ();

protected:
  Listeners <Listener> m_listeners;
};

//------------------------------------------------------------------------------

class Button : public Base
{
public:
  class Listener : public Base::Listener
  {
  public:
    virtual void onModelButton () { }
    virtual void onModelEnablement (Button* model, bool isEnabled) { }
  };

public:
  Button ();

  // If true, hides the cursor when the mouse moves during a button down.
  //
  virtual bool isMomentary ();

  virtual void clicked (ModifierKeys const& modifiers);

  virtual void pressed (); // WHAT ABOUT MODIFIERS??
  virtual void released ();
};

}

//------------------------------------------------------------------------------

namespace Control {
class Button;
}

namespace Facade {

class Base
{
public:
  virtual ~Base ();

  virtual void paint (Graphics& g);

private:
};

class Button : public Base
{
public:
  explicit Button (Control::Button* control);

protected:
  Control::Button* m_control;
};

}

//------------------------------------------------------------------------------

namespace Control {

class Base
{
public:
};

class Button : public Base
             , public VF_JUCE::Button
             , public Model::Button::Listener
             , LeakChecked <Button>
{
public:
  explicit Button (SharedObjectPtr <Model::Button> model);
  ~Button ();

  void setFacade (Facade::Button* facade)
  { m_facade = facade; }

protected:
  void paintButton (Graphics& g,
                    bool isMouseOverButton,
                    bool isButtonDown);

  void paintOverChildren (Graphics& g);

protected:
  void mouseDrag (MouseEvent const& e);
  void mouseUp (MouseEvent const& e);
  void clicked (ModifierKeys const& modifiers);
  void enablementChanged ();
  void updateView ();

private:
  class StateDetector : public VF_JUCE::Button::Listener
                      , NonCopyable
  {
  public:
    explicit StateDetector (Button* owner);
  private:
    Button& m_owner;
    bool m_wasDown;
    void buttonClicked (VF_JUCE::Button* button);
    void buttonStateChanged (VF_JUCE::Button* button);
  };

  SharedObjectPtr <Model::Button> m_model;
  ScopedPointer <Facade::Button> m_facade;

  StateDetector m_stateDetector;
  bool m_bEnabledUnboundedMouseMovement;
};

}

//------------------------------------------------------------------------------

}

#endif
