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

#ifndef DWARFLINT_MESSAGES_HH
#define DWARFLINT_MESSAGES_HH

#include <string>
#include <iosfwd>
#include <vector>
#include <sstream>
#include <map>

#include <elfutils/libdw.h>

#include "locus.hh"

#define MESSAGE_CATEGORIES						\
  /* Severity: */							\
  MC (impact_1,  0)  /* no impact on the consumer */			\
  MC (impact_2,  1)  /* still no impact, but suspicious or worth mentioning */ \
  MC (impact_3,  2)  /* some impact */					\
  MC (impact_4,  3)  /* high impact */					\
									\
  /* Accuracy:  */							\
  MC (acc_bloat, 4)  /* unnecessary constructs (e.g. unreferenced strings) */ \
  MC (acc_suboptimal, 5) /* suboptimal construct (e.g. lack of siblings) */ \
									\
  /* Various: */							\
  MC (error,     6)      /* turn the message into an error */		\
									\
  /* Area: */								\
  MC (leb128,    7)  /* ULEB/SLEB storage */				\
  MC (abbrevs,   8)  /* abbreviations and abbreviation tables */	\
  MC (die_rel,   9)  /* DIE relationship */				\
  MC (die_other, 10) /* other messages related to DIEs */		\
  MC (info,      11) /* messages related to .debug_info, but not particular DIEs */ \
  MC (strings,   12) /* string table */					\
  MC (aranges,   13) /* address ranges table */				\
  MC (elf,       14) /* ELF structure, e.g. missing optional sections */ \
  MC (pubtables, 15) /* table of public names/types */			\
  MC (pubtypes,  16) /* .debug_pubtypes presence */			\
  MC (loc,       17) /* messages related to .debug_loc */		\
  MC (ranges,    18) /* messages related to .debug_ranges */		\
  MC (line,      19) /* messages related to .debug_line */		\
  MC (reloc,     20) /* messages related to relocation handling */	\
  MC (header,    21) /* messages related to header portions in general */ \
  MC (mac,       22) /* messages related to .debug_mac */		\
  MC (other,     31) /* messages unrelated to any of the above */

enum message_category
  {
    mc_none      = 0,

#define MC(CAT, ID)				\
    mc_##CAT = 1u << ID,
    MESSAGE_CATEGORIES
#undef MC
  };

message_category operator | (message_category a, message_category b);
message_category &operator |= (message_category &a, message_category b);
std::ostream &operator<< (std::ostream &o, message_category cat);

struct message_term
{
  /* Given a term like A && !B && C && !D, we decompose it thus: */
  message_category positive; /* non-zero bits for plain predicates */
  message_category negative; /* non-zero bits for negated predicates */

  message_term (message_category pos, message_category neg = mc_none)
    : positive (pos), negative (neg)
  {}

  std::string str () const;
};

std::ostream &operator<< (std::ostream &o, message_term const &term);

struct message_criteria
  : protected std::vector<message_term>
{
  using std::vector<message_term>::at;
  using std::vector<message_term>::size;

  void operator |= (message_term const &term);
  void operator &= (message_term const &term);
  void operator *= (message_criteria const &term);
  void and_not (message_term const &term);

  std::string str () const;
};

std::ostream &operator<< (std::ostream &o, message_criteria const &criteria);

message_criteria operator ! (message_term const &);

extern void wr_error (locus const *wh, const char *format, ...)
  __attribute__ ((format (printf, 2, 3)));

extern void wr_message (unsigned long category, locus const *loc,
			const char *format, ...)
  __attribute__ ((format (printf, 3, 4)));

extern void wr_format_padding_message (message_category category,
				       locus const &loc,
				       uint64_t start, uint64_t end,
				       char const *kind);

extern void wr_format_leb128_message (locus const &loc,
				      const char *what,
				      const char *purpose,
				      const unsigned char *begin,
				      const unsigned char *end);

extern void wr_message_padding_0 (message_category category,
				  locus const &loc,
				  uint64_t start, uint64_t end);

extern void wr_message_padding_n0 (message_category category,
				   locus const &loc,
				   uint64_t start, uint64_t end);

extern bool message_accept (struct message_criteria const *cri,
			    unsigned long cat);


extern unsigned error_count;

/* Messages that are accepted (and made into warning).  */
extern struct message_criteria warning_criteria;

/* Accepted (warning) messages, that are turned into errors.  */
extern struct message_criteria error_criteria;

class message_count_filter
{
  struct counter
  {
    unsigned value;
    counter () : value (0) {}
    unsigned operator++ () { return ++value; }
  };

  typedef std::map<void const *, counter> counters_t;

  // NULL for filtered-out message, otherwise array of <key, count>
  // pairs sorted by key.
  counters_t _m_counters;
  friend void wr_reset_counters ();

  void
  clear ()
  {
    counters_t empty;
    _m_counters.swap (empty);
  }

public:

  int should_emit (void const *key);
  static message_count_filter *
  inst ()
  {
    static message_count_filter inst;
    return &inst;
  }
};

class message_context
{
  static bool _m_last_emitted;

  message_count_filter *_m_filter;
  locus const *_m_loc;
  char const *_m_prefix;

  friend message_context wr_message (locus const &loc, message_category cat);
  friend message_context wr_message (message_category cat);
  friend bool wr_prev_emitted ();

  message_context (message_count_filter *filter,
		   locus const *loc, char const *prefix);

public:
  static message_context filter_message (locus const *loc,
					 message_category category);

  std::ostream &operator << (char const *message);
  std::ostream &operator << (std::string const &message);

  template<class T>
  std::ostream &
  operator << (T const &t)
  {
    std::stringstream ss;
    ss << t;
    return (*this) << ss.str ();
  }

  // Use KEY for count filtering.
  std::ostream &id (void const *key);

  // Use KEY for count filtering.  WHETHER is true if the message will
  // be emitted.  It doesn't touch that value otherwise, so WHETHER
  // must be pre-initialized to false.
  std::ostream &id (void const *key, bool &whether);

  // Return either the full stream, or a sink, depending on WHETHER.
  std::ostream &when (bool whether) const;

  std::ostream &when_prev () const;
};

std::ostream &wr_error (locus const &loc);
std::ostream &wr_error ();
message_context wr_message (locus const &loc, message_category cat);
message_context wr_message (message_category cat);
void wr_reset_counters ();
bool wr_prev_emitted ();

#endif//DWARFLINT_MESSAGES_HH
