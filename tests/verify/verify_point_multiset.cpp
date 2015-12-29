// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_DYN_LINK
#define SPATIAL_ENABLE_ASSERT // detect interal issues that should not occur

#include <boost/test/unit_test.hpp>
#include "../../src/point_multiset.hpp"
#include "spatial_test_types.hpp"

using namespace spatial;

BOOST_AUTO_TEST_CASE( test_point_constructors )
{
  point_multiset<2, int2> points;
  point_multiset<0, int2> runtime_points;
}

BOOST_AUTO_TEST_CASE( test_point_copy_assignment )
{
  point_multiset<2, int2> points;
  points.insert(zeros);
  points.insert(ones);
  points.insert(twos);
  point_multiset<2, int2> copy(points);
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
  points = copy;
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
}

BOOST_AUTO_TEST_CASE( test_zero_point_copy_assignment )
{
  point_multiset<0, int2> points;
  points.insert(zeros);
  points.insert(ones);
  points.insert(twos);
  point_multiset<0, int2> copy(points);
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
  points = copy;
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
}
