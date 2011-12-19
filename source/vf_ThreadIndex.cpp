// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_List.h"
#include "vf/vf_ThreadIndex.h"

namespace ThreadIndex {

namespace {

struct Item;
typedef List <Item> Items;

static Items s_items;

struct Item : Items::Node
{
};

//juce_ThreadLocal Item t_item;

int getCurrent ()
{
  int index = 0;

  return index;
}

}

}

END_VF_NAMESPACE
