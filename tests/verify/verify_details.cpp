// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../../src/bits/spatial_condition.hpp"
#include "../../src/bits/spatial_rank.hpp"
#include "../../src/bits/spatial_template_member_swap.hpp"
#include "spatial_test_types.hpp"

using namespace spatial;

/**
 *  Test the \c condition meta type to make sure it works.
 *
 *  The test uses two types that cannot be casted into each others and assigns
 *  a variable of one type into the result of the condition expression. The test
 *  pass simply if it compiles. That is why there is no BOOST_* construct being
 *  used.
 */
BOOST_AUTO_TEST_CASE(test_details_condition)
{
  typedef details::condition<true, std::pair<int, int>,
    std::allocator<int> >::type type1;
  typedef details::condition<false, std::pair<int, int>,
    std::allocator<int> >::type type2;
  check_is_same<type1, std::pair<int, int> >();
  check_is_same<type2, std::allocator<int> >();
}

BOOST_AUTO_TEST_CASE(test_static_rank)
{
  details::Static_rank<2> rank;
  BOOST_CHECK_EQUAL(incr_dim(rank, 0), 1u);
  BOOST_CHECK_EQUAL(incr_dim(rank, 1), 0u);
  BOOST_CHECK_EQUAL(decr_dim(rank, 0), 1u);
  BOOST_CHECK_EQUAL(decr_dim(rank, 1), 0u);
}

BOOST_AUTO_TEST_CASE( test_details_incr_decr_dim_dynamic )
{
  details::Dynamic_rank rank(2);
  BOOST_CHECK_EQUAL(incr_dim(rank, 0), 1u);
  BOOST_CHECK_EQUAL(incr_dim(rank, 1), 0u);
  BOOST_CHECK_EQUAL(decr_dim(rank, 0), 1u);
  BOOST_CHECK_EQUAL(decr_dim(rank, 1), 0u);
}

BOOST_AUTO_TEST_CASE( test_details_template_swap )
{
  int2 z = zeros;
  int2 o = ones;
  BOOST_REQUIRE_NO_THROW
    (details::template_member_swap<int2>::do_it(z, o));
  BOOST_CHECK_EQUAL(z[0], ones[0]);
  BOOST_CHECK_EQUAL(z[1], ones[1]);
  BOOST_CHECK_EQUAL(o[0], zeros[0]);
  BOOST_CHECK_EQUAL(o[1], zeros[1]);
}

BOOST_AUTO_TEST_CASE( test_details_match )
{
  // Rather than testing match, it's testing that closed_test_range is properly
  // written, which is important for subsequent tests.
  int2 x  (0, 0);
  int2 y  (1, 1);
  int2 _x (-1, -1);
  int2 x_ (2, 2);
  BOOST_CHECK(closed_test_range()(0, 2, x) == matching);
  BOOST_CHECK(closed_test_range()(1, 2, y) == matching);
  BOOST_CHECK(closed_test_range()(0, 2, _x) == below);
  BOOST_CHECK(closed_test_range()(1, 2, x_) == above);
}
