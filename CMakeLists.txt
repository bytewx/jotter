cmake_minimum_required(VERSION 3.20)
project(jotter C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

add_executable(jotter main.c jotter.c)

enable_testing()

add_executable(test_jotter tests/test_jotter.c jotter.c)
add_test(NAME jotter_tests COMMAND test_jotter
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
