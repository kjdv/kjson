#include "parser.hh"
#include <composite/builder.hh>
#include <gtest/gtest.h>
#include <sstream>

using namespace std;
using namespace kdv::composite;
using namespace kdv::json;

TEST(parser, plain_int)
{
  composite_cptr expected =
      builder().with("148").build();

  istringstream input(" 148 ");

  composite_ptr actual = parse(input);

  ASSERT_TRUE((bool)actual);
  EXPECT_EQ(*expected, *actual);
}

TEST(parser, plain_float)
{
  composite_cptr expected =
      builder().with("3.14").build();

  istringstream input("3.14");

  composite_ptr actual = parse(input);

  ASSERT_TRUE((bool)actual);
  EXPECT_EQ(*expected, *actual);
}

TEST(parser, plain_string)
{
  composite_cptr expected =
      builder().with("blah blah").build();

  istringstream input("\"blah blah\"");

  composite_ptr actual = parse(input);

  ASSERT_TRUE((bool)actual);
  EXPECT_EQ(*expected, *actual);
}

TEST(parser, plain_true)
{
  composite_cptr expected =
      builder().with("true").build();

  istringstream input("true");

  composite_ptr actual = parse(input);

  ASSERT_TRUE((bool)actual);
  EXPECT_EQ(*expected, *actual);
}

TEST(parser, plain_false)
{
  composite_cptr expected =
      builder().with("false").build();

  istringstream input("false");

  composite_ptr actual = parse(input);

  ASSERT_TRUE((bool)actual);
  EXPECT_EQ(*expected, *actual);
}

TEST(parser, plain_null)
{
  composite_cptr expected =
      builder().with("null").build();

  istringstream input("null");

  composite_ptr actual = parse(input);

  ASSERT_TRUE((bool)actual);
  EXPECT_EQ(*expected, *actual);
}

TEST(parser, sequence)
{
  composite_cptr expected =
      builder()
          .push_sequence()
          .with(1)
          .with(2)
          .with(3)
          .build();
  const string input = "[1, 2, 3]";

  istringstream stream(input);
  composite_ptr actual = parse(stream);

  ASSERT_TRUE((bool)actual);
  EXPECT_EQ(*expected, *actual);
}

TEST(parser, sequence_empty)
{
  composite_cptr expected =
      builder()
          .push_sequence()
          .build();
  const string input = "[]";

  istringstream stream(input);
  composite_ptr actual = parse(stream);

  ASSERT_TRUE((bool)actual);
  EXPECT_EQ(*expected, *actual);
}

TEST(parser, sequence_trailing_comma)
{
  composite_cptr expected =
      builder()
          .push_sequence()
          .with(1)
          .with(2)
          .with(3)
          .build();
  const string input = "[1, 2, 3, ]";

  istringstream stream(input);
  composite_ptr actual = parse(stream);

  ASSERT_TRUE((bool)actual);
  EXPECT_EQ(*expected, *actual);
}

TEST(parser, sequence_nested)
{
  composite_cptr expected =
      builder()
          .push_sequence()
          .with(1)
          .push_sequence()
          .with(2)
          .with(3)
          .pop()
          .with(4)
          .with(5)
          .build();
  const string input = "[1, [2, 3], 4, 5]";

  istringstream stream(input);
  composite_ptr actual = parse(stream);

  ASSERT_TRUE((bool)actual);
  EXPECT_EQ(*expected, *actual);
}

TEST(parser, mapping)
{
  composite_cptr expected =
      builder()
          .push_mapping()
          .with("key", "value")
          .build();
  const string input = "{\"key\": \"value\"}";

  istringstream stream(input);
  composite_ptr actual = parse(stream);

  ASSERT_TRUE((bool)actual);
  EXPECT_EQ(*expected, *actual);
}

TEST(parser, mapping_empty)
{
  composite_cptr expected =
      builder()
          .push_mapping()
          .build();
  const string input = "{ }";

  istringstream stream(input);
  composite_ptr actual = parse(stream);

  ASSERT_TRUE((bool)actual);
  EXPECT_EQ(*expected, *actual);
}

TEST(parser, mapping_trailing_comma)
{
  composite_cptr expected =
      builder()
          .push_mapping()
          .with("key", "value")
          .with("pi", 3.1459)
          .with("e", 2.7182)
          .build();
  const string input = "{\"key\": \"value\", \"pi\" : 3.145900, \"e\" : 2.718200, }";

  istringstream stream(input);
  composite_ptr actual = parse(stream);

  ASSERT_TRUE((bool)actual);
  EXPECT_EQ(*expected, *actual);
}

TEST(parser, mapping_nested)
{
  composite_cptr expected =
      builder()
          .push_mapping()
          .with("key", "value")
          .with("pi", 3.1459)
          .push_mapping("nest")
          .with("one", 1)
          .with("two", 2)
          .with("three", 3)
          .pop()
          .with("e", 2.7182)
          .build();

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
  composite_ptr actual = parse(stream);

  ASSERT_TRUE((bool)actual);
  EXPECT_EQ(*expected, *actual);
}
