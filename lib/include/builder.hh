#pragma once

#include <memory>
#include <string_view>
#include <cstdint>
#include <iosfwd>
#include <results/result.hh>

namespace kjson {

class builder {
public:
    using result = results::result<builder *>;

    explicit builder(std::ostream &out, bool compact = true);
    ~builder();

    result with_none();
    result with_none(std::string_view key);

    result with_bool(bool v);
    result with_bool(std::string_view key, bool v);

    result with_int(int64_t v);
    result with_int(std::string_view key, int64_t v);

    result with_uint(uint64_t v);
    result with_uint(std::string_view key, uint64_t v);

    result with_float(double v);
    result with_float(std::string_view key, double v);

    result with_string(std::string_view v);
    result with_string(std::string_view key, std::string_view v);

    result with_mapping();
    result with_mapping(std::string_view key);

    result with_sequence();
    result with_sequence(std::string_view key);

    result pop();

    result flush();

private:
    class impl;

    std::unique_ptr<impl> d_pimpl;
};

}
