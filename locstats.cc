/*
   Copyright (C) 2010, 2011, 2012 Red Hat, Inc.
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sstream>
#include <bitset>
#include <cstring>
#include <libintl.h>
#include <algorithm>
#include <dwarf.h>

#include "iterators.hh"
#include "option.hh"
#include "pri.hh"
#include "files.hh"
#include "die_locus.hh"
#include "dwarfstrings.h"

#define DIE_OPTSTRING				\
  "}[,...]"

global_opt<string_option> opt_ignore
  ("Skip certain DIEs.  class may be one of single_addr, artificial, inlined, \
inlined_subroutine, no_coverage, mutable, or immutable.",
   "class[,...]", "ignore");

global_opt<string_option> opt_dump
  ("Dump certain DIEs.  For classes, see option 'ignore'.",
   "class[,...]", "dump");

global_opt<string_option> opt_tabulation_rule
  ("Rule for sorting results into buckets. start is either integer 0..100, \
or special value 0.0 indicating cases with no coverage whatsoever \
(i.e. not those that happen to round to 0%).",
   "start[:step][,...]", "tabulate");

// where.c needs to know how to format certain wheres.  The module
// doesn't know that we don't use these :)
extern "C"
bool
show_refs ()
{
  return false;
}

#define DIE_TYPES		\
  TYPE(single_addr)		\
  TYPE(artificial)		\
  TYPE(inlined)			\
  TYPE(inlined_subroutine)	\
  TYPE(no_coverage)		\
  TYPE(mutable)			\
  TYPE(immutable)

struct tabrule
{
  int start;
  int step;
  tabrule (int a_start, int a_step)
    : start (a_start), step (a_step)
  {}
  bool operator < (tabrule const &other) const {
    return start < other.start;
  }
};

// Sharp 0.0% coverage (i.e. not a single address byte is covered)
const int cov_00 = -1;

struct tabrules_t
  : public std::vector<tabrule>
{
  tabrules_t (std::string const &rule)
  {
    std::stringstream ss;
    ss << rule;

    std::string item;
    while (std::getline (ss, item, ','))
      {
	if (item.empty ())
	  continue;
	int start;
	int step;
	char const *ptr = item.c_str ();

	if (item.length () >= 3
	    && std::strncmp (ptr, "0.0", 3) == 0)
	  {
	    start = cov_00;
	    ptr += 3;
	  }
	else
	  start = std::strtol (ptr, const_cast<char **> (&ptr), 10);

	if (*ptr == 0)
	  step = 0;
	else
	  {
	    if (*ptr != ':')
	      {
		step = 0;
		goto garbage;
	      }
	    else
	      ptr++;

	    step = std::strtol (ptr, const_cast<char **> (&ptr), 10);
	    if (*ptr != 0)
	    garbage:
	      std::cerr << "Ignoring garbage at the end of the rule item: '"
			<< ptr << '\'' << std::endl;
	  }

	push_back (tabrule (start, step));
      }

    push_back (tabrule (100, 0));
    std::sort (begin (), end ());
  }

  void next ()
  {
    if (at (0).step == 0)
      erase (begin ());
    else
      {
	if (at (0).start == cov_00)
	  at (0).start = 0;
	at (0).start += at (0).step;
	if (size () > 1)
	  {
	    if (at (0).start > at (1).start)
	      erase (begin ());
	    while (size () > 1
		   && at (0).start == at (1).start)
	      erase (begin ());
	  }
      }
  }

  bool match (int value) const
  {
    return at (0).start == value;
  }
};

#define TYPE(T) dt_##T,
  enum die_type_e
    {
      DIE_TYPES
      dt__count
    };
#undef TYPE

class die_type_matcher
  : public std::bitset<dt__count>
{
  class invalid {};
  std::pair<die_type_e, bool>
  parse (std::string &desc)
  {
    bool val = true;
    if (desc == "")
      throw invalid ();

#define TYPE(T)					\
    if (desc == #T)				\
      return std::make_pair (dt_##T, val);
    DIE_TYPES
#undef TYPE

      throw invalid ();
  }

public:
  die_type_matcher (std::string const &rule)
  {
    std::stringstream ss;
    ss << rule;

    std::string item;
    while (std::getline (ss, item, ','))
      try
	{
	  std::pair<die_type_e, bool> const &ig = parse (item);
	  set (ig.first, ig.second);
	}
      catch (invalid &i)
	{
	  std::cerr << "Invalid die type: " << item << std::endl;
	}
  }
};

class mutability_t
{
  bool _m_is_mutable;
  bool _m_is_immutable;

public:
  mutability_t ()
    : _m_is_mutable (false)
    , _m_is_immutable (false)
  {
  }

  void set (bool what)
  {
    if (what)
      _m_is_mutable = true;
    else
      _m_is_immutable = true;
  }

  void set_both ()
  {
    set (true);
    set (false);
  }

  void locexpr (Dwarf_Op *expr, size_t len)
  {
    // We scan the expression looking for DW_OP_{bit_,}piece
    // operators which mark ends of sub-expressions to us.
    bool m = true;
    for (size_t i = 0; i < len; ++i)
      switch (expr[i].atom)
	{
	case DW_OP_implicit_value:
	case DW_OP_stack_value:
	  m = false;
	  break;

	case DW_OP_bit_piece:
	case DW_OP_piece:
	  set (m);
	  m = true;
	  break;
	};
    set (m);
  }

  bool is_mutable () const { return _m_is_mutable; }
  bool is_immutable () const { return _m_is_immutable; }
};

struct error
  : public std::runtime_error
{
  explicit error (std::string const &what_arg)
    : std::runtime_error (what_arg)
  {}
};

typedef std::vector<std::pair<Dwarf_Addr, Dwarf_Addr> > ranges_t;

ranges_t
die_ranges (Dwarf_Die *die)
{
  Dwarf_Addr base;
  Dwarf_Addr start, end;
  ranges_t ret;
  for (ptrdiff_t it = 0;
       (it = dwarf_ranges (die, it, &base, &start, &end)) != 0; )
    ret.push_back (std::make_pair (start, end));
  return ret;
}

// Look through parental dies and return the non-empty ranges instance
// closest to IT hierarchically.
ranges_t
find_ranges (all_dies_iterator it)
{
  for (; it != all_dies_iterator::end (); it = it.parent ())
    {
      auto ranges = die_ranges (*it);
      if (!ranges.empty ())
	return ranges;
    }
  throw error ("no ranges for this DIE");
}

bool
die_flag_value (Dwarf_Die *die, unsigned attr_name)
{
  Dwarf_Attribute attr;
  bool val;

  // XXX do we need dwarf_attr_integrate here?
  if (dwarf_attr (die, attr_name, &attr) != nullptr)
    {
      if (dwarf_formflag (&attr, &val) != 0)
	throw error (std::string ("dwarf_formflag(")
		     + dwarf_attr_string (attr_name)
		     + "): " + dwarf_errmsg (-1));
      return val;
    }
  return false;
}

bool
is_inlined (Dwarf_Die *die)
{
  return die_flag_value (die, DW_AT_inline);
}

void
process (Dwarf *dw)
{
  // map percentage->occurrences.  Percentage is cov_00..100, where
  // 0..100 is rounded-down integer division.
  std::map<int, unsigned long> tally;
  unsigned long total = 0;
  for (int i = 0; i <= 100; ++i)
    tally[i] = 0;

  tabrules_t tabrules (opt_tabulation_rule.seen ()
		       ? opt_tabulation_rule.value () : "10:10");
  die_type_matcher ignore (opt_ignore.seen () ? opt_ignore.value () : "");
  die_type_matcher dump (opt_dump.seen () ? opt_dump.value () : "");
  std::bitset<dt__count> interested = ignore | dump;
  bool interested_mutability
    = interested.test (dt_mutable) || interested.test (dt_immutable);

  for (all_dies_iterator it (dw); it != all_dies_iterator::end (); ++it)
    {
      std::bitset<dt__count> die_type;
      Dwarf_Die *die = *it;

      // We are interested in variables and formal parameters
      bool is_formal_parameter = dwarf_tag (die) == DW_TAG_formal_parameter;
      if (!is_formal_parameter && dwarf_tag (die) != DW_TAG_variable)
	continue;

      // Ignore those that are just declarations
      if (die_flag_value (die, DW_AT_declaration))
	continue;

      // Possibly ignore artificial, unless configured othewise.
      if (ignore.test (dt_artificial)
	  && die_flag_value (die, DW_AT_artificial))
	continue;

      // Of formal parameters we ignore those that are children of
      // subprograms that are themselves declarations.
      if (is_formal_parameter)
	{
	  Dwarf_Die *parent = *it.parent ();
	  if (dwarf_tag (parent) == DW_TAG_subroutine_type
	      || die_flag_value (parent, DW_AT_declaration))
	    continue;
	}

      if (interested.test (dt_inlined)
	  || interested.test (dt_inlined_subroutine))
	{
	  bool inlined = false;
	  bool inlined_subroutine = false;
	  for (auto &die2: it.stack ())
	    {
	      if (interested.test (dt_inlined)
		  && dwarf_tag (&die2) == DW_TAG_subprogram
		  && is_inlined (&die2))
		{
		  inlined = true;
		  if (interested.test (dt_inlined_subroutine)
		      && inlined_subroutine)
		    break;
		}
	      if (interested.test (dt_inlined_subroutine)
		  && dwarf_tag (&die2) == DW_TAG_inlined_subroutine)
		{
		  inlined_subroutine = true;
		  if (interested.test (dt_inlined)
		      && inlined)
		    break;
		}
	    }

	  if (inlined)
	    {
	      if (ignore.test (dt_inlined))
		continue;
	      die_type.set (dt_inlined);
	    }
	  if (inlined_subroutine)
	    {
	      if (ignore.test (dt_inlined_subroutine))
		continue;
	      die_type.set (dt_inlined_subroutine, inlined_subroutine);
	    }
	}

      Dwarf_Attribute locattr_mem,
	*locattr = dwarf_attr_integrate (die, DW_AT_location, &locattr_mem);

      // Also ignore extern globals -- these have DW_AT_external and
      // no DW_AT_location.
      if (die_flag_value (die, DW_AT_external) && locattr == NULL)
	continue;

      if (locattr == NULL)
	locattr = dwarf_attr (die, DW_AT_const_value, &locattr_mem);

      /*
      Dwarf_Attribute name_attr_mem,
	*name_attr = dwarf_attr_integrate (die, DW_AT_name, &name_attr_mem);
      std::string name = name_attr != NULL
	? dwarf_formstring (name_attr)
	: (dwarf_hasattr_integrate (die, DW_AT_artificial)
	   ? "<artificial>" : "???");

      std::cerr << "die=" << std::hex << die.offset ()
		<< " '" << name << '\'';
      */

      int coverage;
      Dwarf_Op *expr;
      size_t len;
      mutability_t mut;

      // no location
      if (locattr == NULL)
	{
	  coverage = cov_00;
	  if (interested_mutability)
	    mut.set_both ();
	}

      // consts need no location
      else if (dwarf_whatattr (locattr) == DW_AT_const_value)
	{
	  coverage = 100;
	  if (interested_mutability)
	    mut.set (false);
	}

      // non-list location
      else if (dwarf_getlocation (locattr, &expr, &len) == 0)
	{
	  // Globals and statics have non-list location that is a
	  // singleton DW_OP_addr expression.
	  if (len == 1 && expr[0].atom == DW_OP_addr)
	    die_type.set (dt_single_addr);
	  if (interested_mutability)
	    mut.locexpr (expr, len);
	  coverage = (len == 0) ? cov_00 : 100;
	}

      // location list
      else
	{
	  try
	    {
	      ranges_t ranges = find_ranges (it);
	      size_t length = 0;
	      size_t covered = 0;

	      // Arbitrarily assume that there will be no more than 10
	      // expressions per address.
	      size_t nlocs = 10;
	      Dwarf_Op *exprs[nlocs];
	      size_t exprlens[nlocs];

	      for (auto rit = ranges.begin (); rit != ranges.end (); ++rit)
		{
		  Dwarf_Addr low = rit->first;
		  Dwarf_Addr high = rit->second;
		  length += high - low;
		  //std::cerr << " " << low << ".." << high << std::endl;

		  for (Dwarf_Addr addr = low; addr < high; ++addr)
		    {
		      int got = dwarf_getlocation_addr (locattr, addr,
							exprs, exprlens, nlocs);
		      if (got < 0)
			throw ::error (std::string ("dwarf_getlocation_addr: ")
				       + dwarf_errmsg (-1));

		      // At least one expression for the address must
		      // be of non-zero length for us to count that
		      // address as covered.
		      bool cover = false;
		      for (int i = 0; i < got; ++i)
			{
			  if (interested_mutability)
			    mut.locexpr (exprs[i], exprlens[i]);
			  if (exprlens[i] > 0)
			    cover = true;
			}

		      if (cover)
			covered++;
		    }
		}

	      if (length == 0 || covered == 0)
		coverage = cov_00;
	      else
		coverage = 100 * covered / length;
	    }
	  catch (::error const &e)
	    {
	      std::cerr << "error: " << die_locus (die) << ": "
			<< e.what () << '.' << std::endl;

	      // Skip this DIE altogether.
	      continue;
	    }
	}

      if ((ignore & die_type).any ())
	continue;

      if (coverage == cov_00)
	{
	  if (ignore.test (dt_no_coverage))
	    continue;
	  die_type.set (dt_no_coverage);
	}
      else if (interested_mutability)
	{
	  assert (mut.is_mutable () || mut.is_immutable ());
	  if (mut.is_mutable ())
	    {
	      if (ignore.test (dt_mutable))
		continue;
	      die_type.set (dt_mutable);
	    }
	  if (mut.is_immutable ())
	    {
	      if (ignore.test (dt_immutable))
		continue;
	      die_type.set (dt_immutable);
	    }
	}

      if ((dump & die_type).any ())
	{
#define TYPE(T) << (die_type.test (dt_##T) ? " "#T : "")
	  std::cerr << "dumping" DIE_TYPES << " DIE" << std::endl;
#undef TYPE

	  std::string pad = "";
	  for (auto &die2: it.stack ())
	    {
	      std::cerr << pad << pri::ref (&die2) << " "
			<< dwarf_tag_string (dwarf_tag (&die2)) << std::endl;
	      pad += " ";
	    }
	}

      tally[coverage]++;
      total++;
      //std::cerr << std::endl;
    }

  unsigned long cumulative = 0;
  unsigned long last = 0;
  int last_pct = cov_00;
  if (total == 0)
    {
      std::cout << "No coverage recorded." << std::endl;
      return;
    }

  std::cout << "cov%\tsamples\tcumul" << std::endl;
  for (int i = cov_00; i <= 100; ++i)
    {
      cumulative += tally.find (i)->second;
      if (tabrules.match (i))
	{
	  long int samples = cumulative - last;

	  // The case 0.0..x should be printed simply as 0
	  if (last_pct == cov_00 && i > cov_00)
	    last_pct = 0;

	  if (last_pct == cov_00)
	    std::cout << "0.0";
	  else
	    std::cout << std::dec << last_pct;

	  if (last_pct != i)
	    std::cout << ".." << i;
	  std::cout << "\t" << samples
		    << '/' << (100*samples / total) << '%'
		    << "\t" << cumulative
		    << '/' << (100*cumulative / total) << '%'
		    << std::endl;
	  last = cumulative;
	  last_pct = i + 1;

	  tabrules.next ();
	}
    }
}

int
main(int argc, char *argv[])
{
  /* Set locale.  */
  setlocale (LC_ALL, "");

  /* Initialize the message catalog.  */
  textdomain ("dwlocstats");

  /* Parse and process arguments.  */
  argppp argp (global_opts ());
  int remaining;
  argp.parse (argc, argv, 0, &remaining);

  if (remaining == argc)
    {
      fputs (gettext ("Missing file name.\n"), stderr);
      argp.help (stderr, ARGP_HELP_SEE | ARGP_HELP_EXIT_ERR,
		 program_invocation_short_name);
      std::exit (1);
    }

  bool only_one = remaining + 1 == argc;
  do
    {
      try
	{
	  char const *fname = argv[remaining];
	  if (!only_one)
	    std::cout << std::endl << fname << ":" << std::endl;

	  dwfl dwfl;
	  Dwarf *dw = dwfl.open_dwarf (fname);
	  process (dw);
	}
      catch (std::runtime_error &e)
	{
	  std::cerr << "error: "
		    << e.what () << '.' << std::endl;
	  continue;
	}
    }
  while (++remaining < argc);
}
