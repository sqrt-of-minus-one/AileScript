cmake_minimum_required(VERSION 3.5)
project(ailescript)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_EXE src/main.cpp
	src/encoding.cpp
	src/Parser_char_lit.cpp src/Parser_float_lit.cpp src/Parser_int_lit.cpp src/Parser_next.cpp src/Parser_punct.cpp src/Parser_string_lit.cpp src/Parser_word.cpp src/Parser.cpp
	src/Token.cpp)

add_executable(ailescript ${SOURCE_EXE})
