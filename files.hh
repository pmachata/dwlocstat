/*
   Copyright (C) 2011, 2012 Red Hat, Inc.
   This file is part of dwlocstat.

   This file is free software; you can redistribute it and/or modify
   it under the terms of either

     * the GNU Lesser General Public License as published by the Free
       Software Foundation; either version 3 of the License, or (at
       your option) any later version

   or

     * the GNU General Public License as published by the Free
       Software Foundation; either version 2 of the License, or (at
       your option) any later version

   or both in parallel, as here.

   elfutils is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received copies of the GNU General Public License and
   the GNU Lesser General Public License along with this program.  If
   not, see <http://www.gnu.org/licenses/>.  */

#ifndef _DWARFLINT_FILES_H_
#define _DWARFLINT_FILES_H_

#include <elfutils/libdwfl.h>
#include <elfutils/libdw.h>

// The functions in this module do their own error handling, and throw
// std::runtime_error with descriptive error message on error.
namespace files
{
  int open (char const *fname);

  Dwfl *open_dwfl ()
    __attribute__ ((nonnull, malloc));

  Dwarf *open_dwarf (Dwfl *dwfl, char const *fname, int fd)
    __attribute__ ((nonnull, malloc));
}

class dwfl
{
  Dwfl *m_context;
public:
  dwfl ()
    : m_context (files::open_dwfl ())
  {}

  Dwarf *
  open_dwarf (char const *fname)
  {
    return files::open_dwarf (m_context, fname, files::open (fname));
  }

  ~dwfl ()
  {
    dwfl_end (m_context);
  }
};

#endif /* _DWARFLINT_FILES_H_ */
