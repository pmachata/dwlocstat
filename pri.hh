/*
   Copyright (C) 2009, 2010, 2011, 2012 Red Hat, Inc.
   This file is part of dwlocstat.

   This file is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   elfutils is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef DWARFLINT_PRI_H
#define DWARFLINT_PRI_H

#include <elfutils/libdw.h>
#include <string>

namespace pri
{
  class ref
  {
    Dwarf_Off off;
  public:
    ref (Dwarf_Die *die)
      : off (dwarf_dieoffset (die))
    {}
    friend std::ostream &operator << (std::ostream &os, ref const &obj);
  };
  std::ostream &operator << (std::ostream &os, ref const &obj);

  class hex
  {
    Dwarf_Off value;
    char const *const pre;
  public:
    hex (Dwarf_Off a_value, char const *a_pre = NULL)
      : value (a_value)
      , pre (a_pre)
    {}
    friend std::ostream &operator << (std::ostream &os, hex const &obj);
  };
  std::ostream &operator << (std::ostream &os, hex const &obj);

  struct addr: public hex {
    addr (Dwarf_Off off) : hex (off) {}
  };

  struct DIE: public hex {
    DIE (Dwarf_Off off) : hex (off, "DIE ") {}
  };

  struct CU: public hex {
    CU (Dwarf_Off off) : hex (off, "CU ") {}
  };

  class range
  {
    Dwarf_Off start;
    Dwarf_Off end;
  public:
    range (Dwarf_Off a_start, Dwarf_Off a_end)
      : start (a_start), end (a_end)
    {}
    friend std::ostream &operator << (std::ostream &os, range const &obj);
  };
  std::ostream &operator << (std::ostream &os, range const &obj);

  std::string attr_name (int name);
}

#endif//DWARFLINT_PRI_H
