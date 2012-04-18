// Copyright (C) 2008 by One Guy Group, Inc., All rights reserved worldwide.

#ifndef VF_UI_POPUPBUTTON_H
#define VF_UI_POPUPBUTTON_H

namespace Ui {

namespace Model {

class Popup : public Ui::Model::Base
{
public:
  virtual const String& getCurrentItemText();
  virtual void buildMenu (PopupMenu& menu);
  virtual void doMenuItem (int menuItemId);
};

}

//------------------------------------------------------------------------------

namespace Control {

class PopupButton
  : public juce::Button
  , public Control::View
{
public:
  PopupButton (Facade::Button* facade, Model::Popup* model);
  ~PopupButton ();

  Model::Popup& getModel();
  Facade::Button& getFacade();

  void setMenuItemHeight (int menuitemHeight);

protected:
  void doMenuItem (int menuItemId);
  void paintButton (Graphics& g,
                    bool isMouseOverButton,
                    bool isButtonDown);
  void clicked (const ModifierKeys&);
  void updateView ();

private:
  struct PopupLookAndFeel : LookAndFeel
  {
    LookAndFeel& m_owner;
    PopupLookAndFeel (LookAndFeel& owner);
  };

  struct Callback : ModalComponentManager::Callback, PopupLookAndFeel
  {
    PopupButton& m_owner;
    Callback (PopupButton& owner);
    void modalStateFinished (int returnValue);
  };

private:
  ScopedPointer<Facade::Button> m_facade;
  ReferenceCountedObjectPtr<Model::Popup> m_model;
  int m_menuItemHeight;
};

//------------------------------------------------------------------------------

template<class Facade, class Model>
class PopupButtonType
  : public PopupButton
{
public:
  PopupButtonType (Facade* facade, Model* model)
    : PopupButton (facade, model)
    , m_model (*model)
    , m_facade (*facade)
  {
  }

  Model& getModel()
  {
    return m_model;
  }

  Facade& getFacade()
  {
    return m_facade;
  }

private:
  Model& m_model;
  Facade& m_facade;
};

}

}

#endif
