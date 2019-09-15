#include "tokenizer.hh"
#include <cassert>
#include <cstring>
#include <hex/hex.hh>
#include <stringutils/builder.hh>

using namespace std;
using namespace kdv::json;
using namespace kdv::stringutils;

namespace {

const int eof = char_traits<char>::eof();

inline int non_ws(istream& str)
{
  char c = 0;
  while(str.get(c))
  {
    if(!isspace(c))
      return c;
  }
  return eof;
}

inline void extract_literal(istream& input, char head, string const& tail)
{
  for(char e : tail)
  {
    int c = input.get();
    if(c != e)
      throw tokenizer_error(builder("unexpected char ", (char)c, ", expected ", e, " as part of \"", head, tail, '"'));
  }
}

token extract_number(istream& input, char head)
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

  return token(is_float ? token::e_float : token::e_int, value);
}

string extract_utf8(istream& input)
{
  char32_t wc = 0;
  for(size_t i = 0; i < 4; ++i)
  {
    int c = input.get();
    if(c == eof)
      break;

    if(!kdv::hex::is_hex(c))
      throw tokenizer_error("expected hex digit");

    wc <<= 4;
    wc |= kdv::hex::single_decode((char)c);
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

  return value;
}

token extract_string(istream& input)
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
        value += extract_utf8(input);
        break;

      default:
        value += c;
        break;
      }
    }
    else
      value += c;
  }

  return token(token::e_string, value);
}
}

token kdv::json::next_token(istream& input)
{
  int c = non_ws(input);
  if(c != eof)
  {
    switch(c)
    {
    case '{':
      return token::e_start_mapping;
    case '}':
      return token::e_end_mapping;
    case '[':
      return token::e_start_sequence;
    case ']':
      return token::e_end_sequence;
    case ',':
      return token::e_separator;
    case ':':
      return token::e_mapper;

    case 't':
      extract_literal(input, 't', "rue");
      return token(token::e_true, "true");
    case 'f':
      extract_literal(input, 'f', "alse");
      return token(token::e_false, "false");
    case 'n':
      extract_literal(input, 'n', "ull");
      return token(token::e_null, "null");

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
      throw tokenizer_error(builder("unexpected token ", (char)c));
    }
  }
  return token::e_eof;
}
