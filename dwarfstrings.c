/*
   Copyright (C) 2010, 2011, 2012 Red Hat, Inc.
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

#include <dwarf.h>
#include <inttypes.h>
#include <stdio.h>
#include <libintl.h>

#include "dwarfstrings.h"

const char *
dwarf_tag_string (unsigned int tag)
{
  static const char *const known_tags[]  =
    {
      [DW_TAG_array_type] = "array_type",
      [DW_TAG_class_type] = "class_type",
      [DW_TAG_entry_point] = "entry_point",
      [DW_TAG_enumeration_type] = "enumeration_type",
      [DW_TAG_formal_parameter] = "formal_parameter",
      [DW_TAG_imported_declaration] = "imported_declaration",
      [DW_TAG_label] = "label",
      [DW_TAG_lexical_block] = "lexical_block",
      [DW_TAG_member] = "member",
      [DW_TAG_pointer_type] = "pointer_type",
      [DW_TAG_reference_type] = "reference_type",
      [DW_TAG_compile_unit] = "compile_unit",
      [DW_TAG_string_type] = "string_type",
      [DW_TAG_structure_type] = "structure_type",
      [DW_TAG_subroutine_type] = "subroutine_type",
      [DW_TAG_typedef] = "typedef",
      [DW_TAG_union_type] = "union_type",
      [DW_TAG_unspecified_parameters] = "unspecified_parameters",
      [DW_TAG_variant] = "variant",
      [DW_TAG_common_block] = "common_block",
      [DW_TAG_common_inclusion] = "common_inclusion",
      [DW_TAG_inheritance] = "inheritance",
      [DW_TAG_inlined_subroutine] = "inlined_subroutine",
      [DW_TAG_module] = "module",
      [DW_TAG_ptr_to_member_type] = "ptr_to_member_type",
      [DW_TAG_set_type] = "set_type",
      [DW_TAG_subrange_type] = "subrange_type",
      [DW_TAG_with_stmt] = "with_stmt",
      [DW_TAG_access_declaration] = "access_declaration",
      [DW_TAG_base_type] = "base_type",
      [DW_TAG_catch_block] = "catch_block",
      [DW_TAG_const_type] = "const_type",
      [DW_TAG_constant] = "constant",
      [DW_TAG_enumerator] = "enumerator",
      [DW_TAG_file_type] = "file_type",
      [DW_TAG_friend] = "friend",
      [DW_TAG_namelist] = "namelist",
      [DW_TAG_namelist_item] = "namelist_item",
      [DW_TAG_packed_type] = "packed_type",
      [DW_TAG_subprogram] = "subprogram",
      [DW_TAG_template_type_parameter] = "template_type_parameter",
      [DW_TAG_template_value_parameter] = "template_value_parameter",
      [DW_TAG_thrown_type] = "thrown_type",
      [DW_TAG_try_block] = "try_block",
      [DW_TAG_variant_part] = "variant_part",
      [DW_TAG_variable] = "variable",
      [DW_TAG_volatile_type] = "volatile_type",
      [DW_TAG_dwarf_procedure] = "dwarf_procedure",
      [DW_TAG_restrict_type] = "restrict_type",
      [DW_TAG_interface_type] = "interface_type",
      [DW_TAG_namespace] = "namespace",
      [DW_TAG_imported_module] = "imported_module",
      [DW_TAG_unspecified_type] = "unspecified_type",
      [DW_TAG_partial_unit] = "partial_unit",
      [DW_TAG_imported_unit] = "imported_unit",
      [DW_TAG_condition] = "condition",
      [DW_TAG_shared_type] = "shared_type",
      [DW_TAG_type_unit] = "type_unit",
      [DW_TAG_rvalue_reference_type] = "rvalue_reference_type",
      [DW_TAG_template_alias] = "template_alias",
    };
  const unsigned int nknown_tags = (sizeof (known_tags)
				    / sizeof (known_tags[0]));
  static char buf[40];
  const char *result = NULL;

  if (tag < nknown_tags)
    result = known_tags[tag];

  if (result == NULL)
    /* There are a few known extensions.  */
    switch (tag)
      {
      case DW_TAG_MIPS_loop:
	result = "MIPS_loop";
	break;

      case DW_TAG_format_label:
	result = "format_label";
	break;

      case DW_TAG_function_template:
	result = "function_template";
	break;

      case DW_TAG_class_template:
	result = "class_template";
	break;

      case DW_TAG_GNU_BINCL:
	result = "GNU_BINCL";
	break;

      case DW_TAG_GNU_EINCL:
	result = "GNU_EINCL";
	break;

      case DW_TAG_GNU_template_template_param:
	result = "GNU_template_template_param";
	break;

      case DW_TAG_GNU_template_parameter_pack:
	result = "GNU_template_parameter_pack";
	break;

      case DW_TAG_GNU_formal_parameter_pack:
	result = "GNU_formal_parameter_pack";
	break;

      case DW_TAG_GNU_call_site:
	result = "DW_TAG_GNU_call_site";
	break;

      case DW_TAG_GNU_call_site_parameter:
	result = "DW_TAG_GNU_call_site_parameter";
	break;

      default:
	if (tag < DW_TAG_lo_user)
	  snprintf (buf, sizeof buf, gettext ("unknown tag %hx"), tag);
	else
	  snprintf (buf, sizeof buf, gettext ("unknown user tag %hx"), tag);
	result = buf;
	break;
      }

  return result;
}


const char *
dwarf_attr_string (unsigned int attrnum)
{
  static const char *const known_attrs[] =
    {
      [DW_AT_sibling] = "sibling",
      [DW_AT_location] = "location",
      [DW_AT_name] = "name",
      [DW_AT_ordering] = "ordering",
      [DW_AT_subscr_data] = "subscr_data",
      [DW_AT_byte_size] = "byte_size",
      [DW_AT_bit_offset] = "bit_offset",
      [DW_AT_bit_size] = "bit_size",
      [DW_AT_element_list] = "element_list",
      [DW_AT_stmt_list] = "stmt_list",
      [DW_AT_low_pc] = "low_pc",
      [DW_AT_high_pc] = "high_pc",
      [DW_AT_language] = "language",
      [DW_AT_member] = "member",
      [DW_AT_discr] = "discr",
      [DW_AT_discr_value] = "discr_value",
      [DW_AT_visibility] = "visibility",
      [DW_AT_import] = "import",
      [DW_AT_string_length] = "string_length",
      [DW_AT_common_reference] = "common_reference",
      [DW_AT_comp_dir] = "comp_dir",
      [DW_AT_const_value] = "const_value",
      [DW_AT_containing_type] = "containing_type",
      [DW_AT_default_value] = "default_value",
      [DW_AT_inline] = "inline",
      [DW_AT_is_optional] = "is_optional",
      [DW_AT_lower_bound] = "lower_bound",
      [DW_AT_producer] = "producer",
      [DW_AT_prototyped] = "prototyped",
      [DW_AT_return_addr] = "return_addr",
      [DW_AT_start_scope] = "start_scope",
      [DW_AT_bit_stride] = "bit_stride",
      [DW_AT_upper_bound] = "upper_bound",
      [DW_AT_abstract_origin] = "abstract_origin",
      [DW_AT_accessibility] = "accessibility",
      [DW_AT_address_class] = "address_class",
      [DW_AT_artificial] = "artificial",
      [DW_AT_base_types] = "base_types",
      [DW_AT_calling_convention] = "calling_convention",
      [DW_AT_count] = "count",
      [DW_AT_data_member_location] = "data_member_location",
      [DW_AT_decl_column] = "decl_column",
      [DW_AT_decl_file] = "decl_file",
      [DW_AT_decl_line] = "decl_line",
      [DW_AT_declaration] = "declaration",
      [DW_AT_discr_list] = "discr_list",
      [DW_AT_encoding] = "encoding",
      [DW_AT_external] = "external",
      [DW_AT_frame_base] = "frame_base",
      [DW_AT_friend] = "friend",
      [DW_AT_identifier_case] = "identifier_case",
      [DW_AT_macro_info] = "macro_info",
      [DW_AT_namelist_item] = "namelist_item",
      [DW_AT_priority] = "priority",
      [DW_AT_segment] = "segment",
      [DW_AT_specification] = "specification",
      [DW_AT_static_link] = "static_link",
      [DW_AT_type] = "type",
      [DW_AT_use_location] = "use_location",
      [DW_AT_variable_parameter] = "variable_parameter",
      [DW_AT_virtuality] = "virtuality",
      [DW_AT_vtable_elem_location] = "vtable_elem_location",
      [DW_AT_allocated] = "allocated",
      [DW_AT_associated] = "associated",
      [DW_AT_data_location] = "data_location",
      [DW_AT_byte_stride] = "byte_stride",
      [DW_AT_entry_pc] = "entry_pc",
      [DW_AT_use_UTF8] = "use_UTF8",
      [DW_AT_extension] = "extension",
      [DW_AT_ranges] = "ranges",
      [DW_AT_trampoline] = "trampoline",
      [DW_AT_call_column] = "call_column",
      [DW_AT_call_file] = "call_file",
      [DW_AT_call_line] = "call_line",
      [DW_AT_description] = "description",
      [DW_AT_binary_scale] = "binary_scale",
      [DW_AT_decimal_scale] = "decimal_scale",
      [DW_AT_small] = "small",
      [DW_AT_decimal_sign] = "decimal_sign",
      [DW_AT_digit_count] = "digit_count",
      [DW_AT_picture_string] = "picture_string",
      [DW_AT_mutable] = "mutable",
      [DW_AT_threads_scaled] = "threads_scaled",
      [DW_AT_explicit] = "explicit",
      [DW_AT_object_pointer] = "object_pointer",
      [DW_AT_endianity] = "endianity",
      [DW_AT_elemental] = "elemental",
      [DW_AT_pure] = "pure",
      [DW_AT_recursive] = "recursive",
      [DW_AT_signature] = "signature",
      [DW_AT_main_subprogram] = "main_subprogram",
      [DW_AT_data_bit_offset] = "data_bit_offset",
      [DW_AT_const_expr] = "const_expr",
      [DW_AT_enum_class] = "enum_class",
      [DW_AT_linkage_name] = "linkage_name",
    };
  const unsigned int nknown_attrs = (sizeof (known_attrs)
				     / sizeof (known_attrs[0]));
  static char buf[40];
  const char *result = NULL;

  if (attrnum < nknown_attrs)
    result = known_attrs[attrnum];

  if (result == NULL)
    /* There are a few known extensions.  */
    switch (attrnum)
      {
      case DW_AT_MIPS_fde:
	result = "MIPS_fde";
	break;

      case DW_AT_MIPS_loop_begin:
	result = "MIPS_loop_begin";
	break;

      case DW_AT_MIPS_tail_loop_begin:
	result = "MIPS_tail_loop_begin";
	break;

      case DW_AT_MIPS_epilog_begin:
	result = "MIPS_epilog_begin";
	break;

      case DW_AT_MIPS_loop_unroll_factor:
	result = "MIPS_loop_unroll_factor";
	break;

      case DW_AT_MIPS_software_pipeline_depth:
	result = "MIPS_software_pipeline_depth";
	break;

      case DW_AT_MIPS_linkage_name:
	result = "MIPS_linkage_name";
	break;

      case DW_AT_MIPS_stride:
	result = "MIPS_stride";
	break;

      case DW_AT_MIPS_abstract_name:
	result = "MIPS_abstract_name";
	break;

      case DW_AT_MIPS_clone_origin:
	result = "MIPS_clone_origin";
	break;

      case DW_AT_MIPS_has_inlines:
	result = "MIPS_has_inlines";
	break;

      case DW_AT_MIPS_stride_byte:
	result = "MIPS_stride_byte";
	break;

      case DW_AT_MIPS_stride_elem:
	result = "MIPS_stride_elem";
	break;

      case DW_AT_MIPS_ptr_dopetype:
	result = "MIPS_ptr_dopetype";
	break;

      case DW_AT_MIPS_allocatable_dopetype:
	result = "MIPS_allocatable_dopetype";
	break;

      case DW_AT_MIPS_assumed_shape_dopetype:
	result = "MIPS_assumed_shape_dopetype";
	break;

      case DW_AT_MIPS_assumed_size:
	result = "MIPS_assumed_size";
	break;

      case DW_AT_sf_names:
	result = "sf_names";
	break;

      case DW_AT_src_info:
	result = "src_info";
	break;

      case DW_AT_mac_info:
	result = "mac_info";
	break;

      case DW_AT_src_coords:
	result = "src_coords";
	break;

      case DW_AT_body_begin:
	result = "body_begin";
	break;

      case DW_AT_body_end:
	result = "body_end";
	break;

      case DW_AT_GNU_vector:
	result = "GNU_vector";
	break;

      case DW_AT_GNU_guarded_by:
	result = "GNU_guarded_by";
	break;

      case DW_AT_GNU_pt_guarded_by:
	result = "GNU_pt_guarded_by";
	break;

      case DW_AT_GNU_guarded:
	result = "GNU_guarded";
	break;

      case DW_AT_GNU_pt_guarded:
	result = "GNU_pt_guarded";
	break;

      case DW_AT_GNU_locks_excluded:
	result = "GNU_locks_excluded";
	break;

      case DW_AT_GNU_exclusive_locks_required:
	result = "GNU_exclusive_locks_required";
	break;

      case DW_AT_GNU_shared_locks_required:
	result = "GNU_shared_locks_required";
	break;

      case DW_AT_GNU_odr_signature:
	result = "GNU_odr_signature";
	break;

      case DW_AT_GNU_template_name:
	result = "GNU_template_name";
	break;

      case DW_AT_GNU_call_site_value:
	result = "GNU_call_site_value";
	break;

      case DW_AT_GNU_call_site_data_value:
	result = "GNU_call_site_data_value";
	break;

      case DW_AT_GNU_call_site_target:
	result = "GNU_call_site_target";
	break;

      case DW_AT_GNU_call_site_target_clobbered:
	result = "GNU_call_site_target_clobbered";
	break;

      case DW_AT_GNU_tail_call:
	result = "GNU_tail_call";
	break;

      case DW_AT_GNU_all_tail_call_sites:
	result = "GNU_all_tail_call_sites";
	break;

      case DW_AT_GNU_all_call_sites:
	result = "GNU_all_call_sites";
	break;

      case DW_AT_GNU_all_source_call_sites:
	result = "GNU_all_source_call_sites";
	break;

      default:
	if (attrnum < DW_AT_lo_user)
	  snprintf (buf, sizeof buf, gettext ("unknown attribute %hx"),
		    attrnum);
	else
	  snprintf (buf, sizeof buf, gettext ("unknown user attribute %hx"),
		    attrnum);
	result = buf;
	break;
      }

  return result;
}
