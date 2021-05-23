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
  parser(istream& input, visitor& visitor)
    : d_stream(input)
    , d_visitor(visitor)
    , d_token(token::type_t::e_eof)
  {
  }

  maybe_error parse();

private:
  maybe_error mapping(const maybe_key& key);
  maybe_error sequence(const maybe_key& key);
  maybe_error value(const maybe_key& key);
  void        elements();
  void        members();
  maybe_error kv_pair();

  maybe_error extract_value(const maybe_key& key);

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

maybe_error parser::parse()
{
  return advance()
      .and_then([this](auto) { return value(maybe_key::none()); })
      .and_then([this](auto v) {
        return match_and_consume(token::type_t::e_eof).map([&](auto) {
          return v;
        });
      });
}

maybe_error parser::mapping(const maybe_key& key)
{
  return match_and_consume(token::type_t::e_start_mapping)
      .map([this, &key](auto) {
        key.match(
            [this](string_view k) { d_visitor.push_mapping(k); },
            [this] { d_visitor.push_mapping(); });
        members();
        d_visitor.pop();
        return maybe_error::ok(0);
      })
      .and_then([this](auto) {
        return match_and_consume(token::type_t::e_end_mapping)
            .map([](auto) { return 0; });
      });
}

maybe_error parser::sequence(const maybe_key& key)
{
  return match_and_consume(token::type_t::e_start_sequence)
      .map([this, &key](auto) {
        key.match(
            [this](string_view k) { d_visitor.push_sequence(k); },
            [this] { d_visitor.push_sequence(); });
        elements();
        d_visitor.pop();
        return 0;
      })
      .and_then([this](auto) {
        return match_and_consume(token::type_t::e_end_sequence)
            .map([](auto) { return 0; });
      });
}

maybe_error parser::value(const maybe_key& key)
{
  return mapping(key)
      .or_else([this, &key] { return sequence(key); })
      .or_else([this, &key] { return extract_value(key); });
}

maybe_error parser::extract_value(const maybe_key& key)
{
  auto ok = [this, &key](auto&& v) {
    scalar_t c(move(v));
    key.match(
        [this, &c](string_view k) { d_visitor.scalar(k, c); },
        [this, &c] { d_visitor.scalar(c); });
    return advance()
        .map([](auto&) { return 0; });
  };

  switch(current().tok)
  {
  case token::type_t::e_int:
    return ok(from_string<int64_t>(current().value));
  case token::type_t::e_uint:
    return ok(from_string<uint64_t>(current().value));
  case token::type_t::e_float:
    return ok(from_string<double>(current().value));
  case token::type_t::e_string:
    return ok(current().value);
  case token::type_t::e_true:
    return ok(true);
  case token::type_t::e_false:
    return ok(false);
  case token::type_t::e_null:
    return ok(none{});
  default:
    return maybe_error::err("failed to extract value");
  }
}

void parser::elements()
{
  while(value(maybe_key::none()).is_ok() &&
        match_and_consume(token::type_t::e_separator).is_ok())
    ;
}

void parser::members()
{
  while(kv_pair().is_ok() &&
        match_and_consume(token::type_t::e_separator).is_ok())
    ;
}

maybe_error parser::kv_pair()
{
  if(current().tok != token::type_t::e_string)
    return maybe_error::err("key is not a string");

  string key = move(d_token.value);
  advance();

  return match_and_consume(token::type_t::e_mapper)
      .and_then([this, &key](auto) {
        return value(maybe_key::some(key));
      });
}

composite::composite from_scalar(scalar_t v)
{
  return visit([](auto&& item) {
    using T = decay_t<decltype(item)>;
    if constexpr(std::is_same_v<T, kjson::none>)
    {
      return composite::composite{};
    }
    else
    {
      return composite::make(forward<T>(item));
    }
  },
               v);
}

} // namespace

maybe_error parse(istream& input, visitor& visitor)
{
  try
  {
    parser p(input, visitor);
    return p.parse();
  }
  catch(const std::exception& e)
  {
    return maybe_error::err(e.what());
  }
}

void to_composite::scalar(scalar_t v)
{
  d_builder.with(from_scalar(v));
}

void to_composite::scalar(string_view key, scalar_t v)
{
  d_builder.with(key, from_scalar(v));
}

void to_composite::push_sequence()
{
  d_builder.push_sequence();
}

void to_composite::push_sequence(string_view key)
{
  d_builder.push_sequence(key);
}

void to_composite::push_mapping()
{
  d_builder.push_mapping();
}

void to_composite::push_mapping(string_view key)
{
  d_builder.push_mapping(key);
}

void to_composite::pop()
{
  d_builder.pop();
}

composite::composite to_composite::collect()
{
  return d_builder.build();
}

} // namespace kjson
