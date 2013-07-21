// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   equal_iterator.hpp
 *  @brief  Provides equal_iterator and all the functions around it.
 */

#ifndef SPATIAL_EQUAL_ITERATOR_HPP
#define SPATIAL_EQUAL_ITERATOR_HPP

// One day equal_iterator will be a specialized iterator and will be different
// from the region_iterator.
#include "spatial_equal.hpp"

  template<typename Ct>
  struct equal_iterator_pair
    : region_iterator_pair
  <Ct, equal_bounds<typename container_traits<Ct>::key_type,
                    typename container_traits<Ct>::key_compare> >
  {
    equal_iterator_pair() { }
    equal_iterator_pair
    (const region_iterator
     <Ct, equal_bounds<typename container_traits<Ct>::key_type,
     typename container_traits<Ct>::key_compare> >& a,
     const region_iterator
     <Ct, equal_bounds<typename container_traits<Ct>::key_type,
     typename container_traits<Ct>::key_compare> >& b)
      : region_iterator_pair
        <Ct, equal_bounds<typename container_traits<Ct>::key_type,
                          typename container_traits<Ct>::key_compare> >
        (a, b) { }
  };
  template<typename Ct>
  struct equal_iterator_pair<const Ct>
    : region_iterator_pair
  <const Ct, equal_bounds<typename container_traits<Ct>::key_type,
                          typename container_traits<Ct>::key_compare> >
  {
    equal_iterator_pair() { }
    equal_iterator_pair
    (const region_iterator
     <const Ct, equal_bounds<typename container_traits<Ct>::key_type,
     typename container_traits<Ct>::key_compare> >& a,
     const region_iterator
     <const Ct, equal_bounds<typename container_traits<Ct>::key_type,
     typename container_traits<Ct>::key_compare> >& b)
      : region_iterator_pair
        <const Ct, equal_bounds<typename container_traits<Ct>::key_type,
                                typename container_traits<Ct>::key_compare> >
        (a, b) { }
    equal_iterator_pair(const equal_iterator_pair<Ct>& other)
      : region_iterator_pair
        <const Ct, equal_bounds<typename container_traits<Ct>::key_type,
                                typename container_traits<Ct>::key_compare> >
        (other) { }
  };

  template <typename Ct>
  inline equal_iterator_pair<Ct>
  equal_range(Ct& container,
              const typename container_traits<Ct>::key_type& match)
  { return region_range(container, make_equal_bounds(container, match)); }
  template <typename Ct>
  inline equal_iterator_pair<const Ct>
  equal_range(const Ct& container,
              const typename container_traits<Ct>::key_type& match)
  { return region_range(container, make_equal_bounds(container, match)); }
  template <typename Ct>
  inline equal_iterator_pair<const Ct>
  equal_crange(const Ct& container,
               const typename container_traits<Ct>::key_type& match)
  { return region_crange(container, make_equal_bounds(container, match)); }

#endif // SPATIAL_EQUAL_ITERATOR_HPP
