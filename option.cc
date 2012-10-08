/*
   Copyright (C) 2009, 2010, 2011, 2012 Red Hat, Inc.
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

#include <cassert>
#include <cstring>
#include <iostream>

#include "option.hh"

argppp *argppp::instance = NULL;

option_i *
options::find_opt (int key) const
{
  const_iterator it = find (key);
  if (it == end ())
    return NULL;
  return it->second;
}

option_i const *
options::getopt (int key) const
{
  return find_opt (key);
}

struct last_option
  : public argp_option
{
  last_option ()
  {
    std::memset (this, 0, sizeof (*this));
  }
};

void
options::add (option_i *opt)
{
  int key = opt->key ();
  assert (getopt (key) == NULL);
  (*this)[key] = opt;
}

argp
options::build_argp (bool toplev) const
{
  _m_opts.clear ();
  for (const_iterator it = begin (); it != end (); ++it)
    _m_opts.push_back (it->second->build_option ());
  _m_opts.push_back (last_option ());
  argp a = {
    &_m_opts.front (),
    NULL, // needs to be initialized later, in argppp
    !toplev ? NULL : "FILE...",
    !toplev ? NULL : "\
Pedantic checking of DWARF stored in ELF files.",
    NULL, NULL, NULL
  };
  return a;
}

argppp::argppp (options const &global)
  : _m_inited (false)
{
  argp main = global.build_argp (true);
  main.parser = &parse_opt;
  _m_argp = main;

  // Only one instance is allowed per program.
  assert (instance == NULL);
  instance = this;
}

error_t
argppp::parse_opt (int key, char *arg, argp_state *state)
{
  assert (instance != NULL);
  if (key == ARGP_KEY_INIT && !instance->_m_inited)
    {
      instance->_m_inited = true;
      unsigned i = 0;
      for (std::vector<options const *>::const_iterator it
	     = instance->_m_children_inputs.begin ();
	   it != instance->_m_children_inputs.end (); ++it)
	state->child_inputs[i++] = const_cast<options *> (*it);
      return 0;
    }
  else
    {
      assert (state->input != NULL);
      options const *opts = static_cast<options const *> (state->input);
      option_i *o = opts->find_opt (key);
      if (o == NULL)
	return ARGP_ERR_UNKNOWN;
      return o->parse_opt (arg, state);
    }
}

void
argppp::parse (int argc, char **argv, unsigned flags, int *remaining)
{
  assert (!_m_inited);
  argp_parse (&_m_argp, argc, argv, flags, remaining, &global_opts ());
}

void
argppp::help (FILE *stream, unsigned flags, char *name)
{
  argp_help (&_m_argp, stream, flags, name);
}

int option_i::_m_last_opt = 300;

int
option_i::get_short_option (char opt_short)
{
  if (opt_short)
    return opt_short;
  return _m_last_opt++;
}

namespace
{
  argp_option argp_option_ctor (char const *name, int key,
				char const *arg, int flags,
				char const *doc, int group)
  {
    assert (name != NULL);
    assert (doc != NULL);
    argp_option opt = {
      name, key, arg, flags, doc, group
    };
    return opt;
  }
}

option_common::option_common (char const *description,
			      char const *arg_description,
			      char const *opt_long, char opt_short,
			      int flags)
  : _m_opt (argp_option_ctor (opt_long, get_short_option (opt_short),
			      arg_description, flags,
			      description, 0))
  , _m_seen (false)
{}

// Trick to make sure the static options are always initialized
// before access (it is used from various global initializers.

options &
global_opts ()
{
  static options inst;
  return inst;
}
