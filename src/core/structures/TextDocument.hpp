#ifndef TEXTDOCUMENT_HPP
#define TEXTDOCUMENT_HPP

#include "protocol/types.hpp"

#include <string>
#include <vector>

struct TextDocument {
  size_t positionToOffset(const lsp::Position &position) const;

public:
  std::string uri;
  int version;
  std::string text;

  TextDocument(std::string uri, int version, std::string text)
      : uri(uri), version(version), text(text) {}

  void applyChanges(std::vector<lsp::TextDocumentContentChangeEvent>);
};

#endif // TEXTDOCUMENT_HPP
