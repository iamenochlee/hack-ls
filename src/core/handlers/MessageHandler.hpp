#ifndef MESSAGEHANDLER_HPP
#define MESSAGEHANDLER_HPP

#include "protocol/messages.hpp"
#include "protocol/responses.hpp"
#include <nlohmann/json.hpp>

class MessageHandler {
private:
  nlohmann::json LSPMessage;

  int validate_message(nlohmann::json message);

  int process_request();
  int process_notification();

  lsp::InitializeResult initialize(lsp::RequestMessage req);
  int didOpen(lsp::NotificationMessage notif);
  int didChange(lsp::NotificationMessage notif);

public:
  lsp::MessageType type;

  int set_message(nlohmann::json message);
  int run();
};

#endif // MESSAGEHANDLER_HPP
