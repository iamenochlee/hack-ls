#include "wire.hpp"

#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <variant>

using namespace std;

namespace lsp {

static lsp::LSPResponse generate_success(const variant<string, int> &id,
                                         const lsp::Result &result) {
  nlohmann::ordered_json msg;
  msg["jsonrpc"] = "2.0";

  std::holds_alternative<string>(id) ? msg["id"] = std::get<string>(id)
                                     : msg["id"] = std::get<int>(id);

  msg["result"] = result;
  int contentLength = static_cast<int>(msg.dump().size());
  return {contentLength, msg};
}

void send_response(const variant<string, int> &id, const lsp::Result &result) {
  auto response = generate_success(id, result);
  cout << "Content-Length: " << response.contentLength << "\r\n";
  cout << response.result << endl;
}

static lsp::LSPErrorResponse
generate_error(const optional<variant<string, int>> &id,
               const lsp::Error &error) {

  nlohmann::ordered_json msg;
  msg["jsonrpc"] = "2.0";
  if (id.has_value()) {
    std::holds_alternative<string>(*id) ? msg["id"] = std::get<string>(*id)
                                        : msg["id"] = std::get<int>(*id);
  }
  msg["error"] = error;

  int contentLength = static_cast<int>(msg.dump().size());
  return {contentLength, error};
}

void send_error_response(const variant<string, int> &id, lsp::ErrorCode code,
                         const optional<nlohmann::json> &data,
                         const string &message) {
  lsp::Error error(code, message.empty() ? getErrorMessage(code) : message,
                   data);
  auto response = generate_error(id, error);
  cout << "Content-Length: " << response.contentLength << "\r\n";
  cout << response.error << endl;
}

} // namespace lsp
