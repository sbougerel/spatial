// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_DYN_LINK
#define SPATIAL_ENABLE_ASSERT // detect interal issues that should not occur

#include <boost/test/unit_test.hpp>
#include "../../src/idle_point_multiset.hpp"
#include "spatial_test_types.hpp"

using namespace spatial;

BOOST_AUTO_TEST_CASE( test_idle_point_constructors )
{
  idle_point_multiset<2, int2> idle_points;
  idle_point_multiset<0, int2> runtime_idle_points;
}

BOOST_AUTO_TEST_CASE( test_idle_point_copy_assignment )
{
  idle_point_multiset<2, int2> idle_points;
  idle_points.insert(zeros);
  idle_points.insert(ones);
  idle_points.insert(twos);
  idle_point_multiset<2, int2> copy(idle_points);
  BOOST_CHECK_EQUAL(idle_points.size(), copy.size());
  BOOST_CHECK(*idle_points.begin() == *copy.begin());
  idle_points = copy;
  BOOST_CHECK_EQUAL(idle_points.size(), copy.size());
  BOOST_CHECK(*idle_points.begin() == *copy.begin());
}

BOOST_AUTO_TEST_CASE( test_zero_idle_point_copy_assignment )
{
  idle_point_multiset<0, int2> idle_points;
  idle_points.insert(zeros);
  idle_points.insert(ones);
  idle_points.insert(twos);
  idle_point_multiset<0, int2> copy(idle_points);
  BOOST_CHECK_EQUAL(idle_points.size(), copy.size());
  BOOST_CHECK(*idle_points.begin() == *copy.begin());
  idle_points = copy;
  BOOST_CHECK_EQUAL(idle_points.size(), copy.size());
  BOOST_CHECK(*idle_points.begin() == *copy.begin());
}
