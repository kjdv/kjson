#include <json/json.hh>
#include "json_builder.hh"
#include <composite/builder.hh>
#include <composite/composite.hh>
#include <gtest/gtest.h>
#include <sstream>

using namespace std;
using namespace kdv::composite;
using namespace kdv::json;

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
  composite_cptr d_data;

public:
  json_builder_test()
    : d_data(load(GetParam().input))
  {
    assert(d_data);
  }

  composite const& data() const
  {
    return *d_data;
  }
};

TEST_P(json_builder_test, writing)
{
  ostringstream stream;
  json_builder  jb(stream);
  data().accept(jb);

  EXPECT_EQ(GetParam().output, stream.str());
}

TEST_P(json_builder_test, reading)
{
  ostringstream stream;
  json_builder  jb(stream);
  data().accept(jb);

  composite_cptr actual = load(stream.str());
  ASSERT_TRUE((bool)actual);

  EXPECT_EQ(data(), *actual);
}

json_testcase json_testcases[] = {
    {"{}",
     "{\n  \n}\n"},
    {"{\"aap\": \"noot\"}\n",
     "{\n"
     "  \"aap\": \"noot\"\n"
     "}\n"},
    {"{\"list\": [1, 2, 3]}\n",
     "{\n"
     "  \"list\": [\n"
     "    1,\n"
     "    2,\n"
     "    3\n"
     "  ]\n"
     "}\n"},
    {
        "{\"map\": {\"one\": 1, }}\n",
        "{\n"
        "  \"map\": {\n"
        "    \"one\": 1\n"
        "  }\n"
        "}\n",
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
     "}\n"},
    {"[{\"key\" : 1}, {\"key\" : 2}]",
     "[\n"
     "  {\n"
     "    \"key\": 1\n"
     "  },\n"
     "  {\n"
     "    \"key\": 2\n"
     "  }\n"
     "]\n"}};

INSTANTIATE_TEST_CASE_P(test_json_builder,
                        json_builder_test,
                        testing::ValuesIn(json_testcases));

TEST(typehint, base64)
{
  const string plaintext = "Klaas was hier!\n";
  const string encoded   = "S2xhYXMgd2FzIGhpZXIhCg==";

  composite_cptr message =
      builder().push_mapping().with("binary", plaintext, scalar::e_binary).build();

  const string expected = string("{\n  \"binary\": \"") + encoded + "\"\n}\n";

  ostringstream stream;
  dump(*message, stream, false);

  EXPECT_EQ(expected, stream.str());
}
