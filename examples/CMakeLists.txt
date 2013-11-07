cmake_minimum_required (VERSION 2.6)
project (spatial_examples)

# Basic examples
add_executable (runtime_container_rank basic/runtime_container_rank.cpp)

# Performance examples
add_executable (insert_performance performance/insert_performance.cpp)
