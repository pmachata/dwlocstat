/*
   Copyright (C) 2010, 2011 Red Hat, Inc.
   This file is part of elfutils.

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

#ifndef DWARFSTRINGS_H
#define DWARFSTRINGS_H 1

#ifdef __cplusplus
extern "C"
{
#endif

const char *dwarf_tag_string (unsigned int tag);

const char *dwarf_attr_string (unsigned int attrnum);

const char *dwarf_form_string (unsigned int form);

const char *dwarf_lang_string (unsigned int lang);

const char *dwarf_inline_string (unsigned int code);

const char *dwarf_encoding_string (unsigned int code);

const char *dwarf_access_string (unsigned int code);

const char *dwarf_visibility_string (unsigned int code);

const char *dwarf_virtuality_string (unsigned int code);

const char *dwarf_identifier_case_string (unsigned int code);

const char *dwarf_calling_convention_string (unsigned int code);

const char *dwarf_ordering_string (unsigned int code);

const char *dwarf_discr_list_string (unsigned int code);

const char *dwarf_locexpr_opcode_string (unsigned int code);

const char *dwarf_line_standard_opcode_string (unsigned int code);

const char *dwarf_line_extended_opcode_string (unsigned int code);

#ifdef __cplusplus
}
#endif

#endif  /* dwarfstrings.h */
