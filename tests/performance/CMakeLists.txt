cmake_minimum_required (VERSION 2.8.4)
project (spatial_perfs)

option (USE_LIBCXX "Force libc++ with clang++?" ON)
option (USE_CXX11  "Force use of c++11?" OFF)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND USE_LIBCXX)
  message (STATUS "Building with Clang and libc++ ABI for C++11")
  if ("${CMAKE_SYSTEM}" MATCHES "Linux")
    set (COMPILE_ABI "-std=c++11 -stdlib=libc++")
    set (LINK_ABI "-lc++abi")
  else ()
    set (COMPILE_ABI "-std=c++11")
    set (LINK_ABI "")
  endif ()

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  if (USE_CXX11)
    message(STATUS "Building with Clang and default ABI for C++11")
    set (COMPILE_ABI "-std=c++11")
    set (LINK_ABI "")
  else ()
    message(STATUS "Building with Clang and default ABI for C++03")
    set (COMPILE_ABI "-std=c++98")
    set (LINK_ABI "")
  endif ()

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  if (USE_CXX11)
    message(STATUS "Building with GNU g++ and default ABI for C++11")
    set (COMPILE_ABI "-std=c++11")
    set (LINK_ABI "")
  else ()
    message(STATUS "Building with GNU g++ and default ABI for C++03")
    set (COMPILE_ABI "-std=c++98")
    set (LINK_ABI "")
  endif()

endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"
   OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  # These defines are to select the build type in cmake
  # CMAKE_BUILD_TYPE values DEBUG, RELEASE, PROFILE, COVER
  set (CMAKE_CXX_FLAGS_DEBUG "${COMPILE_ABI} -O2 -g -Wall -Wextra -Wuninitialized")
  set (CMAKE_EXE_LINKER_FLAGS_DEBUG "${LINK_ABI} -lc++abi -O2 -g")
  set (CMAKE_CXX_FLAGS_RELEASE "${COMPILE_ABI} -O4")
  set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${LINK_ABI} -O4")
  set (CMAKE_CXX_FLAGS_PROFILE "${COMPILE_ABI} -O2 -g -pg")
  set (CMAKE_EXE_LINKER_FLAGS_PROFILE "${LINK_ABI} -g -pg")
  set (CMAKE_CXX_FLAGS_COVER "${COMPILE_ABI} -O0 -g --coverage")
  set (CMAKE_EXE_LINKER_FLAGS_COVER "${LINK_ABI} --coverage")

else()
  # Maybe using Microsoft. Performance test users with Microsoft Visual C++
  # should set their build type themselves.

endif()

add_executable (insert_performance insert_performance.cpp)
add_executable (erase_performance erase_performance.cpp)
add_executable (find_performance find_performance.cpp)
add_executable (mapping_performance mapping_performance.cpp)
add_executable (ordered_performance ordered_performance.cpp)
add_executable (iterate_performance iterate_performance.cpp)
add_executable (region_performance region_performance.cpp)
add_executable (nearest_neighbor_performance nearest_neighbor_performance.cpp)
add_executable (farthest_neighbor_performance farthest_neighbor_performance.cpp)
add_executable (neighbor_iterator_performance neighbor_iterator_performance.cpp)
add_executable (spheric_nearest_performance spheric_nearest_performance.cpp)
add_executable (lower_bound_neighbor_performance lower_bound_neighbor_performance.cpp)
add_executable (upper_bound_neighbor_performance upper_bound_neighbor_performance.cpp)
add_executable (equal_performance equal_performance.cpp)
