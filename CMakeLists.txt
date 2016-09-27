PROJECT (jsmn)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

#Pretty colors
SET(CMAKE_COLOR_MAKEFILE ON)
#Force verbose
SET(CMAKE_VERBOSE_MAKEFILE ON)
#Include current dir
SET(CMAKE_INCLUDE_CURRENT_DIR TRUE)

OPTION(JSMN_LIB "Build dynamic libjsmn" ON)
OPTION(TSTFILE_DEFAULT "Build test files" OFF)
OPTION(TSTFILE_STRICT "Build test file with strict option" OFF)
OPTION(TSTFILE_PARENT "Build test file with parent links option" OFF)
OPTION(TSTFILE_PARENT_STRICT "Build test file with strict option and parent links option" OFF)
OPTION(EXAMPLE "Build example" OFF)
OPTION(DMP "Build jsondump" OFF)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

FILE(GLOB SRCFILES *.c)

IF(JSMN_LIB)
  SET(JSMN_LIB_TYPE SHARED)
ELSE(JSMN_LIB)
  SET(JSMN_LIB_TYPE STATIC)
ENDIF(JSMN_LIB)

add_library(${PROJECT_NAME} ${JSMN_LIB_TYPE} ${SRCFILES})

FILE(GLOB TSTFILES test/*.c)

IF(TSTFILE_DEFAULT)
	MESSAGE(STATUS "Building test files")
	add_executable(tests ${TSTFILES})
ENDIF(TSTFILE_DEFAULT)

IF(TSTFILE_STRICT)
	MESSAGE(STATUS "Building test file with strict options")
  	ADD_DEFINITIONS("-DJSMN_STRICT=1")
	add_executable(tests ${TSTFILES})
ENDIF(TSTFILE_STRICT)

IF(TSTFILE_PARENT)
        MESSAGE(STATUS "Building test file with parent links option")
        ADD_DEFINITIONS("-DJSMN_PARENT_LINKS=1")
        add_executable(tests ${TSTFILES})
ENDIF(TSTFILE_PARENT)

IF(TSTFILE_PARENT_STRICT)
	MESSAGE(STATUS "Build test file with strict option and parent links option")
	ADD_DEFINITIONS("-DJSMN_STRICT=1 -DJSMN_PARENT_LINKS=1")
	add_executable(tests ${TSTFILES})
ENDIF(TSTFILE_PARENT_STRICT)

FILE(GLOB SIMPLE example/simple.c)

IF (EXAMPLE) 
	MESSAGE(STATUS "Building example")
	add_executable(simple_example ${SIMPLE})
	target_link_libraries(simple_example ${PROJECT_NAME})
ENDIF(EXAMPLE)

FILE(GLOB DUMP example/jsondump.c)

IF(DMP)
	MESSAGE(STATUS "Building jsondump")
        add_executable(jsondump ${DUMP})
        target_link_libraries(jsondump ${PROJECT_NAME})

ENDIF(DMP)

if (NOT LIB_INSTALL_DIR)
    set(LIB_INSTALL_DIR lib)
endif ()

install (TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${LIB_INSTALL_DIR})
