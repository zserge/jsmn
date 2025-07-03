
# This CMakeLists.txt file is used to import the jsmn library into your CMake based project.
# Use the add_subdirectory command to include this directory in your build and include jsmn in your target_link_libraries command.
# For example
# add_subdirectory(jsmn build) will import the jsmn library into your projects CMake build system.
# target_link_libraries(example jsmn) will link the jsmn library to your example target.
# You can then use #include "jsmn.h" in your source files to include the jsmn library.

add_library(jsmn INTERFACE)

target_include_directories(jsmn INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}
)
