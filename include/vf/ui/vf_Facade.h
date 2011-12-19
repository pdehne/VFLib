// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_FACADE_VFHEADER__
#define __VF_FACADE_VFHEADER__

#include "vf/vf_SharedObjectPtr.h"
#include "vf/ui/vf_ConnectedEdges.h"
#include "vf/ui/vf_Model.h"
#include "vf/ui/vf_TransparentBorder.h"

namespace Ui {

namespace Control {
  class Base;
}

//------------------------------------------------------------------------------

// A Facade encapsulates the drawable appearance of a control
//
namespace Facade {

class Base
  : public ConnectedEdges
  , public Model::Base::Listener
  , LeakChecked <Base>
  , NonCopyable
{
public:
  static const BorderSize <int> fullyOpaque;
  static const BorderSize <int> fullyTransparent;

public:
  Base();

  virtual ~Base ();

  Model::Base& getModel ();
  Control::Base& getControl ();
  Component& getComponent ();
  Rectangle <int> getLocalBounds ();

  void setAlpha (float alpha);

  // Controls call this.
  // Default behavior is to just call paint().
  // Subclasses can override for customization.
  //
  virtual void paintFacade (Graphics& g);

  virtual void paint (Graphics& g) /*= 0*/;
  virtual void paintOverChildren (Graphics& g);


  // legacy
  bool isEnabled ();
  void setEnabled (bool isEnabled);

  virtual BorderSize <int> getTransparency(); // fullyTransparent by default

  virtual void paint (Graphics& g, const Rectangle<int>& bounds);
  virtual void paintOverChildren (Graphics& g, const Rectangle<int>& bounds);

public:
  // utility
  static Path createFittedRoundRect (const Rectangle<int>& bounds,
                                     float frameThickness,
                                     float cornerRadius);

protected:

protected:
  friend class Control::Base;
  
  void attach (Model::Base* model, Control::Base* control);
  
  virtual void onAttach ();
  virtual void onModelChanged (Model::Base* model);

private:
  SharedObjectPtr <Model::Base> m_model;
  Control::Base* m_control;
  bool m_isEnabled;
  TransparentBorder m_transparentBorder;
};

//------------------------------------------------------------------------------

#if 0
// An empty Facade for a container-type Component. Fully transparent.
class Empty : public Base
{
public:
  Empty();

  BorderSize<int> getTransparency();
  void paint (Graphics& g, const Rectangle<int>& bounds);
};
#endif

}

}

#endif