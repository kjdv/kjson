#include "json_builder.hh"
#include <base64/base64.hh>
#include <cassert>
#include <stringutils/escape.hh>

using namespace std;
using namespace kdv::composite;
using namespace kdv::json;

void json_builder::newline()
{
  d_stream << '\n';
  for(size_t i = 0; i < d_depth; ++i)
    d_stream << "  ";
}

void json_builder::visit(const scalar& val)
{
  if(toplevel())
  {
    // special case where the empty document is represented by one empty scalar
    assert(val.value().empty());
    d_stream << "{}\n";
  }
  else
  {
    if(needscomma())
    {
      d_stream << ',';
      newline();
    }

    if(val.typehint() == scalar::e_int ||
       val.typehint() == scalar::e_float ||
       val.typehint() == scalar::e_bool ||
       val.typehint() == scalar::e_null)
      d_stream << val.value();
    else if(val.typehint() == scalar::e_binary)
      d_stream << '"' << kdv::base64::encode(val.as_binary()) << '"'; // todo: many copies
    else
      d_stream << kdv::stringutils::escape(val.value());
    needscomma(true);
  }
}

void json_builder::visit(const sequence& seq)
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

void json_builder::visit(const mapping& map)
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

void json_builder::sentinel(const sequence& seq)
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
