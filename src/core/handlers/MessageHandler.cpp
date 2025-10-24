#include "MessageHandler.hpp"

#include "common/GlobalState.hpp"
#include "core/structures/TextDocument.hpp"
#include "protocol/lsp.hpp"
#include "protocol/params.hpp"
#include "protocol/responses.hpp"

#include <iostream>
#include <optional>
#include <string>
#include <variant>

using namespace std;

using GlobalState::documents;
using GlobalState::initialized;
void log_error_response(lsp::ErrorCode code, const string &message = "",
                        const optional<nlohmann::json> &data = nullopt);

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

int MessageHandler::validate_message(nlohmann::json message) {
  try {
    message.at("jsonrpc");
    message.at("method");
    return 0;
  } catch (const std::exception &e) {
    if (message.contains("id")) {
      variant<string, int> id;
      auto &_id = message.at("id");
      _id.is_string() ? id = _id.get<string>() : id = _id.get<int>();

      send_error_response(id, lsp::ErrorCode::PARSE_ERROR, e.what());
      return 1;
    }

    log_error_response(lsp::ErrorCode::INVALID_MESSAGE, "Invalid Message",
                       e.what());
    return 1;
  }
}

int MessageHandler::set_message(nlohmann::json message) {
  if (validate_message(message)) {
    // it fails
    return 1;
  }
  LSPMessage = message;

  type = lsp::NOTIFICATION;
  if (message.contains("id"))
    type = lsp::REQUEST;

  return 0;
}

int MessageHandler::run() {
  if (type == lsp::REQUEST)
    return process_request();

  return process_notification();
}

int MessageHandler::process_request() {
  try {
    lsp::RequestMessage req(LSPMessage);
    if (!initialized && !(req.method == "initialize")) {
      send_error_response(req.id, lsp::ErrorCode::SERVER_NOT_INITIALIZED);
      return 1;
    }

    if (req.method == "initialize") {
      if (initialized) {
        send_error_response(req.id, lsp::ErrorCode::REQUEST_CANCELLED, "",
                            "Server is already initialzed");
        return 1;
      }
      lsp::InitializeResult result = initialize(req);

      send_response(req.id, result);
      return 0;
    }

    send_error_response(req.id, lsp::ErrorCode::METHOD_NOT_FOUND);
    return 1;
  } catch (const std::exception &e) {
    variant<string, int> id;
    auto &_id = LSPMessage.at("id");
    _id.is_string() ? id = _id.get<string>() : id = _id.get<int>();

    send_error_response(id, lsp::ErrorCode::PARSE_ERROR, e.what());
    return 1;
  }
}

int MessageHandler::process_notification() {
  if (!initialized) {
    log_error_response(lsp::ErrorCode::SERVER_NOT_INITIALIZED);
    return 1;
  }

  try {
    lsp::NotificationMessage notif(LSPMessage);

    if (notif.method == "textDocument/didOpen")
      return didOpen(notif);

    if (notif.method == "textDocument/didChange")
      return didChange(notif);

    log_error_response(lsp::ErrorCode::METHOD_NOT_FOUND);
    return 1;
  } catch (const lsp::Error &e) {
    log_error_response(e.code, e.message, e.data);
    return 1;
  } catch (const std::exception &e) {
    log_error_response(lsp::ErrorCode::INVALID_PARAMS, "Invalid Param",
                       e.what());
    return 1;
  }
}

lsp::InitializeResult MessageHandler::initialize(lsp::RequestMessage req) {

  auto _params = req.params;
  lsp::InitializeParams params(_params);

  lsp::ServerInfo serverInfo{protocol::SERVER_NAME, protocol::SERVER_VERSION};
  lsp::ServerCapabilities caps{protocol::capabilities::POSITION_ENCODING,
                               protocol::capabilities::SUPPORTS_HOVER,
                               protocol::capabilities::SUPPORTS_DEFINITION};

  lsp::InitializeResult result{serverInfo, caps};

  initialized = true;
  return result;
}

int MessageHandler::didOpen(lsp::NotificationMessage notif) {
  auto _params = notif.params.value();
  lsp::DidOpenParams didOpenParams(_params);

  documents.emplace(didOpenParams.textDocument.uri,
                    TextDocument{didOpenParams.textDocument.uri,
                                 didOpenParams.textDocument.version,
                                 didOpenParams.textDocument.text});

  return 0;
}

int MessageHandler::didChange(lsp::NotificationMessage notif) {
  auto _params = notif.params.value();
  lsp::DidChangeParams didChangeParams(_params);

  auto it = documents.find(didChangeParams.textDocument.uri);

  if (it == documents.end()) {
    lsp::Error error(lsp::ErrorCode::INTERNAL_ERROR, string("URI not found"),
                     nullopt);
    throw error;
  }

  TextDocument &textDocument = it->second;
  textDocument.applyChanges(didChangeParams.contentChanges);

  textDocument.version = didChangeParams.textDocument.version;

  // cout << "Document changed: " << textDocument.text << endl;
  return 0;
}
