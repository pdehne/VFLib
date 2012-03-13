// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_CONTROL_VFHEADER
#define VF_CONTROL_VFHEADER

#include "vf/modules/vf_gui/vf_Facade.h"
#include "vf/modules/vf_gui/vf_Model.h"

namespace Ui {

// A control is a Component that combines a Facade with an optional Model
// that displays the Model and allows the user to interact. Derived classes
// provide the actual Component
//

namespace Control {

class View
{
public:
  virtual void updateView () = 0;
};

//------------------------------------------------------------------------------

class Base
  : public View
  , public Model::Base::Listener
  , NonCopyable
{
public:
  Base (Component* component,
        Facade::Base* facade,
        SharedObjectPtr <Model::Base> model);
  ~Base ();

  Model::Base& getModel ();
  Facade::Base& getFacade ();
  Component& getComponent ();

protected:
  void updateView ();

  void onModelEnablement (Model::Base* model);

private:
  Component& m_component;
  ScopedPointer <Facade::Base> m_facade;
  SharedObjectPtr <Model::Base> m_model;
};

}

}

#endif
