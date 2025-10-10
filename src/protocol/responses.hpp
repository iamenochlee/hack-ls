#ifndef LSP_RESPONSES_HPP
#define LSP_RESPONSES_HPP

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <variant>

namespace lsp {

struct ServerCapabilities {
  std::optional<bool> hoverProvider;
  std::optional<bool> definitionProvider;
  std::optional<nlohmann::json> experimental;
};

struct InitializeResult {
  ServerCapabilities serverCapabilities;
};

struct DidOpenResult {
  std::string uri;
};

struct DidChangeResult {
  std::string uri;
};

using Result = std::variant<InitializeResult, DidOpenResult, DidChangeResult>;

struct Response {
  int contentLength;
  nlohmann::json result;
};

// JSON conversion functions
inline void to_json(nlohmann::json &j, const InitializeResult &result) {
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

inline void to_json(nlohmann::json &j, const DidOpenResult &result) {
  j["uri"] = result.uri;
}

inline void to_json(nlohmann::json &j, const DidChangeResult &result) {
  j["uri"] = result.uri;
}

inline void to_json(nlohmann::json &j, const Result &result) {
  std::visit([&j](const auto &res) { j = res; }, result);
}

} // namespace lsp

#endif // LSP_RESPONSES_HPP
