#include "builder.hh"
#include <gtest/gtest.h>
#include <sstream>

// clang-format off

namespace kjson {
namespace {

struct none_standin{};

using namespace std;

TEST(builder, single_scalars) {
    ostringstream stream;

    builder(stream, true).with_none().unwrap()->flush();
    stream << '|';
    builder(stream, true).with_bool(true).unwrap()->flush();
    stream << '|';
    builder(stream, true).with_int(-1).unwrap()->flush();
    stream << '|';
    builder(stream, true).with_uint(1).unwrap()->flush();
    stream << '|';
    builder(stream, true).with_float(3.14).unwrap()->flush();
    stream << '|';
    builder(stream, true).with_string("foo").unwrap()->flush();
    stream << '|';

    EXPECT_EQ(R"(null|true|-1|1|3.1400000000000001|"foo"|)", stream.str());
}

TEST(builder, type_deduction) {
    ostringstream stream;

    builder(stream, true).with(none_standin{}).unwrap()->flush();
    stream << '|';
    builder(stream, true).with(true).unwrap()->flush();
    stream << '|';
    builder(stream, true).with(1).unwrap()->flush();
    stream << '|';
    builder(stream, true).with(static_cast<unsigned>(-1)).unwrap()->flush();
    stream << '|';
    builder(stream, true).with(3.14).unwrap()->flush();
    stream << '|';
    builder(stream, true).with("foo").unwrap()->flush();
    stream << '|';

    EXPECT_EQ(R"(null|true|1|4294967295|3.1400000000000001|"foo"|)", stream.str());
}

TEST(builder, in_sequence) {
    ostringstream stream;

    builder(stream, false)
            .push_sequence().unwrap()
            ->with_none().unwrap()
            ->with_bool(true).unwrap()
            ->with_bool(false).unwrap()
            ->with_int(123).unwrap()
            ->with_int(-123).unwrap()
            ->with_uint(0xffffffffffffffff).unwrap()
            ->with_float(3.14).unwrap()
            ->with_string(R"(foo"bar")").unwrap()
            ->flush();

    EXPECT_EQ(R"([
  null,
  true,
  false,
  123,
  -123,
  18446744073709551615,
  3.1400000000000001,
  "foo\"bar\""
])", stream.str());
}

TEST(builder, in_sequence_compact) {
    ostringstream stream;

    builder(stream, true)
            .push_sequence().unwrap()
            ->with_none().unwrap()
            ->with_bool(true).unwrap()
            ->with_bool(false).unwrap()
            ->with_int(123).unwrap()
            ->with_int(-123).unwrap()
            ->with_uint(0xffffffffffffffff).unwrap()
            ->with_float(3.14).unwrap()
            ->with_string(R"(foo"bar")").unwrap()
            ->flush();

    EXPECT_EQ(R"([null,true,false,123,-123,18446744073709551615,3.1400000000000001,"foo\"bar\""])", stream.str());
}

TEST(builder, in_mapping) {
    ostringstream stream;

    builder(stream, false)
            .push_mapping().unwrap()
            ->key("none").unwrap()->with_none().unwrap()
            ->key("b1").unwrap()->with_bool(true).unwrap()
            ->key("b2").unwrap()->with_bool(false).unwrap()
            ->key("i").unwrap()->with_int(123).unwrap()
            ->key("u").unwrap()->with_uint(0xffffffffffffffff).unwrap()
            ->key("d").unwrap()->with_float(3.14).unwrap()
            ->key("s").unwrap()->with_string(R"(foo"bar")").unwrap()
            ->flush();

    EXPECT_EQ(R"({
  "none": null,
  "b1": true,
  "b2": false,
  "i": 123,
  "u": 18446744073709551615,
  "d": 3.1400000000000001,
  "s": "foo\"bar\""
})", stream.str());
}

TEST(builder, in_mapping_compact) {
    ostringstream stream;

    builder(stream, true)
            .push_mapping().unwrap()
            ->key("none").unwrap()->with_none().unwrap()
            ->key("b1").unwrap()->with_bool(true).unwrap()
            ->key("b2").unwrap()->with_bool(false).unwrap()
            ->key("i").unwrap()->with_int(123).unwrap()
            ->key("u").unwrap()->with_uint(0xffffffffffffffff).unwrap()
            ->key("d").unwrap()->with_float(3.14).unwrap()
            ->key("s").unwrap()->with_string(R"(foo"bar")").unwrap()
            ->flush();

    EXPECT_EQ(R"({"none":null,"b1":true,"b2":false,"i":123,"u":18446744073709551615,"d":3.1400000000000001,"s":"foo\"bar\""})", stream.str());
}

TEST(builder, sequence_in_sequence) {
    ostringstream stream;

    builder(stream, true)
            .push_sequence().unwrap()
            ->with_int(1).unwrap()
            ->push_sequence().unwrap()
            ->with_int(2).unwrap()
            ->with_int(3).unwrap()
            ->pop().unwrap()
            ->with_int(4).unwrap()
            ->flush();

    EXPECT_EQ(R"([1,[2,3],4])", stream.str());
}

TEST(builder, map_in_map) {
    ostringstream stream;

    builder(stream, true)
            .push_mapping().unwrap()
            ->key("a").unwrap()->with(1).unwrap()
            ->key("m").unwrap()->push_mapping().unwrap()
            ->key("b").unwrap()->with(2).unwrap()
            ->key("c").unwrap()->with(3).unwrap()
            ->pop().unwrap()
            ->key("d").unwrap()->with(4).unwrap()
            ->flush();

    EXPECT_EQ(R"({"a":1,"m":{"b":2,"c":3},"d":4})", stream.str());
}

TEST(builder, map_in_sequence) {
    ostringstream stream;

    builder(stream, true)
            .push_sequence().unwrap()
            ->with(1).unwrap()
            ->push_mapping().unwrap()
            ->key("a").unwrap()->with(2).unwrap()
            ->key("b").unwrap()->with(3).unwrap()
            ->pop().unwrap()
            ->with(4).unwrap()
            ->flush();

    EXPECT_EQ(R"([1,{"a":2,"b":3},4])", stream.str());
}

TEST(builder, sequence_in_map) {
    ostringstream stream;

    builder(stream, true)
            .push_mapping().unwrap()
            ->key("a").unwrap()->with(1).unwrap()
            ->key("s").unwrap()->push_sequence().unwrap()
            ->with(2).unwrap()
            ->with(3).unwrap()
            ->pop().unwrap()
            ->key("b").unwrap()->with(4).unwrap()
            ->flush();

    EXPECT_EQ(R"({"a":1,"s":[2,3],"b":4})", stream.str());
}

TEST(builder, pretty) {
    ostringstream stream;

    builder(stream, false)
            .push_mapping().unwrap()
            ->key("a").unwrap()->with(1).unwrap()
            ->key("s").unwrap()->push_sequence().unwrap()
            ->with(2).unwrap()
            ->with(3).unwrap()
            ->pop().unwrap()
            ->key("b").unwrap()->with(4).unwrap()
            ->key("m").unwrap()->push_mapping().unwrap()
            ->key("c").unwrap()->with(5).unwrap()
            ->key("d").unwrap()->with(6).unwrap()
            ->flush();

    EXPECT_EQ(R"({
  "a": 1,
  "s": [
    2,
    3
  ],
  "b": 4,
  "m": {
    "c": 5,
    "d": 6
  }
})", stream.str());
}

TEST(builder, check_bad_key) {
    ostringstream stream;

    EXPECT_TRUE(builder(stream).key("k").is_err());
    EXPECT_TRUE(builder(stream).push_sequence().unwrap()->key("k").is_err());
    EXPECT_TRUE(builder(stream).push_mapping().unwrap()->with(1).is_err());
}

}
}
