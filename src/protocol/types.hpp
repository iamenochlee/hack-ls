#ifndef LSP_TYPES_HPP
#define LSP_TYPES_HPP

#include <optional>
#include <string>
#include <variant>

namespace lsp {

struct Position {
  int line;
  int character;
};

struct Range {
  Position start;
  Position end;
};

struct TextDocumentIdentifier {
  std::string uri;
};

struct TextDocumentItem : public TextDocumentIdentifier {
  std::string languageId;
  int version;
  std::string text;
};

struct VersionedTextDocumentIdentifier : public TextDocumentIdentifier {
  int version;
};

struct TextDocumentContentChangeEventWithRange {
  Range range;
  std::string text;
};

struct TextDocumentContentChangeEventFull {
  std::string text;
};

using TextDocumentContentChangeEvent =
    std::variant<TextDocumentContentChangeEventWithRange,
                 TextDocumentContentChangeEventFull>;

// TODO: fix this
using LSPAny = int;
// Trace value enumeration
enum class TraceValue { Off, Messages, Verbose };

// Client info structure
struct ClientInfo {
  std::string name;
  std::optional<std::string> version;
};

// Workspace folder structure
struct WorkspaceFolder {
  std::string uri;
  std::string name;
};

// Placeholder for ClientCapabilities (to be expanded as needed)
struct ClientCapabilities {
  // Will be expanded with actual capabilities later
};

} // namespace lsp

#endif // LSP_TYPES_HPP
