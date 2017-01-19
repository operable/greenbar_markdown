#include <string>
#include "md_node_base.hpp"

namespace greenbar {
  namespace node2 {
    ERL_NIF_TERM string_to_binary(ErlNifEnv* env, std::string value) {
      ERL_NIF_TERM target;
      auto bufsize = value.length();
      auto buf = enif_make_new_binary(env, bufsize, &target);
      memcpy(buf, value.c_str(), bufsize);
      return target;
    }
    ERL_NIF_TERM type_to_binary(NodeType type, ErlNifEnv* env) {
      switch(type) {
      case MD_EOL:
        return string_to_binary(env, "newline");
      case MD_PARAGRAPH:
        return string_to_binary(env, "paragraph");
      case MD_TEXT:
        return string_to_binary(env, "text");
      case MD_FIXED_WIDTH:
        return string_to_binary(env, "fixed_width");
      case MD_FIXED_WIDTH_BLOCK:
        return string_to_binary(env, "fixed_width_block");
      case MD_HEADER:
        return string_to_binary(env, "header");
      case MD_ITALICS:
        return string_to_binary(env, "italics");
      case MD_BOLD:
        return string_to_binary(env, "bold");
      case MD_LINK:
        return string_to_binary(env, "link");
      case MD_LIST_ITEM:
        return string_to_binary(env, "list_item");
      case MD_ORDERED_LIST:
        return string_to_binary(env, "ordered_list");
      case MD_UNORDERED_LIST:
        return string_to_binary(env, "unordered_list");
      case MD_TABLE_CELL:
        return string_to_binary(env, "table_cell");
      case MD_TABLE_ROW:
        return string_to_binary(env, "table_row");
      case MD_TABLE_HEADER:
        return string_to_binary(env, "table_header");
      case MD_TABLE:
        return string_to_binary(env, "table");
      default:
        return string_to_binary(env, "text");
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

    ERL_NIF_TERM alignment_to_binary(NodeAlignment align, ErlNifEnv* env) {
      switch(align) {
      case ALIGN_RIGHT:
        return string_to_binary(env, "right");
      case ALIGN_CENTER:
        return string_to_binary(env, "center");
      default:
        return string_to_binary(env, "left");
      }
    }

  }
}
