// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_test_function.hpp
 *  All tests for the elements defined in spatial_function.hpp are
 *  located in this file.
 *  \see spatial_function.hpp
 */

#ifndef SPATIAL_TEST_FUNCTION_HPP
#define SPATIAL_TEST_FUNCTION_HPP

BOOST_AUTO_TEST_CASE( test_access_minus )
{
  accessor_minus<at_accessor, int2, int> minus;
  BOOST_CHECK_EQUAL(0, minus(0, zeros, zeros));
  BOOST_CHECK_EQUAL(0, minus(1, zeros, zeros));
  BOOST_CHECK_EQUAL(-1, minus(0, ones, twos));
  BOOST_CHECK_EQUAL(-1, minus(1, ones, twos));
  BOOST_CHECK_EQUAL(1, minus(0, twos, ones));
  BOOST_CHECK_EQUAL(1, minus(1, twos, ones));
}

BOOST_AUTO_TEST_CASE( test_bracket_minus )
{
  bracket_minus<int2, int> minus;
  BOOST_CHECK_EQUAL(0, minus(0, zeros, zeros));
  BOOST_CHECK_EQUAL(0, minus(1, zeros, zeros));
  BOOST_CHECK_EQUAL(-1, minus(0, ones, twos));
  BOOST_CHECK_EQUAL(-1, minus(1, ones, twos));
  BOOST_CHECK_EQUAL(1, minus(0, twos, ones));
  BOOST_CHECK_EQUAL(1, minus(1, twos, ones));
}

BOOST_AUTO_TEST_CASE( test_paren_minus )
{
  paren_minus<int2, int> minus;
  BOOST_CHECK_EQUAL(0, minus(0, zeros, zeros));
  BOOST_CHECK_EQUAL(0, minus(1, zeros, zeros));
  BOOST_CHECK_EQUAL(-1, minus(0, ones, twos));
  BOOST_CHECK_EQUAL(-1, minus(1, ones, twos));
  BOOST_CHECK_EQUAL(1, minus(0, twos, ones));
  BOOST_CHECK_EQUAL(1, minus(1, twos, ones));
}

BOOST_AUTO_TEST_CASE( test_iterator_minus )
{
  iterator_minus<int2, int> minus;
  BOOST_CHECK_EQUAL(0, minus(0, zeros, zeros));
  BOOST_CHECK_EQUAL(0, minus(1, zeros, zeros));
  BOOST_CHECK_EQUAL(-1, minus(0, ones, twos));
  BOOST_CHECK_EQUAL(-1, minus(1, ones, twos));
  BOOST_CHECK_EQUAL(1, minus(0, twos, ones));
  BOOST_CHECK_EQUAL(1, minus(1, twos, ones));
}

BOOST_AUTO_TEST_CASE( test_access_less )
{
  // This test is hard to craft, how to make sure I'm testing "less" and not
  // the accessor?
  int2 x(0, 1);
  int2 y(1, 0);
  accessor_less<at_accessor, int2> compare;
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
  int2 x(0, 1);
  int2 y(1, 0);
  bracket_less<int2> compare;
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
  int2 x(0, 1);
  int2 y(1, 0);
  iterator_less<int2> compare;
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
  int2 x; x[0] = 0; x[1] = 1;
  int2 y; y[0] = 1; y[1] = 0;
  paren_less<int2> compare;
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

#endif // SPATIAL_TEST_FUNCTION_HPP
