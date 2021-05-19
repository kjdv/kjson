#include <benchmark/benchmark.h>
#include <type_traits>
#include <json.hh>
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
