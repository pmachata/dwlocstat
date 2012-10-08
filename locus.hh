/*
   Copyright (C) 2009, 2010, 2011 Red Hat, Inc.
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

#ifndef DWARFLINT_WHERE_HH
#define DWARFLINT_WHERE_HH

#include "section_id.hh"

#include <stdint.h>
#include <iosfwd>
#include <string>

/// Instances of the locus subclasses are used as pointers into
/// debuginfo for documentation purposes (messages and errors).  They
/// are usually tiny structures, and should be used as values, but we
/// need the abstract interface to be able to format them, and copy
/// them into ref_record.
class locus
{
public:
  virtual std::string format (bool brief = false) const = 0;
  virtual ~locus () {}
};

std::ostream &operator << (std::ostream &os, locus const &loc);

/// Helper class for simple_locus to reduce the template bloat.
class simple_locus_aux
{
protected:
  static std::string format_simple_locus (char const *(*N) (),
					  void (*F) (std::ostream &, uint64_t),
					  bool brief, section_id sec,
					  uint64_t off);
};

/// Template for quick construction of straightforward locus
/// subclasses (one address, one way of formatting).  N should be a
/// function that returns the name of the argument.
/// locus_simple_fmt::hex, locus_simple_fmt::dec would be candidate
/// parameters for argument F.
template<char const *(*N) (),
	 void (*F) (std::ostream &, uint64_t)>
class simple_locus
  : public locus
  , private simple_locus_aux
{
  section_id _m_sec;
  uint64_t _m_offset;

public:
  explicit simple_locus (section_id sec, uint64_t offset = -1)
    : _m_sec (sec)
    , _m_offset (offset)
  {}

  std::string
  format (bool brief = false) const
  {
    return format_simple_locus (N, F, brief, _m_sec, _m_offset);
  }
};

/// Constructor of simple_locus that fixes the section_id argument.
template<section_id S,
	 char const *(*N) (),
	 void (*F) (std::ostream &, uint64_t)>
class fixed_locus
  : public simple_locus<N, F>
{
public:
  explicit fixed_locus (uint64_t offset = -1)
    : simple_locus<N, F> (S, offset)
  {}
};

namespace locus_simple_fmt
{
  char const *offset_n ();
  void hex (std::ostream &ss, uint64_t off);
  void dec (std::ostream &ss, uint64_t off);
}

/// Straightforward locus for cases where either offset is not
/// necessary at all, or if it is present, it's simply shown as
/// "offset: 0xf00".
typedef simple_locus<locus_simple_fmt::offset_n,
		     locus_simple_fmt::hex> section_locus;

#endif//DWARFLINT_WHERE_HH
