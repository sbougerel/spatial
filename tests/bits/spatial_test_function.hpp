// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_test_function.hpp
 *  @brief  All tests for the elements defined in spatial_function.hpp are
 *  located in this file.
 *  @see spatial_function.hpp
 */

#ifndef SPATIAL_TEST_FUNCTION_HPP
#define SPATIAL_TEST_FUNCTION_HPP

BOOST_AUTO_TEST_CASE( test_access_less )
{
  // This test is hard to craft, how to make sure I'm test "less" and not the
  // accessor?
  point2d x = { { 0, 1 } };
  point2d y = { { 1, 0 } };
  accessor_less<at_accessor<point2d, int>, point2d> compare;
  BOOST_CHECK(compare(0, x, y));
  BOOST_CHECK(compare(0, x, 0, y));
  BOOST_CHECK(!compare(0, y, x));
  BOOST_CHECK(!compare(0, y, 0, x));
  BOOST_CHECK(!compare(1, x, y));
  BOOST_CHECK(!compare(1, x, 1, y));
  BOOST_CHECK(compare(1, y, x));
  BOOST_CHECK(compare(1, y, 1, x));
  BOOST_CHECK(!compare(0, x, x));
  BOOST_CHECK(!compare(0, x, 0, x));
  BOOST_CHECK(compare(0, x, 1, x));
  BOOST_CHECK(!compare(0, x, 1, y));
}

BOOST_AUTO_TEST_CASE( test_bracket_less )
{
  // This has been used before this test was carried on, but at least we will
  // know here that something could fail.
  point2d x = { { 0, 1 } };
  point2d y = { { 1, 0 } };
  bracket_less<point2d> compare;
  BOOST_CHECK(compare(0, x, y));
  BOOST_CHECK(compare(0, x, 0, y));
  BOOST_CHECK(!compare(0, y, x));
  BOOST_CHECK(!compare(0, y, 0, x));
  BOOST_CHECK(!compare(1, x, y));
  BOOST_CHECK(!compare(1, x, 1, y));
  BOOST_CHECK(compare(1, y, x));
  BOOST_CHECK(compare(1, y, 1, x));
  BOOST_CHECK(!compare(0, x, x));
  BOOST_CHECK(!compare(0, x, 0, x));
  BOOST_CHECK(compare(0, x, 1, x));
  BOOST_CHECK(!compare(0, x, 1, y));
}

BOOST_AUTO_TEST_CASE( test_iterator_less )
{
  point2d x = { { 0, 1 } };
  point2d y = { { 1, 0 } };
  iterator_less<point2d> compare;
  BOOST_CHECK(compare(0, x, y));
  BOOST_CHECK(compare(0, x, 0, y));
  BOOST_CHECK(!compare(0, y, x));
  BOOST_CHECK(!compare(0, y, 0, x));
  BOOST_CHECK(!compare(1, x, y));
  BOOST_CHECK(!compare(1, x, 1, y));
  BOOST_CHECK(compare(1, y, x));
  BOOST_CHECK(compare(1, y, 1, x));
  BOOST_CHECK(!compare(0, x, x));
  BOOST_CHECK(!compare(0, x, 0, x));
  BOOST_CHECK(compare(0, x, 1, x));
  BOOST_CHECK(!compare(0, x, 1, y));
}

BOOST_AUTO_TEST_CASE( test_paren_less )
{
  point2d_paren x; x[0] = 0; x[1] = 1;
  point2d_paren y; y[0] = 1; y[1] = 0;
  paren_less<point2d_paren> compare;
  BOOST_CHECK(compare(0, x, y));
  BOOST_CHECK(compare(0, x, 0, y));
  BOOST_CHECK(!compare(0, y, x));
  BOOST_CHECK(!compare(0, y, 0, x));
  BOOST_CHECK(!compare(1, x, y));
  BOOST_CHECK(!compare(1, x, 1, y));
  BOOST_CHECK(compare(1, y, x));
  BOOST_CHECK(compare(1, y, 1, x));
  BOOST_CHECK(!compare(0, x, x));
  BOOST_CHECK(!compare(0, x, 0, x));
  BOOST_CHECK(compare(0, x, 1, x));
  BOOST_CHECK(!compare(0, x, 1, y));
}

BOOST_AUTO_TEST_CASE( test_equal_bounds )
{
  point2d t = { { 1, 1 } };
  point2d x = { { 0, 1 } };
  point2d y = { { 1, 0 } };
  point2d z = { { 0, 2 } };
  point2d w = { { 2, 0 } };
  Empty_2D_fixture fix;
  // Checking this compiles
  equal_bounds<point2d, bracket_less<point2d> > bounds
    = make_equal_bounds(fix.kdtree, t);
  BOOST_CHECK(bounds(0, t, 2) == matching);
  BOOST_CHECK(bounds(1, t, 2) == matching);
  BOOST_CHECK(bounds(0, x, 2) == below);
  BOOST_CHECK(bounds(1, x, 2) == matching);
  BOOST_CHECK(bounds(0, y, 2) == matching);
  BOOST_CHECK(bounds(1, y, 2) == below);
  BOOST_CHECK(bounds(0, z, 2) == below);
  BOOST_CHECK(bounds(1, z, 2) == above);
  BOOST_CHECK(bounds(0, w, 2) == above);
  BOOST_CHECK(bounds(1, w, 2) == below);
}

BOOST_AUTO_TEST_CASE( test_open_range_bounds )
{
  point2d l = { { 1, 1 } };
  point2d h = { { 3, 3 } };
  point2d x = { { 2, 1 } };
  point2d y = { { 3, 2 } };
  Empty_2D_fixture fix;
  // Checking this compiles
  open_range_bounds<point2d, bracket_less<point2d> > bounds
    = make_open_range_bounds(fix.kdtree, l, h);
  BOOST_CHECK(bounds(0, l, 2) == below);
  BOOST_CHECK(bounds(1, l, 2) == below);
  BOOST_CHECK(bounds(0, h, 2) == above);
  BOOST_CHECK(bounds(1, h, 2) == above);
  BOOST_CHECK(bounds(0, x, 2) == matching);
  BOOST_CHECK(bounds(1, x, 2) == below);
  BOOST_CHECK(bounds(0, y, 2) == above);
  BOOST_CHECK(bounds(1, y, 2) == matching);
}

BOOST_AUTO_TEST_CASE( test_range_bounds )
{
  point2d l = { { 1, 1 } };
  point2d h = { { 3, 3 } };
  point2d x = { { 2, 0 } };
  point2d y = { { 3, 2 } };
  point2d z = { { 0, 0 } };
  Empty_2D_fixture fix;
  // Checking this compiles
  range_bounds<point2d, bracket_less<point2d> > bounds
    = make_range_bounds(fix.kdtree, l, h);
  BOOST_CHECK(bounds(0, l, 2) == matching);
  BOOST_CHECK(bounds(1, l, 2) == matching);
  BOOST_CHECK(bounds(0, h, 2) == above);
  BOOST_CHECK(bounds(1, h, 2) == above);
  BOOST_CHECK(bounds(0, x, 2) == matching);
  BOOST_CHECK(bounds(1, x, 2) == below);
  BOOST_CHECK(bounds(0, y, 2) == above);
  BOOST_CHECK(bounds(1, y, 2) == matching);
  BOOST_CHECK(bounds(0, z, 2) == below);
  BOOST_CHECK(bounds(1, z, 2) == below);
}

BOOST_AUTO_TEST_CASE( test_closed_range_bounds )
{
  point2d l = { { 1, 1 } };
  point2d h = { { 3, 3 } };
  point2d x = { { 2, 0 } };
  point2d y = { { 4, 2 } };
  point2d z = { { 0, 0 } };
  point2d w = { { 4, 4 } };
  Empty_2D_fixture fix;
  // Checking this compiles
  closed_range_bounds<point2d, bracket_less<point2d> > bounds
    = make_closed_range_bounds(fix.kdtree, l, h);
  BOOST_CHECK(bounds(0, l, 2) == matching);
  BOOST_CHECK(bounds(1, l, 2) == matching);
  BOOST_CHECK(bounds(0, h, 2) == matching);
  BOOST_CHECK(bounds(1, h, 2) == matching);
  BOOST_CHECK(bounds(0, x, 2) == matching);
  BOOST_CHECK(bounds(1, x, 2) == below);
  BOOST_CHECK(bounds(0, y, 2) == above);
  BOOST_CHECK(bounds(1, y, 2) == matching);
  BOOST_CHECK(bounds(0, z, 2) == below);
  BOOST_CHECK(bounds(1, z, 2) == below);
  BOOST_CHECK(bounds(0, w, 2) == above);
  BOOST_CHECK(bounds(1, w, 2) == above);
}

BOOST_AUTO_TEST_CASE( test_overlap_range_bounds )
{
  BOOST_CHECK_MESSAGE(false, "test not implemented");
}

BOOST_AUTO_TEST_CASE( test_enclosed_range_bounds )
{
  BOOST_CHECK_MESSAGE(false, "test not implemented");
}

#endif // SPATIAL_TEST_FUNCTION_HPP
