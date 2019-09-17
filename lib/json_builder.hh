#pragma once

#include <composite/composite.hh>
#include <composite/visitor.hh>
#include <iosfwd>

namespace kjson {
class json_builder : public composite::visitor
{
  std::ostream& d_stream;
  size_t        d_depth;
  bool          d_needscomma;

  bool needscomma() const
  {
    return d_needscomma;
  }

  void needscomma(bool val)
  {
    d_needscomma = val;
  }

  bool toplevel() const
  {
    return !d_depth;
  }

  void incdepth()
  {
    ++d_depth;
  }

  void decdepth()
  {
    --d_depth;
  }

  void newline();

  template <typename T>
  void scalar(const T &value);

public:
  explicit json_builder(std::ostream& stream)
    : d_stream(stream)
    , d_depth(0)
    , d_needscomma(false)
  {
  }

  void visit(composite::deduce<void>::type) override;
  void visit(composite::deduce<bool>::type) override;
  void visit(composite::deduce<int>::type) override;
  void visit(composite::deduce<double>::type) override;
  void visit(std::string_view) override;

  void key(std::string_view) override;
  void start_sequence() override;
  void start_mapping() override;
  void sentinel() override;
 };
}

