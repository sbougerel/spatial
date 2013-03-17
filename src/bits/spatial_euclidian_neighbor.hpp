// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_neighbor.hpp
 *  Contains the definition of euclidian neighbor iterators. These iterators
 *  walk through all items in the container in order from the closest to the
 *  furthest away from a given key, using an euclidian metric.
 *
 *  \see neighbor
 */

#ifndef SPATIAL_EUCLIDIAN_NEIGHBOR_HPP
#define SPATIAL_EUCLIDIAN_NEIGHBOR_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

namespace spatial
{
  /*
  template <typename Ct, typename DistanceType, typename Diff>
  struct euclidian_neighbor_iterator
    : neighbor_iterator<Ct, euclidian<Ct, DistanceType, Diff> >
  {
    euclidian_neighbor_iterator() { }
  };

  template <typename Ct, typename DistanceType, typename Diff>
  struct euclidian_neighbor_iterator_pair
    : std::pair<neighbor_iterator<Ct, euclidian<Ct, DistanceType, Diff> >,
                neighbor_iterator<Ct, euclidian<Ct, DistanceType, Diff> > >
  {
    euclidian_neighbor_iterator_pair() { }
  };

  template <typename Ct, typename Diff>
  inline euclidian_neighbor_iterator<Ct, double, Diff>
  euclidian_neighbor_begin
  (Ct& container, const Diff& diff,
   const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_begin
      (container, euclidian<Ct, double, Diff>(diff), target);
  }

  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            euclidian_neighbor_iterator<const Ct, double> >::type
  euclidian_neighbor_begin
  (Ct& container,
   const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_upper_bound
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct, double>::type>
         (details::with_builtin_difference<Ct, double>()(container)),
       target);
  }
  */
} // namespace spatial

#endif // SPATIAL_EUCLIDIAN_NEIGHBOR_HPP
