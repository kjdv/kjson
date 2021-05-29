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

    builder(stream, true).with_none().flush();
    stream << '|';
    builder(stream, true).with_bool(true).flush();
    stream << '|';
    builder(stream, true).with_int(-1).flush();
    stream << '|';
    builder(stream, true).with_uint(1).flush();
    stream << '|';
    builder(stream, true).with_float(3.14).flush();
    stream << '|';
    builder(stream, true).with_string("foo").flush();
    stream << '|';

    EXPECT_EQ(R"(null|true|-1|1|3.1400000000000001|"foo"|)", stream.str());
}

TEST(builder, type_deduction) {
    ostringstream stream;

    builder(stream, true).value(none_standin{}).flush();
    stream << '|';
    builder(stream, true).value(true).flush();
    stream << '|';
    builder(stream, true).value(1).flush();
    stream << '|';
    builder(stream, true).value(static_cast<unsigned>(-1)).flush();
    stream << '|';
    builder(stream, true).value(3.14).flush();
    stream << '|';
    builder(stream, true).value("foo").flush();
    stream << '|';

    EXPECT_EQ(R"(null|true|1|4294967295|3.1400000000000001|"foo"|)", stream.str());
}

TEST(builder, in_sequence) {
    ostringstream stream;

    builder(stream, false)
            .push_sequence()
            .with_none()
            .with_bool(true)
            .with_bool(false)
            .with_int(123)
            .with_int(-123)
            .with_uint(0xffffffffffffffff)
            .with_float(3.14)
            .with_string(R"(foo"bar")")
            .flush();

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
            .push_sequence()
            .with_none()
            .with_bool(true)
            .with_bool(false)
            .with_int(123)
            .with_int(-123)
            .with_uint(0xffffffffffffffff)
            .with_float(3.14)
            .with_string(R"(foo"bar")")
            .flush();

    EXPECT_EQ(R"([null,true,false,123,-123,18446744073709551615,3.1400000000000001,"foo\"bar\""])", stream.str());
}

TEST(builder, in_mapping) {
    ostringstream stream;

    builder(stream, false)
            .push_mapping()
            .key("none").with_none()
            .key("b1").with_bool(true)
            .key("b2").with_bool(false)
            .key("i").with_int(123)
            .key("u").with_uint(0xffffffffffffffff)
            .key("d").with_float(3.14)
            .key("s").with_string(R"(foo"bar")")
            .flush();

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
            .push_mapping()
            .key("none").with_none()
            .key("b1").with_bool(true)
            .key("b2").with_bool(false)
            .key("i").with_int(123)
            .key("u").with_uint(0xffffffffffffffff)
            .key("d").with_float(3.14)
            .key("s").with_string(R"(foo"bar")")
            .flush();

    EXPECT_EQ(R"({"none":null,"b1":true,"b2":false,"i":123,"u":18446744073709551615,"d":3.1400000000000001,"s":"foo\"bar\""})", stream.str());
}

TEST(builder, sequence_in_sequence) {
    ostringstream stream;

    builder(stream, true)
            .push_sequence()
            .with_int(1)
            .push_sequence()
            .with_int(2)
            .with_int(3)
            .pop()
            .with_int(4)
            .flush();

    EXPECT_EQ(R"([1,[2,3],4])", stream.str());
}

TEST(builder, map_in_map) {
    ostringstream stream;

    builder(stream, true)
            .push_mapping()
            .key("a").value(1)
            .key("m").push_mapping()
            .key("b").value(2)
            .key("c").value(3)
            .pop()
            .key("d").value(4)
            .flush();

    EXPECT_EQ(R"({"a":1,"m":{"b":2,"c":3},"d":4})", stream.str());
}

TEST(builder, map_in_sequence) {
    ostringstream stream;

    builder(stream, true)
            .push_sequence()
            .value(1)
            .push_mapping()
            .key("a").value(2)
            .key("b").value(3)
            .pop()
            .value(4)
            .flush();

    EXPECT_EQ(R"([1,{"a":2,"b":3},4])", stream.str());
}

TEST(builder, sequence_in_map) {
    ostringstream stream;

    builder(stream, true)
            .push_mapping()
            .key("a").value(1)
            .key("s").push_sequence()
            .value(2)
            .value(3)
            .pop()
            .key("b").value(4)
            .flush();

    EXPECT_EQ(R"({"a":1,"s":[2,3],"b":4})", stream.str());
}

TEST(builder, pretty) {
    ostringstream stream;

    builder(stream, false)
            .push_mapping()
            .key("a").value(1)
            .key("s").push_sequence()
            .value(2)
            .value(3)
            .pop()
            .key("b").value(4)
            .key("m").push_mapping()
            .key("c").value(5)
            .key("d").value(6)
            .flush();

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

    EXPECT_THROW(builder(stream).key("k"), builder_error);
    EXPECT_THROW(builder(stream).push_sequence().key("k"), builder_error);
    EXPECT_THROW(builder(stream).push_mapping().value(1), builder_error);
}

}
}
