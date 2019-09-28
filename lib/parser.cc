#include "parser.hh"
#include "tokenizer.hh"
#include <composite/make.hh>
#include <utility>
#include <iostream>

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
  result elements();
  result members();
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
  auto on_token = [this](auto) {
    return mapping()
        .or_else([this] { return sequence(); })
        .or_else([this] { return value(); });
  };

  return advance()
      .and_then(on_token)
      .and_then([this](auto&& v) {
        return match_and_consume(token::type_t::e_eof).match(
            [&](auto) { return make_ok<document>(move(v)); },
            [](auto&& err) { return make_err<document>(move(err)); });
      });
}

result parser::mapping()
{
  return match_and_consume(token::type_t::e_start_mapping)
      .and_then([this](auto) { return members(); })
      .and_then([this](auto&& d) {
        return match_and_consume(token::type_t::e_end_mapping)
            .map([&](auto) { return move(d); });
      });
}

result parser::sequence()
{
  return match_and_consume(token::type_t::e_start_sequence)
      .and_then([this](auto) { return elements(); })
      .and_then([this](auto&& d) {
        cout << "end seq on " << (int)current().tok << endl;
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
              .match(
                  [&](auto) { return make_ok<document>(move(v)); },
                  [](auto&& err) { return make_err<document>(move(err)); });
            });
      });
}

result parser::extract_value()
{
  cout << "ev " << current().value << endl;

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

result parser::elements()
{
  composite::sequence seq;

  auto append = [&](auto&& v) {
    cout << "adding " << v << endl;
    seq.emplace_back(move(v));
  };

  cout << "start elem" << endl;

  while (value().consume(append).is_ok())
  {
    cout << "ts " << (int)current().tok << endl;
    auto me = match_and_consume(token::type_t::e_separator);
    if (me.is_err())
      break;
  }

  cout << "end elem " << current().value << ' ' << (int)current().tok << endl;

  return make_ok<document>(move(seq));
}

result parser::members()
{
  composite::mapping map;
  auto append = [&](auto&& v) { map.emplace(move(v)); };

  while(pair().consume(append).is_ok())
  {
    auto me = match_and_consume(token::type_t::e_separator);
    if (me.is_err())
      break;
  }

  return make_ok<document>(move(map));
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

  try
  {
    return p.parse();
  }
  catch(std::exception& e)
  {
    return results::make_err<document>(results::error{e.what()});
  }
}

} // namespace kjson
