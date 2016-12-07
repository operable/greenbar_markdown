#ifndef GREENBAR_MD_NODE_H
#define GREENBAR_MD_NODE_H

#include "md_node_base.hpp"

namespace greenbar {
  namespace node2 {
    class EOLNode : public MarkdownNode {
    public:
      EOLNode() : MarkdownNode(MD_EOL) {
        terminates_line_ = true;
      }
      ~EOLNode() { }
    };

    class BoldNode : public MarkdownNode {
    public:
      BoldNode(const std::string& text) : MarkdownNode(MD_BOLD, text) { }
      ~BoldNode() { }
    };

    class ItalicsNode : public MarkdownNode {
    public:
      ItalicsNode(const std::string& text) : MarkdownNode(MD_ITALICS, text) { }
      ~ItalicsNode() { }
    };

    class FixedWidthNode : public MarkdownNode {
    public:
      FixedWidthNode(const std::string& text) : MarkdownNode(MD_FIXED_WIDTH, text) { }
      ~FixedWidthNode() { }
    };

    class FixedWidthBlockNode : public MarkdownNode {
    public:
      FixedWidthBlockNode(const std::string& text): MarkdownNode(MD_FIXED_WIDTH_BLOCK, text) {
        terminates_line_ = true;
      }
      ~FixedWidthBlockNode() { }
    };

    class TableHeaderNode : public MarkdownNode {
    public:
      TableHeaderNode(): MarkdownNode(MD_TABLE_HEADER) { }
      ~TableHeaderNode() { }
    };

    class HeaderNode : public MarkdownNode {
    protected:
      virtual ERL_NIF_TERM decorate_term(ErlNifEnv* env, ERL_NIF_TERM term);
    public:
      HeaderNode(const std::string& text, int level) : MarkdownNode(MD_HEADER, text) {
        put_attribute(ATTR_LEVEL, level);
      }
      ~HeaderNode() { }
      void set_level(int level) {
        put_attribute(ATTR_LEVEL, level);
      }
    };

    class LinkNode : public MarkdownNode {
    protected:
      virtual ERL_NIF_TERM decorate_term(ErlNifEnv* env, ERL_NIF_TERM term);
    public:
      LinkNode(const std::string& title, const std::string& url) : MarkdownNode(MD_LINK, title) {
        put_attribute(ATTR_URL, url);
      }
      ~LinkNode() { }
    };

    class TextNode : public MarkdownNode {
    public:
      TextNode(const std::string& text) : MarkdownNode(MD_TEXT, text) { }
      ~TextNode() { }
      HeaderNode* to_header(int level) {
        return new HeaderNode(text_, level);
      }

      ItalicsNode* to_italics() {
        return new ItalicsNode(text_);
      }

      BoldNode* to_bold() {
        return new BoldNode(text_);
      }

      LinkNode* to_link() {
        return new LinkNode(text_, "");
      }
    };

    class UnorderedListNode : public MarkdownNodeContainer {
    public:
      UnorderedListNode() : MarkdownNodeContainer(MD_UNORDERED_LIST) { }
      ~UnorderedListNode() { }
    };

    class OrderedListNode : public MarkdownNodeContainer {
    public:
      OrderedListNode() : MarkdownNodeContainer(MD_ORDERED_LIST) { }
      ~OrderedListNode() { }
    };

    class ListItemNode : public MarkdownNodeContainer {
    public:
      ListItemNode() : MarkdownNodeContainer(MD_LIST_ITEM) { }
      ~ListItemNode() { }
    };

    class TableCellNode : public MarkdownNodeContainer {
    protected:
      virtual ERL_NIF_TERM decorate_term(ErlNifEnv* env, ERL_NIF_TERM term);
    public:
      TableCellNode() : MarkdownNodeContainer(MD_TABLE_CELL) { }
      ~TableCellNode() { }
    };

    class TableRowNode : public MarkdownNodeContainer {
    public:
      TableRowNode() : MarkdownNodeContainer(MD_TABLE_ROW) { }
      ~TableRowNode() { }
      void mark_header() { type_ = MD_TABLE_HEADER; }
    };

    class TableNode : public MarkdownNodeContainer {
    public:
      TableNode() : MarkdownNodeContainer(MD_TABLE) { }
      ~TableNode() { }
      void mark_header();
    };

    class ParagraphNode : public MarkdownNodeContainer {
    public:
      ParagraphNode() : MarkdownNodeContainer(MD_PARAGRAPH) { }
      ~ParagraphNode() { }
    };

  }
}


#endif
