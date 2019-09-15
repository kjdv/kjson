#pragma once

#include <composite/composite.hh>
#include <results/result.hh>
#include <iosfwd>
#include <string>

namespace kjson {

using document = composite::composite;
using result = results::result<document, results::error>;

result load(std::istream& input);
result load(std::string const& input);

void dump(document const& data, std::ostream& out, bool compact = true);
}
