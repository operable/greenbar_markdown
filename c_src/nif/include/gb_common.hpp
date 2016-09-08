#ifndef GREENBAR_COMMON_H
#define GREENBAR_COMMON_H

typedef struct {
  ERL_NIF_TERM gb_atom_ok;
  ERL_NIF_TERM gb_atom_error;
  ERL_NIF_TERM gb_atom_out_of_memory;
  ERL_NIF_TERM gb_atom_children;
  ERL_NIF_TERM gb_atom_unknown;
  ERL_NIF_TERM gb_atom_name;
  ERL_NIF_TERM gb_atom_text;
  ERL_NIF_TERM gb_atom_newline;
  ERL_NIF_TERM gb_atom_fixed_width;
  ERL_NIF_TERM gb_atom_header;
  ERL_NIF_TERM gb_atom_italics;
  ERL_NIF_TERM gb_atom_bold;
  ERL_NIF_TERM gb_atom_link;
  ERL_NIF_TERM gb_atom_level;
  ERL_NIF_TERM gb_atom_url;
  ERL_NIF_TERM gb_atom_ordered_list;
  ERL_NIF_TERM gb_atom_unordered_list;
  ERL_NIF_TERM gb_atom_list_item;
  ERL_NIF_TERM gb_atom_table;
  ERL_NIF_TERM gb_atom_table_header;
  ERL_NIF_TERM gb_atom_table_row;
  ERL_NIF_TERM gb_atom_table_cell;
  ERL_NIF_TERM gb_atom_alignment;
  ERL_NIF_TERM gb_atom_left;
  ERL_NIF_TERM gb_atom_right;
  ERL_NIF_TERM gb_atom_center;
} gb_priv_s;

#endif
