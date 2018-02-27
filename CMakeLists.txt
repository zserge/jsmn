cmake_minimum_required(VERSION 3.9)
project(jsmn)

set(CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(LIBRARY_NAME ${PROJECT_NAME})

function(MAKE_TEST ARG_TEST_NAME)
    add_executable(${ARG_TEST_NAME} ${ARGN})
    target_link_libraries(${ARG_TEST_NAME} ${LIBRARY_NAME})
    add_test(NAME ${ARG_TEST_NAME} COMMAND ${ARG_TEST_NAME})
endfunction()

set(JSMN_SOURCES jsmn.c)
set(JSMN_PUBLIC_INCLUDES jsmn.h)

add_library(${LIBRARY_NAME} STATIC ${JSMN_SOURCES})

if (DEFINED BUILD_EXAMPLE)
    add_executable(simple_example example/simple.c ${LIBRARY_NAME})
    add_executable(jsondump example/jsondump.c ${LIBRARY_NAME})
endif ()

if(DEFINED BUILD_TESTS)
    MAKE_TEST(jsmn_test test/tests.c)
    enable_testing()
endif()

install(TARGETS ${LIBRARY_NAME} ARCHIVE DESTINATION ${PROJECT_BINARY_DIR}/lib)
install(FILES ${JSMN_PUBLIC_INCLUDES} DESTINATION ${PROJECT_BINARY_DIR}/include)
