cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(perf_vtune)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard to be used")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE STRING "C++ standard to be used")
set(CMAKE_CONFIGURATION_TYPES Release Debug CACHE INTERNAL "" FORCE)

set(CMAKE_POLICY_DEFAULT_CMP0079 NEW)

find_package(TBB CONFIG REQUIRED)
message(STATUS "TBB version: ${TBB_VERSION}")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")

add_subdirectory(src)
