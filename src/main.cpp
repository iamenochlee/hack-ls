#include "core/handlers/MessageHandler.hpp"
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

using namespace std;
using nlohmann::json;

void log_error_response(lsp::ErrorCode code, const std::string &message = "",
                        const optional<nlohmann::json> &data = nullopt) {

  lsp::Error error(code, message.empty() ? getErrorMessage(code) : message,
                   data);

  cerr << "Error: " + error.message + " ";
  if (error.data.has_value())
    cerr << "Data: " << error.data.value() << endl;
  else
    cerr << endl;
}

int parse_header(const string &line, map<string, int> &headers) {
  istringstream ls(line);
  string field;
  int val;
  getline(ls, field, ':');
  if (!(ls >> ws >> val)) {
    return 1;
  }
  headers[field] = val;
  return 0;
}

int main() {

  MessageHandler server = MessageHandler();
  while (true) {
    map<string, int> headers;
    //  read headers from stdin
    string line;
    while (getline(cin, line)) {
      if (line.empty() || line == "\r")
        break;

      parse_header(line, headers);
    }

    if (headers.size() == 0) {
      continue;
    }

    // read content length
    int content_length = headers["Content-Length"];
    string content(content_length, '\0');
    cin.read(content.data(), content_length);

    json message;
    try {
      message = json::parse(content);
    } catch (const std::exception &e) {
      log_error_response(lsp::ErrorCode::PARSE_ERROR, e.what());
      continue;
    }

    server.set_message(message);
    server.run();
  }
  return 0;
}
