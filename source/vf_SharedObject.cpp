// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_SharedObject.h"
#include "vf/vf_SharedSingleton.h"

static Thread::Interrupted thread_idle ()
{
  Thread::Interrupted interrupted (false);

  return interrupted;
}

SharedObject::Singleton::Singleton ()
  : m_worker (__FILE__)
  , SharedSingleton (true) // persist
{
  m_worker.start (bind (&thread_idle));
}

SharedObject::Singleton::~Singleton ()
{
  // is this needed?
  m_worker.stop_and_wait ();
}

SharedObject::Singleton* SharedObject::Singleton::createInstance ()
{
  return new Singleton;
}

void SharedObject::Singleton::doDelete (SharedObject* sharedObject)
{
  delete sharedObject;
}

void SharedObject::Singleton::Delete (SharedObject* sharedObject)
{
  m_worker.call (&Singleton::doDelete, sharedObject);
}

void SharedObject::destroySharedObject ()
{
  Singleton::getInstance()->Delete (this);
}

END_VF_NAMESPACE
