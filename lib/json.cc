#include "json.hh"
#include "json_builder.hh"
#include "parser.hh"
#include <composite/builder.hh>
#include <sstream>
#include <string>

namespace kjson {

using namespace std;

result load(istream& input)
{
  to_composite v;
  return load(input, v)
      .map([&v](auto) { return v.collect(); });
}

result load(string_view input)
{
  istringstream str{string{input}};
  return load(str);
}

maybe_error load(istream& input, visitor& v)
{
  return parse(input, v)
      .map([](auto&) { return 1; });
}

maybe_error load(string_view input, visitor& v)
{
  istringstream str{string{input}};
  return load(str, v);
}

void dump(const document& data, ostream& out, bool compact)
{
  json_builder jb(out, compact);
  data.visit(jb);
}

} // namespace kjson
