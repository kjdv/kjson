file(GLOB sources *.c *.cc *.cpp *.h *.hh)

add_executable(kjson_test ${sources})
target_link_libraries(kjson_test kjson ${GMOCK_LIBRARIES} GTest::GTest GTest::Main rapidcheck)

gtest_discover_tests(kjson_test)
