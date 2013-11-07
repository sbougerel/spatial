// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file  verify_base.cpp
 *
 *  @brief Test all basic functions needed by the library to perform its
 *  under-the-hood work.
 */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "bits/spatial_test_fixtures.hpp"
#include "bits/spatial_test_output.hpp"
#include "bits/spatial_test_traits.hpp"
#include "bits/spatial_test_details.hpp"
#include "bits/spatial_test_node.hpp"
#include "bits/spatial_test_exceptions.hpp"
#include "bits/spatial_test_function.hpp"
#include "bits/spatial_test_relaxed_kdtree.hpp"
//#include "bits/spatial_test_kdtree.hpp"
