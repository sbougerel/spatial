// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   verify_neighbor_safer.cpp
 *
 *  @brief  Test only certain functions associated with spatial_neighbor.hpp,
 *  however do it with SPATIAL_SAFER_ARITHMETICS enabled.
 */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#define  SPATIAL_SAFER_ARITHMETICS
#include "spatial_test_fixtures.hpp"
#include "spatial_test_neighbor_safer.hpp"
