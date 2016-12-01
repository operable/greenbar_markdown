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
    };

    class BoldNode : public MarkdownNode {
    public:
      BoldNode(const std::string& text) : MarkdownNode(MD_BOLD, text) { }
    };

    class ItalicsNode : public MarkdownNode {
    public:
      ItalicsNode(const std::string& text) : MarkdownNode(MD_ITALICS, text) { }
    };

    class FixedWidthNode : public MarkdownNode {
    public:
      FixedWidthNode(const std::string& text) : MarkdownNode(MD_FIXED_WIDTH, text) { }
    };

    class FixedWidthBlockNode : public MarkdownNode {
    public:
      FixedWidthBlockNode(const std::string& text): MarkdownNode(MD_FIXED_WIDTH_BLOCK, text) {
        terminates_line_ = true;
      }
    };

    class TableHeaderNode : public MarkdownNode {
    public:
      TableHeaderNode(): MarkdownNode(MD_TABLE_HEADER) { }
    };

    class HeaderNode : public MarkdownNode {
    protected:
      virtual ERL_NIF_TERM decorate_term(ErlNifEnv* env, ERL_NIF_TERM term);
    public:
      HeaderNode(const std::string& text, int level) : MarkdownNode(MD_HEADER, text) {
        put_attribute(ATTR_LEVEL, level);
      }
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
    };

    class TextNode : public MarkdownNode {
    public:
      TextNode(const std::string& text) : MarkdownNode(MD_TEXT, text) { }
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
    };

    class OrderedListNode : public MarkdownNodeContainer {
    public:
      OrderedListNode() : MarkdownNodeContainer(MD_ORDERED_LIST) { }
    };

    class ListItemNode : public MarkdownNodeContainer {
    public:
      ListItemNode() : MarkdownNodeContainer(MD_LIST_ITEM) { }
    };

    class TableCellNode : public MarkdownNodeContainer {
    protected:
      virtual ERL_NIF_TERM decorate_term(ErlNifEnv* env, ERL_NIF_TERM term);
    public:
      TableCellNode() : MarkdownNodeContainer(MD_TABLE_CELL) { }
    };

    class TableRowNode : public MarkdownNodeContainer {
    public:
      TableRowNode() : MarkdownNodeContainer(MD_TABLE_ROW) { }
      void mark_header() { type_ = MD_TABLE_HEADER; }
    };

    class TableNode : public MarkdownNodeContainer {
    public:
      TableNode() : MarkdownNodeContainer(MD_TABLE) { }
      void mark_header();
    };

    class ParagraphNode : public MarkdownNodeContainer {
    public:
      ParagraphNode() : MarkdownNodeContainer(MD_PARAGRAPH) { }
    };

  }
}


#endif
