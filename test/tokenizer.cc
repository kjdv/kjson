#include "tokenizer.hh"
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <vector>

namespace kjson {
namespace {

using namespace std;

struct tokenizer_testcase
{
  string        input;
  vector<token> tokens;
};

class tokenizer_test : public testing::TestWithParam<tokenizer_testcase>
{
};

TEST_P(tokenizer_test, tokens)
{
  tokenizer_testcase const& testcase = GetParam();

  istringstream stream(testcase.input);

  for(auto&& expected : testcase.tokens)
  {
    token actual = next_token(stream).unwrap();
    EXPECT_EQ(expected.tok, actual.tok);
    EXPECT_EQ(expected.value, actual.value);
  }
}

tokenizer_testcase tokenizer_testcases[] =
    {
        // empty
        {"", {token::type_t::e_eof}},

        // simple chars
        {"{", {token::type_t::e_start_mapping}},
        {"}", {token::type_t::e_end_mapping}},
        {"[", {token::type_t::e_start_sequence}},
        {"]", {token::type_t::e_end_sequence}},
        {",", {token::type_t::e_separator}},
        {":", {token::type_t::e_mapper}},

        // literals
        {"true", {{token::type_t::e_true, "true"}}},
        {"false", {{token::type_t::e_false, "false"}}},
        {"null", {{token::type_t::e_null, "null"}}},

        // numbers
        {"1", {{token::type_t::e_int, "1"}}},
        {"12 ", {{token::type_t::e_int, "12"}}},
        {"3.14", {{token::type_t::e_float, "3.14"}}},
        {"2e7", {{token::type_t::e_float, "2e7"}}},
        {"23e+2", {{token::type_t::e_float, "23e+2"}}},
        {"23E-2", {{token::type_t::e_float, "23E-2"}}},
        {"-2", {{token::type_t::e_int, "-2"}}},
        {"+2.71", {{token::type_t::e_float, "+2.71"}}},

        // strings
        {"\"Klaas de Vries\"", {{token::type_t::e_string, "Klaas de Vries"}}},
        {"\"with \\\"quotes\\\"", {{token::type_t::e_string, "with \"quotes\""}}},
        {"\"\\\\\"", {{token::type_t::e_string, "\\"}}},
        {"\"/\"", {{token::type_t::e_string, "/"}}},
        {"\"\\b\"", {{token::type_t::e_string, "\b"}}},
        {"\"\\f\"", {{token::type_t::e_string, "\f"}}},
        {"\"\\n\"", {{token::type_t::e_string, "\n"}}},
        {"\"\\r\"", {{token::type_t::e_string, "\r"}}},
        {"\"\\t\"", {{token::type_t::e_string, "\t"}}},
        {"\"\\ud582\"", {{token::type_t::e_string, "\xd5\x82"}}},
        {"\"noot\"", {{token::type_t::e_string, "noot"}}},

        // skip whitespace
        {" \t", {token::type_t::e_eof}},
        {"\t{ ", {token::type_t::e_start_mapping}},
        {"\n[", {token::type_t::e_start_sequence}},

        // serial
        {"3 ,{\t}\"blah\" 2.72", {{token::type_t::e_int, "3"}, token::type_t::e_separator, token::type_t::e_start_mapping, token::type_t::e_end_mapping, {token::type_t::e_string, "blah"}, {token::type_t::e_float, "2.72"}, token::type_t::e_eof}},
        {"{\"aap\": \"noot\"}\n", {token::type_t::e_start_mapping, {token::type_t::e_string, "aap"}, token::type_t::e_mapper, {token::type_t::e_string, "noot"}, token::type_t::e_end_mapping, token::type_t::e_eof}},
};

INSTANTIATE_TEST_SUITE_P(test_tokenizer,
                         tokenizer_test,
                         testing::ValuesIn(tokenizer_testcases));

} // namespace

TEST(tokenizer, invalid_utf8)
{
  stringstream stream("\"\\ug582\"");
  EXPECT_TRUE(next_token(stream).is_err());
}

TEST(tokenizer, bad_literal)
{
  stringstream stream("trfalse");
  EXPECT_TRUE(next_token(stream).is_err());
}

TEST(tokenizer, bad_token)
{
  stringstream stream("!");
  EXPECT_TRUE(next_token(stream).is_err());
}

} // namespace kjson
