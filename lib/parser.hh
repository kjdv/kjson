#pragma once

#include <composite/composite.hh>
#include <iosfwd>
#include <stdexcept>

namespace kdv {
namespace json {

class parse_error : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};

kdv::composite::composite_ptr parse(std::istream& input);
}
}
