#if 0
#include <json/json.hh>
#include "json_compact_builder.hh"
#include <composite/builder.hh>
#include <composite/composite.hh>
#include <gtest/gtest.h>
#include <sstream>

using namespace std;
using namespace kdv::composite;
using namespace kdv::json;

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
  composite_cptr d_data;

public:
  json_compact_builder_test()
    : d_data(load(GetParam().input))
  {
    assert(d_data);
  }

  composite const& data() const
  {
    return *d_data;
  }
};

TEST_P(json_compact_builder_test, writing)
{
  ostringstream        stream;
  json_compact_builder jb(stream);
  data().accept(jb);

  EXPECT_EQ(GetParam().output, stream.str());
}

TEST_P(json_compact_builder_test, reading)
{
  ostringstream        stream;
  json_compact_builder jb(stream);
  data().accept(jb);

  composite_cptr actual = load(stream.str());
  ASSERT_TRUE((bool)actual);

  EXPECT_EQ(data(), *actual);
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

INSTANTIATE_TEST_CASE_P(test_json_compact_builder,
                        json_compact_builder_test,
                        testing::ValuesIn(json_testcases));
}
#endif
