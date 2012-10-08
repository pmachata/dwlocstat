/*
   Copyright (C) 2010, 2011 Red Hat, Inc.
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

#ifndef DWARFLINT_OPTION_HH
#define DWARFLINT_OPTION_HH

#include <string>
#include <argp.h>
#include <map>
#include <vector>
#include <cassert>
#include <iostream>

#include "option_i.hh"

class options
  : private std::map<int, option_i *>
{
  friend class argppp;
  mutable std::vector<argp_option> _m_opts;

  option_i *find_opt (int key) const;

public:
  option_i const *getopt (int key) const;
  argp build_argp (bool toplev = false) const;
  void add (option_i *opt);
  using std::map<int, option_i *>::empty;
  using std::map<int, option_i *>::begin;
  using std::map<int, option_i *>::end;
  using std::map<int, option_i *>::const_iterator;
};

// Wrapper of argp parsing.  While in general argp does a decent job,
// it's not possible to pass user arguments to the parser function
// from the argp structure itself, and therefore share the same parser
// for all the children.  Since that's what we need to do, we need to
// pass the "input" argument to argp_parse, and carefully setup the
// child_inputs arguments.  That means coordinating the whole parsing
// process from one place.  As a result this is hardly a nice,
// reusable piece of code.
class argppp
{
  std::vector<argp> _m_children_argps;
  std::vector<std::string> _m_children_headers;
  std::vector<argp_child> _m_children;
  std::vector<options const *> _m_children_inputs;
  argp _m_argp;
  bool _m_inited;

  static error_t parse_opt (int key, char *arg, argp_state *state);
  static argppp *instance;

public:
  argppp (options const &global);

  void parse (int argc, char **argv, unsigned flags, int *remaining);
  void help (FILE *stream, unsigned flags, char *name);
};

class option_i // we cannot call it simply "option", this conflicts
	       // with another global declaration
{
  // last allocated option unique identifier
  static int _m_last_opt;

protected:
  // Answer either opt_short argument if it's non-0.  Otherwise create
  // new unique identifier.
  static int get_short_option (char opt_short);

public:
  virtual ~option_i () {}

  virtual bool seen () const = 0;
  virtual argp_option const &build_option () const = 0;
  virtual error_t parse_opt (char *arg, argp_state *state) = 0;
  virtual int key () const = 0;
};

class option_common
  : public option_i
{
  argp_option _m_opt;

protected:
  bool _m_seen;

  option_common (char const *description,
		 char const *arg_description,
		 char const *opt_long, char opt_short,
		 int flags = 0);

public:
  bool
  seen () const
  {
    return _m_seen;
  }

  argp_option const &
  build_option () const
  {
    return _m_opt;
  }

  int
  key () const
  {
    return _m_opt.key;
  }
};

template<class arg_type>
class value_converter;

template<class arg_type>
class xoption
  : public option_common
{
  arg_type _m_arg;

public:
  xoption (char const *description,
	   char const *arg_description,
	   char const *opt_long, char opt_short = 0,
	   int flags = 0)
    : option_common (description, arg_description, opt_long, opt_short, flags)
  {
  }

  arg_type const &value () const
  {
    return _m_arg;
  }

  arg_type const &value (arg_type arg)
  {
    return seen () ? _m_arg : arg;
  }

  error_t parse_opt (char *arg, __attribute__ ((unused)) argp_state *state)
  {
    _m_seen = true;
    _m_arg = value_converter<arg_type>::convert (arg);
    return 0;
  }
};

template<>
class xoption<void>
  : public option_common
{
public:
  xoption (char const *description,
	   char const *opt_long, char opt_short = 0, int flags = 0)
    : option_common (description, NULL, opt_long, opt_short, flags)
  {
  }

  error_t parse_opt (char *arg, __attribute__ ((unused)) argp_state *state)
  {
    assert (arg == NULL);
    _m_seen = true;
    return 0;
  }

  // This shouldn't be promoted to option_common, as
  // e.g. xoption<bool> will naturally have a different
  // implementation.
  operator bool () { return seen (); }
};

template<>
struct value_converter<std::string>
{
  static std::string convert (char const *arg)
  {
    if (arg == NULL)
      return "";
    else
      return arg;
  }
};

template<>
struct value_converter<unsigned>
{
  static unsigned convert (char const *arg)
  {
    unsigned u;
    if (std::sscanf (arg, "%u", &u) == 1)
      return u;
    else
      return -1;
  }
};

typedef xoption<void> void_option;
typedef xoption<std::string> string_option;
typedef xoption<unsigned> unsigned_option;

options & global_opts ();

template<class OPT>
struct global_opt
  : public OPT
{
  template<typename... Args>
  global_opt (Args const&... args)
    : OPT (args...)
  {
    global_opts ().add (this);
  }
};

#endif//DWARFLINT_OPTION_HH
