cmake_minimum_required (VERSION 2.6)
project (spatial_examples)

if (DEFINED PROFILE AND CMAKE_COMPILER_IS_GNUCXX)
     set (CMAKE_CXX_FLAGS "-O4")
#     set (CMAKE_CXX_FLAGS "-O0 -pg")
#     set (CMAKE_EXE_LINKER_FLAGS "-pg")
endif()

# Basic examples
add_executable (runtime_container_rank basic/runtime_container_rank.cpp)

# Performance examples
add_executable (insert_performance performance/insert_performance.cpp)
add_executable (erase_performance performance/erase_performance.cpp)
add_executable (find_performance performance/find_performance.cpp)
add_executable (mapping_performance performance/mapping_performance.cpp)
