#pragma once

#include <string_view>
#include <cstdint>
#include <string>
#include <variant>


namespace kjson {

struct none {
};

using scalar_t = std::variant<
    none,
    bool,
    int64_t,
    uint64_t,
    double,
    std::string>;

class visitor {
  public:
    virtual ~visitor() = default;

    virtual void scalar(scalar_t v)                       = 0;
    virtual void scalar(std::string_view key, scalar_t v) = 0;

    virtual void push_sequence()                     = 0;
    virtual void push_sequence(std::string_view key) = 0;

    virtual void push_mapping()                     = 0;
    virtual void push_mapping(std::string_view key) = 0;

    virtual void pop() = 0;
};

} // namespace kjson
