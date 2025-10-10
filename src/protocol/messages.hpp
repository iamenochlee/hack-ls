#ifndef LSP_MESSAGES_HPP
#define LSP_MESSAGES_HPP

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

using nlohmann::json;

namespace lsp {

struct Message {
  std::string jsonrpc;
};

enum MessageType { NOTIFICATION, REQUEST };

struct RequestMessage : public Message {
  std::variant<std::string, int> id;
  std::string method;
  std::optional<json> params;
};

struct NotificationMessage : public Message {
  std::string method;
  std::optional<json> params;
};

// JSON conversion functions
inline void from_json(const json &j, RequestMessage &req) {
  j.at("jsonrpc").get_to(req.jsonrpc);
  if (j["id"].is_string()) {
    req.id = j["id"].get<std::string>();
  } else {
    req.id = j["id"].get<int>();
  }
  j.at("method").get_to(req.method);
  if (j.contains("params")) {
    j.at("params").get_to(req.params);
  }
}

inline void from_json(const json &j, NotificationMessage &req) {
  j.at("jsonrpc").get_to(req.jsonrpc);
  j.at("method").get_to(req.method);
  if (j.contains("params")) {
    j.at("params").get_to(req.params);
  }
}

} // namespace lsp

#endif // LSP_MESSAGES_HPP
