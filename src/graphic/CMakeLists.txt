add_library(graphic STATIC)

target_sources(graphic PRIVATE
    graphic.cc
    sdl.cc
    ui.cc
)

target_link_libraries(graphic PRIVATE
    SDL2
    imgui
)

target_include_directories(graphic PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/imgui)
target_include_directories(graphic PUBLIC ${CMAKE_SOURCE_DIR}/src/core/)