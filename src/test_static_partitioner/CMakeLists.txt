cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

file(GLOB SOURCES "*.cc")

set(target_name1 "test_perf_vtune_static_partitioner")
add_executable(${target_name1} ${SOURCES})

target_link_libraries(${target_name1} PRIVATE TBB::tbb)
target_compile_options(${target_name1} PRIVATE -fno-omit-frame-pointer -fno-optimize-sibling-calls -ggdb)

# target_compile_definitions(${target_name1} PRIVATE CL_TARGET_OPENCL_VERSION=${CL_TARGET_VERSION})
