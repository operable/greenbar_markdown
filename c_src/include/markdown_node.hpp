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
  namespace node {

    enum NodeType {
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

    enum NodeAlignment {
      MD_ALIGN_LEFT,
      MD_ALIGN_RIGHT,
      MD_ALIGN_CENTER,
      MD_ALIGN_NONE
    };

    enum NodeAttributes {
      ATTR_ALIGNMENT,
      ATTR_URL,
      ATTR_LEVEL
    };

    class MarkdownNode {
    public:
      virtual ~MarkdownNode() { };
      virtual NodeType get_type() = 0;
      virtual bool is_leaf() { return false; };
      virtual NodeAlignment get_alignment() = 0;
      virtual void set_alignment(NodeAlignment align) = 0;
      virtual ERL_NIF_TERM to_erl_term(ErlNifEnv* env) = 0;
    };

    typedef std::vector<MarkdownNode*> NodeStack;

    class MarkdownLeafNode : public MarkdownNode {
    private:
      NodeType type_;
      std::string text_;
      std::string url_;
      int level_;
      NodeAlignment alignment_;

      // No copying
      MarkdownLeafNode(MarkdownLeafNode const &);
      MarkdownLeafNode &operator=(MarkdownLeafNode const &);

    public:
      MarkdownLeafNode(NodeType type);
      virtual ~MarkdownLeafNode() { };
      ERL_NIF_TERM to_erl_term(ErlNifEnv* env);
      NodeType get_type();
      NodeAlignment get_alignment();
      void set_alignment(NodeAlignment align);
      void set_type(NodeType type);
      void set_text(std::string text);
      void set_url(std::string url);
      void set_level(int level);
    };

    class MarkdownParentNode : public MarkdownNode {
    private:
      NodeStack children_;
      NodeType type_;
      NodeAlignment alignment_;

      // No copying
      MarkdownParentNode(MarkdownParentNode const &);
      MarkdownParentNode &operator=(MarkdownParentNode const &);
      ERL_NIF_TERM convert_children(ErlNifEnv* env);

    public:
      MarkdownParentNode(NodeType type);
      virtual ~MarkdownParentNode();
      void add_child(MarkdownNode* child);
      ERL_NIF_TERM to_erl_term(ErlNifEnv* env);
      NodeType get_type();
      NodeAlignment get_alignment();
      void set_alignment(NodeAlignment align);
      size_t last_child();
      void set_type(NodeType type);
      bool set_child_type(size_t index, NodeType old_type, NodeType new_type);
    };

    MarkdownLeafNode* new_leaf(NodeType info_type, const hoedown_buffer* buffer);
    MarkdownLeafNode* new_leaf(NodeType info_type, const hoedown_buffer* buffer, int info_level);
    MarkdownLeafNode* new_leaf(NodeType info_type, const std::string& text);
    MarkdownLeafNode* new_leaf(NodeType info_type, const hoedown_buffer* text, const hoedown_buffer* url);

    MarkdownLeafNode* as_leaf(MarkdownNode* info);
    MarkdownParentNode* as_parent(MarkdownNode* info);

    ERL_NIF_TERM type_to_atom(NodeType type, gb_priv_s* priv_data);
    ERL_NIF_TERM alignment_to_atom(NodeAlignment align, gb_priv_s* priv_data);
  }
}
#endif
