#ifndef TEXTDOCUMENT_HPP
#define TEXTDOCUMENT_HPP

#include "protocol/types.hpp"

struct TextDocument {

public:
  string uri;
  int version;
  string text;

  TextDocument(string uri, int version, string text)
      : uri(uri), version(version), text(text) {}

  void applyChanges(std::vector<lsp::TextDocumentContentChangeEvent>);
};

#endif // TEXTDOCUMENT_HPP
