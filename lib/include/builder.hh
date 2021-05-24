#pragma once

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <results/result.hh>
#include <string_view>
#include <type_traits>
#include <utility>

namespace kjson {

class builder {
  public:
    using result = results::result<builder*>;

    explicit builder(std::ostream& out, bool compact = false);
    ~builder();

    result key(std::string_view k);

    template <typename T>
    result with(T&& v);

    result with_none();
    result with_bool(bool v);
    result with_int(int64_t v);
    result with_uint(uint64_t v);
    result with_float(double v);
    result with_string(std::string_view v);
    result push_mapping();
    result push_sequence();

    result pop();

    void flush();

  private:
    class impl;

    std::unique_ptr<impl> d_pimpl;
};

template <typename T>
auto builder::with(T&& v) -> result {
    using U = std::decay_t<T>;

    if constexpr(std::is_void_v<U> || std::is_empty_v<U>) {
        return with_none();
    } else if constexpr(std::is_same_v<U, bool>) {
        return with_bool(std::forward<T>(v));
    } else if constexpr(std::is_integral_v<U> && std::is_signed_v<U>) {
        return with_int(std::forward<T>(v));
    } else if constexpr(std::is_integral_v<U> && std::is_unsigned_v<U>) {
        return with_uint(std::forward<T>(v));
    } else if constexpr(std::is_floating_point_v<U>) {
        return with_float(std::forward<T>(v));
    } else if constexpr(std::is_convertible_v<U, std::string_view>) {
        return with_string(std::forward<T>(v));
    } else {
        return result::err("no overload available");
    }
}

} // namespace kjson
