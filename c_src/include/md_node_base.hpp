#ifndef GREENBAR_MD_NODE_BASE_H
#define GREENBAR_MD_NODE_BASE_H

#include <map>
#include <string>
#include <cstring>
#include <vector>
#include "erl_nif.h"
#include "gb_common.hpp"

namespace greenbar {
  namespace node2 {
    // Markdown node types
    enum NodeType {
      MD_NONE = 10,
      MD_PARAGRAPH,
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
    };

    // Markdown node alignments
    // Only applies to tables
    enum NodeAlignment {
      ALIGN_NONE = 100,
      ALIGN_LEFT,
      ALIGN_RIGHT,
      ALIGN_CENTER
    };

    // Common node attributes
    enum NodeAttribute {
      ATTR_ALIGNMENT = 200,
      ATTR_TITLE,
      ATTR_URL,
      ATTR_LEVEL
    };

    // Wrapper for attribute values
    class AttributeValue {
    private:
      bool empty_;
      std::string s_;
      int n_;
    public:
      AttributeValue() : empty_(true), s_(""), n_(0) {}
      AttributeValue(const std::string& s) : empty_(false), s_(s), n_(0) {}
      AttributeValue(int n) : empty_(false), s_(""), n_(n) {}

      bool is_empty() { return empty_; }
      const std::string& s() { return s_; }
      int n() { return n_; }
      bool operator==(const AttributeValue& other) {
        if (empty_ == other.empty_) {
          return true;
        }
        if (s_ == other.s_ && n_ == other.n_) {
          return true;
        }
        return false;
      }
    };

    // Indicates attribute isn't set
    const AttributeValue ATTR_NOT_SET = AttributeValue();

    // Map of attributes
    typedef std::map<NodeAttribute, AttributeValue> AttributeMap;

    // Helper functions
    std::string type_to_string(NodeType type);
    ERL_NIF_TERM type_to_atom(NodeType type, gb_priv_s* priv_data);
    ERL_NIF_TERM alignment_to_atom(NodeAlignment align, gb_priv_s* priv_data);
    inline bool is_markdown_list(NodeType type) { return type == MD_ORDERED_LIST || type == MD_UNORDERED_LIST; }

    // Base Markdown node type
    class MarkdownNode {
    private:
      // No copying
      MarkdownNode(MarkdownNode const &);
      MarkdownNode &operator=(MarkdownNode const &);
    protected:
      std::string text_;
      NodeType type_;
      AttributeMap attributes_;
      bool terminates_line_;
      virtual ERL_NIF_TERM decorate_term(ErlNifEnv* env, ERL_NIF_TERM term);
    public:
      MarkdownNode(NodeType type);
      MarkdownNode(NodeType type, const std::string& text);
      virtual ~MarkdownNode() {
        if (!attributes_.empty()) {
          attributes_.clear();
        }
      }

      NodeType get_type() { return type_; }
      const std::string& get_text() { return text_; }
      void set_text(std::string text) { text_ = text; }

      virtual std::string to_string();

      const AttributeValue& get_attribute(const NodeAttribute& attr);
      void put_attribute(const NodeAttribute& attr, const AttributeValue value);
      bool has_attribute(const NodeAttribute& attr);

      virtual bool line_terminator();
      bool terminates_line(bool flag);

      virtual ERL_NIF_TERM to_erl_term(ErlNifEnv* env);
    };

    // Vector of markdown nodes
    typedef std::vector<MarkdownNode*> NodeVector;

    // Base Markdown node for types with children
    class MarkdownNodeContainer : public MarkdownNode {
    private:
      // No copying
      MarkdownNodeContainer(MarkdownNodeContainer const &);
      MarkdownNodeContainer &operator=(MarkdownNodeContainer const &);
    protected:
      NodeVector children_;
      ERL_NIF_TERM children_to_term_list(ErlNifEnv* env);
      virtual ERL_NIF_TERM decorate_term(ErlNifEnv* env, ERL_NIF_TERM term);
    public:
      MarkdownNodeContainer(NodeType type) : MarkdownNode(type) { }
      virtual ~MarkdownNodeContainer();
      virtual std::string to_string();
      const NodeVector& get_children() { return children_; }
      void add_child(MarkdownNode* child);
      void drop_last(NodeType type);
      bool empty();
      bool line_terminator();
    };
  }
}
#endif
