cmake_minimum_required(VERSION 3.25.1)

project(nestalgic C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(FetchContent)
include(GetYYJSON)

option(LOGGING "Enable logging" OFF)
option(NESTEST "Enable nestest related" OFF)

if (LOGGING)
    add_definitions(-DLOGGING=1)
endif ()

if (NESTEST)
    add_definitions(-DNESTEST=1)
endif ()

add_compile_options(-Wall)

add_subdirectory(thirdparty)
add_subdirectory(src)
add_subdirectory(utils)