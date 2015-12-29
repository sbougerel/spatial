// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_DYN_LINK
#define SPATIAL_ENABLE_ASSERT // detect interal issues that should not occur

#include <boost/test/unit_test.hpp>
#include <utility> // std::make_pair
#include "../../src/idle_point_multimap.hpp"
#include "spatial_test_types.hpp"

using namespace spatial;

BOOST_AUTO_TEST_CASE( test_idle_point_map_constructors )
{
  idle_point_multimap<2, int2, int> idle_points;
  idle_point_multimap<0, int2, int> runtime_idle_points;
}

BOOST_AUTO_TEST_CASE( test_idle_point_map_copy_assignment )
{
  idle_point_multimap<2, int2, int> idle_points;
  idle_points.insert(std::make_pair(zeros, 0));
  idle_points.insert(std::make_pair(ones, 1));
  idle_points.insert(std::make_pair(twos, 2));
  idle_point_multimap<2, int2, int> copy(idle_points);
  BOOST_CHECK_EQUAL(idle_points.size(), copy.size());
  BOOST_CHECK(*idle_points.begin() == *copy.begin());
  idle_points = copy;
  BOOST_CHECK_EQUAL(idle_points.size(), copy.size());
  BOOST_CHECK(*idle_points.begin() == *copy.begin());
}

BOOST_AUTO_TEST_CASE( test_zero_idle_point_map_copy_assignment )
{
  idle_point_multimap<0, int2, int> idle_points;
  idle_points.insert(std::make_pair(zeros, 0));
  idle_points.insert(std::make_pair(ones, 1));
  idle_points.insert(std::make_pair(twos, 2));
  idle_point_multimap<0, int2, int> copy(idle_points);
  BOOST_CHECK_EQUAL(idle_points.size(), copy.size());
  BOOST_CHECK(*idle_points.begin() == *copy.begin());
  idle_points = copy;
  BOOST_CHECK_EQUAL(idle_points.size(), copy.size());
  BOOST_CHECK(*idle_points.begin() == *copy.begin());
}
