/*
   Copyright (C) 2008, 2009, 2010, 2011 Red Hat, Inc.
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

#include "locus.hh"
#include "section_id.hh"
#include <sstream>
#include <iostream>

std::ostream &
operator << (std::ostream &os, locus const &loc)
{
  os << loc.format ();
  return os;
}

char const *
locus_simple_fmt::offset_n ()
{
  return "offset";
}

void
locus_simple_fmt::hex (std::ostream &ss, uint64_t off)
{
  ss << "0x" << std::hex << off;
}

void
locus_simple_fmt::dec (std::ostream &ss, uint64_t off)
{
  ss << std::dec << off;
}

std::string
simple_locus_aux::format_simple_locus (char const *(*N) (),
				       void (*F) (std::ostream &, uint64_t),
				       bool brief, section_id sec, uint64_t off)
{
  std::stringstream ss;
  if (!brief)
    ss << section_name[sec];
  if (off != (uint64_t)-1)
    {
      if (!brief)
	ss << ": ";
      ss << N() << " ";
      F (ss, off);
    }
  return ss.str ();
}
