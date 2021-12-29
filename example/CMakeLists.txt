
cmake_minimum_required(VERSION 3.13...3.18)

project(examples)

add_executable(simple simple.c ../jsmn.h)
		 
add_executable(jsondump jsondump.c ../jsmn.h)

add_custom_target(fmt clang-format -i jsmn.h test/*.[ch] example/*.[ch])

add_custom_target(lint clang-tidy jsmn.h --checks='*')

set_target_properties(
    simple jsondump
    PROPERTIES 
        SUFFIX ".exe"
)
