/*
   Copyright (C) 2011, 2012 Red Hat, Inc.
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

#ifndef DWLOCSTAT_FILES_HH
#define DWLOCSTAT_FILES_HH

#include <elfutils/libdwfl.h>
#include <elfutils/libdw.h>

class dwfl
{
  Dwfl *m_context;
public:
  dwfl ();
  Dwarf *open_dwarf (char const *fname);
  ~dwfl ();
};

#endif /* DWLOCSTAT_FILES_HH */
