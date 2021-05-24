#pragma once

#include <cstdint>
#include <string>
#include <variant>

namespace kjson {

struct none
{
};

using scalar_t = std::variant<
    none,
    bool,
    int64_t,
    uint64_t,
    double,
    std::string>;
}
