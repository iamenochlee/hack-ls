#ifndef UTILS_HPP
#define UTILS_HPP

#include "../protocol/responses.hpp"

namespace lsp {
// Success
void send_response(const std::variant<std::string, int> &id,
                   const lsp::Result &result);

void send_error_response(
    const std::variant<std::string, int> &id, lsp::ErrorCode code,
    const std::optional<nlohmann::json> &data = std::nullopt,
    const std::string &message = "");

void log_error_response(
    lsp::ErrorCode code,
    const std::optional<nlohmann::json> &data = std::nullopt,
    const std::string &message = "");

} // namespace lsp
#endif