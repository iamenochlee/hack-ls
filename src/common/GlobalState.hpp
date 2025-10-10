#ifndef GLOBALSTATE_HPP
#define GLOBALSTATE_HPP

#include "core/structures/TextDocument.hpp"

#include <map>
#include <string>

namespace GlobalState {
inline bool initialized = false;
inline std::map<std::string, TextDocument> documents;
} // namespace GlobalState

#endif // GLOBALSTATE_HPP
