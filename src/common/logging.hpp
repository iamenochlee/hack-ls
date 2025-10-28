#ifndef LSP_COMMON_LOGGING_HPP
#define LSP_COMMON_LOGGING_HPP

#include <optional>
#include <string>

#include "../protocol/responses.hpp"

namespace lsp {

void log_error_response(
    lsp::ErrorCode code,
    const std::optional<nlohmann::json> &data = std::nullopt,
    const std::string &message = "");

} // namespace lsp

#endif // LSP_COMMON_LOGGING_HPP
