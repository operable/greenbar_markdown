#include "erl_nif.h"
#include "markdown_node.hpp"
#include "gb_common.hpp"

namespace greenbar {

  MarkdownParentNode::MarkdownParentNode(MarkdownInfoType info_type) {
    type_ = info_type;
    alignment_ = MD_ALIGN_NONE;
  }

  MarkdownParentNode::~MarkdownParentNode() {
    for (size_t i = 0; i < children_.size(); i++) {
      delete children_.at(i);
    }
  }

  void MarkdownParentNode::add_child(MarkdownNode* child) {
    children_.push_back(child);
  }

  MarkdownInfoType MarkdownParentNode::get_type() {
    return type_;
  }

  MarkdownAlignment MarkdownParentNode::get_alignment() {
    return alignment_;
  }

  void MarkdownParentNode::set_alignment(MarkdownAlignment align) {
    alignment_ = align;
  }

  void MarkdownParentNode::set_type(MarkdownInfoType type) {
    type_ = type;
  }

  size_t MarkdownParentNode::last_child() {
    if (children_.empty()) {
      return -1;
    }
    return children_.size() - 1;
  }

  bool MarkdownParentNode::set_child_type(size_t index, MarkdownInfoType old_type, MarkdownInfoType new_type) {
    bool retval = false;
    if (!children_.empty() && index < children_.size()) {
      auto child = children_.at(index);
      if (child->get_type() == old_type) {
        auto leaf_info = as_leaf(child);
        if (leaf_info == nullptr) {
          auto parent_info = as_parent(child);
          if (parent_info != nullptr) {
            parent_info->set_type(new_type);
            retval = true;
          }
        } else {
          leaf_info->set_type(new_type);
          retval = true;
        }
      }
    }
    return retval;
  }

  ERL_NIF_TERM MarkdownParentNode::convert_children(ErlNifEnv *env) {
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

  ERL_NIF_TERM MarkdownParentNode::to_erl_term(ErlNifEnv* env) {
    gb_priv_s *priv_data = (gb_priv_s*) enif_priv_data(env);
    ERL_NIF_TERM type_name = type_to_atom(this->type_, priv_data);
    ERL_NIF_TERM retval = enif_make_new_map(env);
    enif_make_map_put(env, retval, priv_data->gb_atom_name, type_name, &retval);
    ERL_NIF_TERM children = convert_children(env);
    enif_make_map_put(env, retval, priv_data->gb_atom_children, children, &retval);
    if (this->alignment_ != MD_ALIGN_NONE) {
      enif_make_map_put(env, retval, priv_data->gb_atom_alignment, alignment_to_atom(this->alignment_, priv_data), &retval);
    }
    return retval;
  }
}
