#pragma once

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <results/result.hh>
#include <string_view>
#include <type_traits>

namespace kjson {

class builder
{
public:
  using result = results::result<builder*>;

  explicit builder(std::ostream& out, bool compact = false);
  ~builder();

  template <typename T>
  result with(T&& v);
  template <typename T>
  result with(std::string_view key, T&& v);

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

template <typename T>
auto builder::with(T&& v) -> result {
    using U = std::decay_t<T>;

    if constexpr (std::is_void_v<U> || std::is_empty_v<U>) {
        return with_none();
    } else if constexpr (std::is_same_v<U, bool>) {
        return with_bool(std::forward<U>(v));
    } else if constexpr (std::is_integral_v<U> && std::is_signed_v<U>) {
        return with_int(std::forward<U>(v));
    } else if constexpr (std::is_integral_v<U> && std::is_unsigned_v<U>) {
        return with_uint(std::forward<U>(v));
    } else if constexpr (std::is_floating_point_v<U>) {
        return with_float(std::forward<U>(v));
    } else if constexpr (std::is_convertible_v<U, std::string_view>) {
        return with_string(std::forward<U>(v));
    } else {
        return result::err("no overload available");
    }
}

// todo: get rid of duplication
template <typename T>
auto builder::with(std::string_view key, T&& v) -> result {
    using U = std::decay_t<T>;

    if constexpr (std::is_void_v<U> || std::is_empty_v<U>) {
        return with_none(key);
    } else if constexpr (std::is_same_v<U, bool>) {
        return with_bool(key, std::forward<U>(v));
    } else if constexpr (std::is_integral_v<U> && std::is_signed_v<U>) {
        return with_int(key, std::forward<U>(v));
    } else if constexpr (std::is_integral_v<U> && std::is_unsigned_v<U>) {
        return with_uint(key, std::forward<U>(v));
    } else if constexpr (std::is_floating_point_v<U>) {
        return with_float(key, std::forward<U>(v));
    } else if constexpr (std::is_convertible_v<U, std::string_view>) {
        return with_string(key, std::forward<U>(v));
    } else {
        return result::err("no overload available");
    }
}

} // namespace kjson
