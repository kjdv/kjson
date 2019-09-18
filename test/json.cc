#include "json.hh"
#include <composite/make.hh>
#include <gtest/gtest.h>

namespace kjson {
namespace {

using namespace std;
using namespace composite;

TEST(toplevel, load)
{
  const string document =
      "{\n"
      "  \"key\" : \"value\","
      "  \"list\" : [\n"
      "    \"string\",\n"
      "    1,\n"
      "    true,\n"
      "    {\"pi\":3.14,\"e\":2.71},\n"
      "  ],\n"
      "  \"foo\" : \"bar\"\n"
      "}";

  auto expected = make_map(
      "key", "value",
      "list", make_seq(
          "string",
          1,
          true,
          make_map(
              "pi", 3.14,
              "e", 2.71)),
      "foo", "bar");

  auto actual = load(document);

  EXPECT_EQ(expected, actual.unwrap());
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

  auto actual = load(document);
  EXPECT_TRUE(actual.is_err());
}

}
}
