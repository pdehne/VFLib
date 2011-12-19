// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_COMPONENTBROADCAST_VFHEADER__
#define __VF_COMPONENTBROADCAST_VFHEADER__

// Calling this functor will recursively traverse a Component and all of its
// children, and call a member function for each Component that exposes the
// interface (determined via dynamic_cast).
//
// This requires boost::bind, std::tr1::bind, or std::bind to work.
//
/* Usage example

  struct PersistentObject
  {
    virtual void onSaveSettings (XmlElement* xml) = 0;
    virtual void onLoadSettings (XmlElement* xml) = 0;
  };

  void saveAllWindowSettings (XmlElement* xml)
  {
    for (int i = 0; i < Desktop::getInstance().getNumComponents(); ++i)
    {
      Component* c = Desktop::getInstance().getComponent (i);
    
      // Any Component of any open window that derives from PersistentObject
      // will have its onSaveSettings() member called by this broadcast.
      //
      componentBroadcast (c, &PersistentObject::onSaveSettings, xml);
    }
  }
*/

class componentBroadcast
{
public:
  template <class C>
  componentBroadcast (Component* c, void (C::*f)())
  { call <C> (c, boost::bind (f, _1)); }

  template <class C, class T1>
  componentBroadcast (Component* c, void (C::*f)(T1), T1 t1)
  { call <C> (c, boost::bind (f, _1, t1)); }

  template <class C, class T1, class T2>
  componentBroadcast (Component* c, void (C::*f)(T1, T2), T1 t1, T2 t2)
  { call <C> (c, boost::bind (f, _1, t1, t2)); }

  template <class C, class T1, class T2, class T3>
  componentBroadcast (Component* c, void (C::*f)(T1, T2, T3), T1 t1, T2 t2, T3 t3)
  { call <C> (c, boost::bind (f, _1, t1, t2, t3)); }

  template <class C, class T1, class T2, class T3, class T4>
  componentBroadcast (Component* c, void (C::*f)(T1, T2, T3, T4), T1 t1, T2 t2, T3 t3, T4 t4)
  { call <C> (c, boost::bind (f, _1, t1, t2, t3, t4)); }

  template <class C, class T1, class T2, class T3, class T4, class T5>
  componentBroadcast (Component* c, void (C::*f)(T1, T2, T3, T4, T5), T1 t1, T2 t2, T3 t3, T4 t4, T5 t5)
  { call <C> (c, boost::bind (f, _1, t1, t2, t3, t4, t5)); }

  template <class C, class T1, class T2, class T3, class T4, class T5, class T6>
  componentBroadcast (Component* c, void (C::*f)(T1, T2, T3, T4, T5, T6),
             T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6)
  { call <C> (c, boost::bind (f, _1, t1, t2, t3, t4, t5, t6)); }

  template <class C, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
  componentBroadcast (Component* c, void (C::*f)(T1, T2, T3, T4, T5, T6, T7),
             T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7)
  { call <C> (c, boost::bind (f, _1, t1, t2, t3, t4, t5, t6, t7)); }

  template <class C, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
  componentBroadcast (Component* c, void (C::*f)(T1, T2, T3, T4, T5, T6, T7, T8),
             T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8)
  { call <C> (c, boost::bind (f, _1, t1, t2, t3, t4, t5, t6, t7, t8)); }

private:
  template <class Interface, class Functor>
  void call (Component* component, Functor const& f)
  {
    Interface* const object = dynamic_cast <Interface*> (component);

    if (object != nullptr)
      f (object);

    for (int i = 0; i < component->getNumChildComponents (); ++i)
      call <Interface> (component->getChildComponent (i), f);
  }
};

// Like componentBroadcast, but sends the message to the first
// parent it finds that exposes the interface (then stops).
//
class componentBroadcastParent
{
public:
  template <class C>
  componentBroadcastParent (Component* c, void (C::*f)())
  { call <C> (c, boost::bind (f, _1)); }

  template <class C, class T1>
  componentBroadcastParent (Component* c, void (C::*f)(T1), T1 t1)
  { call <C> (c, boost::bind (f, _1, t1)); }

  template <class C, class T1, class T2>
  componentBroadcastParent (Component* c, void (C::*f)(T1, T2), T1 t1, T2 t2)
  { call <C> (c, boost::bind (f, _1, t1, t2)); }

  template <class C, class T1, class T2, class T3>
  componentBroadcastParent (Component* c, void (C::*f)(T1, T2, T3), T1 t1, T2 t2, T3 t3)
  { call <C> (c, boost::bind (f, _1, t1, t2, t3)); }

  template <class C, class T1, class T2, class T3, class T4>
  componentBroadcastParent (Component* c, void (C::*f)(T1, T2, T3, T4), T1 t1, T2 t2, T3 t3, T4 t4)
  { call <C> (c, boost::bind (f, _1, t1, t2, t3, t4)); }

  template <class C, class T1, class T2, class T3, class T4, class T5>
  componentBroadcastParent (Component* c, void (C::*f)(T1, T2, T3, T4, T5), T1 t1, T2 t2, T3 t3, T4 t4, T5 t5)
  { call <C> (c, boost::bind (f, _1, t1, t2, t3, t4, t5)); }

  template <class C, class T1, class T2, class T3, class T4, class T5, class T6>
  componentBroadcastParent (Component* c, void (C::*f)(T1, T2, T3, T4, T5, T6),
             T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6)
  { call <C> (c, boost::bind (f, _1, t1, t2, t3, t4, t5, t6)); }

  template <class C, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
  componentBroadcastParent (Component* c, void (C::*f)(T1, T2, T3, T4, T5, T6, T7),
             T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7)
  { call <C> (c, boost::bind (f, _1, t1, t2, t3, t4, t5, t6, t7)); }

  template <class C, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
  componentBroadcastParent (Component* c, void (C::*f)(T1, T2, T3, T4, T5, T6, T7, T8),
             T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8)
  { call <C> (c, boost::bind (f, _1, t1, t2, t3, t4, t5, t6, t7, t8)); }

private:
  template <class Interface, class Functor>
  void call (Component* component, Functor const& f)
  {
    component = component->getParentComponent();
    
    while (component != nullptr)
    {
      Interface* const object = dynamic_cast <Interface*> (component);

      if (object != nullptr)
      {
        f (object);
        break;
      }

      component = component->getParentComponent();
    }
  }
};

#endif
