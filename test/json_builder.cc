#include "json_builder.hh"
#include "json.hh"
#include <composite/composite.hh>
#include <composite/make.hh>
#include <gtest/gtest.h>
#include <sstream>

namespace kjson {
namespace {

using namespace std;
using namespace composite;

struct json_testcase
{
  const string input;
  const string output;
};

inline ostream& operator<<(ostream& out, json_testcase const& tc)
{
  return out << "input: " << tc.input << "\noutput:" << tc.output << '\n';
}

class json_builder_test : public testing::TestWithParam<json_testcase>
{
  document d_data;

public:
  json_builder_test()
    : d_data(load(GetParam().input).unwrap())
  {
  }

  document const& data() const
  {
    return d_data;
  }
};

TEST_P(json_builder_test, writing)
{
  ostringstream stream;
  json_builder  jb(stream);
  data().visit(jb);

  EXPECT_EQ(GetParam().output, stream.str());
}

TEST_P(json_builder_test, reading)
{
  ostringstream stream;
  json_builder  jb(stream);
  data().visit(jb);

  auto actual = load(stream.str());
  EXPECT_EQ(data(), actual.unwrap());
}

json_testcase json_testcases[] = {
    {"{}",
     "{\n  \n}"},
    {"{\"aap\": \"noot\"}",
     "{\n"
     "  \"aap\": \"noot\"\n"
     "}"},
    {"{\"list\": [1, 2, 3]}\n",
     "{\n"
     "  \"list\": [\n"
     "    1,\n"
     "    2,\n"
     "    3\n"
     "  ]\n"
     "}"},
    {
        "{\"map\": {\"one\": 1, }}\n",
        "{\n"
        "  \"map\": {\n"
        "    \"one\": 1\n"
        "  }\n"
        "}",
    },
    {"{\"nested_list\": [1,[2,3]]}\n",
     "{\n"
     "  \"nested_list\": [\n"
     "    1,\n"
     "    [\n"
     "      2,\n"
     "      3\n"
     "    ]\n"
     "  ]\n"
     "}"},
    {"[{\"key\" : 1}, {\"key\" : 2}]",
     "[\n"
     "  {\n"
     "    \"key\": 1\n"
     "  },\n"
     "  {\n"
     "    \"key\": 2\n"
     "  }\n"
     "]"}};

INSTANTIATE_TEST_SUITE_P(test_json_builder,
                         json_builder_test,
                         testing::ValuesIn(json_testcases));

} // namespace
} // namespace kjson
