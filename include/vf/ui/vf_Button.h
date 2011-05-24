// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_BUTTON_VFHEADER__
#define __VF_BUTTON_VFHEADER__

#include "vf/ui/vf_UiBase.h"

namespace Model {

// A Button can be pressed and released
//
class Button : public Model::Base
{
public:
  Button ();

  class Listener : public ListenerBase
  {
  public:
    virtual void onModelChecked (Button* model) = 0;
  };

  bool isChecked ();
  void setChecked (bool checked);

  // A momentary button maintains some continuous action or state while it
  // is pressed. The default control hides the cursor for momentary buttons.
  //
  virtual bool isMomentary ();

  virtual void clicked (ModifierKeys const& modifiers);
  virtual void pressed ();
  virtual void released ();

private:
  bool m_checked;
};

}

//------------------------------------------------------------------------------

namespace Facade {

class Button : public ConnectedEdges
{
public:
  Button ();

  bool isMouseOverButton () const;
  bool isButtonDown () const;
  bool isEnabled () const;
  bool isChecked () const;

  void setMouseOverButton (bool isMouseOverButton);
  void setButtonDown (bool isButtonDown);
  void setEnabled (bool isEnabled);
  void setChecked (bool isChecked);

private:
  bool m_isMouseOverButton;
  bool m_isButtonDown;
  bool m_isEnabled;
  bool m_isChecked;
};

}

//------------------------------------------------------------------------------

namespace Control {

class Button
  : public Base
  , public Model::Button::Listener
  , public VF_JUCE::Button
{
public:
  explicit Button (Model::Button* model = nullptr);
  ~Button ();

  void setConnectedEdgeFlag (int flag, bool connected)
  {
    if (connected)
      setConnectedEdges (getConnectedEdgeFlags () | flag);
    else
      setConnectedEdges (getConnectedEdgeFlags () & ~flag);
  }

  void setConnectedOnLeft (bool connected)
    { setConnectedEdgeFlag (Button::ConnectedOnLeft, connected); }

  void setConnectedOnRight (bool connected)
    { setConnectedEdgeFlag (Button::ConnectedOnRight, connected); }

  void setConnectedOnTop (bool connected)
    { setConnectedEdgeFlag (Button::ConnectedOnTop, connected); }

  void setConnectedOnBottom (bool connected)
    { setConnectedEdgeFlag (Button::ConnectedOnBottom, connected); }

  virtual bool isMomentary ();
  virtual void clicked (ModifierKeys const& modifiers);
  virtual void pressed ();
  virtual void released ();

  void paintButton (Graphics& g,
                    bool isMouseOverButton,
                    bool isButtonDown);

  void paintOverChildren (Graphics& g);

protected:
  virtual void updateFacade ();

  void enablementChanged ();
  void mouseDrag (MouseEvent const& e);
  void mouseUp (MouseEvent const& e);

  void onModelChecked (Model::Button* model);

private:
  class StateDetector
    : public VF_JUCE::Button::Listener
    , public Uncopyable
  {
  public:
    explicit StateDetector (Button* owner);

  private:
    void buttonClicked (VF_JUCE::Button* button);
    void buttonStateChanged (VF_JUCE::Button* button);

  private:
    Button& m_owner;
    bool m_wasDown;
  };

private:
  StateDetector m_listener;
  bool m_bEnabledUnboundedMouseMovement;
};

}

#endif
