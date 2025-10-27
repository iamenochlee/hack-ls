#include "../protocol/responses.hpp"

#include <iostream>
#include <nlohmann/json.hpp>

using namespace std;

namespace lsp {

// Success
lsp::LSPResponse generate_response(const variant<string, int> &id,
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

  auto response = generate_response(id, result);
  cout << "Content-Length: " << response.contentLength << "\r\n\r\n";
  cout << response.result << endl;
}

// Error
lsp::LSPErrorResponse
generate_response(const optional<variant<string, int>> &id,
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
                         const optional<nlohmann::json> &data = nullopt,
                         const string &message = "") {

  lsp::Error error(code, message.empty() ? getErrorMessage(code) : message,
                   data);

  auto response = generate_response(id, error);

  cout << "Content-Length: " << response.contentLength << "\r\n\r\n";
  cout << response.error << endl;
}

void log_error_response(lsp::ErrorCode code,
                        const optional<nlohmann::json> &data = nullopt,
                        const string &message = "") {

  lsp::Error error(code, message.empty() ? getErrorMessage(code) : message,
                   data);

  cerr << "Error: " + error.message + " ";
  if (error.data.has_value())
    cerr << "Data: " << error.data.value() << endl;
  else
    cout << endl;
}

} // namespace lsp