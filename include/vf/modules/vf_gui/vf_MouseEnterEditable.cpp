// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/modules/vf_gui/vf_MouseEnterEditable.h"

/*
 NOTE

 This will not handle the case where a component is in edit
 mode and becomes de-parented.
*/

//------------------------------------------------------------------------------

MouseEnterEditable::Helper::Helper (Component* component,
                                    MouseEnterEditable* editable)
: m_component (component)
, m_editable (editable)
{
  m_component->addMouseListener (this, true);
}

MouseEnterEditable::Helper::~Helper ()
{
  m_component->removeMouseListener (this);
}

Component& MouseEnterEditable::Helper::getComponent ()
{
  return *m_component;
}

void MouseEnterEditable::Helper::mouseEnter (const MouseEvent& e)
{
  Container* container = m_component->findParentComponentOfClass <Container> ();
  vfassert (container != nullptr);

  if (container != nullptr)
    container->mouseEnterEditableActivity (m_editable, true);
}

void MouseEnterEditable::Helper::mouseExit (const MouseEvent& e)
{
  Container* container = m_component->findParentComponentOfClass <Container> ();
  vfassert (container != nullptr);

  if (container != nullptr)
    container->mouseEnterEditableActivity (m_editable, false);
}

//------------------------------------------------------------------------------

MouseEnterEditable::Container::Container ()
  : m_currentEdit (nullptr)
  , m_nextEdit (nullptr)
{
}

MouseEnterEditable::Container::~Container ()
{
}

MouseEnterEditable* MouseEnterEditable::Container::getCurrentEditable ()
{
  return m_currentEdit;
}

void MouseEnterEditable::Container::exitCurrentEditMode ()
{
  if (m_currentEdit != nullptr)
  {
    stopTimer ();

    MouseEnterEditable* editable = m_currentEdit;
    m_currentEdit = nullptr;
    editable->onMouseEnterEditMode (false);
  }
}

void MouseEnterEditable::Container::mouseEnterEditableActivity (
  MouseEnterEditable* editable,
  bool isMouseEnter)
{
  if (isMouseEnter)
  {
    stopTimer ();

    if (m_currentEdit != editable)
    {
      if (m_currentEdit != nullptr)
      {
        MouseEnterEditable* editable = m_currentEdit;
        m_currentEdit = nullptr;
        editable->onMouseEnterEditMode (false);
      }

      m_currentEdit = editable;
      m_currentEdit->onMouseEnterEditMode (true);
    }
  }
  else
  {
    startTimer (lingerMilliseconds);
  }
}

void MouseEnterEditable::Container::timerCallback ()
{
  vfassert (m_currentEdit != nullptr);

  exitCurrentEditMode ();
}

//------------------------------------------------------------------------------

MouseEnterEditable::MouseEnterEditable (Component* component)
  : m_helper (component, this)
{
}

MouseEnterEditable::~MouseEnterEditable ()
{
  exitMouseEnterEditMode ();
}

bool MouseEnterEditable::isInMouseEnterEditMode ()
{
  bool inEditMode;
  Container* container = m_helper.getComponent().findParentComponentOfClass <Container> ();

  if (container != nullptr)
  {
    inEditMode = container->getCurrentEditable() == this;
  }
  else
  {
    inEditMode = false;
  }

  return inEditMode;
}

void MouseEnterEditable::exitMouseEnterEditMode ()
{
  Container* container = m_helper.getComponent().findParentComponentOfClass <Container> ();

  if (container != nullptr)
  {
    if (container->getCurrentEditable() == this)
      container->exitCurrentEditMode ();
  }
}


END_VF_NAMESPACE
