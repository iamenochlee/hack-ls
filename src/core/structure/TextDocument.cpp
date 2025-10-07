
#include "./TextDocument.hpp"
#include "protocol/types.hpp"
#include <variant>

void TextDocument::applyChanges(
    std::vector<lsp::TextDocumentContentChangeEvent> changes) {
  for (const auto &change : changes) {
    if (std::holds_alternative<lsp::TextDocumentContentChangeEventFull>(
            change)) {
      // Full document replacement
      text = std::get<lsp::TextDocumentContentChangeEventFull>(change).text;
    } else if (std::holds_alternative<
                   lsp::TextDocumentContentChangeEventWithRange>(change)) {
      // Ranged change - TODO: Implement proper range-based text replacement
      auto rangedChange =
          std::get<lsp::TextDocumentContentChangeEventWithRange>(change);
      // For now, just append the new text (proper implementation would replace
      // the range)
      text += rangedChange.text;
    }
  }
}
