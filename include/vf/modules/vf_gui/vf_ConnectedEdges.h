// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_CONNECTEDEDGES_VFHEADER__
#define __VF_CONNECTEDEDGES_VFHEADER__

class ConnectedEdges
{
public:
  typedef Button::ConnectedEdgeFlags ConnectedEdgeFlags;

  void setConnectedEdges (int connectedEdgeFlags_)
  {
    connectedEdgeFlags.flags = connectedEdgeFlags_;
  }

  void setConnectedEdgeFlag (int flag, bool connected)
  {
    if (connected)
      connectedEdgeFlags.flags |= flag;
    else
      connectedEdgeFlags.flags &= ~flag;
  }

  void setConnectedOnLeft (bool connected)
    { setConnectedEdgeFlag (Button::ConnectedOnLeft, connected); }

  void setConnectedOnRight (bool connected)
    { setConnectedEdgeFlag (Button::ConnectedOnRight, connected); }

  void setConnectedOnTop (bool connected)
    { setConnectedEdgeFlag (Button::ConnectedOnTop, connected); }

  void setConnectedOnBottom (bool connected)
    { setConnectedEdgeFlag (Button::ConnectedOnBottom, connected); }

  int getConnectedEdgeFlags() const  { return connectedEdgeFlags.flags; }
  bool isConnectedOnLeft() const     { return (connectedEdgeFlags.flags & Button::ConnectedOnLeft) != 0; }
  bool isConnectedOnRight() const    { return (connectedEdgeFlags.flags & Button::ConnectedOnRight) != 0; }
  bool isConnectedOnTop() const      { return (connectedEdgeFlags.flags & Button::ConnectedOnTop) != 0; }
  bool isConnectedOnBottom() const   { return (connectedEdgeFlags.flags & Button::ConnectedOnBottom) != 0; }

private:
  struct Flags
  {
    Flags() : flags(0) {}
    int flags;
  };
  Flags connectedEdgeFlags;
};

#endif
