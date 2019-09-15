#pragma once

#include <composite/composite.hh>
#include <composite/visitor.hh>
#include <iosfwd>

namespace kdv {
namespace json {
class json_compact_builder : public kdv::composite::visitor
{
  std::ostream& d_stream;
  int           d_needscomma;

  bool toplevel() const
  {
    return d_needscomma == -1; // todo: magic number
  }

  bool needscomma() const
  {
    return d_needscomma == 1;
  }

  void needscomma(bool val)
  {
    d_needscomma = val ? 1 : 0;
  }

public:
  explicit json_compact_builder(std::ostream& stream)
    : d_stream(stream)
    , d_needscomma(-1)
  {
  }

  void visit(kdv::composite::scalar const& val) override;
  void visit(kdv::composite::sequence const& seq) override;
  void visit(kdv::composite::mapping const& map) override;

  void sentinel(kdv::composite::sequence const& seq) override;
  void sentinel(kdv::composite::mapping const& map) override;
  void visit_key(std::string const& key) override;
};
}
}
