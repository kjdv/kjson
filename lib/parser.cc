#include "parser.hh"
#include "tokenizer.hh"
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

class parser
{
public:
  parser(istream& input)
    : d_stream(input)
    , d_token(token::type_t::e_eof)
  {
  }

  result parse();

private:
  result mapping();
  result sequence();
  result value();
  composite::sequence elements();
  composite::mapping members();
  maybe_error<pair<string, document>> pair();

  result extract_value();

  const token &current() const
  {
    return d_token;
  }

  maybe_error<token> advance()
  {
    return next_token(d_stream).map([this](auto&& t) { this->d_token = t; return t; });
  }

  maybe_error<token> match_and_consume(token::type_t expect)
  {
    if (current().tok != expect)
      return make_err<token>("unexpected token");
    else
      return advance();
  }

  istream& d_stream;
  token d_token;
};

result parser::parse()
{
  return advance()
      .and_then([this](auto) { return value(); })
      .and_then([this](auto&& v) {
        return match_and_consume(token::type_t::e_eof).map(
            [&](auto) { return move(v); });
      });
}

result parser::mapping()
{
  return match_and_consume(token::type_t::e_start_mapping)
      .map([this](auto) { return composite::make(members()); })
      .and_then([this](auto&& d) {
        return match_and_consume(token::type_t::e_end_mapping)
            .map([&](auto) { return move(d); });
      });
}

result parser::sequence()
{
  return match_and_consume(token::type_t::e_start_sequence)
      .map([this](auto) { return composite::make(elements()); })
      .and_then([this](auto&& d) {
        return match_and_consume(token::type_t::e_end_sequence)
            .map([&](auto) { return move(d); });
      });
}

result parser::value()
{
  return mapping()
      .or_else([this] { return sequence(); })
      .or_else([this] {
        return extract_value()
          .and_then([this](auto&& v ) {
            return advance()
                  .map([&](auto) { return v; });
            });
      });
}

result parser::extract_value()
{
  auto cmp = [](auto&& v) { return make_ok<document>(move(v)); };

  switch(current().tok)
  {
  case token::type_t::e_int:
    return cmp(from_string<int64_t>(current().value));
  case token::type_t::e_float:
    return cmp(from_string<double>(current().value));
  case token::type_t::e_string:
    return cmp(current().value);
  case token::type_t::e_true:
    return cmp(true);
  case token::type_t::e_false:
    return cmp(false);
  case token::type_t::e_null:
    return cmp(composite::none{});
  default:
    return make_err<document>("failed to extract value");
  }
}

composite::sequence parser::elements()
{
  composite::sequence seq;

  auto append = [&](auto&& v) { seq.emplace_back(move(v)); };

  while (value().consume(append).is_ok())
  {
    auto me = match_and_consume(token::type_t::e_separator);
    if (me.is_err())
      break;
  }

  return seq;
}

composite::mapping parser::members()
{
  composite::mapping map;
  auto append = [&](auto&& v) { map.emplace(move(v)); };

  while(pair().consume(append).is_ok())
  {
    auto me = match_and_consume(token::type_t::e_separator);
    if (me.is_err())
      break;
  }

  return map;
}

maybe_error<pair<string, document>> parser::pair()
{
  using T = std::pair<string, document>;

  if (current().tok != token::type_t::e_string)
    return make_err<T>("key is not a string");

  string key = move(d_token.value);
  advance();

  return match_and_consume(token::type_t::e_mapper)
      .and_then([this](auto) { return value(); })
      .map([key=move(key)](auto&& val) { return make_pair(move(key), move(val)); });
}


} // namespace

result parse(istream& input)
{
  parser p(input);
  return p.parse();
}

} // namespace kjson
