// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_test_details.hpp
 *  @brief  All tests for the elements defined in spatial_details.hpp are located
 *  in this file.
 *  @see spatial_details.hpp
 */

#ifndef SPATIAL_TEST_DETAILS_HPP
#define SPATIAL_TEST_DETAILS_HPP

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
  BOOST_CHECK_EQUAL(incr_dim(rank, 0), 1);
  BOOST_CHECK_EQUAL(incr_dim(rank, 1), 0);
  BOOST_CHECK_EQUAL(decr_dim(rank, 0), 1);
  BOOST_CHECK_EQUAL(decr_dim(rank, 1), 0);
}

BOOST_AUTO_TEST_CASE( test_details_incr_decr_dim_dynamic )
{
  details::Dynamic_rank rank(2);
  BOOST_CHECK_EQUAL(incr_dim(rank, 0), 1);
  BOOST_CHECK_EQUAL(incr_dim(rank, 1), 0);
  BOOST_CHECK_EQUAL(decr_dim(rank, 0), 1);
  BOOST_CHECK_EQUAL(decr_dim(rank, 1), 0);
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

BOOST_AUTO_TEST_CASE( test_details_less_by_ref )
{
  int2 x(0, 1);
  int2 y(1, 0);
  int2 z(0, 1);
  bracket_less<int2> cmp;
  BOOST_CHECK(details::less_by_ref(cmp, 0, x, y));
  BOOST_CHECK(!details::less_by_ref(cmp, 0, y, x));
  BOOST_CHECK(!details::less_by_ref(cmp, 1, x, y));
  BOOST_CHECK(details::less_by_ref(cmp, 1, y, x));
  BOOST_CHECK((&x < &z)
              ? details::less_by_ref(cmp, 0, x, z)
              : !details::less_by_ref(cmp, 0, x, z));
  BOOST_CHECK((&x < &z)
              ? !details::less_by_ref(cmp, 0, z, x)
              : details::less_by_ref(cmp, 0, z, x));
  BOOST_CHECK(!details::less_by_ref(cmp, 1, x, x));
}

BOOST_AUTO_TEST_CASE( test_details_match )
{
  // Rather than testing match, it's testing that closed_test_range is properly
  // written, which is important for subsequent tests.
  int2 x  (0, 0);
  int2 y  (1, 1);
  int2 _x (-1, -1);
  int2 x_ (2, 2);
  BOOST_CHECK(closed_test_range()(0, x, 2) == matching);
  BOOST_CHECK(closed_test_range()(1, y, 2) == matching);
  BOOST_CHECK(closed_test_range()(0, _x, 2) == below);
  BOOST_CHECK(closed_test_range()(1, x_, 2) == above);
}

BOOST_AUTO_TEST_CASE( test_details_match_all )
{
  pointset_fix<int2> fix;
  int2 x   (0, 0);
  int2 y   (1, 1);
  int2 z   (1, 0);
  int2 w   (0, 1);
  int2 _x  (0, -1);
  int2 y_  (2, 0);
  int2 _w_ (2, 2);
  BOOST_CHECK(details::match_all
              (fix.container.rank(), x, closed_test_range()));
  BOOST_CHECK(details::match_all
              (fix.container.rank(), y, closed_test_range()));
  BOOST_CHECK(details::match_all
              (fix.container.rank(), z, closed_test_range()));
  BOOST_CHECK(details::match_all
              (fix.container.rank(), w, closed_test_range()));
  BOOST_CHECK(!details::match_all
              (fix.container.rank(), _x, closed_test_range()));
  BOOST_CHECK(!details::match_all
              (fix.container.rank(), y_, closed_test_range()));
  BOOST_CHECK(!details::match_all
              (fix.container.rank(), _w_, closed_test_range()));
}

BOOST_AUTO_TEST_CASE( test_details_match_any )
{
  pointset_fix<int2> fix;
  int2 x   (0, 0);
  int2 y   (1, 1);
  int2 z   (1, 0);
  int2 w   (0, 1);
  int2 _x  (0, -1);
  int2 y_  (2, 0);
  int2 _w_ (2, 2);
  BOOST_CHECK(details::match_any
              (fix.container.rank(), x, closed_test_range()));
  BOOST_CHECK(details::match_any
              (fix.container.rank(), y, closed_test_range()));
  BOOST_CHECK(details::match_any
              (fix.container.rank(), z, closed_test_range()));
  BOOST_CHECK(details::match_any
              (fix.container.rank(), w, closed_test_range()));
  BOOST_CHECK(details::match_any
              (fix.container.rank(), _x, closed_test_range()));
  BOOST_CHECK(details::match_any
              (fix.container.rank(), y_, closed_test_range()));
  BOOST_CHECK(!details::match_any
              (fix.container.rank(), _w_, closed_test_range()));
}

BOOST_AUTO_TEST_CASE( test_details_match_most )
{
  pointset_fix<int2> fix;
  int2 x   (0, 0);
  int2 y   (1, 1);
  int2 z   (1, 0);
  int2 w   (0, 1);
  int2 _x  (0, -1);
  int2 y_  (2, 0);
  int2 _w_ (2, 2);
  BOOST_CHECK(details::match_most
              (fix.container.rank(), 0, x, closed_test_range()));
  BOOST_CHECK(details::match_most
              (fix.container.rank(), 1, y, closed_test_range()));
  BOOST_CHECK(details::match_most
              (fix.container.rank(), 1, z, closed_test_range()));
  BOOST_CHECK(details::match_most
              (fix.container.rank(), 0, w, closed_test_range()));
  BOOST_CHECK(!details::match_most
              (fix.container.rank(), 0, _x, closed_test_range()));
  BOOST_CHECK(details::match_most
              (fix.container.rank(), 1, _x, closed_test_range()));
  BOOST_CHECK(details::match_most
              (fix.container.rank(), 0, y_, closed_test_range()));
  BOOST_CHECK(!details::match_most
              (fix.container.rank(), 1, y_, closed_test_range()));
  BOOST_CHECK(!details::match_most
              (fix.container.rank(), 0, _w_, closed_test_range()));
  BOOST_CHECK(!details::match_most
              (fix.container.rank(), 1, _w_, closed_test_range()));
}

#endif // SPATIAL_TEST_DETAILS_HPP
