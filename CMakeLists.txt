cmake_minimum_required(VERSION 3.20)
project(jotter C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra)

add_executable(jotter main.c notes_handler.c)

enable_testing()

add_executable(test_notes_handler tests/test_notes_handler.c notes_handler.c)
add_test(NAME notes_handler_tests COMMAND test_notes_handler
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
