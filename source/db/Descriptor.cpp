// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/db/Descriptor.h"

namespace db {

Descriptor::Descriptor (VF_JUCE::File const& file)
  : m_fullPath (file.getFullPathName ())
{
}

}

END_VF_NAMESPACE
