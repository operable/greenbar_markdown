#include <assert.h>
#include "md_node.hpp"

namespace greenbar {
  namespace node2 {

    ERL_NIF_TERM HeaderNode::decorate_term(ErlNifEnv* env, ERL_NIF_TERM term) {
      // Put node text into the map
      term = MarkdownNode::decorate_term(env, term);
      gb_priv_s *priv_data = (gb_priv_s*) enif_priv_data(env);
      auto level_attr = get_attribute(ATTR_LEVEL);
      ERL_NIF_TERM level = enif_make_int(env, level_attr.n());
      enif_make_map_put(env, term, priv_data->gb_atom_level, level, &term);
      return term;
    }

    ERL_NIF_TERM LinkNode::decorate_term(ErlNifEnv* env, ERL_NIF_TERM term) {
      ERL_NIF_TERM url, title;
      gb_priv_s *priv_data = (gb_priv_s*) enif_priv_data(env);
      auto value = get_attribute(ATTR_URL);
      const std::string& url_text = value.s();
      auto url_bin = enif_make_new_binary(env, url_text.size(), &url);
      memcpy(url_bin, url_text.c_str(), url_text.size());
      auto title_bin = enif_make_new_binary(env, text_.size(), &title);
      memcpy(title_bin, text_.c_str(), text_.size());
      enif_make_map_put(env, term, priv_data->gb_atom_url, url, &term);
      enif_make_map_put(env, term, priv_data->gb_atom_text, title, &term);
      return term;
    }

    ERL_NIF_TERM TableCellNode::decorate_term(ErlNifEnv* env, ERL_NIF_TERM term) {
      // Add children
      term = MarkdownNodeContainer::decorate_term(env, term);
      gb_priv_s* priv_data = (gb_priv_s*) enif_priv_data(env);
      auto value = get_attribute(ATTR_ALIGNMENT);
      if (value == ATTR_NOT_SET) {
        return term;
      }
      NodeAlignment alignment = (NodeAlignment) value.n();
      if (alignment == ALIGN_NONE) {
        return term;
      }
      ERL_NIF_TERM align_atom = alignment_to_atom(alignment, priv_data);
      enif_make_map_put(env, term, priv_data->gb_atom_alignment, align_atom, &term);
      return term;
    }

    void TableNode::mark_header() {
      if (!children_.empty()) {
        TableRowNode* header_row = dynamic_cast<TableRowNode*>(children_.back());
        if (header_row) {
          header_row->mark_header();
        }
      }
    }

  }
}
