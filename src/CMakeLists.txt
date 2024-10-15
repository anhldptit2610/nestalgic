add_subdirectory(core)
add_subdirectory(graphic)
add_subdirectory(testing)

add_executable(nestalgic main.cc)

target_link_libraries(nestalgic PRIVATE
    nestalgic_core
    imgui
    SDL2
)

target_include_directories(nestalgic PUBLIC ${CMAKE_SOURCE_DIR}/src/include)
target_include_directories(nestalgic PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/imgui/)