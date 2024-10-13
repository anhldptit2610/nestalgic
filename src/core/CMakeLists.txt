add_library(nestalgic_core SHARED)

add_subdirectory(cpu)

target_include_directories(nestalgic_core PUBLIC 
    ${CMAKE_SOURCE_DIR}/src/include/
)
