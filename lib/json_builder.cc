#include "json_builder.hh"
#include "escape.hh"
#include <ostream>
#include <limits>

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
  d_out.precision(std::numeric_limits<double>::max_digits10);
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
  ++d_indent;
  newline();

  for (auto && item : v)
    item.visit(*this);

  --d_indent;
  newline();
  d_out << ']';

  if (toplevel())
    newline();
  d_needscomma = true;
}

void json_builder::operator()(const composite::mapping &v)
{
  comma();

  d_out << '{';
  d_needscomma = false;
  ++d_indent;
  newline();

  for (auto && kv : v)
  {
    comma();
    d_out << escape(kv.first);
    element();
    d_needscomma = false;
    kv.second.visit(*this);
  }

  --d_indent;
  newline();
  d_out << '}';

  if (toplevel())
    newline();
  d_needscomma = true;
}

void json_builder::comma()
{
  if (d_needscomma)
  {
    d_out << ",";
    newline();
  }
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

bool json_builder::toplevel() const
{
  return d_indent == 0;
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
