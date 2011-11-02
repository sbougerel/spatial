// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_test_traits.hpp
 *  @brief  All tests for the elements defined in spatial_traits.hpp are located
 *  in this file.
 *  @see spatial_traits.hpp
 */

#ifndef SPATIAL_TEST_TRAITS_HPP
#define SPATIAL_TEST_TRAITS_HPP

BOOST_AUTO_TEST_CASE( test_traits_condition )
{
  typedef details::condition<true, std::pair<int, int>,
    std::allocator<int> >::type type1;
  type1 must_compile_1 = std::pair<int, int>();
  silence_unused(must_compile_1);
  typedef details::condition<false, std::pair<int, int>,
    std::allocator<int> >::type type2;
  type2 must_compile_2 = std::allocator<int>();
  silence_unused(must_compile_2);
}

BOOST_AUTO_TEST_CASE( test_node_traits )
{
  typedef details::node_traits<details::Kdtree_node<int> >
    ::invariant_category type1;
  type1 must_compile_1 = details::strict_invariant_tag();
  silence_unused(must_compile_1);
  typedef details::node_traits<details::Relaxed_kdtree_node<int> >
    ::invariant_category type2;
  type2 must_compile_2 = details::relaxed_invariant_tag();
  silence_unused(must_compile_2);
}

#endif // SPATIAL_TEST_TRAITS_HPP
