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

BOOST_AUTO_TEST_CASE( test_details_incr_decr_dim_static )
{
  details::Static_rank<2> dim;
  BOOST_CHECK_EQUAL(incr_dim(dim, 0), 1);
  BOOST_CHECK_EQUAL(incr_dim(dim, 1), 0);
  BOOST_CHECK_EQUAL(decr_dim(dim, 0), 1);
  BOOST_CHECK_EQUAL(decr_dim(dim, 1), 0);
}

BOOST_AUTO_TEST_CASE( test_details_incr_decr_dim_dynamic )
{
  details::Dynamic_rank dim(2);
  BOOST_CHECK_EQUAL(incr_dim(dim, 0), 1);
  BOOST_CHECK_EQUAL(incr_dim(dim, 1), 0);
  BOOST_CHECK_EQUAL(decr_dim(dim, 0), 1);
  BOOST_CHECK_EQUAL(decr_dim(dim, 1), 0);
}

BOOST_AUTO_TEST_CASE( test_details_template_swap )
{
  point2d z = zeros;
  point2d o = ones;
  BOOST_REQUIRE_NO_THROW
    (details::template_member_swap<point2d>::do_it(z, o));
  BOOST_CHECK_EQUAL(z[0], ones[0]);
  BOOST_CHECK_EQUAL(z[1], ones[1]);
  BOOST_CHECK_EQUAL(o[0], zeros[0]);
  BOOST_CHECK_EQUAL(o[1], zeros[1]);
}

BOOST_AUTO_TEST_CASE( test_details_less_by_ref )
{
  point2d x = { { 0, 1 } };
  point2d y = { { 1, 0 } };
  point2d z = { { 0, 1 } };
  bracket_less<point2d> cmp;
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
  point2d x = { { 0, 0 } };
  point2d y = { { 1, 1 } };
  point2d _x = { { -1, -1 } };
  point2d x_ = { { 2, 2 } };
  BOOST_CHECK(closed_test_range()(0, x, 2) == matching);
  BOOST_CHECK(closed_test_range()(1, y, 2) == matching);
  BOOST_CHECK(closed_test_range()(0, _x, 2) == below);
  BOOST_CHECK(closed_test_range()(1, x_, 2) == above);
}

BOOST_AUTO_TEST_CASE( test_details_match_all )
{
  Empty_2D_fixture fix;
  point2d x = { { 0, 0 } };
  point2d y = { { 1, 1 } };
  point2d z = { { 1, 0 } };
  point2d w = { { 0, 1 } };
  point2d _x = { { 0, -1 } };
  point2d y_ = { { 2, 0 } };
  point2d _w_ = { { 2, 2 } };
  BOOST_CHECK(details::match_all
              (fix.kdtree.rank(), x, closed_test_range()));
  BOOST_CHECK(details::match_all
              (fix.kdtree.rank(), y, closed_test_range()));
  BOOST_CHECK(details::match_all
              (fix.kdtree.rank(), z, closed_test_range()));
  BOOST_CHECK(details::match_all
              (fix.kdtree.rank(), w, closed_test_range()));
  BOOST_CHECK(!details::match_all
              (fix.kdtree.rank(), _x, closed_test_range()));
  BOOST_CHECK(!details::match_all
              (fix.kdtree.rank(), y_, closed_test_range()));
  BOOST_CHECK(!details::match_all
              (fix.kdtree.rank(), _w_, closed_test_range()));
}

BOOST_AUTO_TEST_CASE( test_details_match_any )
{
  Empty_2D_fixture fix;
  point2d x = { { 0, 0 } };
  point2d y = { { 1, 1 } };
  point2d z = { { 1, 0 } };
  point2d w = { { 0, 1 } };
  point2d _x = { { 0, -1 } };
  point2d y_ = { { 2, 0 } };
  point2d _w_ = { { 2, 2 } };
  BOOST_CHECK(details::match_any
              (fix.kdtree.rank(), x, closed_test_range()));
  BOOST_CHECK(details::match_any
              (fix.kdtree.rank(), y, closed_test_range()));
  BOOST_CHECK(details::match_any
              (fix.kdtree.rank(), z, closed_test_range()));
  BOOST_CHECK(details::match_any
              (fix.kdtree.rank(), w, closed_test_range()));
  BOOST_CHECK(details::match_any
              (fix.kdtree.rank(), _x, closed_test_range()));
  BOOST_CHECK(details::match_any
              (fix.kdtree.rank(), y_, closed_test_range()));
  BOOST_CHECK(!details::match_any
              (fix.kdtree.rank(), _w_, closed_test_range()));
}

BOOST_AUTO_TEST_CASE( test_details_match_most )
{
  Empty_2D_fixture fix;
  point2d x = { { 0, 0 } };
  point2d y = { { 1, 1 } };
  point2d z = { { 1, 0 } };
  point2d w = { { 0, 1 } };
  point2d _x = { { 0, -1 } };
  point2d y_ = { { 2, 0 } };
  point2d _w_ = { { 2, 2 } };
  BOOST_CHECK(details::match_most
              (fix.kdtree.rank(), 0, x, closed_test_range()));
  BOOST_CHECK(details::match_most
              (fix.kdtree.rank(), 1, y, closed_test_range()));
  BOOST_CHECK(details::match_most
              (fix.kdtree.rank(), 1, z, closed_test_range()));
  BOOST_CHECK(details::match_most
              (fix.kdtree.rank(), 0, w, closed_test_range()));
  BOOST_CHECK(!details::match_most
              (fix.kdtree.rank(), 0, _x, closed_test_range()));
  BOOST_CHECK(details::match_most
              (fix.kdtree.rank(), 1, _x, closed_test_range()));
  BOOST_CHECK(details::match_most
              (fix.kdtree.rank(), 0, y_, closed_test_range()));
  BOOST_CHECK(!details::match_most
              (fix.kdtree.rank(), 1, y_, closed_test_range()));
  BOOST_CHECK(!details::match_most
              (fix.kdtree.rank(), 0, _w_, closed_test_range()));
  BOOST_CHECK(!details::match_most
              (fix.kdtree.rank(), 1, _w_, closed_test_range()));
}

#endif // SPATIAL_TEST_DETAILS_HPP
