#include "MessageHandler.hpp"

#include "common/GlobalState.hpp"
#include "core/structures/TextDocument.hpp"
#include "protocol/responses.hpp"

#include <iostream>
#include <optional>
#include <string>
#include <variant>

using namespace std;
using nlohmann::json;

using GlobalState::documents;
using GlobalState::initialized;

void print_response(lsp::Response response) {
  cout << "Content-Length: " << response.contentLength << "\r\n\r\n";
  cout << response.result << endl;
}

void MessageHandler::validate_message() {
  try {
    message.at("jsonrpc");
    message.at("method");
  } catch (const std::exception &e) {
    throw std::runtime_error("Invalid message: " + std::string(e.what()));
  }
}

void MessageHandler::is_initialized() {
  if (!initialized && !(message.at("method").get<string>() == "initialize")) {
    throw std::runtime_error("Not initialized");
  }
}

MessageHandler::MessageHandler(json _message) : message(_message) {
  type = lsp::NOTIFICATION;
  if (message.contains("id"))
    type = lsp::REQUEST;
}

int MessageHandler::run() {
  try {
    validate_message();
    is_initialized();
    if (type == lsp::REQUEST)
      return process_request();
    else
      return process_notification();
  } catch (const std::exception &e) {
    cout << "Error: " << e.what() << endl;
    return 1;
  }
}

int MessageHandler::process_request() {
  try {
    lsp::RequestMessage req = message;

    if (req.method == "initialize") {
      lsp::InitializeResult result = initialize();
      lsp::Response response =
          generate_response(std::make_optional(req.id), result);
      print_response(response);
      return 0;
    }

    throw std::runtime_error("Unknown method: " + req.method);
    return 1;
  } catch (const std::exception &e) {
    cout << "Error: " << e.what() << endl;
    return 1;
  }
}

int MessageHandler::process_notification() {
  try {
    lsp::NotificationMessage notif = message;

    if (notif.method == "textDocument/didOpen") {
      lsp::DidOpenResult result = didOpen(notif);
      lsp::Response response = generate_response(std::nullopt, result);
      print_response(response);
      return 0;
    }

    if (notif.method == "textDocument/didChange") {
      lsp::DidChangeResult result = didChange(notif);
      lsp::Response response = generate_response(std::nullopt, result);
      print_response(response);
      return 0;
    }

    throw std::runtime_error("Unknown method: " + notif.method);
    return 1;
  } catch (const std::exception &e) {
    cout << "Error: " << e.what() << endl;
    return 1;
  }
}

lsp::InitializeResult MessageHandler::initialize() {

  lsp::ServerCapabilities caps;
  caps.hoverProvider = false;
  caps.definitionProvider = true;

  lsp::InitializeResult result;
  result.serverCapabilities = caps;

  initialized = true;

  return result;
}

lsp::DidOpenResult MessageHandler::didOpen(lsp::NotificationMessage notif) {
  json textDocument = notif.params.value().at("textDocument");
  string uri = textDocument.at("uri").get<std::string>();
  int version = textDocument.at("version").get<int>();
  string text = textDocument.at("text").get<std::string>();

  documents.emplace(uri, TextDocument{uri, version, text});

  return lsp::DidOpenResult{uri};
}

lsp::DidChangeResult MessageHandler::didChange(lsp::NotificationMessage notif) {

  json j_didChangeParams = notif.params.value();
  lsp::DidChangeParams didChangeParams = j_didChangeParams;

  TextDocument &textDocument = documents.at(didChangeParams.textDocument.uri);

  textDocument.applyChanges(didChangeParams.contentChanges);
  //  // after all changes have been applied
  textDocument.version = didChangeParams.textDocument.version;

  cout << "Document changed: " << textDocument.text << endl;
  return lsp::DidChangeResult{didChangeParams.textDocument.uri};
}

lsp::Response
MessageHandler::generate_response(const optional<variant<string, int>> &id,
                                  const lsp::Result &result) {

  // deal withalphabetical order
  json response;
  response["jsonrpc"] = "2.0";
  if (id.has_value()) {
    if (holds_alternative<string>(id.value())) {
      response["id"] = get<string>(id.value());
    } else {
      response["id"] = get<int>(id.value());
    }
  }

  response["result"] = result;

  int contentLength = response.dump().size();

  return {contentLength, response};
}
