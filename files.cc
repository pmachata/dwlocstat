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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include <cstring>
#include <cerrno>
#include <stdexcept>
#include <unistd.h>

#include "eu-config.h"
#include "files.hh"
#include "messages.hh"

namespace
{
  inline bool failed (void *ptr) { return ptr == NULL; }
  inline bool failed (int i) { return i < 0; }

  template <class T>
  inline T
  throw_if_failed (T x, char const *msg,
		   char const *(*errmsgcb) (int) = NULL)
  {
    if (unlikely (failed (x)))
      {
	std::stringstream ss;
	ss << msg;
	if (errmsgcb != NULL)
	  ss << ": " << errmsgcb (-1);
	throw std::runtime_error (ss.str ());
      }
    return x;
  }

  char const *
  mystrerror (int i)
  {
    if (i == -1)
      i = errno;
    return strerror (i);
  }
}

int
files::open (char const *fname)
{
  int fd = ::open (fname, O_RDONLY);
  if (fd == -1)
    {
      std::stringstream ss;
      ss << "Cannot open input file: " << strerror (errno) << ".";
      throw std::runtime_error (ss.str ());
    }

  return fd;
}

Dwfl *
files::open_dwfl ()
{
  static class my_callbacks
    : public Dwfl_Callbacks
  {
    // Stub libdwfl callback, only the ELF handle already open is ever used.
    static int
    find_no_debuginfo (Dwfl_Module *mod __attribute__ ((unused)),
		       void **userdata __attribute__ ((unused)),
		       const char *modname __attribute__ ((unused)),
		       Dwarf_Addr base __attribute__ ((unused)),
		       const char *file_name __attribute__ ((unused)),
		       const char *debuglink_file __attribute__ ((unused)),
		       GElf_Word debuglink_crc __attribute__ ((unused)),
		       char **debuginfo_file_name __attribute__ ((unused)))
    {
      return -1;
    }

  public:
    my_callbacks ()
    {
      section_address = dwfl_offline_section_address;
      find_debuginfo = find_no_debuginfo;
    }
  } cbs;

  return throw_if_failed (dwfl_begin (&cbs),
			  "Couldn't initialize DWFL");
}

Dwarf *
files::open_dwarf (Dwfl *dwfl, char const *fname, int fd)
{
  dwfl_report_begin (dwfl);

  // Dup FD for dwfl to consume.
  int dwfl_fd
    = throw_if_failed (dup (fd), "Error: dup", mystrerror);

  Dwfl_Module *mod
    = throw_if_failed (dwfl_report_offline (dwfl, fname, fname, dwfl_fd),
		       "Couldn't add DWFL module", dwfl_errmsg);
  dwfl_report_end (dwfl, NULL, NULL);
  Dwarf_Addr bias;
  throw_if_failed (dwfl_module_getelf (mod, &bias),
		   "Couldn't open ELF.", dwfl_errmsg);
  return throw_if_failed (dwfl_module_getdwarf (mod, &bias),
			  "Couldn't obtain DWARF descriptor", dwfl_errmsg);
}
