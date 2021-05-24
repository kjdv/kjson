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
    to_exc(d_base.push_sequence());

    for(auto&& item : v) {
        item.visit(*this);
    }

    to_exc(d_base.pop());
}

void json_builder::to_exc(const builder::result& r) const {
    r.map_err([](auto&& e) {
        throw std::runtime_error(e.msg);
        return e;
    });
}

void json_builder::operator()(const composite::mapping& v) {
    to_exc(d_base.push_mapping());

    for(auto&& kv : v) {
        to_exc(d_base.key(kv.first));
        kv.second.visit(*this);
    }

    to_exc(d_base.pop());
}

} // namespace kjson
