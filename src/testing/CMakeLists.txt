add_executable(cpu_test
    main.cc
    cpu_test.cc
)

add_compile_options(-O3)

target_link_libraries(cpu_test PRIVATE
    nestalgic_core
    yyjson
)

target_include_directories(cpu_test PUBLIC
    ${CMAKE_SOURCE_DIR}/src/core/)

add_executable(nestest_cmp
    nestest_cmp.cc)

target_link_libraries(nestest_cmp PRIVATE
    nestalgic_core
    imgui
    SDL2
)

target_include_directories(nestest_cmp PUBLIC
    ${CMAKE_SOURCE_DIR}/src/core/)