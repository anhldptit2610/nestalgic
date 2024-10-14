add_executable(nestalgic main.cc)

target_link_libraries(nestalgic PRIVATE nestalgic_core)

target_include_directories(nestalgic PUBLIC ${CMAKE_SOURCE_DIR}/src/include)