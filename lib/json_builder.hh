#pragma once

#include "builder.hh"
#include <composite/composite.hh>
#include <iosfwd>
#include <stack>

namespace kjson {

class json_builder
{
public:
  explicit json_builder(std::ostream& out, bool compact = false);

  void operator()(composite::none);

  void operator()(composite::bool_t v);

  void operator()(composite::int_t v);

  void operator()(composite::uint_t v);

  void operator()(composite::float_t v);

  void operator()(std::string_view v);

  void operator()(const composite::sequence& v);

  void operator()(const composite::mapping& v);

private:
  builder                      d_base;
  std::stack<std::string_view> d_keystack;
};

} // namespace kjson
