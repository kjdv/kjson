#include <benchmark/benchmark.h>
#include <type_traits>
#include "json.hh"
#include "visitor.hh"
#include <fstream>
#include <sstream>

namespace kjson {
namespace {

constexpr const char sample[] = R"({
    "key": "value",
    "n": 123,
    "x": 3.14,
    "b": true,
    "v": null,
    "list": [
        "string",
        1,
        false,
        { "pi": 3.14, "e": 2.71 }
    ],
    "foo": "bar"
})";

class null_visitor : public visitor {
public:
    void scalar(scalar_t) override
    {}
    void scalar(std::string_view, scalar_t) override
    {}

    void push_mapping() override
    {}
    void push_mapping(std::string_view) override
    {}

    void push_sequence() override
    {}
    void push_sequence(std::string_view) override
    {}

    void pop() override
    {}
};

document sample_as_doc() {
    return load(sample).expect("invalid json");
}

void bm_load(benchmark::State &state) {
    std::istringstream stream{sample};

    for (auto _ : state) {
        load(stream).expect("valid json");
        stream.clear();
        stream.seekg(0);
    }
}

BENCHMARK(bm_load);

void bm_load_parse_only(benchmark::State &state) {
    std::istringstream stream{sample};
    null_visitor v;

    for (auto _ : state) {
        load(stream, v).expect("valid json");
        stream.clear();
        stream.seekg(0);
    }
}

BENCHMARK(bm_load_parse_only);

void bm_dump(benchmark::State &state) {
    auto doc = sample_as_doc();
    std::ofstream out("/dev/null");

    for (auto _ : state) {
        dump(doc, out);
    }
}

BENCHMARK(bm_dump);

}
}

BENCHMARK_MAIN();
