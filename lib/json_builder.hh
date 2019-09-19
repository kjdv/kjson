#pragma once

#include <composite/composite.hh>
#include <iosfwd>

namespace kjson {

class json_builder {
public:
  explicit json_builder(std::ostream &out, bool compact = false);

  void operator()(composite::none);

  void operator()(composite::bool_t v);

  void operator()(composite::int_t v);

  void operator()(composite::float_t v);

  void operator()(std::string_view v);

  void operator()(const composite::sequence &v);

  void operator()(const composite::mapping &v);

private:
  template <typename T>
  void scalar(const T &v);

  void comma();
  void newline();
  void element();
  bool toplevel() const;

  std::ostream &d_out;
  bool d_compact{false};
  bool d_needscomma{false};
  unsigned d_indent{0};
};

}
