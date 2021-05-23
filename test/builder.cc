#include <gtest/gtest.h>
#include <sstream>
#include "builder.hh"

// clang-format off

namespace kjson {
namespace {

using namespace std;

TEST(builder, in_sequence) {
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
            ->flush().unwrap();

    EXPECT_EQ(R"([null,true,false,123,-123,18446744073709551615,3.14,"foo\"bar\""])", stream.str());
}


}
}
