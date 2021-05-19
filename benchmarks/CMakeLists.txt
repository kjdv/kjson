add_executable(benchmarks benchmarks.cpp)
target_link_libraries(benchmarks PUBLIC kjson benchmark::benchmark benchmark::benchmark_main)
add_test(benchmarks.test benchmarks)
