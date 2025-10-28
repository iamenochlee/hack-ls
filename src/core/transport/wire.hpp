#ifndef LSP_CORE_TRANSPORT_WIRE_HPP
#define LSP_CORE_TRANSPORT_WIRE_HPP

#include <optional>
#include <string>
#include <variant>

#include "../../protocol/responses.hpp"

namespace lsp {

void send_response(const std::variant<std::string, int> &id,
                   const lsp::Result &result);

void send_error_response(
    const std::variant<std::string, int> &id, lsp::ErrorCode code,
    const std::optional<nlohmann::json> &data = std::nullopt,
    const std::string &message = "");

} // namespace lsp

#endif // LSP_CORE_TRANSPORT_WIRE_HPP


