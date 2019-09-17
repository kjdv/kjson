#pragma once

#include <iosfwd>
#include <stack>
#include <string>
#include <results/result.hh>

namespace kjson {

template <typename T>
using maybe_error = typename results::result<T, results::error>;

struct token
{
  enum class type_t {
    e_start_mapping,  // {
    e_end_mapping,    // }
    e_start_sequence, // [
    e_end_sequence,   // ]
    e_separator,      // ,
    e_mapper,         // :
    e_string,
    e_int,
    e_float,
    e_true,  // true
    e_false, // false
    e_null,  // null
    e_eof,
  };

  type_t      tok;
  std::string value;

  token(type_t t)
    : tok(t)
  {
  }

  token(type_t t, const std::string& v)
    : tok(t)
    , value(v)
  {
  }

  token(type_t t, std::string&& v)
    : tok(t)
    , value(std::move(v))
  {
  }
};

maybe_error<token> next_token(std::istream& input);
}

