#include "TextDocument.hpp"

#include <cstddef>
#include <string>
#include <variant>
#include <vector>

size_t TextDocument::positionToOffset(const lsp::Position &position) const {

  // todo: make this more efficient for large documents
  size_t offset = 0;
  for (int i = 0; i < position.line; ++i) {
    offset = text.find('\n', offset);
    if (offset == std::string::npos) {
      // line number out of bounds, clamp to end of text
      return text.size();
    }
    offset += 1; // move past the newline
  }

  // clamp offset to end of line if out of bounds
  size_t lineEnd = text.find('\n', offset);
  size_t lineLength =
      (lineEnd == std::string::npos ? text.size() : lineEnd) - offset;
  size_t charPos = std::min((size_t)position.character, lineLength);

  return offset + charPos;
}

void TextDocument::applyChanges(
    std::vector<lsp::TextDocumentContentChangeEvent> changes) {

  for (const auto &change : changes) {
    if (std::holds_alternative<lsp::TextDocumentContentChangeEventFull>(
            change)) {

      text = std::get<lsp::TextDocumentContentChangeEventFull>(change).text;
      continue;
    } 
    
    auto rangedChange =
          std::get<lsp::TextDocumentContentChangeEventWithRange>(change);

     // Apply range-based change by replacing text within the specified range
     size_t startOffset = positionToOffset(rangedChange.range.start);
     size_t endOffset = positionToOffset(rangedChange.range.end);

     text.replace(startOffset, endOffset - startOffset, rangedChange.text);
    
  }
}
