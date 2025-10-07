#ifndef LSP_PARAMS_HPP
#define LSP_PARAMS_HPP

#include "protocol/responses.hpp"
#include "types.hpp"
#include <iostream>
#include <vector>

namespace lsp {
struct InitializeParams {

  optional<int> processId;
  optional<ClientInfo> clientInfo;
  optional<string> locale;

  // @deprecated in favour of `rootUri`.
  // optional<string> rootPath;

  // @deprecated in favour of `workspaceFolders`
  // optional<DocumentUri> rootUri;

  optional<LSPAny> initializationOptions;
  ClientCapabilities capabilities;
  optional<TraceValue> trace;
  optional<std::vector<WorkspaceFolder>> workspaceFolders;
};

struct DidOpenParams {
  TextDocumentItem textDocument;
};

struct DidChangeParams {
  VersionedTextDocumentIdentifier textDocument;
  std::vector<TextDocumentContentChangeEvent> contentChanges;
};

inline void from_json(const json &j, lsp::DidChangeParams &didChangeParams) {
  j["textDocument"]["uri"].get_to(didChangeParams.textDocument.uri);
  j["textDocument"]["version"].get_to(didChangeParams.textDocument.version);

  for (auto &change : j["contentChanges"]) {
    if (change.contains("range")) {

      int start_line, start_character, end_line, end_character;
      lsp::Position start, end;

      change["range"]["start"]["line"].get_to<int>(start_line);
      change["range"]["start"]["character"].get_to<int>(start_character);

      change["range"]["end"]["line"].get_to<int>(end_line);
      change["range"]["end"]["character"].get_to<int>(end_character);

      start = lsp::Position{.line = start_line, .character = start_character};
      end = lsp::Position{.line = end_line, .character = end_character};

      auto range = lsp::Range{.start = start, .end = end};

      string text;
      change["text"].get_to<string>(text);
      didChangeParams.contentChanges.push_back(
          lsp::TextDocumentContentChangeEventWithRange{.text = text,
                                                       .range = range});
      continue;
    }

    string text;
    change["text"].get_to<string>(text);
    didChangeParams.contentChanges.push_back(
        lsp::TextDocumentContentChangeEventFull{.text = text});
  }
}
} // namespace lsp

#endif // LSP_PARAMS_HPP
