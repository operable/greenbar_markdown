#ifndef GREENBAR_MARKDOWN_NODE_H
#define GREENBAR_MARKDOWN_NODE_H

#include "erl_nif.h"
#include <memory>
#include <string>
#include <cstring>
#include <vector>
#include "buffer.h"
#include "gb_common.hpp"

namespace greenbar {

  enum MarkdownInfoType {
    MD_EOL,
    MD_TEXT,
    MD_FIXED_WIDTH,
    MD_FIXED_WIDTH_BLOCK,
    MD_HEADER,
    MD_ITALICS,
    MD_BOLD,
    MD_LINK,
    MD_LIST_ITEM,
    MD_ORDERED_LIST,
    MD_UNORDERED_LIST,
    MD_TABLE_CELL,
    MD_TABLE_ROW,
    MD_TABLE_HEADER,
    MD_TABLE_BODY,
    MD_TABLE,
    MD_NONE
  };

  enum MarkdownAlignment {
    MD_ALIGN_LEFT,
    MD_ALIGN_RIGHT,
    MD_ALIGN_CENTER,
    MD_ALIGN_NONE
  };

  class MarkdownNode {
  public:
    virtual ~MarkdownNode() { };
    virtual MarkdownInfoType get_type() = 0;
    virtual bool is_leaf() { return false; };
    virtual MarkdownAlignment get_alignment() = 0;
    virtual void set_alignment(MarkdownAlignment align) = 0;
    virtual ERL_NIF_TERM to_erl_term(ErlNifEnv* env) = 0;
  };

  typedef std::vector<MarkdownNode*> NodeStack;

  class MarkdownLeafNode : public MarkdownNode {
  private:
    MarkdownInfoType type_;
    std::string text_;
    std::string url_;
    int level_;
    MarkdownAlignment alignment_;

    // No copying
    MarkdownLeafNode(MarkdownLeafNode const &);
    MarkdownLeafNode &operator=(MarkdownLeafNode const &);

  public:
    MarkdownLeafNode(MarkdownInfoType type);
    virtual ~MarkdownLeafNode() { };
    ERL_NIF_TERM to_erl_term(ErlNifEnv* env);
    MarkdownInfoType get_type();
    MarkdownAlignment get_alignment();
    void set_alignment(MarkdownAlignment align);
    void set_type(MarkdownInfoType type);
    void set_text(std::string text);
    void set_url(std::string url);
    void set_level(int level);
  };

  class MarkdownParentNode : public MarkdownNode {
  private:
    NodeStack children_;
    MarkdownInfoType type_;
    MarkdownAlignment alignment_;

    // No copying
    MarkdownParentNode(MarkdownParentNode const &);
    MarkdownParentNode &operator=(MarkdownParentNode const &);
    ERL_NIF_TERM convert_children(ErlNifEnv* env);

  public:
    MarkdownParentNode(MarkdownInfoType type);
    virtual ~MarkdownParentNode();
    void add_child(MarkdownNode* child);
    ERL_NIF_TERM to_erl_term(ErlNifEnv* env);
    MarkdownInfoType get_type();
    MarkdownAlignment get_alignment();
    void set_alignment(MarkdownAlignment align);
    size_t last_child();
    void set_type(MarkdownInfoType type);
    bool set_child_type(size_t index, MarkdownInfoType old_type, MarkdownInfoType new_type);
  };

  MarkdownLeafNode* new_leaf(MarkdownInfoType info_type, const hoedown_buffer* buffer);
  MarkdownLeafNode* new_leaf(MarkdownInfoType info_type, const hoedown_buffer* buffer, int info_level);
  MarkdownLeafNode* new_leaf(MarkdownInfoType info_type, const std::string& text);
  MarkdownLeafNode* new_leaf(MarkdownInfoType info_type, const hoedown_buffer* text, const hoedown_buffer* url);

  MarkdownLeafNode* as_leaf(MarkdownNode* info);
  MarkdownParentNode* as_parent(MarkdownNode* info);

  ERL_NIF_TERM type_to_atom(MarkdownInfoType type, gb_priv_s* priv_data);
  ERL_NIF_TERM alignment_to_atom(MarkdownAlignment align, gb_priv_s* priv_data);
}

#endif
