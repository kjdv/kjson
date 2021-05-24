#pragma once

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <results/result.hh>
#include <string_view>

namespace kjson {

class builder
{
public:
  using result = results::result<builder*>;

  explicit builder(std::ostream& out, bool compact = false);
  ~builder();

  result with_none();
  result with_none(std::string_view key);

  result with_bool(bool v);
  result with_bool(std::string_view key, bool v);

  result with_int(int64_t v);
  result with_int(std::string_view key, int64_t v);

  result with_uint(uint64_t v);
  result with_uint(std::string_view key, uint64_t v);

  result with_float(double v);
  result with_float(std::string_view key, double v);

  result with_string(std::string_view v);
  result with_string(std::string_view key, std::string_view v);

  result with_mapping();
  result with_mapping(std::string_view key);

  result with_sequence();
  result with_sequence(std::string_view key);

  result pop();

  void flush();

private:
  class impl;

  std::unique_ptr<impl> d_pimpl;
};

} // namespace kjson
