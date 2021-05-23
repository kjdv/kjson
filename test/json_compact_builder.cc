#include "json.hh"
#include "json_builder.hh"
#include <composite/composite.hh>
#include <composite/make.hh>
#include <gtest/gtest.h>
#include <sstream>

namespace kjson {
namespace {

using namespace std;

namespace {

struct json_testcase
{
  const string input;
  const string output;
};

inline ostream& operator<<(ostream& out, json_testcase const& tc)
{
  return out << "input: " << tc.input << "\noutput:" << tc.output << '\n';
}

class json_compact_builder_test : public testing::TestWithParam<json_testcase>
{
  document d_data;

public:
  json_compact_builder_test()
    : d_data(load(GetParam().input).unwrap())
  {
  }

  document const& data() const
  {
    return d_data;
  }
};

TEST_P(json_compact_builder_test, writing)
{
  ostringstream stream;
  json_builder  jb(stream, true);
  data().visit(jb);

  EXPECT_EQ(GetParam().output, stream.str());
}

TEST_P(json_compact_builder_test, reading)
{
  ostringstream stream;
  json_builder  jb(stream, true);
  data().visit(jb);

  auto actual = load(stream.str());

  EXPECT_EQ(data(), actual.unwrap());
}

json_testcase json_testcases[] = {
    {"{}",
     "{}"},
    {"{\"aap\": \"noot\"}\n",
     "{\"aap\":\"noot\"}"},
    {"{\"list\": [1, 2, 3]}\n",
     "{\"list\":[1,2,3]}"},
    {"{\"map\": {\"one\": 1, }}\n",
     "{\"map\":{\"one\":1}}"},
    {"{\"nested_list\": [1,[2,3]]}\n",
     "{\"nested_list\":[1,[2,3]]}"},
    {"[{\"key\" : 1}, {\"key\" : 2}]",
     "[{\"key\":1},{\"key\":2}]"}};

INSTANTIATE_TEST_SUITE_P(test_json_compact_builder,
                         json_compact_builder_test,
                         testing::ValuesIn(json_testcases));
} // namespace

} // namespace
} // namespace kjson
