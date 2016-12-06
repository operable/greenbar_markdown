#include "md_node_base.hpp"

namespace greenbar {
  namespace node2 {
    ERL_NIF_TERM MarkdownNode::decorate_term(ErlNifEnv* env, ERL_NIF_TERM term) {
      if (text_.size() > 0) {
        ERL_NIF_TERM text;
        gb_priv_s *priv_data = (gb_priv_s*) enif_priv_data(env);
        auto text_bin = enif_make_new_binary(env, text_.size(), &text);
        memcpy(text_bin, text_.c_str(), text_.size());
        enif_make_map_put(env, term, priv_data->gb_atom_text, text, &term);
      }
      return term;
    }

    MarkdownNode::MarkdownNode(NodeType type) {
        type_ = type;
        text_ = "";
        terminates_line_ = false;
    }

    MarkdownNode::MarkdownNode(NodeType type, const std::string& text) {
      type_ = type;
      text_ = text;
      terminates_line_ = false;
    }

    const AttributeValue& MarkdownNode::get_attribute(const NodeAttribute& attr) {
      auto iter = attributes_.find(attr);
      if (iter == attributes_.end()) {
        return ATTR_NOT_SET;
      }
      else {
        return iter->second;
      }
    }

    void MarkdownNode::put_attribute(const NodeAttribute& attr, const AttributeValue value) {
      attributes_[attr] = value;
    }

    bool MarkdownNode::has_attribute(const NodeAttribute& attr) {
      return attributes_.find(attr) != attributes_.end();
    }

    std::string MarkdownNode::to_string() {
      auto text = type_to_string(type_);
      if (text_ != "") {
        text = text + ": \"" + text_ + "\"";
      }
      return text;
    }

    bool MarkdownNode::line_terminator() {
      return terminates_line_;
    }

    bool MarkdownNode::terminates_line(bool flag) {
      bool previous = terminates_line_;
      terminates_line_ = flag;
      return previous;
    }

    ERL_NIF_TERM MarkdownNode::to_erl_term(ErlNifEnv* env) {
      gb_priv_s *priv_data = (gb_priv_s*) enif_priv_data(env);
      ERL_NIF_TERM type_name = type_to_atom(this->type_, priv_data);
      ERL_NIF_TERM retval = enif_make_new_map(env);
      enif_make_map_put(env, retval, priv_data->gb_atom_name, type_name, &retval);
      return decorate_term(env, retval);
    }

    ERL_NIF_TERM MarkdownNodeContainer::children_to_term_list(ErlNifEnv* env) {
      ERL_NIF_TERM head, tail;
      tail = enif_make_list(env, 0);
      if (children_.empty()) {
        return tail;
      }
      for(size_t i = 0; i < children_.size(); i++) {
        auto child = children_.at(i);
        head = child->to_erl_term(env);
        tail = enif_make_list_cell(env, head, tail);
      }
      return tail;
    }
    ERL_NIF_TERM MarkdownNodeContainer::decorate_term(ErlNifEnv* env, ERL_NIF_TERM term) {
      auto child_terms = children_to_term_list(env);
      gb_priv_s *priv_data = (gb_priv_s*) enif_priv_data(env);
      enif_make_map_put(env, term, priv_data->gb_atom_children, child_terms, &term);
      return term;
    }

    MarkdownNodeContainer::~MarkdownNodeContainer() {
      while(!children_.empty()) {
        auto child = children_.back();
        children_.pop_back();
        delete child;
      }
    }

    std::string MarkdownNodeContainer::to_string() {
      auto text = MarkdownNode::to_string() + " \nchildren: [";
      for (int i = 0; i < children_.size(); i++) {
        auto child = children_.at(i);
        if (i > 0) {
          text = text + ", ";
        }
        text = text + child->to_string();
      }
      text = text + "]";
      return text;
    }

    void MarkdownNodeContainer::add_child(MarkdownNode* child) {
      children_.push_back(child);
    }

    bool MarkdownNodeContainer::empty() {
      return children_.empty();
    }

    bool MarkdownNodeContainer::line_terminator() {
      if (children_.empty()) {
        return false;
      }
      MarkdownNode* last_child = nullptr;
      MarkdownNodeContainer *ctmp = nullptr;
      last_child = children_.back();
      ctmp = dynamic_cast<MarkdownNodeContainer*>(last_child);
      while (ctmp != nullptr) {
        if (ctmp->empty()) {
          break;
        }
        last_child = ctmp->children_.back();
        ctmp = dynamic_cast<MarkdownNodeContainer*>(last_child);
      }
      return last_child->line_terminator();
    }

    void MarkdownNodeContainer::drop_last(NodeType type) {
      if (empty()) {
        return;
      }
      auto last_child = children_.back();
      if (last_child->get_type() == type) {
        children_.pop_back();
        delete last_child;
      }
    }
  }
}

