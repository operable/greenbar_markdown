#include <iostream>
#include <cstddef>
#include "erl_nif.h"
#include "markdown_node.hpp"
#include "gb_common.hpp"

#define TYPE_TO_STRING(type) #type

namespace greenbar {
  namespace node {
    MarkdownLeafNode* new_leaf(NodeType info_type, const hoedown_buffer* buf) {
      auto retval = new MarkdownLeafNode(info_type);
      if (buf != nullptr) {
        retval->set_text(std::string((char*) buf->data, buf->size));
      }
      return retval;
    }

    MarkdownLeafNode* new_leaf(NodeType info_type, const hoedown_buffer* buf, int info_level) {
      auto retval = new MarkdownLeafNode(info_type);
      if (buf != nullptr) {
        retval->set_text(std::string((char*) buf->data, buf->size));
      }
      retval->set_level(info_level);
      return retval;
    }

    MarkdownLeafNode* new_leaf(NodeType info_type, const std::string& text) {
      auto retval = new MarkdownLeafNode(info_type);
      retval->set_text(text);
      return retval;
    }

    MarkdownLeafNode* new_leaf(NodeType info_type, const hoedown_buffer* title, const hoedown_buffer* link) {
      auto retval = new MarkdownLeafNode(info_type);
      if (title != nullptr) {
        retval->set_text(std::string((char*) title->data, title->size));
      }
      if (link != nullptr) {
        retval->set_url(std::string((char*) link->data, link->size));
      }
      return retval;
    }

    MarkdownLeafNode* as_leaf(MarkdownNode* info) {
      if (info) {
        return dynamic_cast<MarkdownLeafNode*>(info);
      }
      return nullptr;
    }

    MarkdownParentNode* as_parent(MarkdownNode* info) {
      if (info) {
        return dynamic_cast<MarkdownParentNode*>(info);
      }
      return nullptr;
    }

    ERL_NIF_TERM type_to_atom(NodeType type, gb_priv_s* priv_data) {
      switch(type) {
      case MD_EOL:
        return priv_data->gb_atom_newline;
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

    ERL_NIF_TERM alignment_to_atom(NodeAlignment align, gb_priv_s* priv_data) {
      switch(align) {
      case MD_ALIGN_LEFT:
        return priv_data->gb_atom_left;
      case MD_ALIGN_RIGHT:
        return priv_data->gb_atom_right;
      case MD_ALIGN_CENTER:
        return priv_data->gb_atom_center;
      default:
        return priv_data->gb_atom_unknown;
      }
    }
  }
}
