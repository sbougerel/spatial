cmake_minimum_required (VERSION 2.8.4)
project (spatial_verify)

option (USE_LIBCXX "Force libc++ with clang++?" ON)
option (USE_CXX11  "Force use of c++11?" OFF)

find_package (Boost REQUIRED COMPONENTS unit_test_framework)

# A whole bunch of warnings we are interested in
set (SPATIAL_GNU_WARNINGS "-Wall -Wextra -Wshadow -Wcast-qual -Wconversion -Wsign-conversion -Wformat")

# Use Clang with libc++ ABI, like on Apple OS (only supports C++11)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND USE_LIBCXX)
  # Some of the flags below are to force Clang not to use anything but libc++
  # I don't think it's an issue on Darwin, but it is on Linux.
  if ("${CMAKE_SYSTEM}" MATCHES "Linux")
    message (STATUS "Building with pedantic clang++ for c++11 with libc++ on Linux")
    set (CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -O0 -g -pedantic-errors")
    set (CMAKE_EXE_LINKER_FLAGS "-lc++abi")
  else ()
    message(STATUS "Building with pedantic clang++ for c++11 with libc++")
    set (CMAKE_CXX_FLAGS "-std=c++11 -O0 -g -pedantic-errors")
  endif ()
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SPATIAL_GNU_WARNINGS}")

# Else use Clang compiler with other ABI
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  if (USE_CXX11)
    message(STATUS "Building with pedantic clang++ for c++11 with libstdc++")
    set (CMAKE_CXX_FLAGS "-std=c++11 -O0 -g -pedantic-errors")
  else ()
    message (STATUS "Building with pendantic clang++ for c++98 and c++03 with libcstdc++")
    set (CMAKE_CXX_FLAGS "-std=c++98 -O0 -g -pedantic-errors")
  endif ()
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SPATIAL_GNU_WARNINGS}")

# Use Gnu compiler with libstdc++ ABI
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  if (USE_CXX11)
    message (STATUS "Building with pedantic g++ for c++11")
    set (CMAKE_CXX_FLAGS "-std=c++11 -O0 -g -pedantic-errors")
  else ()
    message (STATUS "Building with pedantic g++ for c++98 and c++03")
    set (CMAKE_CXX_FLAGS "-std=c++98 -O0 -g -pedantic-errors")
  endif ()
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SPATIAL_GNU_WARNINGS}")

# Use Microsoft Visual Studio
elseif (MSVC)
  # Set MSVC Warning level to 4
  message (STATUS "Building with pedantic MSVC")
  if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif ()

# We are not sure which compiler you are using...
else ()
  message (WARNING "No warnings flags have been set for your compiler")
endif ()

include_directories(${Boost_INCLUDE_DIRS})

#
# The verify exectuables checks correctness
add_executable (verify verify.cpp
                verify_traits.cpp
                verify_details.cpp
                verify_node.cpp
                verify_exception.cpp
                verify_function.cpp
                verify_kdtree.cpp
                verify_relaxed_kdtree.cpp
                verify_mapping.cpp
                verify_region.cpp
                verify_metric.cpp
                verify_neighbor.cpp
                verify_neighbor_safer.cpp
                verify_ordered.cpp
                verify_equal.cpp
                verify_point_multiset.cpp
                verify_idle_point_multiset.cpp
                verify_point_multimap.cpp
                verify_idle_point_multimap.cpp
                verify_box_multiset.cpp
                verify_idle_box_multiset.cpp
                verify_box_multimap.cpp
                verify_idle_box_multimap.cpp
                )

if (MSVC)
  set_target_properties (verify PROPERTIES COMPILE_FLAGS "/EHa")
endif ()

target_link_libraries(verify ${Boost_LIBRARIES})
