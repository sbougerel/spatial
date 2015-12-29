// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_DYN_LINK
#define SPATIAL_ENABLE_ASSERT // detect interal issues that should not occur

#include <boost/test/unit_test.hpp>
#include "../../src/point_multiset.hpp"
#include "../../src/idle_point_multiset.hpp"
#include "spatial_test_types.hpp"

using namespace spatial;

/**
 *  Test that \c mode_traits returns the expected mode for 2 specific node
 *  types.
 */
BOOST_AUTO_TEST_CASE(test_mode_traits)
{
  check_is_same
    <mode_traits<point_multiset<2, int2>::mode_type>::invariant_category,
    details::relaxed_invariant_tag>();
  check_is_same
    <mode_traits<idle_point_multiset<2, int2>::mode_type>::invariant_category,
    details::strict_invariant_tag>();
}

/**
 *  To test \c container_traits we use a real container and attempt to recover
 *  every of its sub-defined types.
 */
BOOST_AUTO_TEST_CASE(test_container_traits)
{
  typedef point_multiset<2, int2> container_type;
# define SPATIAL_TRAIT_CHECK(Type)         \
  check_is_same                            \
  <container_traits<container_type>::Type, \
   container_type::Type>()
  SPATIAL_TRAIT_CHECK(key_type);
  SPATIAL_TRAIT_CHECK(value_type);
  SPATIAL_TRAIT_CHECK(pointer);
  SPATIAL_TRAIT_CHECK(const_pointer);
  SPATIAL_TRAIT_CHECK(reference);
  SPATIAL_TRAIT_CHECK(const_reference);
  SPATIAL_TRAIT_CHECK(mode_type);
  SPATIAL_TRAIT_CHECK(size_type);
  SPATIAL_TRAIT_CHECK(difference_type);
  SPATIAL_TRAIT_CHECK(allocator_type);
  SPATIAL_TRAIT_CHECK(key_compare);
  SPATIAL_TRAIT_CHECK(value_compare);
  SPATIAL_TRAIT_CHECK(rank_type);
  SPATIAL_TRAIT_CHECK(iterator);
  SPATIAL_TRAIT_CHECK(const_iterator);
# undef SPATIAL_TRAIT_CHECK
}
