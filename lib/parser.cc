#include "parser.hh"
#include "tokenizer.hh"
#include "visitor.hh"
#include <composite/make.hh>
#include <utility>

namespace kjson {

using namespace std;
using namespace results;

namespace {

template <typename T>
T from_string(const string& v)
{
  stringstream stream{v};
  T            r;
  stream >> r;
  return r;
}

using maybe_key = results::option<string_view>;

class parser
{
public:
  parser(istream& input, visitor &visitor)
    : d_stream(input)
    , d_visitor(visitor)
    , d_token(token::type_t::e_eof)
  {
  }

  result parse();

private:
  result                              mapping(const maybe_key &key);
  result                              sequence(const maybe_key &key);
  result                              value(const maybe_key &key);
  composite::sequence                 elements();
  composite::mapping                  members();
  token_error<pair<string, document>> kv_pair();

  result extract_value(const maybe_key &key);

  const token& current()
  {
    return d_token;
  }

  token_error<token> advance()
  {
    return next_token(d_stream).map([this](auto&& t) {
      this->d_token = t;
      return t;
    });
  }

  token_error<token> match_and_consume(token::type_t expect)
  {
    if(current().tok != expect)
      return make_err<token>("unexpected token");
    else
      return advance();
  }

  istream& d_stream;
  visitor& d_visitor;
  token    d_token;
};

result parser::parse()
{
  return advance()
      .and_then([this](auto) { return value(maybe_key::none()); })
      .and_then([this](auto&& v) {
        return match_and_consume(token::type_t::e_eof).map([&](auto) {
          return v;
        });
      });
}

result parser::mapping(const maybe_key &key)
{
  return match_and_consume(token::type_t::e_start_mapping)
      .map([this, &key](auto) {
          key.match(
              [this](string_view k) { d_visitor.push_mapping(k); },
              [this] { d_visitor.push_mapping(); }
          );
          auto ms = composite::make(members());
          d_visitor.pop();
          return ms;
      })
      .and_then([this](auto&& d) {
        return match_and_consume(token::type_t::e_end_mapping)
            .map([&](auto) { return d; });
      });
}

result parser::sequence(const maybe_key &key)
{
  return match_and_consume(token::type_t::e_start_sequence)
      .map([this, &key](auto) {
          key.match(
              [this](string_view k) { d_visitor.push_sequence(k); },
              [this] { d_visitor.push_sequence(); }
          );
         auto ems = composite::make(elements());
         d_visitor.pop();
         return ems;
      })
      .and_then([this](auto&& d) {
        return match_and_consume(token::type_t::e_end_sequence)
            .map([&](auto) { return d; });
      });
}

result parser::value(const maybe_key &key)
{
  return mapping(key)
      .or_else([this, &key] { return sequence(key); })
      .or_else([this, &key] { return extract_value(key); });
}

result parser::extract_value(const maybe_key &key)
{
  auto ok = [this, &key](auto&& v) {
    auto c = composite::make(move(v));
    key.match(
        [this, &c](string_view k) { d_visitor.scalar(k, c); },
        [this, &c] { d_visitor.scalar(c); }
    );
    return advance()
        .map([c=move(c)](auto) { return c; });
  };

  switch(current().tok)
  {
  case token::type_t::e_int:
    return ok(from_string<int64_t>(current().value));
  case token::type_t::e_float:
    return ok(from_string<double>(current().value));
  case token::type_t::e_string:
    return ok(current().value);
  case token::type_t::e_true:
    return ok(true);
  case token::type_t::e_false:
    return ok(false);
  case token::type_t::e_null:
    return ok(composite::none{});
  default:
    return make_err<document>("failed to extract value");
  }
}

composite::sequence parser::elements()
{
  composite::sequence seq;

  auto append = [&](auto&& v) {
      seq.emplace_back(move(v));
  };

  while(value(maybe_key::none()).consume(append).is_ok() &&
        match_and_consume(token::type_t::e_separator).is_ok())
    ;

  return seq;
}

composite::mapping parser::members()
{
  composite::mapping map;
  auto               append = [&](auto&& v) { map.emplace(move(v)); };

  while(kv_pair().consume(append).is_ok() &&
        match_and_consume(token::type_t::e_separator).is_ok())
    ;

  return map;
}

token_error<pair<string, document>> parser::kv_pair()
{
  using T = std::pair<string, document>;

  if(current().tok != token::type_t::e_string)
    return make_err<T>("key is not a string");

  string key = move(d_token.value);
  advance();

  return match_and_consume(token::type_t::e_mapper)
      .and_then([this, &key](auto) { return value(maybe_key::some(key)); })
      .map([key = move(key)](auto&& val) { return make_pair(move(key), move(val)); });
}

} // namespace

result parse(istream& input, visitor &visitor)
{
  try {
      parser p(input, visitor);
      return p.parse();
  } catch (const std::exception &e) {
      return result::err(e.what());
  }
}

void to_composite::scalar(composite::composite v) {
    d_builder.with(move(v));
}

void to_composite::scalar(string_view key, composite::composite v) {
    d_builder.with(key, move(v));
}

void to_composite::push_sequence() {
    d_builder.push_sequence();
}

void to_composite::push_sequence(string_view key) {
    d_builder.push_sequence(key);
}

void to_composite::push_mapping() {
    d_builder.push_mapping();
}

void to_composite::push_mapping(string_view key) {
    d_builder.push_mapping(key);
}

void to_composite::pop() {
    d_builder.pop();
}

composite::composite to_composite::collect() {
    return d_builder.build();
}

} // namespace kjson
