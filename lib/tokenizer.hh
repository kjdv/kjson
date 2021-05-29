#pragma once

#include <iosfwd>
#include <results/result.hh>
#include <stack>
#include <string>

namespace kjson {

template <typename T>
using token_error = typename results::result<T, results::error>;

struct token {
    enum class type_t {
        e_start_mapping,  // {
        e_end_mapping,    // }
        e_start_sequence, // [
        e_end_sequence,   // ]
        e_separator,      // ,
        e_mapper,         // :
        e_string,
        e_int,
        e_uint,
        e_float,
        e_true,  // true
        e_false, // false
        e_null,  // null
        e_eof,
    };

    type_t      tok{type_t::e_eof};
    std::string value{};
};

token_error<token> next_token(std::istream& input);
} // namespace kjson
