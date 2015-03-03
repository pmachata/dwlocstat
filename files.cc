/*
   Copyright (C) 2011, 2012, 2015 Red Hat, Inc.
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

#include "files.hh"

namespace
{
  inline bool failed (void *ptr)
  { return ptr == NULL; }

  inline bool failed (int i)
  { return i < 0; }

  template <class T>
  inline T
  throw_if_failed (T x, char const *msg,
		   char const *(*errmsgcb) (int) = NULL)
  {
    if (failed (x))
      {
	std::stringstream ss;
	ss << msg;
	if (errmsgcb != NULL)
	  ss << ": " << errmsgcb (-1);
	throw std::runtime_error (ss.str ());
      }
    return x;
  }

  Dwfl *
  open_dwfl ()
  {
    const static Dwfl_Callbacks callbacks =
      {
	.find_elf = dwfl_build_id_find_elf,
	.find_debuginfo = dwfl_standard_find_debuginfo,
	.section_address = dwfl_offline_section_address,
      };

    return throw_if_failed (dwfl_begin (&callbacks),
			    "Couldn't initialize DWFL");
  }
}

dwfl::dwfl ()
  : m_context (open_dwfl ())
{}

namespace
{
  struct fd
  {
    int m_fd;
    operator int () { return m_fd; }

    fd (int fd)
      : m_fd (fd)
    {
      if (m_fd == -1)
	throw std::runtime_error (strerror (errno));
    }

    fd (fd const &that); /* never implemented */

    int
    release ()
    {
      int ret = m_fd;
      m_fd = -1;
      return ret;
    }

    ~fd ()
    {
      // Note that we ignore errors that could come from close.
      // This is good enough for this use case.
      ::close (m_fd);
    }
  };

  struct dwfl_report
  {
    Dwfl *m_dwfl;

    explicit dwfl_report (Dwfl *dwfl)
      : m_dwfl (dwfl)
    {
      dwfl_report_begin (m_dwfl);
    }

    Dwfl_Module *
    offline (Dwfl *dwfl, const char *name,
	     const char *file_name, int fd)
    {
      return throw_if_failed
	(dwfl_report_offline (m_dwfl, name, file_name, fd),
	 "dwfl_report_offline", dwfl_errmsg);
    }

    ~dwfl_report ()
    {
      dwfl_report_end (m_dwfl, NULL, NULL);
    }
  };
}

Dwarf *
dwfl::open_dwarf (char const *fname)
{
  Dwfl_Module *mod;
  {
    fd fd = open (fname, O_RDONLY);

    dwfl_report report (m_context);
    mod = report.offline (m_context, fname, fname, fd);

    fd.release ();
  }

  Dwarf_Addr bias;
  throw_if_failed (dwfl_module_getelf (mod, &bias),
		   "Couldn't open ELF.", dwfl_errmsg);

  Dwarf *ret = throw_if_failed (dwfl_module_getdwarf (mod, &bias),
				"Couldn't obtain DWARF descriptor",
				dwfl_errmsg);
  return ret;
}

dwfl::~dwfl ()
{
  dwfl_end (m_context);
}
