#include "json_builder.hh"
#include "escape.hh"
#include <ostream>

namespace kjson {

template<typename T>
void json_builder::scalar(const T &v)
{
  comma();

  d_out << v;
  d_needscomma = true;
}

json_builder::json_builder(std::ostream &out, bool compact)
  : d_out(out)
  , d_compact(compact)
{}

void json_builder::operator()(composite::float_t v)
{
  scalar(v);
}

void json_builder::operator()(std::string_view v)
{
  scalar(escape(v));
}

void json_builder::operator()(const composite::sequence &v)
{
  comma();

  d_out << '[';
  d_needscomma = false;

  for (auto && item : v)
    item.visit(*this);
  d_out << ']';
  d_needscomma = false;
}

void json_builder::operator()(const composite::mapping &v)
{
  comma();

  d_out << '{';
  d_needscomma = false;
  ++d_indent;

  for (auto && kv : v)
  {
    newline();

    scalar(escape(kv.first));
    element();
    d_needscomma = false;
    kv.second.visit(*this);
  }

  --d_indent;
  newline();
  d_out << '}';
  newline();
  d_needscomma = false;
}

void json_builder::comma()
{
  if (d_needscomma)
    d_out << (d_compact ? "," : ", ");
}

void json_builder::newline()
{
  if (!d_compact)
  {
    d_out << '\n';
    for (unsigned i = 0; i < d_indent; ++i)
      d_out << "  ";
  }
}

void json_builder::element()
{
  d_out << (d_compact ? ":" : ": ");
}

void json_builder::operator()(composite::int_t v)
{
  scalar(v);
}

void json_builder::operator()(composite::bool_t v)
{
  scalar(v ? "true" : "false");
}

void json_builder::operator()(composite::none)
{
  scalar("null");
}


}

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
