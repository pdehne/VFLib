// Copyright (C) 2008 by One Guy Group, Inc., All rights reserved worldwide.

namespace Ui {

namespace Model {

const String& Popup::getCurrentItemText ()
{
  return String::empty;
}

void Popup::buildMenu (PopupMenu& menu)
{
}

void Popup::doMenuItem (int menuItemId)
{
}

}

//------------------------------------------------------------------------------

namespace Control {

PopupButton::PopupLookAndFeel::PopupLookAndFeel (LookAndFeel& owner)
: m_owner (owner)
{
}

PopupButton::Callback::Callback (PopupButton& owner)
  : PopupLookAndFeel (owner.getLookAndFeel())
  , m_owner (owner)
{
}

void PopupButton::Callback::modalStateFinished (int returnValue)
{
  if (returnValue)
    m_owner.doMenuItem (returnValue);
}

PopupButton::PopupButton (Facade::Button* facade, Model::Popup* model)
: Button (String::empty)
, m_facade (facade)
, m_model (model)
, m_menuItemHeight (15)
{
  if (m_model)
    m_model->addView (this);
}

PopupButton::~PopupButton ()
{
  if (m_model)
    m_model->removeView (this);
}

Model::Popup& PopupButton::getModel ()
{
  return *m_model;
}

Facade::Button& PopupButton::getFacade()
{
  return *m_facade;
}

void PopupButton::setMenuItemHeight (int menuItemHeight)
{
  m_menuItemHeight = menuItemHeight;
}

void PopupButton::doMenuItem (int menuItemId)
{
  if (m_model)
    m_model->doMenuItem (menuItemId);
}

void PopupButton::paintButton (Graphics& g,
                             bool isMouseOverButton,
                             bool isButtonDown)
{
  m_facade->setEnabled (isEnabled());
  m_facade->setButtonState (isMouseOverButton, isButtonDown);
  m_facade->paint (g, getLocalBounds());
}

void PopupButton::clicked (const ModifierKeys&)
{
  if (m_model)
  {
    PopupMenu menu;
    m_model->buildMenu (menu);

    Callback* c = new Callback (*this);

    Colour color = findColour (PopupMenu::backgroundColourId, true);
    c->setColour (PopupMenu::backgroundColourId, findColour (PopupMenu::backgroundColourId, true));
    c->setColour (PopupMenu::textColourId, findColour (PopupMenu::textColourId, true));
    c->setColour (PopupMenu::headerTextColourId, findColour (PopupMenu::headerTextColourId, true));
    c->setColour (PopupMenu::highlightedBackgroundColourId, findColour (PopupMenu::highlightedBackgroundColourId, true));
    c->setColour (PopupMenu::highlightedTextColourId, findColour (PopupMenu::highlightedTextColourId, true));

    setState (buttonDown);

    menu.setLookAndFeel (c);
    menu.showAt (this, 0, 0, 0, m_menuItemHeight, c);
  }
}

void PopupButton::updateView ()
{
  if (m_model)
  {
    m_facade->setEnabled (m_model->isEnabled ());
    //m_facade->setTextLabel (m_model->getCurrentItemText());
    repaint ();
  }
}

}

}
