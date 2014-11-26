cmake_minimum_required (VERSION 2.6)
project (spatial_examples)

if (CMAKE_COMPILER_IS_GNUCXX)
  set (CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wall -Wextra -Wuninitialized")
  set (CMAKE_EXE_LINKER_FLAGS_DEBUG "-O0 -g")
  set (CMAKE_CXX_FLAGS_RELEASE "-O4")
  set (CMAKE_EXE_LINKER_FLAGS_RELEASE "-O4")
  set (CMAKE_CXX_FLAGS_PROFILE "-O2 -g -pg")
  set (CMAKE_EXE_LINKER_FLAGS_PROFILE "-g -pg")
  set (CMAKE_CXX_FLAGS_COVER "-O0 -g -fprofile-arcs -ftest-coverage")
  set (CMAKE_EXE_LINKER_COVER "-fprofile-arcs -ftest-coverage")
endif()

# Basic examples
add_executable (runtime_container_rank basic/runtime_container_rank.cpp)

# Performance examples
add_executable (insert_performance performance/insert_performance.cpp)
add_executable (erase_performance performance/erase_performance.cpp)
add_executable (find_performance performance/find_performance.cpp)
add_executable (mapping_performance performance/mapping_performance.cpp)
add_executable (iterate_performance performance/iterate_performance.cpp)
