#include "parser.hh"
#include "tokenizer.hh"
#include <cassert>
#include <composite/builder.hh>

using namespace std;
using namespace kdv::json;

typedef kdv::composite::scalar::type_t hint_t;

namespace {

class parser
{
public:
  parser(istream& input)
    : d_stream(input)
    , d_token(token::e_eof)
  {
  }

  kdv::composite::composite_ptr parse();

private:
  bool mapping();
  bool sequence();
  bool value();
  bool elements();
  bool members();
  bool pair();

  token const& current() const
  {
    return d_token;
  }

  void advance()
  {
    if(!d_builder.empty()) // optimization: don't read anything more if we don't expect anything (can be relevant on blocking io)
      d_token = next_token(d_stream);
  }

  void start()
  {
    d_token = next_token(d_stream);
  }

  istream&                          d_stream;
  kdv::composite::key_aware_builder d_builder;
  token                             d_token;
};

kdv::composite::composite_ptr parser::parse()
{
  start();

  if(mapping() ||
     sequence() ||
     value())
    return d_builder.build();
  else
    return kdv::composite::composite_ptr();
}

bool parser::mapping()
{
  if(current().tok == token::e_start_mapping)
  {
    d_builder.push_mapping();
    advance();

    members();

    if(current().tok != token::e_end_mapping)
      throw parse_error("expected '}'");

    d_builder.pop();
    advance();
    return true;
  }
  return false;
}

bool parser::sequence()
{
  if(current().tok == token::e_start_sequence)
  {
    d_builder.push_sequence();
    advance();

    elements();

    if(current().tok != token::e_end_sequence)
      throw parse_error("expected ']'");

    d_builder.pop();
    advance();

    return true;
  }
  return false;
}

bool parser::value()
{
  if(mapping() ||
     sequence())
    return true;

  hint_t h = hint_t::e_unspecified;

  switch(current().tok)
  {
  case token::e_int:
    h = hint_t::e_int;
    break;
  case token::e_float:
    h = hint_t::e_float;
    break;
  case token::e_string:
    h = hint_t::e_string;
    break;
  case token::e_true:
    h = hint_t::e_bool;
    break;
  case token::e_false:
    h = hint_t::e_bool;
    break;
  case token::e_null:
    h = hint_t::e_null;
    break;
  default:
    return false;
  }

  assert(h != hint_t::e_unspecified);

  d_builder.with(current().value, h);
  advance();

  return true;
}

bool parser::elements()
{
  while(value())
  {
    if(current().tok == token::e_separator)
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
    if(current().tok == token::e_separator)
      advance();
    else
      break;
  }
  return true;
}

bool parser::pair()
{
  if(current().tok == token::e_string)
  {
    d_builder.with_key(current().value);
    advance();

    if(current().tok != token::e_mapper)
      throw parse_error("expected ':'");

    advance();

    if(!value())
      throw parse_error("value expected after ':'");

    return true;
  }
  return false;
}
}

kdv::composite::composite_ptr kdv::json::parse(istream& input)
{
  parser p(input);
  return p.parse();
}
