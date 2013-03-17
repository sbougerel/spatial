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
  /**
   *  Facilitate the creation of neighbor iterator that works with an euclidian
   *  metric.
   *
   *  This class has an associated group of functions designed to
   *  initialize the iterator position at the beginning, end, lower bound or
   *  upper bound of the container to iterate.
   *
   *  \tparam Ct The container to iterate.
   *  \tparam DistanceType The type used to represent the distance, it must be a
   *  primitive arithmetic type.
   *  \tparam Diff The difference functor that will compute the difference
   *  between 2 key element in the container, along a specific dimension. See
   *  \ref DifferenceConcept for further explanation.
   */
  ///@{
  template<typename Ct, typename DistanceType,
           typename Diff = void,   // Sink for non-built-in compare types
           typename Enable = void> // Sink for non-arithmetic types
  class euclidian_neighbor_iterator { };

  template <typename Ct, typename DistanceType, typename Diff>
  class euclidian_neighbor_iterator
  <Ct, euclidian<Ct, DistanceType, Diff>,
   typename enable_if<std::tr1::is_arithmetic<DistanceType> >::type>
    : public neighbor_iterator<Ct, euclidian<Ct, DistanceType, Diff> >
  {
  public:
    euclidian_neighbor_iterator() { }

    euclidian_neighbor_iterator
    (const neighbor_iterator<Ct, euclidian<Ct, DistanceType, Diff> >& other)
      : neighbor_iterator<Ct, euclidian<Ct, DistanceType, Diff> >(other) { }

    template <typename OtherType>
    euclidian_neighbor_iterator
    (const neighbor_iterator<Ct, euclidian<Ct, OtherType, Diff> >& other)
      : neighbor_iterator<Ct, euclidian<Ct, DistanceType, Diff> >
        (other.rank(), other.key_comp(), other.metric(),
         other.key_target(), other.node_dim, other.node) { }
  };

  template <typename Ct, typename DistanceType, typename Diff>
  class euclidian_neighbor_iterator
  <const Ct, euclidian<Ct, DistanceType, Diff>,
   typename enable_if<std::tr1::is_arithmetic<DistanceType> >::type>
    : public neighbor_iterator<const Ct, euclidian<Ct, DistanceType, Diff> >
  {
  public:
    euclidian_neighbor_iterator() { }

    euclidian_neighbor_iterator
    (const neighbor_iterator
     <const Ct, euclidian<Ct, DistanceType, Diff> >&other)
      : neighbor_iterator<const Ct, euclidian<Ct, DistanceType, Diff> >
        (other) { }

    euclidian_neighbor_iterator
    (const neighbor_iterator<Ct, euclidian<Ct, DistanceType, Diff> >& other)
      : neighbor_iterator<const Ct, euclidian<Ct, DistanceType, Diff> >
        (other) { }

    template <typename OtherType>
    euclidian_neighbor_iterator
    (const neighbor_iterator<const Ct, euclidian<Ct, OtherType, Diff> >& other)
      : neighbor_iterator<const Ct, euclidian<Ct, DistanceType, Diff> >
        (other.rank(), other.key_comp(), other.metric(),
         other.key_target(), other.node_dim, other.node) { }

    template <typename OtherType>
    euclidian_neighbor_iterator
    (const neighbor_iterator<Ct, euclidian<Ct, OtherType, Diff> >& other)
      : neighbor_iterator<const Ct, euclidian<Ct, DistanceType, Diff> >
        (other.rank(), other.key_comp(), other.metric(),
         other.key_target(), other.node_dim, other.node) { }
  };

  template <typename Ct, typename DistanceType>
  class euclidian_neighbor_iterator
  <Ct, euclidian<Ct, DistanceType,
                 typename details::with_builtin_difference
                 <Ct, DistanceType>::type>,
   typename enable_if<details::is_compare_builtin<Ct>,
                      typename enable_if<std::tr1::is_arithmetic<DistanceType> >
                      ::type>
   ::type>
    : public neighbor_iterator
  <Ct, euclidian<Ct, DistanceType,
                 typename details::with_builtin_difference
                 <Ct, DistanceType>::type> >
  {
  private:
    typedef typename details::with_builtin_difference
      <Ct, DistanceType>::type diff_type;

  public:
    euclidian_neighbor_iterator() { }

    euclidian_neighbor_iterator
    (const neighbor_iterator
     <Ct, euclidian<Ct, DistanceType, diff_type> >& other)
      : neighbor_iterator<Ct, euclidian<Ct, DistanceType,
                                        diff_type> >(other) { }

    template <typename OtherType>
    euclidian_neighbor_iterator
    (const neighbor_iterator
     <Ct, euclidian<Ct, OtherType, diff_type> >& other)
      : neighbor_iterator<Ct, euclidian<Ct, DistanceType, diff_type> >
        (other.rank(), other.key_comp(), other.metric(),
         other.key_target(), other.node_dim, other.node) { }
  };

  template <typename Ct, typename DistanceType>
  class euclidian_neighbor_iterator
  <const Ct, euclidian<Ct, DistanceType,
                       typename details::with_builtin_difference
                       <Ct, DistanceType>::type>,
   typename enable_if<std::tr1::is_arithmetic<DistanceType>,
                      typename enable_if<details::is_compare_builtin<Ct> >
                      ::type>
   ::type>
    : public neighbor_iterator
  <const Ct, euclidian<Ct, DistanceType,
                       typename details::with_builtin_difference
                       <Ct, DistanceType>::type> >
  {
  private:
    typedef typename details::with_builtin_difference
      <Ct, DistanceType>::type diff_type;

  public:
    euclidian_neighbor_iterator() { }

    euclidian_neighbor_iterator
    (const neighbor_iterator
     <const Ct, euclidian<Ct, DistanceType, diff_type> >&other)
      : neighbor_iterator<const Ct, euclidian<Ct, DistanceType, diff_type> >
        (other) { }

    euclidian_neighbor_iterator
    (const neighbor_iterator
     <Ct, euclidian<Ct, DistanceType, diff_type> >& other)
      : neighbor_iterator<const Ct, euclidian<Ct, DistanceType, diff_type> >
        (other) { }

    template <typename OtherType>
    euclidian_neighbor_iterator
    (const neighbor_iterator
     <const Ct, euclidian<Ct, OtherType, diff_type> >& other)
      : neighbor_iterator<const Ct, euclidian<Ct, DistanceType, diff_type> >
        (other.rank(), other.key_comp(), other.metric(),
         other.key_target(), other.node_dim, other.node) { }

    template <typename OtherType>
    euclidian_neighbor_iterator
    (const neighbor_iterator
     <Ct, euclidian<Ct, OtherType, diff_type> >& other)
      : neighbor_iterator<const Ct, euclidian<Ct, DistanceType, diff_type> >
        (other.rank(), other.key_comp(), other.metric(),
         other.key_target(), other.node_dim, other.node) { }
  };
  ///@}

  /**
   *  Facilitate the creation of an iterator range representing a sequence from
   *  closest to furthest from the target key position, in euclidian space.
   *
   *  \tparam Ct The container to iterator.
   *  \tparam DistanceType The type used to represent the distance, it must be a
   *  primitive arithmetic type.
   *  \tparam Diff The difference functor that will compute the difference
   *  between 2 key element in the container, along a specific dimension. See
   *  \ref DifferenceConcept for further explanation.
   *
   *  This class has an associated group of functions designed to
   *  initialize the iterator position at the beginning, end, lower bound or
   *  upper bound of the container to iterate.
   */
  ///@{
  template<typename Ct, typename DistanceType,
           typename Diff = void,   // Sink for non-built-in compare types
           typename Enable = void> // Sink for non-arithmetic types
  class euclidian_neighbor_iterator_pair { };

  template <typename Ct, typename DistanceType, typename Diff>
  class euclidian_neighbor_iterator_pair
  <Ct, euclidian<Ct, DistanceType, Diff>,
   typename enable_if<std::tr1::is_arithmetic<DistanceType> >::type>
    : public neighbor_iterator_pair<Ct, euclidian<Ct, DistanceType, Diff> >
  {
  public:
    euclidian_neighbor_iterator_pair() { }

    euclidian_neighbor_iterator_pair
    (const neighbor_iterator<Ct, euclidian<Ct, DistanceType, Diff> >& a,
     const neighbor_iterator<Ct, euclidian<Ct, DistanceType, Diff> >& b)
      : neighbor_iterator_pair<Ct, euclidian<Ct, DistanceType, Diff> >
        (a, b) { }

    template <typename OtherType>
    euclidian_neighbor_iterator_pair
    (const neighbor_iterator<Ct, euclidian<Ct, OtherType, Diff> >& a,
     const neighbor_iterator<Ct, euclidian<Ct, OtherType, Diff> >& b)
      : neighbor_iterator_pair<Ct, euclidian<Ct, DistanceType, Diff> >
        (a, b) { }
  };

  template <typename Ct, typename DistanceType, typename Diff>
  class euclidian_neighbor_iterator_pair
  <const Ct, euclidian<Ct, DistanceType, Diff>,
   typename enable_if<std::tr1::is_arithmetic<DistanceType> >::type>
    : public neighbor_iterator_pair
  <const Ct, euclidian<Ct, DistanceType, Diff> >
  {
  public:
    euclidian_neighbor_iterator_pair() { }

    euclidian_neighbor_iterator_pair
    (const neighbor_iterator<const Ct, euclidian<Ct, DistanceType, Diff> >& a,
     const neighbor_iterator<const Ct, euclidian<Ct, DistanceType, Diff> >& b)
      : neighbor_iterator_pair<const Ct, euclidian<Ct, DistanceType, Diff> >
        (a, b) { }

    euclidian_neighbor_iterator_pair
    (const neighbor_iterator<Ct, euclidian<Ct, DistanceType, Diff> >& a,
     const neighbor_iterator<Ct, euclidian<Ct, DistanceType, Diff> >& b)
      : neighbor_iterator_pair<const Ct, euclidian<Ct, DistanceType, Diff> >
        (a, b) { }

    template <typename OtherType>
    euclidian_neighbor_iterator_pair
    (const neighbor_iterator<const Ct, euclidian<Ct, OtherType, Diff> >& a,
     const neighbor_iterator<const Ct, euclidian<Ct, OtherType, Diff> >& b)
      : neighbor_iterator_pair<const Ct, euclidian<Ct, DistanceType, Diff> >
        (a, b) { }

    template <typename OtherType>
    euclidian_neighbor_iterator_pair
    (const neighbor_iterator<Ct, euclidian<Ct, OtherType, Diff> >& a,
     const neighbor_iterator<Ct, euclidian<Ct, OtherType, Diff> >& b)
      : neighbor_iterator_pair<const Ct, euclidian<Ct, DistanceType, Diff> >
        (a, b) { }
  };

  template <typename Ct, typename DistanceType>
  class euclidian_neighbor_iterator_pair
  <Ct, euclidian<Ct, DistanceType,
                 typename details::with_builtin_difference
                 <Ct, DistanceType>::type>,
   typename enable_if<std::tr1::is_arithmetic<DistanceType>,
                      typename enable_if<details::is_compare_builtin<Ct> >
                      ::type>
   ::type>
    : public neighbor_iterator_pair
  <Ct, euclidian<Ct, DistanceType,
                 typename details::with_builtin_difference
                 <Ct, DistanceType>::type> >
  {
  private:
    typedef typename details::with_builtin_difference
      <Ct, DistanceType>::type diff_type;

  public:
    euclidian_neighbor_iterator_pair() { }

    euclidian_neighbor_iterator_pair
    (const neighbor_iterator<Ct, euclidian<Ct, DistanceType, diff_type> >& a,
     const neighbor_iterator<Ct, euclidian<Ct, DistanceType, diff_type> >& b)
      : neighbor_iterator_pair<Ct, euclidian<Ct, DistanceType, diff_type> >
        (a, b) { }

    template <typename OtherType>
    euclidian_neighbor_iterator_pair
    (const neighbor_iterator<Ct, euclidian<Ct, OtherType, diff_type> >& a,
     const neighbor_iterator<Ct, euclidian<Ct, OtherType, diff_type> >& b)
      : neighbor_iterator_pair<Ct, euclidian<Ct, DistanceType, diff_type> >
        (a, b) { }
  };

  template <typename Ct, typename DistanceType>
  class euclidian_neighbor_iterator_pair
  <const Ct, euclidian<Ct, DistanceType,
                       typename details::with_builtin_difference
                       <Ct, DistanceType>::type>,
   typename enable_if<std::tr1::is_arithmetic<DistanceType>,
                      typename enable_if<details::is_compare_builtin<Ct> >
                      ::type>
   ::type>
    : public neighbor_iterator_pair
  <const Ct, euclidian<Ct, DistanceType,
                       typename details::with_builtin_difference
                       <Ct, DistanceType>::type> >
  {
  private:
    typedef typename details::with_builtin_difference
      <Ct, DistanceType>::type diff_type;

  public:
    euclidian_neighbor_iterator_pair() { }

    euclidian_neighbor_iterator_pair
    (const neighbor_iterator
     <const Ct, euclidian<Ct, DistanceType, diff_type> >& a,
     const neighbor_iterator
     <const Ct, euclidian<Ct, DistanceType, diff_type> >& b)
      : neighbor_iterator_pair
        <const Ct, euclidian<Ct, DistanceType, diff_type> > (a, b) { }

    euclidian_neighbor_iterator_pair
    (const neighbor_iterator
     <Ct, euclidian<Ct, DistanceType, diff_type> >& a,
     const neighbor_iterator
     <Ct, euclidian<Ct, DistanceType, diff_type> >& b)
      : neighbor_iterator_pair
        <const Ct, euclidian<Ct, DistanceType, diff_type> > (a, b) { }

    template <typename OtherType>
    euclidian_neighbor_iterator_pair
    (const neighbor_iterator
     <const Ct, euclidian<Ct, OtherType, diff_type> >& a,
     const neighbor_iterator
     <const Ct, euclidian<Ct, OtherType, diff_type> >& b)
      : neighbor_iterator_pair
        <const Ct, euclidian<Ct, DistanceType, diff_type> > (a, b) { }

    template <typename OtherType>
    euclidian_neighbor_iterator_pair
    (const neighbor_iterator
     <Ct, euclidian<Ct, OtherType, diff_type> >& a,
     const neighbor_iterator
     <Ct, euclidian<Ct, OtherType, diff_type> >& b)
      : neighbor_iterator_pair
        <const Ct, euclidian<Ct, DistanceType, diff_type> > (a, b) { }
  };
  ///@}

  template <typename Ct, typename Diff>
  inline euclidian_neighbor_iterator<Ct, double, Diff>
  euclidian_neighbor_begin
  (Ct& container, const Diff& diff,
   const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_begin
      (container, euclidian<Ct, double, Diff>(diff), target);
  }

  template <typename Ct, typename Diff>
  inline euclidian_neighbor_iterator<Ct, double, Diff>
  euclidian_neighbor_begin
  (const Ct& container, const Diff& diff,
   const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_begin
      (container, euclidian<Ct, double, Diff>(diff), target);
  }

  template <typename Ct, typename Diff>
  inline euclidian_neighbor_iterator<Ct, double, Diff>
  euclidian_neighbor_cbegin
  (const Ct& container, const Diff& diff,
   const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_begin
      (container, euclidian<Ct, double, Diff>(diff), target);
  }

  template <typename Ct>
  inline typename
  enable_if<details::is_compare_builtin<Ct>,
            euclidian_neighbor_iterator<Ct, double> >::type
  euclidian_neighbor_begin
  (Ct& container,
   const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_begin
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct, double>::type>
       (details::with_builtin_difference<Ct, double>()(container)),
       target);
  }

  template <typename Ct>
  inline typename
  enable_if<details::is_compare_builtin<Ct>,
            euclidian_neighbor_iterator<const Ct, double> >::type
  euclidian_neighbor_begin
  (const Ct& container,
   const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_begin
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct, double>::type>
       (details::with_builtin_difference<Ct, double>()(container)),
       target);
  }

  template <typename Ct>
  inline typename
  enable_if<details::is_compare_builtin<Ct>,
            euclidian_neighbor_iterator<const Ct, double> >::type
  euclidian_neighbor_cbegin
  (const Ct& container,
   const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_begin
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct, double>::type>
       (details::with_builtin_difference<Ct, double>()(container)),
       target);
  }

} // namespace spatial

#endif // SPATIAL_EUCLIDIAN_NEIGHBOR_HPP
