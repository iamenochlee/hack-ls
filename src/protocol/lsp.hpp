#ifndef PROTOCOL_LSP_HPP
#define PROTOCOL_LSP_HPP

#include "messages.hpp"
#include "params.hpp"
#include "responses.hpp"
#include "types.hpp"

// Main protocol namespace
namespace protocol {

using namespace lsp;

// Protocol version information
constexpr const char *LSP_VERSION = "3.17.0";
constexpr const char *SERVER_NAME = "hack-language-server";
constexpr const char *SERVER_VERSION = "0.1.0";

namespace capabilities {

constexpr const char *POSITION_ENCODING = "utf-8"; // only utf-8 for now
constexpr int TEXT_DOCUMENT_SYNC = 2;
constexpr bool SUPPORTS_DIAGNOSTICS = true;
constexpr bool SUPPORTS_HOVER = true;

} // namespace capabilities

} // namespace protocol

#endif // PROTOCOL_LSP_HPP
