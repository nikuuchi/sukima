CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(lisp)

set(CMAKE_C_FLAGS_RELEASE "-W -Wall -O2")
set(CMAKE_C_FLAGS_DEBUG "-W -Wall -g")
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR})

FIND_LIBRARY(READLINE_LIBRARY NAMES readline)
set(ExtraLibs ${ExtraLibs} ${READLINE_LIBRARY}) 

include_directories(${INCLUDE_DIRS})
link_directories(${READLINE_INCLUDE_DIR})

ADD_EXECUTABLE(lisp
	src/cons.c
	src/hash.c
	src/lex.c
	src/lisp.c
	src/parser.c
	src/codegen.c
	src/vm.c
)

target_link_libraries(lisp ${ExtraLibs})
