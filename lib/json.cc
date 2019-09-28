#include "json.hh"
#include "json_builder.hh"
#include "parser.hh"
#include <sstream>
#include <string>

namespace kjson {

using namespace std;

result load(istream& input)
{
  return parse(input);
}

result load(string_view input)
{
  istringstream str{string{input}};
  return load(str);
}

void dump(const document& data, ostream& out, bool compact)
{
  json_builder jb(out, compact);
  data.visit(jb);
}

}
