#include "parser.hh"
#include "tokenizer.hh"
#include <cassert>
#include <composite/builder.hh>
#include <optional>

namespace kjson {

using namespace std;

namespace {

template <typename T>
T from_string(const string &v)
{
  stringstream stream{v};
  T r;
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
  bool mapping();
  bool sequence();
  bool value(const optional<string> &key = nullopt);
  bool elements();
  bool members();
  bool pair();

  std::optional<composite::composite> extract_value();

  token const& current() const
  {
    return d_token;
  }

  void advance()
  {
    if(!d_err.has_value())
    {
      next_token(d_stream).match(
          [this](auto&& tok) { d_token = tok; },
          [this](auto&& err) { d_err = err; });

    }
  }

  void start()
  {
    d_err.reset();
    advance();
  }

  istream&           d_stream;
  composite::builder d_builder;
  token              d_token;
  optional<results::error> d_err;
};

result parser::parse()
{
  start();

  if((mapping() ||
     sequence() ||
      value()) && !d_err.has_value())
    return results::make_ok<document>(d_builder.build());
  else
    return results::make_err<document>(d_err.value_or(results::error{"no valid document"}));
}

bool parser::mapping()
{
  if(current().tok == token::type_t::e_start_mapping)
  {
    d_builder.push_mapping();
    advance();

    members();

    if(current().tok != token::type_t::e_end_mapping)
    {
      d_err = results::error{"expected '}'"};
      return false;
    }

    d_builder.pop();
    advance();
    return true;
  }
  return false;
}

bool parser::sequence()
{
  if(current().tok == token::type_t::e_start_sequence)
  {
    d_builder.push_sequence();
    advance();

    elements();

    if(current().tok != token::type_t::e_end_sequence)
    {
      d_err = results::error{"expected ']'"};
      return false;
    }

    d_builder.pop();
    advance();

    return true;
  }
  return false;
}

bool parser::value(const optional<string> &key)
{
  if(mapping() ||
     sequence())
    return true;

  auto value = extract_value();
  if (!value.has_value())
    return false;

  advance();

  if (key.has_value())
    d_builder.with(*key, move(*value));
  else
    d_builder.with(move(*value));

  return true;
}

std::optional<composite::composite> parser::extract_value()
{
  using cmp = composite::composite;

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
    return nullopt;
  }
}

bool parser::elements()
{
  while(value())
  {
    if(current().tok == token::type_t::e_separator)
      advance();
    else
      break;
  }
  return true;
}

bool parser::members()
{
  while(pair())
  {
    if(current().tok == token::type_t::e_separator)
      advance();
    else
      break;
  }
  return true;
}

bool parser::pair()
{
  if(current().tok == token::type_t::e_string)
  {
    string key = current().value;
    advance();

    if(current().tok != token::type_t::e_mapper)
    {
      d_err = results::error{"expected ':'"};
      return false;
    }

    advance();

    if(!value(key))
    {
      d_err = results::error{"value expected after ':'"};
      return false;
    }

    return true;
  }
  return false;
}
}

result parse(istream& input)
{
  parser p(input);
  return p.parse();
}

}
