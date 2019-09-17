#if 0
#include "json_builder.hh"
#include <kb64/base64.hh>
#include <cassert>

namespace kjson {

using namespace std;

template <typename T>
void json_builder::scalar(const T &value)
{
  if(needscomma())
  {
    d_stream << ',';
    newline();
  }
  d_stream << value;
  needscomma(true);
}


void json_builder::newline()
{
  d_stream << '\n';
  for(size_t i = 0; i < d_depth; ++i)
    d_stream << "  ";
}

void json_builder::visit(composite::deduce<void>::type)
{
  scalar("null");
}

void json_builder::visit(composite::deduce<bool>::type value)
{
  scalar(value ? "true" : "false");
}

void json_builder::visit(composite::deduce<int>::type value)
{
  scalar(value);
}

void json_builder::visit(composite::deduce<double>::type value)
{
  scalar(value);
}

void json_builder::visit(string_view value)
{
  scalar(value);
}

void json_builder::start_sequence()
{
  if(needscomma())
  {
    d_stream << ',';
    newline();
  }

  d_stream << "[";
  incdepth();
  newline();
  needscomma(false);
}

void json_builder::start_mapping()
{
  if(needscomma())
  {
    d_stream << ',';
    newline();
  }

  d_stream << "{";
  incdepth();
  newline();
  needscomma(false);
}

void json_builder::sentinel()
{
  decdepth();
  newline();
  d_stream << ']';

  if(toplevel())
    newline();

  needscomma(true);
}

void json_builder::sentinel(const mapping& map)
{
  decdepth();
  newline();
  d_stream << '}';

  if(toplevel())
    newline();

  needscomma(true);
}

void json_builder::visit_key(const string& key)
{
  assert(!toplevel());
  if(needscomma())
  {
    d_stream << ',';
    newline();
  }
  d_stream << '"' << key << '"' << ": ";
  needscomma(false);
}

}
#endif
