#include "json_compact_builder.hh"
#include <base64/base64.hh>
#include <cassert>
#include <stringutils/escape.hh>

using namespace std;
using namespace kdv::composite;
using namespace kdv::json;

void json_compact_builder::visit(const scalar& val)
{
  if(toplevel())
  {
    // special case where the empty document is represented by one empty scalar
    assert(val.value().empty());
    d_stream << "{}";
  }
  else
  {
    if(needscomma())
      d_stream << ',';

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

void json_compact_builder::visit(const sequence& seq)
{
  if(needscomma())
    d_stream << ',';

  d_stream << "[";
  needscomma(false);
}

void json_compact_builder::visit(const mapping& map)
{
  if(needscomma())
    d_stream << ',';

  d_stream << "{";
  needscomma(false);
}

void json_compact_builder::sentinel(const sequence& seq)
{
  d_stream << ']';
  needscomma(true);
}

void json_compact_builder::sentinel(const mapping& map)
{
  d_stream << '}';
  needscomma(true);
}

void json_compact_builder::visit_key(const string& key)
{
  assert(!toplevel());
  if(needscomma())
    d_stream << ',';
  d_stream << '"' << key << "\":";
  needscomma(false);
}
