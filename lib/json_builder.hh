#pragma once

#include <composite/composite.hh>
#include <composite/visitor.hh>
#include <iosfwd>

namespace kdv {
namespace json {
class json_builder : public kdv::composite::visitor
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

public:
  explicit json_builder(std::ostream& stream)
    : d_stream(stream)
    , d_depth(0)
    , d_needscomma(false)
  {
  }

  virtual void visit(kdv::composite::scalar const& val) override;
  virtual void visit(kdv::composite::sequence const& seq) override;
  virtual void visit(kdv::composite::mapping const& map) override;

  // optional
  virtual void sentinel(kdv::composite::sequence const& seq) override;
  virtual void sentinel(kdv::composite::mapping const& map) override;
  virtual void visit_key(std::string const& key) override;
};
}
}
