#include "tokenizer.hh"

#include <cassert>
#include <cstring>
#include <istream>
#include <sstream>
#include <composite/composite.hh>
#include <cstdint>

namespace kjson {

using namespace std;
using none = composite::none;

namespace {

const int eof = char_traits<char>::eof();

inline void build(std::ostream&)
{
}

template <typename head_t, typename... tail_t>
void build(std::ostream& stream,
           head_t&&      head,
           tail_t&&... tail)
{
  stream << head;
  build(stream, std::forward<tail_t>(tail)...);
}

template <typename... args_t>
std::string builder(args_t&&... args)
{
  std::ostringstream stream;
  build(stream, std::forward<args_t>(args)...);
  return stream.str();
}

bool is_hex(char c)
{
  return (c >= '0' && c <= '9') ||
         (c >= 'a' && c <= 'f') ||
         (c >= 'A' && c <= 'F');
}

uint8_t single_decode(char h)
{
  if(h >= '0' && h <= '9')
    return h - '0';
  else if(h >= 'a' && h <= 'f')
    return h - 'a' + 10;
  else if(h >= 'A' && h <= 'F')
    return h - 'A' + 10;

  assert(false);
  return 0;
}

int non_ws(istream& str)
{
  char c = 0;
  while(str.get(c))
  {
    if(!isspace(c))
      return c;
  }
  return eof;
}

token_error<none> extract_literal(istream& input, char head, string const& tail)
{
  for(char e : tail)
  {
    int c = input.get();
    if(c != e)
      return results::make_err<none>(builder("unexpected char ", (char)c, ", expected ", e, " as part of \"", head, tail, '"'));
  }

  return results::make_ok<none>();
}

token_error<token> extract_number(istream& input, char head)
{
  bool is_float  = false;
  bool had_point = false;
  bool had_exp   = false;

  string value;
  value += head;

  int c;
  while((c = input.peek()) != eof)
  {
    if(c >= '0' && c <= '9')
    {
      input.get();
      value += c;
    }
    else if(!had_point && c == '.')
    {
      input.get();
      value += c;
      is_float  = true;
      had_point = true;
    }
    else if(!had_exp && (c == 'e' || c == 'E'))
    {
      input.get();
      value += c;

      if(input.peek() == '+' || input.peek() == '-')
      {
        c = input.get();
        value += c;
      }
      is_float = true;
      had_exp  = true;
    }
    else
      break;
  }

  return results::make_ok<token>(is_float ? token::type_t::e_float : token::type_t::e_int, value);
}

token_error<string> extract_utf8(istream& input)
{
  char32_t wc = 0;
  for(size_t i = 0; i < 4; ++i)
  {
    int c = input.get();
    if(c == eof)
      break;

    if(!is_hex(c))
      return results::make_err<string>("expected hex digit");

    wc <<= 4;
    wc |= single_decode((char)c);
  }

  string value;

  char byte;

  byte = wc >> 24;
  if(byte)
    value += byte;

  byte = wc >> 16;
  if(byte)
    value += byte;

  byte = wc >> 8;
  if(byte)
    value += byte;

  byte = wc & 0xff;
  value += byte;

  return results::make_ok<string>(move(value));
}

token_error<token> extract_string(istream& input)
{
  string value;

  int c;
  while((c = input.get()) != eof && c != '"')
  {
    if(c == '\\')
    {
      c = input.get();
      switch(c)
      {
      case '/':
      case '\\':
      case '"':
        value += c;
        break;

      case 'b':
        value += '\b';
        break;
      case 'f':
        value += '\f';
        break;
      case 'n':
        value += '\n';
        break;
      case 'r':
        value += '\r';
        break;
      case 't':
        value += '\t';
        break;

      case 'u':
      {
        auto utf8 = extract_utf8(input);
        if (utf8.is_err())
          return utf8.map([](auto&&) { return token(token::type_t::e_eof); });
        else
          value += utf8.unwrap();
      }
       break;

      default:
        value += c;
        break;
      }
    }
    else
      value += c;
  }

  return results::make_ok<token>(token::type_t::e_string, value);
}
}

token_error<token> next_token(istream& input)
{
  auto ok = [](auto&& t) { return results::make_ok<token>(forward<token>(t)); };

  int c = non_ws(input);
  if(c != eof)
  {
    switch(c)
    {
    case '{':
      return ok(token::type_t::e_start_mapping);
    case '}':
      return ok(token::type_t::e_end_mapping);
    case '[':
      return ok(token::type_t::e_start_sequence);
    case ']':
      return ok(token::type_t::e_end_sequence);
    case ',':
      return ok(token::type_t::e_separator);
    case ':':
      return ok(token::type_t::e_mapper);

    case 't':
      return extract_literal(input, 't', "rue").map([](auto&&) { return token(token::type_t::e_true, "true"); });
    case 'f':
      return extract_literal(input, 'f', "alse").map([](auto&&) { return token(token::type_t::e_false, "false"); });
    case 'n':
      return extract_literal(input, 'n', "ull").map([](auto&&) { return token(token::type_t::e_null, "null"); });

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '-':
    case '+':
      return extract_number(input, c);

    case '"':
      return extract_string(input);

    default:
      return results::make_err<token>(builder("unexpected token ", (char)c));
    }
  }
  return results::make_ok<token>(token::type_t::e_eof);
}

}
