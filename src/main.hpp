#ifndef MAIN_HPP
#define MAIN_HPP

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

using namespace std;
using nlohmann::json;

struct Message {
  string jsonrpc;
};

struct ResponseMessage : public Message {
  optional<json> id;
  optional<json> result;
};

#endif // MAIN_HPP
