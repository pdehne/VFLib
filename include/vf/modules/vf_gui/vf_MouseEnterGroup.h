// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_MOUSEENTERGROUP_VFHEADER__
#define __VF_MOUSEENTERGROUP_VFHEADER__

// Derive your Component-derived class from this object in order to receive
// mouse enter and mouse exit messages for your component and all of it's
// children.
//
class MouseEnterGroup
{
public:
  explicit MouseEnterGroup (Component* parentComponent)
    : m_parentComponent (parentComponent)
    , m_helper (this)
    , m_mouseInside (false)
    , m_mouseInsideNext (false)
  {
  }

  ~MouseEnterGroup ()
  {
  }

  bool isMouseInsideGroup () const
  {
    return m_mouseInside;
  }

  // Called when the mouse enters or exits the group.
  // The event will be relative to the parent Component.
  //
  virtual void onMouseEnterGroup (MouseEvent const& e) { }
  virtual void onMouseExitGroup (MouseEvent const& e) { }

private:
  void updateState ()
  {
    if (m_mouseInside != m_mouseInsideNext)
    {
      if (m_mouseInsideNext)
      {
        m_mouseInside = true;
        onMouseEnterGroup (getMouseEvent ());
      }
      else
      {
        m_mouseInside = false;
        onMouseExitGroup (getMouseEvent ());
      }
    }
  }

private:
   // HACK because of Juce
  inline void setMouseEvent (MouseEvent const& mouseEvent)
    { memcpy (m_mouseEvent, &mouseEvent, sizeof (mouseEvent)); }
  inline MouseEvent const& getMouseEvent () const
    { return *reinterpret_cast <MouseEvent const*> (m_mouseEvent); }

  class Helper
    : private MouseListener
    , private AsyncUpdater
  {
  public:
    explicit Helper (MouseEnterGroup* const owner)
      : m_owner (*owner)
    {
      m_owner.m_parentComponent->addMouseListener (this, true);
    }

    ~Helper ()
    {
      m_owner.m_parentComponent->removeMouseListener (this);
    }

    void mouseEnter (MouseEvent const& e)
    {
      m_owner.m_mouseInsideNext = true;
      m_owner.setMouseEvent (e.getEventRelativeTo (m_owner.m_parentComponent));
      triggerAsyncUpdate ();
    }

    void mouseExit (MouseEvent const& e)
    {
      m_owner.m_mouseInsideNext = false;
      m_owner.setMouseEvent (e.getEventRelativeTo (m_owner.m_parentComponent));
      triggerAsyncUpdate ();
    }

    void handleAsyncUpdate ()
    {
      m_owner.updateState ();
    }

  private:
    MouseEnterGroup& m_owner;
  };

private:
  struct SavedMouseEvent
  {
    SavedMouseEvent (MouseEvent const& e)
    {
      source = &e.source;
      position = Point <int> (e.x, e.y);
      modifiers = e.mods;
      eventComponent = e.eventComponent;
      originator = e.originalComponent;
      eventTime = e.eventTime;
      mouseDownPos = e.getMouseDownPosition ();
      mouseDownTime = Time (0); // ???
      numberOfClicks = e.getNumberOfClicks();
      mouseWasDragged = !e.mouseWasClicked ();
    }

    MouseInputSource* source;
    Point <int>       position;
    ModifierKeys      modifiers;
    Component*        eventComponent;
    Component*        originator;
    Time              eventTime;
    Point <int>       mouseDownPos;
    Time              mouseDownTime;
    int               numberOfClicks;
    bool              mouseWasDragged;
  };

  Component* const m_parentComponent;
  Helper m_helper;
  bool m_mouseInside;
  bool m_mouseInsideNext;
  char m_mouseEvent [sizeof (MouseEvent)]; // HACK because of Juce!
};

#endif

