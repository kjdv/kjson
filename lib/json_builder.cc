#include "json_builder.hh"
#include "escape.hh"
#include <limits>
#include <ostream>

namespace kjson {

namespace {

void to_exc(const builder::result& r)
{
  r.map_err([](auto&& e) {
    throw std::runtime_error(e.msg);
    return e;
  });
}

} // namespace

json_builder::json_builder(std::ostream& out, bool compact)
  : d_base(out, compact)
{
}

void json_builder::operator()(composite::none)
{
  if(!d_keystack.empty())
  {
    auto key = d_keystack.top();
    d_keystack.pop();
    to_exc(d_base.with_none(key));
  }
  else
  {
    to_exc(d_base.with_none());
  }
}

void json_builder::operator()(composite::bool_t v)
{
  if(!d_keystack.empty())
  {
    auto key = d_keystack.top();
    d_keystack.pop();
    to_exc(d_base.with_bool(key, v));
  }
  else
  {
    to_exc(d_base.with_bool(v));
  }
}

void json_builder::operator()(composite::int_t v)
{
  if(!d_keystack.empty())
  {
    auto key = d_keystack.top();
    d_keystack.pop();
    to_exc(d_base.with_int(key, v));
  }
  else
  {
    to_exc(d_base.with_int(v));
  }
}

void json_builder::operator()(composite::uint_t v)
{
  if(!d_keystack.empty())
  {
    auto key = d_keystack.top();
    d_keystack.pop();
    to_exc(d_base.with_uint(key, v));
  }
  else
  {
    to_exc(d_base.with_uint(v));
  }
}

void json_builder::operator()(composite::float_t v)
{
  if(!d_keystack.empty())
  {
    auto key = d_keystack.top();
    d_keystack.pop();
    to_exc(d_base.with_float(key, v));
  }
  else
  {
    to_exc(d_base.with_float(v));
  }
}

void json_builder::operator()(std::string_view v)
{
  if(!d_keystack.empty())
  {
    auto key = d_keystack.top();
    d_keystack.pop();
    to_exc(d_base.with_string(key, v));
  }
  else
  {
    to_exc(d_base.with_string(v));
  }
}

void json_builder::operator()(const composite::sequence& v)
{
  if(!d_keystack.empty())
  {
    auto key = d_keystack.top();
    d_keystack.pop();
    to_exc(d_base.with_sequence(key));
  }
  else
  {
    to_exc(d_base.with_sequence());
  }

  for(auto&& item : v)
  {
    item.visit(*this);
  }

  to_exc(d_base.pop());
}

void json_builder::operator()(const composite::mapping& v)
{
  if(!d_keystack.empty())
  {
    auto key = d_keystack.top();
    d_keystack.pop();
    to_exc(d_base.with_mapping(key));
  }
  else
  {
    to_exc(d_base.with_mapping());
  }

  for(auto&& kv : v)
  {
    d_keystack.push(kv.first);
    kv.second.visit(*this);
  }

  to_exc(d_base.pop());
}

} // namespace kjson
