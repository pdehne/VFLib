// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_DRAGANDDROPDATA_VFHEADER__
#define __VF_DRAGANDDROPDATA_VFHEADER__

#include "vf/vf_List.h"

// A useful container that encapsulates data suitable for Drag and Drop
//
class DragAndDropData : public VF_JUCE::ReferenceCountedObject
{
public:
  typedef VF_JUCE::ReferenceCountedObjectPtr <DragAndDropData> Ptr;

  DragAndDropData ();

  ~DragAndDropData ();

  template <class Object>
  void add (Object const& object)
  {
    vfassert (find <Object> () == nullptr);
    m_items.push_back (*new StoredItem <Object> (object));
  }

  template <class Object>
  bool find (Object* pResult = 0) const
  {
    bool found = false;

    for (Items::const_iterator iter = m_items.begin (); iter != m_items.end ();)
    {
      Item const* item = &(*iter++);
      StoredItem <Object> const* stored =
        dynamic_cast <StoredItem <Object> const*> (item);

      if (stored != 0)
      {
        if (pResult)
          *pResult = stored->getObject ();
        found = true;
        break;
      }
    }
    
    return found;
  }

private:
  class Item;
  typedef List <Item> Items;

  class Item : public Items::Node
  {
  public:
    virtual ~Item() { }
  };

  template <class Object>
  class StoredItem : public Item
  {
  public:
    explicit StoredItem (Object const& object)
      : m_object (object)
    {
    }

    Object const& getObject () const { return m_object; }
  
  private:
    Object m_object;
  };

  Items m_items;
};

#endif
