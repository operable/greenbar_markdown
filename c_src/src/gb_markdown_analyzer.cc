#include <string>
#include <iostream>
#include <cstring>
#include <cstdlib>

#include "erl_nif.h"
#include "document.h"
#include "buffer.h"
#include "md_node.hpp"
#include "debug.hpp"

typedef hoedown_renderer markdown_analyzer;

static void gb_markdown_blockcode(hoedown_buffer *ob, const hoedown_buffer *text, const hoedown_buffer *lang, const hoedown_renderer_data *data);
static void gb_markdown_header(hoedown_buffer *ob, const hoedown_buffer *content, int level, const hoedown_renderer_data *data);
static void gb_markdown_list(hoedown_buffer *ob, const hoedown_buffer *content, hoedown_list_flags flags, const hoedown_renderer_data *data);
static void gb_markdown_listitem(hoedown_buffer *ob, const hoedown_buffer *content, hoedown_list_flags flags, const hoedown_renderer_data *data);
static void gb_markdown_paragraph(hoedown_buffer *ob, const hoedown_buffer *content, const hoedown_renderer_data *data);
static void gb_markdown_table(hoedown_buffer *ob, const hoedown_buffer *content, const hoedown_renderer_data *data);
static void gb_markdown_table_header(hoedown_buffer *ob, const hoedown_buffer *content, const hoedown_renderer_data *data);
static void gb_markdown_table_row(hoedown_buffer *ob, const hoedown_buffer *content, const hoedown_renderer_data *data);
static void gb_markdown_table_cell(hoedown_buffer *ob, const hoedown_buffer *content, hoedown_table_flags flags, const hoedown_renderer_data *data);

static int gb_markdown_autolink(hoedown_buffer *ob, const hoedown_buffer *link, hoedown_autolink_type type, const hoedown_renderer_data *data);
static int gb_markdown_codespan(hoedown_buffer *ob, const hoedown_buffer *text, const hoedown_renderer_data *data);
static int gb_markdown_emphasis(hoedown_buffer *ob, const hoedown_buffer *content, const hoedown_renderer_data *data);
static int gb_markdown_double_emphasis(hoedown_buffer *ob, const hoedown_buffer *content, const hoedown_renderer_data *data);
static int gb_markdown_linebreak(hoedown_buffer *ob, const hoedown_renderer_data *data);
static int gb_markdown_link(hoedown_buffer *ob, const hoedown_buffer *content, const hoedown_buffer *link, const hoedown_buffer *title,
                     const hoedown_renderer_data *data);
static int gb_markdown_linebreak(hoedown_buffer *ob, const hoedown_renderer_data *data);
static void gb_markdown_normal_text(hoedown_buffer *ob, const hoedown_buffer *text, const hoedown_renderer_data *data);

#define GB_HOEDOWN_EXTENSIONS (hoedown_extensions) (HOEDOWN_EXT_DISABLE_INDENTED_CODE | HOEDOWN_EXT_SPACE_HEADERS | \
                                                    HOEDOWN_EXT_MATH_EXPLICIT | HOEDOWN_EXT_NO_INTRA_EMPHASIS | \
                                                    HOEDOWN_EXT_TABLES | HOEDOWN_EXT_FENCED_CODE)
#define GB_MAX_NESTING 16

using namespace greenbar::node2;

namespace greenbar {

  markdown_analyzer* new_markdown_analyzer() {
    // Create renderer
    auto analyzer = (markdown_analyzer *) malloc(sizeof(markdown_analyzer));
    if (!analyzer) {
      return nullptr;
    }
    // Zero out all fields
    memset(analyzer, 0, sizeof(markdown_analyzer));

    // Block callbacks
    analyzer->blockcode = gb_markdown_blockcode;
    analyzer->paragraph = gb_markdown_paragraph;
    analyzer->header = gb_markdown_header;

    // Span callbacks
    analyzer->codespan = gb_markdown_codespan;
    analyzer->emphasis = gb_markdown_emphasis;
    analyzer->double_emphasis = gb_markdown_double_emphasis;
    analyzer->triple_emphasis = gb_markdown_double_emphasis;
    analyzer->autolink = gb_markdown_autolink;
    analyzer->link = gb_markdown_link;
    analyzer->list = gb_markdown_list;
    analyzer->listitem = gb_markdown_listitem;
    analyzer->table = gb_markdown_table;
    analyzer->table_header = gb_markdown_table_header;
    analyzer->table_row = gb_markdown_table_row;
    analyzer->table_cell = gb_markdown_table_cell;
    analyzer->normal_text = gb_markdown_normal_text;
    analyzer->linebreak = gb_markdown_linebreak;
    analyzer->opaque = (void *) new NodeVector();
    return analyzer;
  }

  hoedown_document* new_hoedown_document(markdown_analyzer* renderer) {
    return hoedown_document_new(renderer, (hoedown_extensions) GB_HOEDOWN_EXTENSIONS, GB_MAX_NESTING);
  }

  void free_markdown_analyzer(markdown_analyzer* analyzer) {
    if (analyzer->opaque != nullptr) {
      auto collector = (NodeVector*) analyzer->opaque;
      while (!collector->empty()) {
        auto node = collector->back();
        collector->pop_back();
        delete node;
      }
      delete collector;
    }
    free(analyzer);
  }

  NodeVector* get_collector(markdown_analyzer* analyzer) {
    return (NodeVector*) analyzer->opaque;
  }

}

static std::string hoedown_buffer_to_string(const hoedown_buffer* buf) {
  if (buf == nullptr) {
    return std::string("");
  }
  return std::string((char*) buf->data, buf->size);
}

static std::string hoedown_buffer_to_string(const hoedown_buffer* buf, unsigned int begin, unsigned int end) {
  if (buf == nullptr) {
    return std::string("");
  }
  return std::string((char*) &buf->data[begin], end);
}

static NodeVector* get_collector(const hoedown_renderer_data *data) {
  return (NodeVector*) data->opaque;
}

static void gb_markdown_blockcode(hoedown_buffer *ob, const hoedown_buffer *text, const hoedown_buffer *lang,
                                  const hoedown_renderer_data *data) {
  if (text == nullptr || text->size == 0 || (text->size == 1 && text->data[0] == '\n')) {
    return;
  }
  auto collector = get_collector(data);
  std::string block_text = "";
  if (text->size > 1) {
    uint8_t last_char = text->data[text->size - 1];
    if (last_char == '\n') {
      block_text = std::string((char*) text->data, text->size - 1);
    } else {
      block_text = hoedown_buffer_to_string(text);
    }
    collector->push_back(new FixedWidthBlockNode(block_text));
  }
}

static void gb_markdown_header(hoedown_buffer *ob, const hoedown_buffer *content, int level,
                               const hoedown_renderer_data *data) {
  auto collector = get_collector(data);
  if (content->size == 0 && collector->empty() == false) {
    TextNode* tn = dynamic_cast<TextNode*>(collector->back());
    if (tn) {
      HeaderNode* hn = tn->to_header(level);
      collector->pop_back();
      collector->push_back(hn);
      delete tn;
    }
  }
  else {
    if (content->size > 0) {
      HeaderNode* hn = new HeaderNode(hoedown_buffer_to_string(content), level);
      collector->push_back(hn);
    }
  }
}

static bool is_valid_paragraph_child(NodeType type) {
  if (type == MD_PARAGRAPH || type == MD_TABLE || type == MD_TABLE_ROW || type == MD_TABLE_CELL ||
      type == MD_TABLE_HEADER || type == MD_ORDERED_LIST || type == MD_UNORDERED_LIST || type == MD_LIST_ITEM) {
    return false;
  }
  return true;
}

static void gb_markdown_paragraph(hoedown_buffer *ob, const hoedown_buffer *content, const hoedown_renderer_data *data) {
  auto collector = get_collector(data);
  if (collector->empty()) {
    return;
  }
  ParagraphNode* pn = new ParagraphNode();
  while (!collector->empty()) {
    auto last_node = collector->back();
    NodeType type = last_node->get_type();
    if (!is_valid_paragraph_child(type)) {
      break;
    }
    collector->pop_back();
    // If text of previous node terminates a line AND ends with two spaces
    // then strip the two spaces and insert a newline in the paragraph.
    if (last_node->line_terminator()) {
      pn->add_child(new EOLNode());
    }
    pn->add_child(last_node);
  }
  if (pn->empty()) {
    delete pn;
  } else {
    pn->terminates_line(true);
    collector->push_back(pn);
  }
}

static int gb_markdown_autolink(hoedown_buffer *ob, const hoedown_buffer *link, hoedown_autolink_type type, const hoedown_renderer_data *data) {
  if (link == nullptr) {
    return 1;
  }
  auto collector = get_collector(data);
  auto link_text = hoedown_buffer_to_string(link);
  collector->push_back(new LinkNode(link_text, link_text));
  return 1;
}

static int gb_markdown_codespan(hoedown_buffer *ob, const hoedown_buffer *text, const hoedown_renderer_data *data) {
  if (text == nullptr || text->size == 0 || (text->size == 1 && text->data[0] == '\n')) {
    return 1;
  }
  auto collector = get_collector(data);
  collector->push_back(new FixedWidthNode(hoedown_buffer_to_string(text)));
  return 1;
}

static int gb_markdown_emphasis(hoedown_buffer *ob, const hoedown_buffer *content, const hoedown_renderer_data *data) {
  auto collector = get_collector(data);
  if (content == nullptr || content->size == 0) {
    TextNode* tn = dynamic_cast<TextNode*>(collector->back());
    if (tn) {
      ItalicsNode* italn = tn->to_italics();
      collector->pop_back();
      collector->push_back(italn);
      delete tn;
    }
  } else {
    auto collector = get_collector(data);
    collector->push_back(new ItalicsNode(hoedown_buffer_to_string(content)));
  }
  return 1;
}

static int gb_markdown_double_emphasis(hoedown_buffer *ob, const hoedown_buffer *content, const hoedown_renderer_data *data) {
  auto collector = get_collector(data);
  if (content == nullptr || content->size == 0) {
    TextNode* tn = dynamic_cast<TextNode*>(collector->back());
    if (tn) {
      BoldNode* bn = tn->to_bold();
      collector->pop_back();
      collector->push_back(bn);
      delete tn;
    }
  } else {
    collector->push_back(new BoldNode(hoedown_buffer_to_string(content)));
  }
  return 1;
}

static int gb_markdown_link(hoedown_buffer *ob, const hoedown_buffer *content, const hoedown_buffer *url, const hoedown_buffer *link,
                            const hoedown_renderer_data *data) {
  auto collector = get_collector(data);
  MarkdownNode* last_node = nullptr;
  if (!collector->empty()) {
    last_node = collector->back();
  }
  if (last_node == nullptr || last_node->get_type() != MD_TEXT) {
    std::string url_text = hoedown_buffer_to_string(url);
    std::string link_text = hoedown_buffer_to_string(link);
    collector->push_back(new LinkNode(link_text, url_text));
  } else {
    TextNode* tn = dynamic_cast<TextNode*>(last_node);
    if (tn) {
      LinkNode* ln = tn->to_link();
      ln->put_attribute(ATTR_URL, AttributeValue(hoedown_buffer_to_string(url)));
      collector->pop_back();
      collector->push_back(ln);
      delete tn;
    }
  }
  return 1;
}

static int gb_markdown_linebreak(hoedown_buffer *ob, const hoedown_renderer_data *data) {
   auto collector = get_collector(data);
   if (!collector->empty()) {
     auto last_node = collector->back();
     last_node->terminates_line(true);
   }
   return 1;
 }

static void gb_markdown_normal_text(hoedown_buffer *ob, const hoedown_buffer *text, const hoedown_renderer_data *data) {
  if (text == nullptr) {
    return;
  }
  TextNode* tn = nullptr;
  auto collector = get_collector(data);
  switch(text->size) {
  case 0:
    break;
  default:
    if (text->data[0] == '\n') {
      if (!collector->empty()) {
        auto last_node = collector->back();
        last_node->terminates_line(true);
        if (text->size == 1) {
          break;
        }
        tn = new TextNode(hoedown_buffer_to_string(text, 1, text->size - 1));
      } else {
        tn = new TextNode(hoedown_buffer_to_string(text));
      }
    } else {
      auto last_char_idx = text->size - 1;
      if (text->data[last_char_idx] == '\n') {
        tn = new TextNode(hoedown_buffer_to_string(text, 0, last_char_idx - 1));
        tn->terminates_line(true);
      } else {
        tn = new TextNode(hoedown_buffer_to_string(text));
      }
    }
  }
  if (tn == nullptr) {
    return;
  }
  collector->push_back(tn);
}

static void gb_markdown_list(hoedown_buffer *ob, const hoedown_buffer *content,
                             hoedown_list_flags flags, const hoedown_renderer_data *data) {
  auto collector = get_collector(data);
  MarkdownNodeContainer *list = nullptr;
  if (flags & HOEDOWN_LIST_ORDERED) {
    list = new OrderedListNode();
  } else {
    list = new UnorderedListNode();
  }
  while (!collector->empty()) {
    auto last_node = collector->back();
    if (last_node->get_type() != MD_LIST_ITEM) {
      break;
    }
    collector->pop_back();
    list->add_child(last_node);
  }
  if (list->empty()) {
    delete list;
  } else {
    collector->push_back(list);
  }
}

static void gb_markdown_listitem(hoedown_buffer *ob, const hoedown_buffer *content, hoedown_list_flags flags, const hoedown_renderer_data *data) {
  auto collector = get_collector(data);
  auto item = new ListItemNode();
  if (!collector->empty()) {
    auto first_child = collector->back();
    auto first_child_type = first_child->get_type();
    if (first_child->get_type() == MD_LIST_ITEM) {
      return;
    }
    collector->pop_back();
    item->add_child(first_child);
    while (!collector->empty()) {
      auto child = collector->back();
      if (child->get_type() == MD_LIST_ITEM) {
        break;
      }
      if (child->line_terminator()) {
        if (is_markdown_list(first_child_type) || first_child_type == MD_FIXED_WIDTH_BLOCK) {
          collector->pop_back();
          item->add_child(child);
        }
        break;
      }
      collector->pop_back();
      item->add_child(child);
    }
  }
  if (item->empty()) {
    delete item;
  } else {
    collector->push_back(item);
  }
}


static void gb_markdown_table(hoedown_buffer *ob, const hoedown_buffer *content, const hoedown_renderer_data *data) {
  auto collector = get_collector(data);
  if (collector->empty()) {
    return;
  }
  bool has_header = false;
  TableNode* table = new TableNode();
  auto child = collector->back();
  while (child->get_type() == MD_TABLE_HEADER || child->get_type() == MD_TABLE_ROW) {
    collector->pop_back();
    if (child->get_type() == MD_TABLE_HEADER) {
      has_header = true;
    } else {
      table->add_child(child);
    }
    if (collector->empty()) {
      break;
    }
    child = collector->back();
  }
  if (has_header) {
    table->mark_header();
  }
  if (table->empty()) {
    delete table;
  } else {
    collector->push_back(table);
  }
}
static void gb_markdown_table_header(hoedown_buffer *ob, const hoedown_buffer *content, const hoedown_renderer_data *data) {
  auto collector = get_collector(data);
  if (collector->empty()) {
    return;
  }
  collector->push_back(new TableHeaderNode());
}

static void gb_markdown_table_row(hoedown_buffer *ob, const hoedown_buffer *content, const hoedown_renderer_data *data) {
  auto collector = get_collector(data);
  if (collector->empty()) {
    return;
  }
  TableRowNode* row = new TableRowNode();
  while (!collector->empty()) {
    auto child = collector->back();
    if (child->get_type() == MD_TABLE_CELL) {
      collector->pop_back();
      row->add_child(child);
    } else {
      break;
    }
  }
  if (row->empty()) {
    delete row;
  } else {
    collector->push_back(row);
  }
}

static void gb_markdown_table_cell(hoedown_buffer *ob, const hoedown_buffer *content, hoedown_table_flags flags, const hoedown_renderer_data *data) {
  auto collector = get_collector(data);
  if (collector->empty()) {
    return;
  }
  TableCellNode* cell = new TableCellNode();
  bool table_done = false;
  while (!collector->empty() && !table_done) {
    auto child = collector->back();
    switch(child->get_type()) {
    case MD_TABLE_CELL:
    case MD_TABLE_ROW:
    case MD_TABLE_HEADER:
    case MD_TABLE:
      table_done = true;
      break;
    default:
      collector->pop_back();
      cell->add_child(child);
    }
  }
  NodeAlignment alignment = ALIGN_NONE;
  if (flags & HOEDOWN_TABLE_ALIGN_LEFT) {
    alignment = ALIGN_LEFT;
  }
  if (flags & HOEDOWN_TABLE_ALIGN_RIGHT) {
    alignment = ALIGN_RIGHT;
  }
  if (flags & HOEDOWN_TABLE_ALIGN_CENTER) {
    alignment = ALIGN_CENTER;
  }
  cell->put_attribute(ATTR_ALIGNMENT, AttributeValue((int) alignment));
  collector->push_back(cell);
}
