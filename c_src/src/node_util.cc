#include <string>
#include "md_node_base.hpp"

namespace greenbar {
  namespace node2 {
    ERL_NIF_TERM type_to_atom(NodeType type, gb_priv_s* priv_data) {
      switch(type) {
      case MD_EOL:
        return priv_data->gb_atom_newline;
      case MD_PARAGRAPH:
        return priv_data->gb_atom_paragraph;
      case MD_TEXT:
        return priv_data->gb_atom_text;
      case MD_FIXED_WIDTH:
        return priv_data->gb_atom_fixed_width;
      case MD_FIXED_WIDTH_BLOCK:
        return priv_data->gb_atom_fixed_width_block;
      case MD_HEADER:
        return priv_data->gb_atom_header;
      case MD_ITALICS:
        return priv_data->gb_atom_italics;
      case MD_BOLD:
        return priv_data->gb_atom_bold;
      case MD_LINK:
        return priv_data->gb_atom_link;
      case MD_LIST_ITEM:
        return priv_data->gb_atom_list_item;
      case MD_ORDERED_LIST:
        return priv_data->gb_atom_ordered_list;
      case MD_UNORDERED_LIST:
        return priv_data->gb_atom_unordered_list;
      case MD_TABLE_CELL:
        return priv_data->gb_atom_table_cell;
      case MD_TABLE_ROW:
        return priv_data->gb_atom_table_row;
      case MD_TABLE_HEADER:
        return priv_data->gb_atom_table_header;
      case MD_TABLE:
        return priv_data->gb_atom_table;
      default:
        return priv_data->gb_atom_text;
      }
    }

#define STRINGIFY2(T) #T
#define STRINGIFY(T) case T: return STRINGIFY2(T);

    std::string type_to_string(NodeType type) {
      switch(type) {
      STRINGIFY(MD_EOL)
      STRINGIFY(MD_PARAGRAPH)
      STRINGIFY(MD_TEXT)
      STRINGIFY(MD_FIXED_WIDTH)
      STRINGIFY(MD_FIXED_WIDTH_BLOCK)
      STRINGIFY(MD_HEADER)
      STRINGIFY(MD_ITALICS)
      STRINGIFY(MD_BOLD)
      STRINGIFY(MD_LINK)
      STRINGIFY(MD_LIST_ITEM)
      STRINGIFY(MD_ORDERED_LIST)
      STRINGIFY(MD_UNORDERED_LIST)
      STRINGIFY(MD_TABLE_CELL)
      STRINGIFY(MD_TABLE_ROW)
      STRINGIFY(MD_TABLE_HEADER)
      STRINGIFY(MD_TABLE)
      default:
        return "MD_TEXT";
      }
    }

    ERL_NIF_TERM alignment_to_atom(NodeAlignment align, gb_priv_s* priv_data) {
      switch(align) {
      case ALIGN_RIGHT:
        return priv_data->gb_atom_right;
      case ALIGN_CENTER:
        return priv_data->gb_atom_center;
      default:
        return priv_data->gb_atom_left;
      }
    }

  }
}
