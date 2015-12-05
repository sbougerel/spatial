// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../../src/idle_box_multiset.hpp"
#include "spatial_test_types.hpp"

using namespace spatial;

BOOST_AUTO_TEST_CASE( test_idle_box_constructors )
{
  idle_box_multiset<2, int2> idle_box;
  idle_box_multiset<0, int2> runtime_idle_box;
}

BOOST_AUTO_TEST_CASE( test_idle_box_copy_assignment )
{
  idle_box_multiset<2, int2> idle_box;
  idle_box.insert(zeros);
  idle_box.insert(ones);
  idle_box.insert(twos);
  idle_box_multiset<2, int2> copy(idle_box);
  BOOST_CHECK_EQUAL(idle_box.size(), copy.size());
  BOOST_CHECK(*idle_box.begin() == *copy.begin());
  idle_box = copy;
  BOOST_CHECK_EQUAL(idle_box.size(), copy.size());
  BOOST_CHECK(*idle_box.begin() == *copy.begin());
}

BOOST_AUTO_TEST_CASE( test_zero_idle_box_copy_assignment )
{
  idle_box_multiset<0, int2> idle_box;
  idle_box.insert(zeros);
  idle_box.insert(ones);
  idle_box.insert(twos);
  idle_box_multiset<0, int2> copy(idle_box);
  BOOST_CHECK_EQUAL(idle_box.size(), copy.size());
  BOOST_CHECK(*idle_box.begin() == *copy.begin());
  idle_box = copy;
  BOOST_CHECK_EQUAL(idle_box.size(), copy.size());
  BOOST_CHECK(*idle_box.begin() == *copy.begin());
}
