#include "json.hh"
#include "json_builder.hh"
#include "parser.hh"
#include <sstream>

namespace kjson {

using namespace std;

result load(istream& input)
{
  return parse(input);
}

result load(string const& input)
{
  istringstream str(input);
  return load(str);
}

void dump(const document& data, ostream& out, bool compact)
{
  json_builder jb(out);
  data.visit(jb);
}

}
