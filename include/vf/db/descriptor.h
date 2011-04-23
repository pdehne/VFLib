// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#ifndef __VF_DB_DESCRIPTOR_VFHEADER__
#define __VF_DB_DESCRIPTOR_VFHEADER__

namespace db {

class Descriptor
{
public:
  explicit Descriptor (VF_JUCE::File const& file);

  String const getFullPath () const { return m_fullPath; }

private:
  String m_fullPath;
};

}

#endif
