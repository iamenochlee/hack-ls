#ifndef MESSAGEHANDLER_HPP
#define MESSAGEHANDLER_HPP

#include "protocol/lsp.hpp"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <variant>

class MessageHandler {
private:
  nlohmann::json message;

  void validate_message();
  void is_initialized();
  lsp::Response
  generate_response(const std::optional<std::variant<std::string, int>> &id,
                    const lsp::Result &result);

  int process_request();
  int process_notification();

  lsp::InitializeResult initialize();
  lsp::DidOpenResult didOpen(lsp::NotificationMessage notif);
  lsp::DidChangeResult didChange(lsp::NotificationMessage notif);

public:
  lsp::MessageType type;

  MessageHandler(nlohmann::json _message);
  int run();
};

#endif // MESSAGEHANDLER_HPP
