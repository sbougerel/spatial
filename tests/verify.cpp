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
 *  Uses Boost.Test; needs to be compiled with -lboost_unit_test_framework and
 *  to run it you must have compiled the Boost unit test framework as a shared
 *  library (DLL - for Windows developers). If you do not have the shared
 *  library at your disposal, you can remove all occurrences of
 *  BOOST_TEST_DYN_LINK, and the library will be linked with the static
 *  library. This still requires that you have compiled the Boost unit test
 *  framework as a static library.
 */

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

//#include "bits/spatial_test_geometry.hpp"
//#include "bits/spatial_test_neighbor.hpp"
//#include "bits/spatial_test_kdtree.hpp"
//#include "bits/spatial_test_relaxed_kdtree.hpp"
//#include "bits/spatial_test_containers.hpp"

BOOST_AUTO_TEST_CASE(install_srand)
{
  // Not sure where else to put this...
  srand((unsigned int)time(NULL));
}
