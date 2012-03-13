// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_LISTBOXSELECTION_VFHEADER__
#define __VF_LISTBOXSELECTION_VFHEADER__

// Implements fast selection management for a really big list
// (hundreds of thousands or millions of rows).
//

class ListBoxSelection
{
public:
  struct RowData
  {
  };

  bool isSelected (int index) const;

  void select (int index);
  void selectAll ();

  void deselect (int index);
  void deselectAll ();

  void setSelected (int index, bool selected);


protected:

private:
  int m_id;
};

#endif

