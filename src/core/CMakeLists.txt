add_library(nestalgic_core SHARED)

add_subdirectory(cpu)
add_subdirectory(mmu)
add_subdirectory(rom)
add_subdirectory(mapper)

target_sources(nestalgic_core PRIVATE
    nestalgic.cc)

target_include_directories(nestalgic_core PUBLIC 
    ${CMAKE_SOURCE_DIR}/src/include/
)
