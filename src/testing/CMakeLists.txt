add_executable(cpu_test
    main.cc
    cpu_test.cc
)

add_compile_options(-O3)

target_link_libraries(cpu_test PRIVATE
    nestalgic_core
    yyjson
)