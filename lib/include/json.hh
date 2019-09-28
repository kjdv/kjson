#pragma once

#include <composite/composite.hh>
#include <results/result.hh>
#include <iosfwd>
#include <string_view>

namespace kjson {

using document = composite::composite;
using result = results::result<document>;

result load(std::istream& input);
result load(std::string_view input);

void dump(document const& data, std::ostream& out, bool compact = true);

}
