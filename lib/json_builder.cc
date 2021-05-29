#include "json_builder.hh"
#include "escape.hh"
#include <limits>
#include <ostream>

namespace kjson {

namespace {

} // namespace

json_builder::json_builder(std::ostream& out, bool compact)
  : d_base(out, compact) {
}

void json_builder::operator()(const composite::sequence& v) {
    d_base.push_sequence();

    for(auto&& item : v) {
        item.visit(*this);
    }

    d_base.pop();
}

void json_builder::operator()(const composite::mapping& v) {
    d_base.push_mapping();

    for(auto&& kv : v) {
        d_base.key(kv.first);
        kv.second.visit(*this);
    }

    d_base.pop();
}

} // namespace kjson
