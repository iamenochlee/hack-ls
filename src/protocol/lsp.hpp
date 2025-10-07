#ifndef PROTOCOL_LSP_HPP
#define PROTOCOL_LSP_HPP

#include "messages.hpp"
#include "params.hpp"
#include "responses.hpp"
#include "types.hpp"

// Main protocol namespace
namespace protocol {
// Import all LSP types into protocol namespace for easier access
using namespace lsp;

// Protocol version information
constexpr const char *LSP_VERSION = "3.17.0";
constexpr const char *SERVER_NAME = "hack-language-server";
constexpr const char *SERVER_VERSION = "0.1.0";
} // namespace protocol

#endif // PROTOCOL_LSP_HPP
