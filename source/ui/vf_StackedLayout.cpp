// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/ui/vf_StackedLayout.h"

StackedLayout::StackedLayout (BorderSize<int> borderSize,
                              int gapSize)
  : m_active (false)
  , m_borderSize (borderSize)
  , m_gapSize (gapSize)
{
}

StackedLayout::~StackedLayout()
{
  for (size_t i = 0; i < m_bands.size(); ++i)
    delete m_bands[i].component;
}

void StackedLayout::addBand (Component* component)
{
  addAndMakeVisible (component);
  component->addComponentListener (this);
  Item item;
  item.component = component;
  item.resizableChild = dynamic_cast<vf::ResizableChild*>(component);
  m_bands.push_back (item);
}

void StackedLayout::setBandVisible (int index, bool visible)
{
  Component* c = m_bands[index].component;
  
  c->setVisible (visible);
}

void StackedLayout::recalculateLayout ()
{
  if (m_active)
  {
    const Rectangle<int> bounds = getLocalBounds ();

    int totalSize = m_borderSize.getTop();
    int lastSize = 0;

    for (size_t i = 0; i < m_bands.size(); ++i)
    {
      Component* c = m_bands[i].component;
      if (c->isVisible ())
      {
        lastSize = c->getHeight ();
        totalSize += lastSize;
      }
    }

    totalSize += m_borderSize.getBottom ();

    Component* lastComponent = 0;
    Rectangle<int> lastBounds;
    lastBounds.setX (bounds.getX() + m_borderSize.getLeft());
    lastBounds.setRight (bounds.getRight() - m_borderSize.getRight());
    lastBounds.setY (bounds.getY() + m_borderSize.getTop());

    for (size_t i = 0; i < m_bands.size(); ++i)
    {
      Component* c = m_bands[i].component;
      if (c->isVisible ())
      {
        if (lastComponent)
        {
          lastBounds.setHeight (lastComponent->getHeight());
          lastComponent->setBounds (lastBounds);
          
          lastBounds.setY (lastBounds.getBottom() + m_gapSize);
        }

        lastComponent = c;
      }
    }

    if (lastComponent)
    {
      lastBounds.setBottom (bounds.getBottom() - m_borderSize.getBottom());
      lastComponent->setBounds (lastBounds);
    }
  }
}

void StackedLayout::activateStackedLayout ()
{
  m_active = true;

  recalculateLayout ();
}

void StackedLayout::resized ()
{
  recalculateLayout ();
}

void StackedLayout::resizeStart ()
{
  for (size_t i = 0; i < m_bands.size(); ++i)
  {
    ResizableChild* c = m_bands[i].resizableChild;
    if (c)
      c->resizeStart ();
  }

  for (size_t i = 0; i < m_bands.size(); ++i)
  {
    ResizableChild* c = m_bands[i].resizableChild;
    if (c)
      c->resizeStart ();
  }

  int minW = 0;
  int minH = 0;

  minH = m_borderSize.getTop();
  int lastH = -1;

  for (size_t i = 0; i < m_bands.size(); ++i)
  {
    Component* c = m_bands[i].component;
    if (c->isVisible ())
    {
      if (lastH != -1)
      {
        minH += m_gapSize + lastH;
      }

      ResizableChild* rc = m_bands[i].resizableChild;
      if (rc)
      {
        lastH = rc->getMinimumHeight ();

        minW = jmax (minW, rc->getMinimumWidth ());
      }
      else
      {
        lastH = m_bands[i].component->getHeight ();
      }
    }
  }

  if (lastH != -1)
  {
    minH += lastH + m_borderSize.getBottom ();
  }

  setMinimumWidth (minW);
  setMinimumHeight (minH);
}

void StackedLayout::componentMovedOrResized (Component& component,
                                             bool wasMoved,
                                             bool wasResized)
{
}

void StackedLayout::componentVisibilityChanged (Component& component)
{
  recalculateLayout ();
}

void StackedLayout::componentBeingDeleted (Component& component)
{
}

END_VF_NAMESPACE
