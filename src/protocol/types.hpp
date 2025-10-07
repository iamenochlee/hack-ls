#ifndef LSP_TYPES_HPP
#define LSP_TYPES_HPP

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <variant>

using nlohmann::json;
using std::optional;
using std::string;

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
  string uri;
};

struct TextDocumentItem : public TextDocumentIdentifier {
  string languageId;
  int version;
  string text;
};

struct VersionedTextDocumentIdentifier : public TextDocumentIdentifier {
  int version;
};

struct TextDocumentContentChangeEventWithRange {
  Range range;
  string text;
};

struct TextDocumentContentChangeEventFull {
  string text;
};

using TextDocumentContentChangeEvent =
    std::variant<TextDocumentContentChangeEventWithRange,
                 TextDocumentContentChangeEventFull>;

using LSPAny = json;
// Trace value enumeration
enum class TraceValue { Off, Messages, Verbose };

// Client info structure
struct ClientInfo {
  string name;
  optional<string> version;
};

// Workspace folder structure
struct WorkspaceFolder {
  string uri;
  string name;
};

// Placeholder for ClientCapabilities (to be expanded as needed)
struct ClientCapabilities {
  // Will be expanded with actual capabilities later
};

} // namespace lsp

#endif // LSP_TYPES_HPP
