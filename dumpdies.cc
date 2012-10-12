/*
   Copyright (C) 2012 Red Hat, Inc.
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

#include <dwarf.h>
#include <cinttypes>

#include "iterators.hh"
#include "dwarfstrings.h"
#include "files.hh"

int
main(int argc, char *argv[])
{
  dwfl dwfl;
  Dwarf *dw = dwfl.open_dwarf (argv[1]);

  for (all_dies_iterator it (dw); it != all_dies_iterator::end (); ++it)
    {
      if (dwarf_tag (*it) == DW_TAG_compile_unit)
	printf (" Compilation unit at offset %#" PRIx64 ":\n",
		dwarf_dieoffset (*it));
      auto stk = it.stack ();
      for (auto &die: stk)
	printf (" %#" PRIx64, dwarf_dieoffset (&die));
      printf ("\n  ");
      for (attr_iterator at (*it); at != attr_iterator::end (); ++at)
	printf (" %s", dwarf_attr_string (dwarf_whatattr (*at)));
      printf ("\n");
    }

  return 0;
}
