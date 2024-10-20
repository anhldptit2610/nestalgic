add_library(nestalgic_core STATIC)

add_subdirectory(cpu)
add_subdirectory(mapper)

target_sources(nestalgic_core PRIVATE
    nestalgic.cc
    mmu.cc
    rom.cc
    ppu.cc
    logging.cc
    controller.cc
)

target_link_libraries(nestalgic_core PRIVATE
    graphic
)

target_include_directories(nestalgic_core PUBLIC 
    ${CMAKE_SOURCE_DIR}/src/core/include/
    ${CMAKE_SOURCE_DIR}/src/graphic/
)
