#pragma once

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <utility>

namespace kjson {

class builder_error : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

class builder {
  public:
    explicit builder(std::ostream& out, bool compact = false);
    ~builder();

    builder& key(std::string_view k);

    template <typename T>
    builder& value(T&& v);

    builder& with_none();
    builder& with_bool(bool v);
    builder& with_int(int64_t v);
    builder& with_uint(uint64_t v);
    builder& with_float(double v);
    builder& with_string(std::string_view v);
    builder& push_mapping();
    builder& push_sequence();

    builder& pop();

    builder& flush();

  private:
    class impl;

    std::unique_ptr<impl> d_pimpl;
};

template <typename T>
builder& builder::value(T&& v) {
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
        throw builder_error("no overload available");
    }
}

} // namespace kjson
