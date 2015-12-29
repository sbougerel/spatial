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
#include "../../src/box_multimap.hpp"
#include "spatial_test_types.hpp"

using namespace spatial;

BOOST_AUTO_TEST_CASE( test_box_map_constructors )
{
  box_multimap<2, int2, int> boxes;
  box_multimap<0, int2, int> runtime_boxes;
}

BOOST_AUTO_TEST_CASE( test_box_map_copy_assignment )
{
  box_multimap<2, int2, int> boxes;
  boxes.insert(std::make_pair(zeros, 0));
  boxes.insert(std::make_pair(ones, 1));
  boxes.insert(std::make_pair(twos, 2));
  box_multimap<2, int2, int> copy(boxes);
  BOOST_CHECK_EQUAL(boxes.size(), copy.size());
  BOOST_CHECK(*boxes.begin() == *copy.begin());
  boxes = copy;
  BOOST_CHECK_EQUAL(boxes.size(), copy.size());
  BOOST_CHECK(*boxes.begin() == *copy.begin());
}

BOOST_AUTO_TEST_CASE( test_zero_box_map_copy_assignment )
{
  box_multimap<0, int2, int> boxes;
  boxes.insert(std::make_pair(zeros, 0));
  boxes.insert(std::make_pair(ones, 1));
  boxes.insert(std::make_pair(twos, 2));
  box_multimap<0, int2, int> copy(boxes);
  BOOST_CHECK_EQUAL(boxes.size(), copy.size());
  BOOST_CHECK(*boxes.begin() == *copy.begin());
  boxes = copy;
  BOOST_CHECK_EQUAL(boxes.size(), copy.size());
  BOOST_CHECK(*boxes.begin() == *copy.begin());
}
