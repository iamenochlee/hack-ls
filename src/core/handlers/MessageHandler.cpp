#include "MessageHandler.hpp"

#include "common/GlobalState.hpp"
#include "common/logging.hpp"
#include "core/structures/TextDocument.hpp"
#include "core/transport/wire.hpp"
#include "protocol/lsp.hpp"
#include "protocol/params.hpp"
#include "protocol/responses.hpp"

#include <optional>
#include <string>
#include <variant>

using namespace std;

int MessageHandler::validateMessage(nlohmann::json message) {
  try {
    message.at("jsonrpc");
    message.at("method");

    return 1;

  } catch (const std::exception &e) {
    if (message.contains("id")) {
      variant<string, int> id;
      auto &_id = message.at("id");

      _id.is_string() ? id = _id.get<string>() : id = _id.get<int>();
      lsp::send_error_response(id, lsp::ErrorCode::PARSE_ERROR, e.what());

      return 0;
    }

    lsp::log_error_response(lsp::ErrorCode::INVALID_MESSAGE, e.what());
    return 0;
  }
}

int MessageHandler::setMessage(nlohmann::json message) {
  if (!validateMessage(message))
    return 0;

  LSPMessage = message;
  type = lsp::NOTIFICATION;
  if (message.contains("id"))
    type = lsp::REQUEST;

  return 1;
}

int MessageHandler::run() {
  if (type == lsp::REQUEST)
    return processRequest();

  return handleNotification();
}

int MessageHandler::processRequest() {
  try {
    lsp::RequestMessage req(LSPMessage);

    if (!GlobalState::initialized && !(req.method == "initialize")) {
      lsp::send_error_response(req.id, lsp::ErrorCode::SERVER_NOT_INITIALIZED);
      return 1;
    }

    if (req.method == "initialize") {
      if (GlobalState::initialized) {
        lsp::send_error_response(req.id, lsp::ErrorCode::REQUEST_CANCELLED,
                                 "Server is already initialzed");
        return 1;
      }

      lsp::InitializeResult result = initialize(req);
      lsp::send_response(req.id, result);

      return 0;
    }

    lsp::send_error_response(req.id, lsp::ErrorCode::METHOD_NOT_FOUND);
    return 1;

  } catch (const std::exception &e) {
    variant<string, int> id;
    auto &_id = LSPMessage.at("id");

    _id.is_string() ? id = _id.get<string>() : id = _id.get<int>();
    lsp::send_error_response(id, lsp::ErrorCode::PARSE_ERROR, e.what());

    return 1;
  }
}

int MessageHandler::handleNotification() {
  if (!GlobalState::initialized) {
    lsp::log_error_response(lsp::ErrorCode::SERVER_NOT_INITIALIZED);
    return 1;
  }

  try {
    lsp::NotificationMessage notif(LSPMessage);

    if (notif.method == "textDocument/didOpen")
      return didOpen(notif);

    if (notif.method == "textDocument/didChange")
      return didChange(notif);

    lsp::log_error_response(lsp::ErrorCode::METHOD_NOT_FOUND);
    return 1;

  } catch (const lsp::Error &e) {
    lsp::log_error_response(e.code, e.data, e.message);
    return 1;

  } catch (const std::exception &e) {
    lsp::log_error_response(lsp::ErrorCode::INVALID_PARAMS, e.what());
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

  GlobalState::initialized = true;
  return result;
}

int MessageHandler::didOpen(lsp::NotificationMessage notif) {
  auto _params = notif.params.value();
  lsp::DidOpenParams didOpenParams(_params);

  GlobalState::documents.emplace(
      didOpenParams.textDocument.uri,
      TextDocument{didOpenParams.textDocument.uri,
                   didOpenParams.textDocument.version,
                   didOpenParams.textDocument.text});

  return 0;
}

int MessageHandler::didChange(lsp::NotificationMessage notif) {
  auto _params = notif.params.value();
  lsp::DidChangeParams didChangeParams(_params);

  auto it = GlobalState::documents.find(didChangeParams.textDocument.uri);

  if (it == GlobalState::documents.end()) {
    lsp::Error error(lsp::ErrorCode::INTERNAL_ERROR, "URI not found", nullopt);
    throw error;
  }

  TextDocument &textDocument = it->second;
  textDocument.applyChanges(didChangeParams.contentChanges);

  textDocument.version = didChangeParams.textDocument.version;

  // cout << "Document changed: " << textDocument.text << endl;
  return 0;
}
