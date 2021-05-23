#include "parser.hh"
#include <composite/make.hh>
#include <gtest/gtest.h>
#include <sstream>

namespace kjson {
namespace {

using namespace std;
using namespace composite;

::composite::composite parse(string input) {
    istringstream stream(input);
    to_composite v;
    parse(stream, v).unwrap();
    return v.collect();
}


TEST(parser, plain_int)
{
  auto expected = make(148);
  auto actual = parse(" 148 ");

  EXPECT_EQ(expected, actual);
}

TEST(parser, plain_float)
{
  auto expected = make(3.14);
  auto actual = parse(" 3.14 ");

  EXPECT_EQ(expected, actual);
}


TEST(parser, plain_string)
{
  auto expected = make("blah blah");
  auto actual = parse(R"("blah blah)");

  EXPECT_EQ(expected, actual);
}

TEST(parser, plain_true)
{
  auto expected = make(true);
  auto actual = parse("true");

  EXPECT_EQ(expected, actual);
}

TEST(parser, plain_false)
{
  auto expected = make(false);
  auto actual = parse("false");

  EXPECT_EQ(expected, actual);
}

TEST(parser, plain_null)
{
  auto expected = make(none{});
  auto actual = parse("null");

  EXPECT_EQ(expected, actual);
}

TEST(parser, sequence)
{
  auto expected = make_seq(1, 2, 3);
  auto actual = parse("[1, 2, 3]");

  EXPECT_EQ(expected, actual);
}

TEST(parser, sequence_empty)
{
  auto expected = make_seq();
  auto actual = parse("[]");

  EXPECT_EQ(expected, actual);
}

TEST(parser, sequence_trailing_comma)
{
  auto expected = make_seq(1, 2, 3);
  auto actual = parse("[1, 2, 3, ]");

  EXPECT_EQ(expected, actual);
}

TEST(parser, sequence_nested)
{
  auto expected = make_seq(1, make_seq(2, 3), 4, 5);
  auto actual = parse("[1, [2, 3], 4, 5]");

  EXPECT_EQ(expected, actual);
}

TEST(parser, mapping)
{
  auto expected = make_map("key", "value");
  auto actual = parse(R"({"key": "value"})");

  EXPECT_EQ(expected, actual);
}

TEST(parser, mapping_empty)
{
  auto expected = make_map();
  auto actual = parse("{ }");

  EXPECT_EQ(expected, actual);
}

TEST(parser, mapping_trailing_comma)
{
  auto expected = make_map("key", "value", "pi", 3.1459, "e", 2.7182);
  auto actual = parse(R"({"key": "value", "pi" : 3.145900, "e" : 2.718200, })");

  EXPECT_EQ(expected, actual);
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
  auto actual = parse(R"({
      "key": "value",
      "pi" : 3.145900,
      "nest" : {
         "one" : 1,
         "two" : 2,
         "three" : 3
      },
      "e" : 2.718200
   })");

  EXPECT_EQ(expected, actual);

}

TEST(parser, bad_mapping)
{
  to_composite v;
  istringstream stream("{");
  EXPECT_TRUE(parse(stream, v).is_err());
}

TEST(parser, bad_sequence)
{
  to_composite v;
  istringstream stream("[1,");
  EXPECT_TRUE(parse(stream, v).is_err());
}

TEST(parser, no_document)
{
  to_composite v;
  istringstream stream("<>");
  EXPECT_TRUE(parse(stream, v).is_err());
}

TEST(parser, bad_values)
{
  to_composite v;
  istringstream stream("{1,2,3}");
  EXPECT_TRUE(parse(stream, v).is_err());
}

}
}
