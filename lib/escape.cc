#include "escape.hh"
#include <cassert>

namespace kjson {

using namespace std;

void iescape(string& str)
{
  static const char escaped[] = "\"\\/\b\f\n\r\t";

  str.insert(0, 1, '"');

  string::size_type i = 1;
  while((i = str.find_first_of(escaped, i)) != string::npos)
  {
    char c = str[i];

    switch(c)
    {
    case '"':
    case '\\':
    case '/':
      break;
    case '\b':
      c = 'b';
      break;
    case '\f':
      c = 'f';
      break;
    case '\n':
      c = 'n';
      break;
    case '\r':
      c = 'r';
      break;
    case '\t':
      c = 't';
      break;

    default:
      assert(false);
    }

    str.replace(i, 1, string("\\") + c);
    i += 2;
  }

  str += '"';
}

string escape(string_view input)
{
  string result(input);
  iescape(result);
  return result;
}

} // namespace kjson
