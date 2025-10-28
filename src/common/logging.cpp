#include "logging.hpp"

#include <iostream>
#include <nlohmann/json.hpp>

using namespace std;

namespace lsp {

void log_error_response(lsp::ErrorCode code,
                        const optional<nlohmann::json> &data,
                        const string &message) {
  lsp::Error error(code, message.empty() ? getErrorMessage(code) : message,
                   data);

  cerr << "Error: " + error.message + " ";
  if (error.data.has_value())
    cerr << "Data: " << error.data.value() << endl;
  else
    cout << endl;
}

} // namespace lsp
