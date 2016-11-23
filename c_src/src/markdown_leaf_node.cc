#include "erl_nif.h"
#include "markdown_node.hpp"
#include "gb_common.hpp"

namespace greenbar {
  namespace node {

    MarkdownLeafNode::MarkdownLeafNode(NodeType info_type) {
      type_ = info_type;
      text_ = "";
    }

    NodeType MarkdownLeafNode::get_type() {
      return type_;
    }

    void MarkdownLeafNode::set_type(NodeType type) {
      type_ = type;
    }

    void MarkdownLeafNode::set_text(std::string text) {
      text_ = text;
    }

    ERL_NIF_TERM MarkdownLeafNode::to_erl_term(ErlNifEnv* env) {
      gb_priv_s *priv_data = (gb_priv_s*) enif_priv_data(env);
      ERL_NIF_TERM type_name = type_to_atom(this->type_, priv_data);
      ERL_NIF_TERM retval = enif_make_new_map(env);
      enif_make_map_put(env, retval, priv_data->gb_atom_name, type_name, &retval);
      if (this->get_type() != MD_EOL) {
        ERL_NIF_TERM text;
        auto contents = enif_make_new_binary(env, this->text_.size(), &text);
        memcpy(contents, this->text_.c_str(), this->text_.size());
        enif_make_map_put(env, retval, priv_data->gb_atom_text, text, &retval);

        if (this->type_ == MD_HEADER) {
          auto level_attr = this->get_attribute(ATTR_LEVEL);
          ERL_NIF_TERM level = enif_make_int(env, level_attr.n());
          enif_make_map_put(env, retval, priv_data->gb_atom_level, level, &retval);
        }

        if (this->type_ == MD_LINK) {
          ERL_NIF_TERM url;
          if (this->has_attribute(ATTR_URL)) {
            auto value = this->get_attribute(ATTR_URL);
            const std::string& text = value.s();
            auto contents = enif_make_new_binary(env, text.size(), &url);
            memcpy(contents, text.c_str(), text.size());
            enif_make_map_put(env, retval, priv_data->gb_atom_url, url, &retval);
          }
        }
      }
      if (this->has_attribute(ATTR_ALIGNMENT)) {
        auto alignment_attr = this->get_attribute(ATTR_ALIGNMENT);
        enif_make_map_put(env, retval, priv_data->gb_atom_alignment, alignment_to_atom((NodeAlignment) alignment_attr.n(), priv_data), &retval);
      }
      return retval;
    }
  }
}
