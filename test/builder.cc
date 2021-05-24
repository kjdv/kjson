#include "builder.hh"
#include <gtest/gtest.h>
#include <sstream>

// clang-format off

namespace kjson {
namespace {

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

TEST(builder, in_sequence) {
    ostringstream stream;

    builder(stream, false)
            .with_sequence().unwrap()
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
            .with_sequence().unwrap()
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
            .with_mapping().unwrap()
            ->with_none("none").unwrap()
            ->with_bool("b1", true).unwrap()
            ->with_bool("b2", false).unwrap()
            ->with_int("i", 123).unwrap()
            ->with_uint("u", 0xffffffffffffffff).unwrap()
            ->with_float("d", 3.14).unwrap()
            ->with_string("s", R"(foo"bar")").unwrap()
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
            .with_mapping().unwrap()
            ->with_none("none").unwrap()
            ->with_bool("b1", true).unwrap()
            ->with_bool("b2", false).unwrap()
            ->with_int("i", 123).unwrap()
            ->with_uint("u", 0xffffffffffffffff).unwrap()
            ->with_float("d", 3.14).unwrap()
            ->with_string("s", R"(foo"bar")").unwrap()
            ->flush();

    EXPECT_EQ(R"({"none":null,"b1":true,"b2":false,"i":123,"u":18446744073709551615,"d":3.1400000000000001,"s":"foo\"bar\""})", stream.str());
}

TEST(builder, sequence_in_sequence) {
    ostringstream stream;

    builder(stream, true)
            .with_sequence().unwrap()
            ->with_int(1).unwrap()
            ->with_sequence().unwrap()
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
            .with_mapping().unwrap()
            ->with_int("a", 1).unwrap()
            ->with_mapping("m").unwrap()
            ->with_int("b", 2).unwrap()
            ->with_int("c", 3).unwrap()
            ->pop().unwrap()
            ->with_int("d", 4).unwrap()
            ->flush();

    EXPECT_EQ(R"({"a":1,"m":{"b":2,"c":3},"d":4})", stream.str());
}

TEST(builder, map_in_sequence) {
    ostringstream stream;

    builder(stream, true)
            .with_sequence().unwrap()
            ->with_int(1).unwrap()
            ->with_mapping().unwrap()
            ->with_int("a", 2).unwrap()
            ->with_int("b", 3).unwrap()
            ->pop().unwrap()
            ->with_int(4).unwrap()
            ->flush();

    EXPECT_EQ(R"([1,{"a":2,"b":3},4])", stream.str());
}

TEST(builder, sequence_in_map) {
    ostringstream stream;

    builder(stream, true)
            .with_mapping().unwrap()
            ->with_int("a", 1).unwrap()
            ->with_sequence("s").unwrap()
            ->with_int(2).unwrap()
            ->with_int(3).unwrap()
            ->pop().unwrap()
            ->with_int("b", 4).unwrap()
            ->flush();

    EXPECT_EQ(R"({"a":1,"s":[2,3],"b":4})", stream.str());
}

TEST(builder, pretty) {
    ostringstream stream;

    builder(stream, false)
            .with_mapping().unwrap()
            ->with_int("a", 1).unwrap()
            ->with_sequence("s").unwrap()
            ->with_int(2).unwrap()
            ->with_int(3).unwrap()
            ->pop().unwrap()
            ->with_int("b", 4).unwrap()
            ->with_mapping("m").unwrap()
            ->with_int("c", 5).unwrap()
            ->with_int("d", 6).unwrap()
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


}
}
