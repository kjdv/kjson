#include <json/json.hh>
#include "json_builder.hh"
#include "json_compact_builder.hh"
#include "parser.hh"
#include <sstream>

using namespace std;
using namespace kdv::composite;
using namespace kdv::json;

composite_ptr kdv::json::load(istream& input)
{
  return parse(input);
}

composite_ptr kdv::json::load(string const& input)
{
  istringstream str(input);
  return load(str);
}

void kdv::json::dump(const kdv::composite::composite& data, ostream& out, bool compact)
{
  if(compact)
  {
    json_compact_builder jb(out);
    data.accept(jb);
  }
  else
  {
    json_builder jb(out);
    data.accept(jb);
  }
}
