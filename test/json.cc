#include <json/json.hh>
#include <composite/builder.hh>
#include <gtest/gtest.h>

using namespace std;
using namespace kdv::composite;
using namespace kdv::json;

TEST(toplevel, load)
{
  const string document =
      "{\n"
      "  \"key\" : \"value\","
      "  \"list\" : [\n"
      "    \"string\",\n"
      "    1,\n"
      "    true,\n"
      "    {\"pi\":314,\"e\":271},\n"
      "  ],\n"
      "  \"foo\" : \"bar\"\n"
      "}";

  composite_cptr expected =
      builder()
          .push_mapping()
          .with("key", "value")
          .push_sequence("list")
          .with("string")
          .with(1)
          .with("true")
          .push_mapping()
          .with("pi", 314)
          .with("e", 271)
          .pop()
          .pop()
          .with("foo", "bar")
          .build();

  composite_ptr actual = load(document);

  ASSERT_TRUE((bool)actual);
  EXPECT_EQ(*expected, *actual);
}

TEST(toplevel, incomplete)
{
  // should not block
  const string document =
      "{\n"
      "  \"key\" : \"value\","
      "  \"list\" : [\n"
      "    \"string\",\n"
      "    1,\n";

  EXPECT_THROW(load(document), std::runtime_error);
}
