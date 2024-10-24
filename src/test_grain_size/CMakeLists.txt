cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

file(GLOB SOURCES "*.cc")

set(target_name1 "test_grain_size")
add_executable(${target_name1} ${SOURCES})

target_link_libraries(${target_name1} PRIVATE TBB::tbb)
target_compile_options(${target_name1} PRIVATE -fno-omit-frame-pointer -fno-optimize-sibling-calls -ggdb)

# target_compile_options(${target_name1} PRIVATE -fprefetch-loop-arrays)
