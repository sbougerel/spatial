// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   verify.cpp
 *  @brief  Executes the series of test one by one and display simple messages
 *  for the outcome of the test.
 *
 *  Uses Boost.Test; needs to be compiled with -lboost_unit_test_framework.
 */

#include <cstdlib> // rand(), srand()
#include <memory> // std::allocator
#include <utility> // std::pair
#include <algorithm> // std::find() and others
#include <vector>
#include <limits>
#include <iomanip>

#ifdef __GLIBCXX__
#  include <tr1/array>
#else
#  ifdef __IBMCPP__
#    define __IBMCPP_TR1__
#  endif
#  include <array>
#endif

#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#define SPATIAL_ENABLE_ASSERT // detect interal issues that should not occur
#include "../src/pointset.hpp"
#include "../src/frozen_pointset.hpp"
#include "../src/boxset.hpp"
#include "../src/frozen_boxset.hpp"
#include "../src/pointmap.hpp"
#include "../src/frozen_pointmap.hpp"
#include "../src/boxmap.hpp"
#include "../src/frozen_boxmap.hpp"

using namespace spatial;

// Used to silence some warnings during the tests.
template <typename T> void silence_unused(const T&) { }

BOOST_AUTO_TEST_CASE(install_srand)
{
  // not sure where else to put this...
  srand((unsigned int)time(NULL));
}

#include "bits/spatial_test_fixtures.hpp"
#include "bits/spatial_test_traits.hpp"
#include "bits/spatial_test_details.hpp"
#include "bits/spatial_test_node.hpp"
#include "bits/spatial_test_exceptions.hpp"
#include "bits/spatial_test_function.hpp"
#include "bits/spatial_test_mapping.hpp"
//#include "bits/spatial_test_region.hpp"
//#include "bits/spatial_test_geometry.hpp"
//#include "bits/spatial_test_neighbor.hpp"
//#include "bits/spatial_test_kdtree.hpp"
//#include "bits/spatial_test_relaxed_kdtree.hpp"
//#include "bits/spatial_test_pointset.hpp"
