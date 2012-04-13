// Copyright (C) 2008 by One Guy Group, Inc., All rights reserved worldwide.

#ifndef VF_UI_BUTTON_H
#define VF_UI_BUTTON_H

namespace Ui {

namespace Model {

// A Button can be pressed and released
//
class Button : public Model::Base
{
public:
  virtual const String getName ();
  virtual bool isMomentary (); // hides the cursor if true

  virtual void clicked (const ModifierKeys& modifiers);
  virtual void pressed ();
  virtual void released ();
};

// A CheckButton has a checked state (on or off).
//
class CheckButton : public Button
{
public:
  class Listener : public ListenerBase
  {
  public:
    virtual void onCheckButtonChecked (CheckButton*) { }
  };

public:
  CheckButton ();

  bool isChecked ();

protected:
  void setChecked (bool checked);

private:
  bool m_checked;
};

//------------------------------------------------------------------------------

class DummyButton : public Button
{
public:
  explicit DummyButton (bool enabled = false);
};

//------------------------------------------------------------------------------

class DummyCheckButton : public CheckButton
{
public:
  explicit DummyCheckButton (bool enabled = false);
  void clicked (ModifierKeys const& modifiers);
};

}

//------------------------------------------------------------------------------

namespace Control {
class Button;
}

namespace Facade {

// All Button facades have at least this functionality
class Button
  : public Base
{
public:
  Model::Button& getModel ();
  Control::Button& getControl ();

  void setButtonState (bool isMouseOverButton, bool isButtonDown);

public:
  bool isMouseOverButton();
  bool isButtonDown();

protected:
  void onAttach ();

private:
  bool m_isMouseOverButton;
  bool m_isButtonDown;
};

class CheckButton : public Button
{
public:
  Model::CheckButton& getModel ();
};


}

//------------------------------------------------------------------------------

namespace Control {

class Button
  : public juce::Button
  , public Ui::Control::Base
{
public:
  Button (Facade::Button* facade,
          ReferenceCountedObjectPtr <Model::Button> model);
  ~Button ();

  Model::Button& getModel();
  Facade::Button& getFacade();

  bool isButtonDown ();
  bool isMouseOverButton ();

  void paintButton (Graphics& g,
                    bool isMouseOverButton,
                    bool isButtonDown);

  void paintOverChildren (Graphics& g);

protected:
  void mouseDrag (const MouseEvent& e);
  void mouseUp (const MouseEvent& e);
  void clicked (const ModifierKeys& modifiers);
  void enablementChanged ();
  void updateView ();

private:
  class StateDetector : public juce::Button::Listener
  {
  public:
    explicit StateDetector (Button* owner);
  private:
    JUCE_DECLARE_NON_COPYABLE (StateDetector);

    Button& m_owner;
    bool m_bWasDown;
    void buttonClicked (juce::Button* button);
    void buttonStateChanged (juce::Button* button);
  };

  StateDetector m_listener;
  bool m_bEnabledUnboundedMouseMovement;
};

}

}

#endif