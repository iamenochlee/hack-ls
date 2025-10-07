#ifndef LSP_RESPONSES_HPP
#define LSP_RESPONSES_HPP

#include "../main.hpp"
#include <optional>
#include <string>
#include <variant>

using nlohmann::json;
using std::optional;
using std::string;
using std::variant;

namespace lsp {

struct ServerCapabilities {
  optional<bool> hoverProvider;
  optional<bool> definitionProvider;
  optional<json> experimental;
};

struct InitializeResult {
  ServerCapabilities serverCapabilities;
};

struct DidOpenResult {
  string uri;
};

struct DidChangeResult {
  string uri;
};

using Result = std::variant<InitializeResult, DidOpenResult, DidChangeResult>;

struct Response {
  int contentLength;
  json result;
};

// JSON conversion functions
inline void to_json(json &j, const InitializeResult &result) {
  ServerCapabilities caps = result.serverCapabilities;
  if (caps.hoverProvider.has_value()) {
    j["serverCapabilities"]["hoverProvider"] = caps.hoverProvider.value();
  }
  if (caps.definitionProvider.has_value()) {
    j["serverCapabilities"]["definitionProvider"] =
        caps.definitionProvider.value();
  }
  if (caps.experimental.has_value()) {
    j["serverCapabilities"]["experimental"] = caps.experimental.value();
  }
}

inline void to_json(json &j, const DidOpenResult &result) {
  j["uri"] = result.uri;
}

inline void to_json(json &j, const DidChangeResult &result) {
  j["uri"] = result.uri;
}

inline void to_json(json &j, const Result &result) {
  std::visit([&j](const auto &res) { j = res; }, result);
}

} // namespace lsp

#endif // LSP_RESPONSES_HPP
