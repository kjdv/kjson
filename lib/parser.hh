#pragma once

#include <iosfwd>
#include "json.hh"
#include "visitor.hh"
#include <composite/builder.hh>

namespace kjson {

class to_composite : public visitor {
public:
    void scalar(composite::composite v) override;
    void scalar(std::string_view key, composite::composite v) override;

    void push_sequence() override;
    void push_sequence(std::string_view key) override;

    void push_mapping() override;
    void push_mapping(std::string_view key) override;

    void pop() override;

    composite::composite collect();

private:
    composite::builder d_builder;
};

result parse(std::istream& input, visitor &visitor);

}

