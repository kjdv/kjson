#include "tokenizer.hh"
#include <gtest/gtest.h>
#include <hex/hex.hh>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace kdv::json;

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
    token actual = next_token(stream);
    EXPECT_EQ(expected.tok, actual.tok);
    EXPECT_EQ(expected.value, actual.value);
  }
}

tokenizer_testcase tokenizer_testcases[] =
    {
        // empty
        {"", {token::e_eof}},

        // simple chars
        {"{", {token::e_start_mapping}},
        {"}", {token::e_end_mapping}},
        {"[", {token::e_start_sequence}},
        {"]", {token::e_end_sequence}},
        {",", {token::e_separator}},
        {":", {token::e_mapper}},

        // literals
        {"true", {{token::e_true, "true"}}},
        {"false", {{token::e_false, "false"}}},
        {"null", {{token::e_null, "null"}}},

        // numbers
        {"1", {{token::e_int, "1"}}},
        {"12 ", {{token::e_int, "12"}}},
        {"3.14", {{token::e_float, "3.14"}}},
        {"2e7", {{token::e_float, "2e7"}}},
        {"23e+2", {{token::e_float, "23e+2"}}},
        {"23E-2", {{token::e_float, "23E-2"}}},
        {"-2", {{token::e_int, "-2"}}},
        {"+2.71", {{token::e_float, "+2.71"}}},

        // strings
        {"\"Klaas de Vries\"", {{token::e_string, "Klaas de Vries"}}},
        {"\"with \\\"quotes\\\"", {{token::e_string, "with \"quotes\""}}},
        {"\"\\\\\"", {{token::e_string, "\\"}}},
        {"\"/\"", {{token::e_string, "/"}}},
        {"\"\\b\"", {{token::e_string, "\b"}}},
        {"\"\\f\"", {{token::e_string, "\f"}}},
        {"\"\\n\"", {{token::e_string, "\n"}}},
        {"\"\\r\"", {{token::e_string, "\r"}}},
        {"\"\\t\"", {{token::e_string, "\t"}}},
        {"\"\\ud582\"", {{token::e_string, "\xd5\x82"}}},
        {"\"noot\"", {{token::e_string, "noot"}}},

        // skip whitespace
        {" \t", {token::e_eof}},
        {"\t{ ", {token::e_start_mapping}},
        {"\n[", {token::e_start_sequence}},

        // serial
        {"3 ,{\t}\"blah\" 2.72", {{token::e_int, "3"}, token::e_separator, token::e_start_mapping, token::e_end_mapping, {token::e_string, "blah"}, {token::e_float, "2.72"}, token::e_eof}},
        {"{\"aap\": \"noot\"}\n", {token::e_start_mapping, {token::e_string, "aap"}, token::e_mapper, {token::e_string, "noot"}, token::e_end_mapping, token::e_eof}},

};

INSTANTIATE_TEST_CASE_P(test_tokenizer,
                        tokenizer_test,
                        testing::ValuesIn(tokenizer_testcases));
