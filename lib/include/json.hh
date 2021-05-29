#pragma once

#include <composite/composite.hh>
#include <iosfwd>
#include <results/option.hh>
#include <results/result.hh>
#include <string_view>

namespace kjson {

using document    = composite::composite;
using result      = results::result<document>;
using maybe_error = results::result<int>;

class visitor;

result      load(std::istream& input);
result      load(std::string_view input);
maybe_error load(std::istream& input, visitor& v);
maybe_error load(std::string_view input, visitor& v);

void dump(document const& data, std::ostream& out, bool compact = true);

} // namespace kjson
