#ifndef MESSAGEHANDLER_HPP
#define MESSAGEHANDLER_HPP

#include "../../main.hpp"
#include "../../protocol/lsp.hpp"

class MessageHandler {
private:
  json message;

  void validate_message();
  void is_initialized();
  lsp::Response generate_response(const optional<variant<string, int>> &id,
                                  const lsp::Result &result);

  int process_request();
  int process_notification();

  lsp::InitializeResult initialize();
  lsp::DidOpenResult didOpen(lsp::NotificationMessage notif);
  lsp::DidChangeResult didChange(lsp::NotificationMessage notif);

public:
  lsp::MessageType type;

  MessageHandler(json _message);
  int run();
};

#endif // MESSAGEHANDLER_HPP
