#pragma once

#include <string_view>
#include <composite/composite.hh>

namespace kjson {

class visitor {
public:
    virtual ~visitor() = default;

    virtual void scalar(composite::composite v) = 0;
    virtual void scalar(std::string_view key, composite::composite v) = 0;

    virtual void push_sequence() = 0;
    virtual void push_sequence(std::string_view key) = 0;

    virtual void push_mapping() = 0;
    virtual void push_mapping(std::string_view key) = 0;

    virtual void pop() = 0;
};

}
