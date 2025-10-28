#include "io.hpp"

#include <istream>
#include <map>
#include <sstream>
#include <string>

using namespace std;

namespace lsp {

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

} // namespace lsp


