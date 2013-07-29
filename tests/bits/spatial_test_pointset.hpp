// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_test_pointset.hpp
 *  All tests for the elements defined in spatial_pointset.hpp are
 *  located in this file.
 *  \see spatial_pointset.hpp
 */

#ifndef SPATIAL_TEST_POINTSET_HPP
#define SPATIAL_TEST_POINTSET_HPP

BOOST_AUTO_TEST_CASE( test_pointset_constructors )
{
  pointset<2, point2d> points;
  pointset<0, point2d> runtime_points;
  runtime_pointset<point2d> more_runtime_points;
}

BOOST_AUTO_TEST_CASE( test_frozen_pointset_constructors )
{
  frozen_pointset<2, point2d> points;
  frozen_pointset<0, point2d> runtime_points;
  runtime_frozen_pointset<point2d> more_runtime_points;
}

BOOST_AUTO_TEST_CASE( test_pointset_copy_assignment )
{
  pointset<2, point2d> points;
  points.insert(zeros);
  points.insert(ones);
  points.insert(twos);
  pointset<2, point2d> copy(points);
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
  points = copy;
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
}

BOOST_AUTO_TEST_CASE( test_zero_pointset_copy_assignment )
{
  pointset<0, point2d> points;
  points.insert(zeros);
  points.insert(ones);
  points.insert(twos);
  pointset<0, point2d> copy(points);
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
  points = copy;
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
}

BOOST_AUTO_TEST_CASE( test_runtime_pointset_copy_assignment )
{
  runtime_pointset<point2d> points;
  points.insert(zeros);
  points.insert(ones);
  points.insert(twos);
  runtime_pointset<point2d> copy(points);
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
  points = copy;
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
}

BOOST_AUTO_TEST_CASE( test_frozen_pointset_copy_assignment )
{
  frozen_pointset<2, point2d> points;
  points.insert(zeros);
  points.insert(ones);
  points.insert(twos);
  frozen_pointset<2, point2d> copy(points);
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
  points = copy;
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
}

BOOST_AUTO_TEST_CASE( test_zero_frozen_pointset_copy_assignment )
{
  frozen_pointset<0, point2d> points;
  points.insert(zeros);
  points.insert(ones);
  points.insert(twos);
  frozen_pointset<0, point2d> copy(points);
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
  points = copy;
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
}

BOOST_AUTO_TEST_CASE( test_runtime_frozen_pointset_copy_assignment )
{
  runtime_frozen_pointset<point2d> points;
  points.insert(zeros);
  points.insert(ones);
  points.insert(twos);
  runtime_frozen_pointset<point2d> copy(points);
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
  points = copy;
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
}

#endif // SPATIAL_TEST_POINTSET_HPP
