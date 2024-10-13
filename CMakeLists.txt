cmake_minimum_required(VERSION 3.25.1)

project(nestalgic C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GetYYJSON)

add_compile_options(-Wall)

add_subdirectory(src)