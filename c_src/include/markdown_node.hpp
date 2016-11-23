#ifndef GREENBAR_MARKDOWN_NODE_H
#define GREENBAR_MARKDOWN_NODE_H

#include "erl_nif.h"
#include <memory>
#include <string>
#include <cstring>
#include <map>
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

    enum NodeAttribute {
      ATTR_ALIGNMENT,
      ATTR_TITLE,
      ATTR_URL,
      ATTR_LEVEL
    };

    class AttributeValue {
    private:
      bool empty_;
      std::string s_;
      int n_;
    public:
      AttributeValue() : empty_(true), s_(""), n_(0) {}
      AttributeValue(std::string s) : empty_(false), s_(s), n_(0) {}
      AttributeValue(int n) : empty_(false), s_(""), n_(n) {}

      bool empty() { return empty_; }
      const std::string& s() { return s_; }
      int n() { return n_; }
    };

    const AttributeValue ATTR_NOT_FOUND = AttributeValue();

    typedef std::map<NodeAttribute, AttributeValue> AttributeMap;

    class MarkdownNode {
    protected:
      AttributeMap attributes_;
    public:
      virtual ~MarkdownNode() { };
      virtual NodeType get_type() = 0;
      virtual bool is_leaf() { return false; };
      virtual ERL_NIF_TERM to_erl_term(ErlNifEnv* env) = 0;

      const AttributeValue& get_attribute(const NodeAttribute& attr) {
        auto iter = attributes_.find(attr);
        if (iter == attributes_.end()) {
          return ATTR_NOT_FOUND;
        }
        else {
          return iter->second;
        }
      }

      void put_attribute(const NodeAttribute& attr, const AttributeValue value) {
        attributes_[attr] = value;
      }

      bool has_attribute(const NodeAttribute& attr) {
        return attributes_.find(attr) != attributes_.end();
      }
    };

    typedef std::vector<MarkdownNode*> NodeStack;

    class MarkdownLeafNode : public MarkdownNode {
    private:
      NodeType type_;
      std::string text_;

      // No copying
      MarkdownLeafNode(MarkdownLeafNode const &);
      MarkdownLeafNode &operator=(MarkdownLeafNode const &);

    public:
      MarkdownLeafNode(NodeType type);
      virtual ~MarkdownLeafNode() { };
      ERL_NIF_TERM to_erl_term(ErlNifEnv* env);
      NodeType get_type();
      void set_type(NodeType type);
      void set_text(std::string text);
    };

    class MarkdownParentNode : public MarkdownNode {
    private:
      NodeStack children_;
      NodeType type_;

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
