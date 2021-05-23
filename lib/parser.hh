#pragma once

#include "json.hh"
#include "visitor.hh"
#include <composite/builder.hh>
#include <iosfwd>

namespace kjson {

class to_composite : public visitor
{
public:
  void scalar(scalar_t) override;
  void scalar(std::string_view key, scalar_t) override;

  void push_sequence() override;
  void push_sequence(std::string_view key) override;

  void push_mapping() override;
  void push_mapping(std::string_view key) override;

  void pop() override;

  composite::composite collect();

private:
  composite::builder d_builder;
};

maybe_error parse(std::istream& input, visitor& visitor);

} // namespace kjson
