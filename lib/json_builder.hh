#pragma once

#include "builder.hh"
#include <composite/composite.hh>
#include <iosfwd>
#include <stack>

namespace kjson {

class json_builder {
  public:
    explicit json_builder(std::ostream& out, bool compact = false);

    template <typename T>
    void operator()(T&& v);

    void operator()(const composite::sequence& v);

    void operator()(const composite::mapping& v);

  private:
    builder d_base;
};

template <typename T>
void json_builder::operator()(T&& v) {
    d_base.value(std::forward<T>(v));
}

} // namespace kjson
