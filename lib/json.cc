#include "json.hh"
#include "json_builder.hh"
#include "visitor.hh"
#include "parser.hh"
#include <composite/builder.hh>
#include <sstream>
#include <string>

namespace kjson {

using namespace std;

namespace {

class to_composite : public visitor {
public:
    void null() override {
        with(composite::none{});
    }
    void null(std::string_view key) override {
        with(key, composite::none{});
    }

    void boolean(bool v) override {
        with(v);
    }
    void boolean(std::string_view key, bool v) override {
        with(key, v);
    }

    void integer(int64_t v) override {
        with(v);
    }
    void integer(std::string_view key, int64_t v) override {
        with(key, v);
    }

    void floating_point(double v) override {
        with(v);
    }

    void floating_point(std::string_view key, double v) override {
        with(key, v);
    }

    void string(std::string v) override {
        with(v);
    }
    void string(std::string_view key, std::string v) override {
        with(key, v);
    }

    void push_sequence() override {
        d_builder.push_sequence();
    }
    void push_sequence(std::string_view key) override {
        d_builder.push_sequence(key);
    }

    void push_mapping() override {
        d_builder.push_mapping();
    }
    void push_mapping(std::string_view key) override {
        d_builder.push_mapping(key);
    }

    void pop() override {
        d_builder.pop();
    }

    composite::composite collect() {
        return d_builder.build();
    }

private:
    template <typename T>
    void with(T&& value) {
        d_builder.with(std::forward<T>(value));
    }

    template <typename T>
    void with(string_view key, T&& value) {
        d_builder.with(key, std::forward<T>(value));
    }

    composite::builder d_builder;
};

}

result load(istream& input)
{
  return parse(input);
}

result load(string_view input)
{
  istringstream str{string{input}};
  return load(str);
}

maybe_error load(istream &input, visitor &v) {
    return maybe_error::none();
}

maybe_error load(string_view input, visitor &v) {
    istringstream str{string{input}};
    return load(str, v);
}

void dump(const document& data, ostream& out, bool compact)
{
  json_builder jb(out, compact);
  data.visit(jb);
}

}
