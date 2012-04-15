CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(lisp)

SET(CMAKE_C_FLAGS "-Wall -O2 -g3")

set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${INCLUDE_DIRS})

ADD_EXECUTABLE(lisp
	src/cons.c
	src/hash.c
	src/lex.c
	src/lisp.c
	src/parser.c
	src/runtime.c
	src/vm.c
)

