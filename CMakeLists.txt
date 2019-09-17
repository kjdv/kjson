cmake_minimum_required(VERSION 3.10)

project(kjson)
set(CMAKE_CXX_STANDARD 17)

include(GoogleTest)
find_package(GTest MODULE REQUIRED)
find_library(GMOCK_LIBRARIES gmock)
enable_testing()


# dependencies:
find_package(Results CONFIG REQUIRED)
find_package(Kb64 CONFIG REQUIRED)
find_package(Composite CONFIG REQUIRED)


add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(examples)

# install rules
install(TARGETS kjson
        EXPORT KjsonTargets
        ARCHIVE DESTINATION lib COMPONENT lib
        PUBLIC_HEADER DESTINATION include/kjson COMPONENT dev
)
install(FILES KjsonConfig.cmake DESTINATION share/kjson)

install(EXPORT KjsonTargets DESTINATION share/kjson NAMESPACE Kjson::)
export(TARGETS kjson NAMESPACE Kjson:: FILE ${CMAKE_CURRENT_BINARY_DIR}/KjsonTargets.cmake)
