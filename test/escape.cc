#include "escape.hh"
#include <gtest/gtest.h>

namespace kjson {
namespace {

using namespace std;

struct escape_testcase {
    string input;
    string expected;
};

inline ostream& operator<<(ostream& o, escape_testcase const& et) {
    return o << "'" << et.input << "', '" << et.expected << "'";
}

class escape_test : public testing::TestWithParam<escape_testcase> {
};

TEST_P(escape_test, extract_and_append) {
    string const& input    = GetParam().input;
    string const& expected = GetParam().expected;

    string actual = escape(input);

    EXPECT_EQ(expected, actual);
}

escape_testcase escape_testcases[] =
    {
        {"a", "\"a\""},
        {"b", "\"b\""},
        {"multi char", "\"multi char\""},
        {"with 'quotes'", "\"with 'quotes'\""},
        {"with \"dquotes\"", "\"with \\\"dquotes\\\"\""},
        {"a\\", "\"a\\\\\""},
        {"/", "\"\\/\""},
        {"\b", "\"\\b\""},
        {"\f", "\"\\f\""},
        {"\n", "\"\\n\""},
        {"\r", "\"\\r\""},
        {"\t", "\"\\t\""},
        {"\r\n", "\"\\r\\n\""},
        {"\ta\bc\fd", "\"\\ta\\bc\\fd\""},
        {"\xd5\x82", "\"\xd5\x82\""},
        {"\xf0\x9d\x84\x8b", "\"\xf0\x9d\x84\x8b\""},
};

INSTANTIATE_TEST_SUITE_P(escape_tests,
                         escape_test,
                         testing::ValuesIn(escape_testcases));

} // namespace
} // namespace kjson
