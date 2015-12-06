// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <utility> // std::make_pair
#include "../../src/point_multimap.hpp"
#include "spatial_test_types.hpp"

using namespace spatial;

BOOST_AUTO_TEST_CASE( test_point_map_constructors )
{
  point_multimap<2, int2, int> points;
  point_multimap<0, int2, int> runtime_points;
}

BOOST_AUTO_TEST_CASE( test_point_map_copy_assignment )
{
  point_multimap<2, int2, int> points;
  points.insert(std::make_pair(zeros, 0));
  points.insert(std::make_pair(ones, 1));
  points.insert(std::make_pair(twos, 2));
  point_multimap<2, int2, int> copy(points);
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
  points = copy;
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
}

BOOST_AUTO_TEST_CASE( test_zero_point_map_copy_assignment )
{
  point_multimap<0, int2, int> points;
  points.insert(std::make_pair(zeros, 0));
  points.insert(std::make_pair(ones, 1));
  points.insert(std::make_pair(twos, 2));
  point_multimap<0, int2, int> copy(points);
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
  points = copy;
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
}
