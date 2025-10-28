#ifndef LSP_CORE_TRANSPORT_IO_HPP
#define LSP_CORE_TRANSPORT_IO_HPP

#include <map>
#include <string>

namespace lsp {

int parse_header(const std::string &line, std::map<std::string, int> &headers);

} // namespace lsp

#endif // LSP_CORE_TRANSPORT_IO_HPP


