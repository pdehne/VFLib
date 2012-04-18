// Copyright (C) 2008 by One Guy Group, Inc., All rights reserved worldwide.

#ifndef RESIZINGPANES_H
#define RESIZINGPANES_H

#include "Static.h"

namespace Ui {

namespace Control {

// holds 2 components stacked vertically or horizontally that
// can have their size adjusted by a gripper between them.
class ResizingPanes
  : public Component
  , public ResizableChild
{
public:
  enum
  {
    dividerThickness = 7
  };

  ResizingPanes (bool isVertical,
                 double growthFraction,
                 double initialRatio,
                 Facade::Base* facade,
                 Component* left,
                 Component* right);

  void setLeftSize (int size);

  void resized();

  void resizeStart();

protected:
  void adjustPanes ();

private:
  class Divider : public Control::Static
  {
  public:
    Divider (bool isVertical,
             Facade::Base* facade,
             ResizingPanes* owner);
    void updateCursor ();
    void mouseDown (const MouseEvent& e);
    void mouseDrag (const MouseEvent& e);
    void enablementChanged ();
  private:
    bool m_isVertical;
    int m_startTopLeft;
    ResizingPanes& m_owner;
    int m_min;
    int m_max;
  };

  bool m_isVertical;
  int m_dividerThickness;
  double m_growthFraction;
  double m_initialRatio;
  ScopedPointer <Divider> m_divider;
  ScopedPointer <Component> m_left;
  ScopedPointer <Component> m_right;
  ResizableChild* m_leftChild;
  ResizableChild* m_rightChild;
  int m_lastKnownSize;
};

}

}

#endif
