// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_MOUSEENTEREDITABLE_VFHEADER__
#define __VF_MOUSEENTEREDITABLE_VFHEADER__

// A MouseEnterEditable allows your Component to go into an 'edit mode' when
// the mouse enters. For example, zoom in/out buttons can appear when the mouse
// enters, and disappear when the mouse leaves.
//
class MouseEnterEditable : NonCopyable
{
private:
  class Helper;

public:
  // A parent component must derive from this. Usually its the top level window.
  //
  class Container
    : private Timer
  {
  public:
    enum
    {
      lingerMilliseconds = 500
    };

    Container ();
    ~Container ();

    MouseEnterEditable* getCurrentEditable ();
    void exitCurrentEditMode ();

  private:
    void mouseEnterEditableActivity (MouseEnterEditable* editable,
                                     bool isMouseEnter);
    void timerCallback();

  private:
    friend class Helper;

    MouseEnterEditable* m_currentEdit;
    MouseEnterEditable* m_nextEdit;
  };

public:
  explicit MouseEnterEditable (Component* component);
  ~MouseEnterEditable ();

  bool isInMouseEnterEditMode ();
  void exitMouseEnterEditMode ();

protected:
  virtual void onMouseEnterEditMode (bool shouldBeInEditMode) = 0;

private:
  class Helper
    : private MouseListener
    , NonCopyable
  {
  public:
    Helper (Component* component, MouseEnterEditable* editable);
    ~Helper ();

    Component& getComponent ();

  private:
    void mouseEnter (const MouseEvent& e);
    void mouseExit (const MouseEvent& e);

  private:
    Component* const m_component;
    MouseEnterEditable* const m_editable;
  };

private:
  Helper m_helper;
};

#endif
