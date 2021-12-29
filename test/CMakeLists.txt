
cmake_minimum_required(VERSION 3.13...3.18)

project(full_test)

add_executable(test_default tests.c ../jsmn.h)
		 
add_executable(test_strict tests.c ../jsmn.h)
target_compile_definitions(test_strict PUBLIC JSMN_STRICT)

add_executable(test_links tests.c ../jsmn.h)
target_compile_definitions(test_links PUBLIC JSMN_PARENT_LINKS)

add_executable(test_strict_links tests.c ../jsmn.h)
target_compile_definitions(test_strict_links PUBLIC JSMN_STRICT JSMN_PARENT_LINKS)

set_target_properties(
    test_default test_strict test_links test_strict_links
    PROPERTIES 
        SUFFIX ".exe"
)

enable_testing()
add_test(NAME running_test_default COMMAND test_default)
add_test(NAME running_test_strict COMMAND test_strict)
add_test(NAME running_test_links COMMAND test_links)
add_test(NAME running_test_strict_links COMMAND test_strict_links)
