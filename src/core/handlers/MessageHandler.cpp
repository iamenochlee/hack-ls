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

// Success
lsp::LSPResponse generate_response(std::variant<std::string, int> &id,
                                   const lsp::Result &result) {
  nlohmann::ordered_json msg;
  msg["jsonrpc"] = "2.0";

  if (std::holds_alternative<std::string>(id))
    msg["id"] = std::get<std::string>(id);
  else
    msg["id"] = std::get<int>(id);

  msg["result"] = result;
  int contentLength = static_cast<int>(msg.dump().size());
  return {contentLength, msg};
}

void send_response(std::variant<std::string, int> &id, lsp::Result result) {

  auto response = generate_response(id, result);
  cout << "Content-Length: " << response.contentLength << "\r\n\r\n";
  cout << response.result << endl;
}

// Error
lsp::LSPErrorResponse
generate_response(const std::optional<std::variant<std::string, int>> &id,
                  const lsp::Error &error) {

  nlohmann::ordered_json msg;
  msg["jsonrpc"] = "2.0";
  if (id.has_value()) {
    if (std::holds_alternative<std::string>(*id))
      msg["id"] = std::get<std::string>(*id);
    else
      msg["id"] = std::get<int>(*id);
  }
  msg["error"] = error;

  int contentLength = static_cast<int>(msg.dump().size());

  return {contentLength, error};
}

void send_error_response(
    const std::optional<std::variant<std::string, int>> &id,
    lsp::ErrorCode code, const std::string &message = "",
    const optional<nlohmann::json> &data = nullopt) {

  lsp::Error error;
  error.code = code;
  error.message = message.empty() ? getErrorMessage(code) : message;
  error.data = data;

  if (id.has_value()) {
    auto response = generate_response(id.value(), error);

    cout << "Content-Length: " << response.contentLength << "\r\n\r\n";
    cout << response.error << endl;
  }

  // cout to sderr or log notif errors
}

int MessageHandler::validate_message(nlohmann::json message) {
  try {
    message.at("jsonrpc");
    message.at("method");
    return 0;
  } catch (const std::exception &e) {
    send_error_response(std::make_optional(std::variant<std::string, int>{6}),
                        lsp::ErrorCode::PARSE_ERROR, "Invalid Message",
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
    lsp::RequestMessage req = LSPMessage;
    if (!initialized && !(req.method == "initialize")) {
      send_error_response(req.id, lsp::ErrorCode::SERVER_NOT_INITIALIZED);
      return 1;
    }

    if (req.method == "initialize") {
      if (initialized) {
        send_error_response(std::make_optional(req.id),
                            lsp::ErrorCode::INVALID_REQUEST,
                            "Server is already initialzed");
        return 1;
      }
      lsp::InitializeResult result = initialize(req);

      send_response(req.id, result);
      return 0;
    }

    send_error_response(std::make_optional(req.id),
                        lsp::ErrorCode::METHOD_NOT_FOUND);
    return 1;
  } catch (std::exception &e) {
    send_error_response(std::nullopt, lsp::ErrorCode::PARSE_ERROR,
                        "Invalid Param", e.what());
    return 1;
  }
}

int MessageHandler::process_notification() {
  try {
    if (!initialized) {
      send_error_response(std::nullopt, lsp::ErrorCode::SERVER_NOT_INITIALIZED);
      return 1;
    }

    lsp::NotificationMessage notif = LSPMessage;

    if (notif.method == "textDocument/didOpen")
      return didOpen(notif);

    if (notif.method == "textDocument/didChange")
      return didChange(notif);

    send_error_response(std::nullopt, lsp::ErrorCode::METHOD_NOT_FOUND);
    return 1;
  } catch (std::exception &e) {
    send_error_response(std::nullopt, lsp::ErrorCode::PARSE_ERROR,
                        "Invalid Param", e.what());
    return 1;
  }
}

lsp::InitializeResult MessageHandler::initialize(lsp::RequestMessage req) {
  try {
    auto _params = req.params;
    lsp::InitializeParams params = _params;

    lsp::ServerInfo serverInfo{protocol::SERVER_NAME, protocol::SERVER_VERSION};
    lsp::ServerCapabilities caps{protocol::capabilities::POSITION_ENCODING,
                                 protocol::capabilities::SUPPORTS_HOVER,
                                 protocol::capabilities::SUPPORTS_DEFINITION};

    lsp::InitializeResult result{serverInfo, caps};

    initialized = true;
    return result;
  } catch (const std::exception &e) {
    send_error_response(std::make_optional(req.id),
                        lsp::ErrorCode::INVALID_PARAMS, e.what());
  }
}

int MessageHandler::didOpen(lsp::NotificationMessage notif) {
  auto _didOpenParams = notif.params.value();
  lsp::DidOpenParams didOpenParams = _didOpenParams;

  documents.emplace(didOpenParams.textDocument.uri,
                    TextDocument{didOpenParams.textDocument.uri,
                                 didOpenParams.textDocument.version,
                                 didOpenParams.textDocument.text});

  return 0;
}

int MessageHandler::didChange(lsp::NotificationMessage notif) {
  auto _didChangeParams = notif.params.value();
  lsp::DidChangeParams didChangeParams = _didChangeParams;

  auto it = documents.find(didChangeParams.textDocument.uri);
  if (it == documents.end())
    throw std::runtime_error("URI is not initialized");

  TextDocument &textDocument = it->second;
  textDocument.applyChanges(didChangeParams.contentChanges);

  textDocument.version = didChangeParams.textDocument.version;

  // cout << "Document changed: " << textDocument.text << endl;
  return 0;
}
