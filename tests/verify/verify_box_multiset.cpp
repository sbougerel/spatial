// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_DYN_LINK
#define SPATIAL_ENABLE_ASSERT // detect interal issues that should not occur

#include <boost/test/unit_test.hpp>
#include "../../src/box_multiset.hpp"
#include "spatial_test_types.hpp"

using namespace spatial;

BOOST_AUTO_TEST_CASE( test_box_constructors )
{
  box_multiset<2, int2> boxes;
  box_multiset<0, int2> runtime_boxes;
}

BOOST_AUTO_TEST_CASE( test_box_copy_assignment )
{
  box_multiset<2, int2> boxes;
  boxes.insert(zeros);
  boxes.insert(ones);
  boxes.insert(twos);
  box_multiset<2, int2> copy(boxes);
  BOOST_CHECK_EQUAL(boxes.size(), copy.size());
  BOOST_CHECK(*boxes.begin() == *copy.begin());
  boxes = copy;
  BOOST_CHECK_EQUAL(boxes.size(), copy.size());
  BOOST_CHECK(*boxes.begin() == *copy.begin());
}

BOOST_AUTO_TEST_CASE( test_zero_box_copy_assignment )
{
  box_multiset<0, int2> boxes;
  boxes.insert(zeros);
  boxes.insert(ones);
  boxes.insert(twos);
  box_multiset<0, int2> copy(boxes);
  BOOST_CHECK_EQUAL(boxes.size(), copy.size());
  BOOST_CHECK(*boxes.begin() == *copy.begin());
  boxes = copy;
  BOOST_CHECK_EQUAL(boxes.size(), copy.size());
  BOOST_CHECK(*boxes.begin() == *copy.begin());
}
