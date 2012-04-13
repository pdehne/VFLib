// Copyright (C) 2008 by One Guy Group, Inc., All rights reserved worldwide.

namespace Ui {

namespace Control {

Static::Static (Facade::Base* facade, ReferenceCountedObjectPtr <Model::Base> model)
  : Base (this,
          facade,
          (model == nullptr) ? (new Model::Base) : model)
{
}

Static::~Static ()
{
  // BAD!
  deleteAllChildren ();
}

void Static::paint (Graphics& g)
{
  //getFacade().paint (g, getLocalBounds());
  getFacade().paint (g);
}

void Static::paintOverChildren (Graphics& g)
{
  //getFacade().paintOverChildren (g, getLocalBounds());
  getFacade().paintOverChildren (g);
}

void Static::mouseDown (const MouseEvent& e)
{
  //setEnabled (!isEnabled ());
}

void Static::enablementChanged ()
{
  updateView ();
}

void Static::updateView ()
{
  bool enabled = isEnabled ();
  if (getFacade().isEnabled () != enabled)
  {
    getFacade().setEnabled (enabled);
  }
  repaint();
}

}

}
