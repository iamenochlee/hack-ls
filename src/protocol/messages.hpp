#ifndef LSP_MESSAGES_HPP
#define LSP_MESSAGES_HPP

#include "../main.hpp"
#include <optional>
#include <string>
#include <variant>

using nlohmann::json;
using std::optional;
using std::string;
using std::variant;

namespace lsp {

enum MessageType { NOTIFICATION, REQUEST };

struct RequestMessage : public Message {
  variant<string, int> id;
  string method;
  optional<json> params;
};

struct NotificationMessage : public Message {
  string method;
  optional<json> params;
};

// JSON conversion functions
inline void from_json(const json &j, RequestMessage &req) {
  j.at("jsonrpc").get_to(req.jsonrpc);
  if (j["id"].is_string()) {
    req.id = j["id"].get<string>();
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
