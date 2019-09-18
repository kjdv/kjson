#include "parser.hh"
#include <composite/make.hh>
#include <gtest/gtest.h>
#include <sstream>

namespace kjson {
namespace {

using namespace std;
using namespace composite;

TEST(parser, plain_int)
{
  auto expected = make(148);
  istringstream input(" 148 ");

  auto actual = parse(input);

  EXPECT_EQ(expected, actual.unwrap());
}

TEST(parser, plain_float)
{
  auto expected = make(3.14);
  istringstream input(" 3.14 ");

  auto actual = parse(input);

  EXPECT_EQ(expected, actual.unwrap());
}


TEST(parser, plain_string)
{
  auto expected = make("blah blah");
  istringstream input("\"blah blah\"");

  auto actual = parse(input);

  EXPECT_EQ(expected, actual.unwrap());
}

TEST(parser, plain_true)
{
  auto expected = make(true);
  istringstream input("true");

  auto actual = parse(input);

  EXPECT_EQ(expected, actual.unwrap());
}

TEST(parser, plain_false)
{
  auto expected = make(false);
  istringstream input("false");

  auto actual = parse(input);

  EXPECT_EQ(expected, actual.unwrap());
}

TEST(parser, plain_null)
{
  auto expected = make(none{});
  istringstream input("null");

  auto actual = parse(input);

  EXPECT_EQ(expected, actual.unwrap());
}

TEST(parser, sequence)
{
  auto expected = make_seq(1, 2, 3);
  const string input = "[1, 2, 3]";

  istringstream stream(input);
  auto actual = parse(stream);

  EXPECT_EQ(expected, actual.unwrap());
}

TEST(parser, sequence_empty)
{
  auto expected = make_seq();
  const string input = "[]";

  istringstream stream(input);
  auto actual = parse(stream);

  EXPECT_EQ(expected, actual.unwrap());
}

TEST(parser, sequence_trailing_comma)
{
  auto expected = make_seq(1, 2, 3);
  const string input = "[1, 2, 3, ]";

  istringstream stream(input);
  auto actual = parse(stream);

  EXPECT_EQ(expected, actual.unwrap());
}

TEST(parser, sequence_nested)
{
  auto expected = make_seq(1, make_seq(2, 3), 4, 5);
  const string input = "[1, [2, 3], 4, 5]";

  istringstream stream(input);
  auto actual = parse(stream);

  EXPECT_EQ(expected, actual.unwrap());
}

TEST(parser, mapping)
{
  auto expected = make_map("key", "value");
  const string input = "{\"key\": \"value\"}";

  istringstream stream(input);
  auto actual = parse(stream);

  EXPECT_EQ(expected, actual.unwrap());
}

TEST(parser, mapping_empty)
{
  auto expected = make_map();
  const string input = "{ }";

  istringstream stream(input);
  auto actual = parse(stream);

  EXPECT_EQ(expected, actual.unwrap());
}

TEST(parser, mapping_trailing_comma)
{
  auto expected = make_map("key", "value", "pi", 3.1459, "e", 2.7182);
  const string input = "{\"key\": \"value\", \"pi\" : 3.145900, \"e\" : 2.718200, }";

  istringstream stream(input);
  auto actual = parse(stream);

  EXPECT_EQ(expected, actual.unwrap());
}

TEST(parser, mapping_nested)
{
  auto expected = make_map(
      "key", "value",
      "pi", 3.1459,
      "nest", make_map(
          "one", 1,
          "two", 2,
          "three", 3),
      "e", 2.7182);

  const string input =
      "{\"key\": \"value\",\n"
      "  \"pi\" : 3.145900,\n"
      "  \"nest\" : {\n"
      "    \"one\" : 1,\n"
      "    \"two\" : 2,\n"
      "    \"three\" : 3\n"
      "  },\n"
      "  \"e\" : 2.718200\n"
      "}";

  istringstream stream(input);
  auto actual = parse(stream);

  EXPECT_EQ(expected, actual.unwrap());

}

}
}
