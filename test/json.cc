#include "json.hh"
#include <composite/make.hh>
#include <gtest/gtest.h>
#include <rapidcheck/gtest.h>
#include <algorithm>

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

template <typename T>
void check_marshalling(const T& orig, bool compact) {
    auto doc = make(T(orig));
    stringstream stream;
    dump(doc, stream, compact);

    auto actual = load(stream).expect("invalid json").to<T>();
    RC_ASSERT(orig == actual);
}

RC_GTEST_PROP(toplevel, marshalling_bool, (bool b, bool compact)) {
    check_marshalling(b, compact);
}

RC_GTEST_PROP(toplevel, marshalling_int, (int i, bool compact)) {
    check_marshalling(i, compact);
}

RC_GTEST_PROP(toplevel, marshalling_int64, (int64_t i, bool compact)) {
    check_marshalling(i, compact);
}

RC_GTEST_PROP(toplevel, marshalling_uint64, (uint64_t u, bool compact)) {
    check_marshalling(u, compact);
}

RC_GTEST_PROP(toplevel, marshalling_float, (double d, bool compact)) {
    check_marshalling(d, compact);
}

RC_GTEST_PROP(toplevel, marshalling_string, (string s, bool compact)) {
    check_marshalling(s, compact);
}

RC_GTEST_PROP(toplevel, marshalling_sequence, (vector<int> orig, bool compact)) {
    auto doc_as_seq = sequence();
    transform(orig.begin(), orig.end(), back_inserter(doc_as_seq), [](int i) { return make(i); });
    ::composite::composite doc(move(doc_as_seq));

    stringstream stream;
    dump(doc, stream, compact);

    auto seq = load(stream).expect("invalid json").as<sequence>();
    vector<int> actual;
    transform(seq.begin(), seq.end(), back_inserter(actual), [](const ::composite::composite& item) { return item.to<int>(); });
    RC_ASSERT(orig == actual);
}

RC_GTEST_PROP(toplevel, marshalling_mapping, (map<string, double> orig, bool compact)) {
    if (!all_of(orig.begin(), orig.end(), [](auto&& kv) { return !kv.first.empty(); })) {
        return;
    }

    auto doc_as_map = mapping();
    for (auto &&kv : orig) {
        doc_as_map.insert(make_pair(kv.first, make(kv.second)));
    }
    ::composite::composite doc(move(doc_as_map));

    stringstream stream;
    dump(doc, stream, compact);

    std::cerr << stream.str() << std::endl;

    auto m = load(stream).expect("invalid json").as<mapping>();
    map<string, double> actual;
    for (auto&& kv : m) {
        actual.insert(make_pair(kv.first, kv.second.to<double>()));
    }

    RC_ASSERT(orig == actual);
}

}
}
