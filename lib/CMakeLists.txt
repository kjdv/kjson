file(GLOB public_headers include/*.hh include/*.h)
file(GLOB sources *.cc *.cpp *.c *.hh *.h)

add_library(kjson
    ${public_headers}
    ${sources}
)

set_target_properties(kjson PROPERTIES PUBLIC_HEADER "${public_headers}")
target_include_directories(kjson
    PUBLIC
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
      "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
)


target_link_libraries(kjson
    PUBLIC Composite::composite
)

