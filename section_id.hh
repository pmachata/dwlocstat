/*
   Copyright (C) 2009, 2010, 2011 Red Hat, Inc.
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

#ifndef DWARFLINT_SECTION_ID_HH
#define DWARFLINT_SECTION_ID_HH

#define DEBUGINFO_SECTIONS \
  SEC (info)		   \
  SEC (abbrev)		   \
  SEC (aranges)		   \
  SEC (pubnames)	   \
  SEC (pubtypes)	   \
  SEC (str)		   \
  SEC (line)		   \
  SEC (loc)		   \
  SEC (mac)		   \
  SEC (ranges)

enum section_id
  {
    sec_invalid = 0,

    /* Debuginfo sections:  */
#define SEC(n) sec_##n,
    DEBUGINFO_SECTIONS
#undef SEC

    count_debuginfo_sections
  };

// section_name[0] is for sec_invalid.  The last index is for
// count_debuginfo_sections and is NULL.
extern char const *section_name[];

#endif//DWARFLINT_SECTION_ID_HH
