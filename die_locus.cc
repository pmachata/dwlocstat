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

#include <sstream>

#include "die_locus.hh"
#include "pri.hh"

char const *
locus_simple_fmt::cu_n ()
{
  return "CU";
}

std::string
die_locus::format (bool brief) const
{
  std::stringstream ss;
  if (!brief)
    ss << section_name[sec_info] << ": ";

  ss << "DIE 0x" << std::hex << _m_offset;

  if (_m_attrib_name != -1)
    ss << ", attr. " << pri::attr_name (_m_attrib_name);

  return ss.str ();
}

