#pragma once

#include <string_view>
#include <string>
#include <cstdint>

namespace kjson {

class visitor {
public:
    virtual ~visitor() = default;

    virtual void null() = 0;
    virtual void null(std::string_view key) = 0;

    virtual void boolean(bool v) = 0;
    virtual void boolean(std::string_view key, bool v) = 0;

    virtual void integer(int64_t v) = 0;
    virtual void integer(std::string_view key, int64_t v) = 0;

    virtual void floating_point(double v) = 0;
    virtual void floating_point(std::string_view key, double v) = 0;

    virtual void string(std::string v) = 0;
    virtual void string(std::string_view key, std::string v) = 0;

    virtual void push_sequence() = 0;
    virtual void push_sequence(std::string_view key) = 0;

    virtual void push_mapping() = 0;
    virtual void push_mapping(std::string_view key) = 0;

    virtual void pop() = 0;
};

}
