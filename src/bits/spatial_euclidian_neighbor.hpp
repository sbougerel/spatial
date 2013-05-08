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
 *  \see neighbor_iterator
 */

#ifndef SPATIAL_EUCLIDIAN_NEIGHBOR_HPP
#define SPATIAL_EUCLIDIAN_NEIGHBOR_HPP

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
   *  primitive floating point type.
   *  \tparam Diff The difference functor that will compute the difference
   *  between 2 key element in the container, along a specific dimension. See
   *  \ref DifferenceConcept for further explanation.
   */
  ///@{
  template<typename Ct, typename DistanceType,
           typename Diff
           = typename details::with_builtin_difference<Ct, DistanceType>::type,
           typename Enable = void> // Sink for non-floating point types
  class euclidian_neighbor_iterator { };

  template <typename Ct, typename DistanceType, typename Diff>
  class euclidian_neighbor_iterator
  <Ct, euclidian<Ct, DistanceType, Diff>,
   typename enable_if<std::tr1::is_floating_point<DistanceType> >::type>
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
   typename enable_if<std::tr1::is_floating_point<DistanceType> >::type>
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
   typename enable_if<std::tr1::is_floating_point<DistanceType>,
                      typename enable_if<details::is_compare_builtin<Ct> >
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
   typename enable_if<std::tr1::is_floating_point<DistanceType>,
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
   *  primitive floating point type.
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
           typename Enable = void> // Sink for non-floating-point types
  class euclidian_neighbor_iterator_pair { };

  template <typename Ct, typename DistanceType, typename Diff>
  class euclidian_neighbor_iterator_pair
  <Ct, euclidian<Ct, DistanceType, Diff>,
   typename enable_if<std::tr1::is_floating_point<DistanceType> >::type>
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
   typename enable_if<std::tr1::is_floating_point<DistanceType> >::type>
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
   typename enable_if<std::tr1::is_floating_point<DistanceType>,
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
   typename enable_if<std::tr1::is_floating_point<DistanceType>,
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

  /**
   *  Returns a \ref euclidian_neighbor_iterator<Ct, double, Diff> pointing to
   *  the nearest neighbor of \c target.
   *
   *  \param container The container to iterate.
   *  \param diff A model of \ref DifferenceConcept.
   *  \param target Search for element in container closest to target.
   *
   *  The search will occur in euclidian space, where distance are computed in
   *  double, by default. However distances can be expressed in any floating
   *  point type by simply assigning the result to an similar iterator using a
   *  different distance type:
   *
   *  \code
   *  euclidian_neighbor_iterator<Ct, float, Diff> my_float_nearest_iterator
   *    = euclidian_neighbor_begin(container, diff(), target);
   *  \endcode
   */
  ///@{
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
  ///@}

  /**
   *  Returns a \ref euclidian_neighbor_iterator<Ct, double, Diff> pointing to
   *  the nearest neighbor of \c target.
   *
   *  \param container The container to iterate.
   *  \param target Search for element in container closest to target.
   *
   *  The search will occur in euclidian space, where distance are computed in
   *  double, by default. However distances can be expressed in any floating
   *  point type by simply assigning the result to an similar iterator using a
   *  different distance type:
   *
   *  \code
   *  euclidian_neighbor_iterator<Ct, float, Diff> my_float_nearest_iterator
   *    = euclidian_neighbor_begin(container, diff(), target);
   *  \endcode
   */
  ///@{
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
  ///@}

  /**
   *  Returns a \ref euclidian_neighbor_iterator<Ct, double, Diff> pointing
   *  past-the-end.
   *
   *  \param container The container to iterate.
   *  \param diff A model of \ref DifferenceConcept.
   *  \param target Search for element in container closest to target.
   *
   *  The search will occur in euclidian space, where distance are computed in
   *  double, by default. However distances can be expressed in any floating
   *  point type by simply assigning the result to an similar iterator using a
   *  different distance type:
   *
   *  \code
   *  euclidian_neighbor_iterator<Ct, float, Diff> my_float_nearest_iterator
   *    = euclidian_neighbor_end(container, diff(), target);
   *  \endcode
   */
  ///@{
  template <typename Ct, typename Diff>
  inline euclidian_neighbor_iterator<Ct, double, Diff>
  euclidian_neighbor_end
  (Ct& container, const Diff& diff,
   const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_end
      (container, euclidian<Ct, double, Diff>(diff), target);
  }

  template <typename Ct, typename Diff>
  inline euclidian_neighbor_iterator<Ct, double, Diff>
  euclidian_neighbor_end
  (const Ct& container, const Diff& diff,
   const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_end
      (container, euclidian<Ct, double, Diff>(diff), target);
  }

  template <typename Ct, typename Diff>
  inline euclidian_neighbor_iterator<Ct, double, Diff>
  euclidian_neighbor_cend
  (const Ct& container, const Diff& diff,
   const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_end
      (container, euclidian<Ct, double, Diff>(diff), target);
  }
  ///@}

  /**
   *  Returns a \ref euclidian_neighbor_iterator<Ct, double, Diff> pointing
   *  past-the-end.
   *
   *  \param container The container to iterate.
   *  \param target Search for element in container closest to target.
   *
   *  The search will occur in euclidian space, where distance are computed in
   *  double, by default. However distances can be expressed in any floating
   *  point type by simply assigning the result to an similar iterator using a
   *  different distance type:
   *
   *  \code
   *  euclidian_neighbor_iterator<Ct, float, Diff> my_float_nearest_iterator
   *    = euclidian_neighbor_end(container, diff(), target);
   *  \endcode
   */
  ///@{
  template <typename Ct>
  inline typename
  enable_if<details::is_compare_builtin<Ct>,
            euclidian_neighbor_iterator<Ct, double> >::type
  euclidian_neighbor_end
  (Ct& container,
   const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_end
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
  euclidian_neighbor_end
  (const Ct& container,
   const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_end
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
  euclidian_neighbor_cend
  (const Ct& container,
   const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_end
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct, double>::type>
       (details::with_builtin_difference<Ct, double>()(container)),
       target);
  }
  ///@}

  /**
   *  Returns a \ref euclidian_neighbor_iterator<Ct, double, Diff> pointing
   *  to the closest element to \c target that is a least as far as \c bound.
   *
   *  \param container The container to iterate.
   *  \param diff A model of \ref DifferenceConcept.
   *  \param target Search for element in container closest to target.
   *  \param bound The minimum distance at which a neighbor should be found.
   */
  ///@{
  template <typename Ct, typename Diff, typename DistanceType>
  inline typename
  enable_if<std::tr1::is_floating_point<DistanceType>,
            euclidian_neighbor_iterator<Ct, DistanceType, Diff> >::type
  euclidian_neighbor_lower_bound
  (Ct& container, const Diff& diff,
   const typename container_traits<Ct>::key_type& target,
   DistanceType bound)
  {
    return neighbor_lower_bound
      (container, euclidian<Ct, DistanceType, Diff>(diff), target, bound);
  }

  template <typename Ct, typename Diff, typename DistanceType>
  inline typename
  enable_if<std::tr1::is_floating_point<DistanceType>,
            euclidian_neighbor_iterator<const Ct, DistanceType, Diff> >::type
  euclidian_neighbor_lower_bound
  (const Ct& container, const Diff& diff,
   const typename container_traits<Ct>::key_type& target,
   DistanceType bound)
  {
    return neighbor_lower_bound
      (container, euclidian<Ct, DistanceType, Diff>(diff), target, bound);
  }

  template <typename Ct, typename Diff, typename DistanceType>
  inline typename
  enable_if<std::tr1::is_floating_point<DistanceType>,
            euclidian_neighbor_iterator<const Ct, DistanceType, Diff> >::type
  euclidian_neighbor_clower_bound
  (const Ct& container, const Diff& diff,
   const typename container_traits<Ct>::key_type& target,
   DistanceType bound)
  {
    return neighbor_lower_bound
      (container, euclidian<Ct, DistanceType, Diff>(diff), target, bound);
  }
  ///@}

  /**
   *  Returns a \ref euclidian_neighbor_iterator<Ct, double, Diff> pointing
   *  to the closest element to \c target that is a least as far as \c bound.
   *
   *  \param container The container to iterate.
   *  \param target Search for element in container closest to target.
   *  \param bound The minimum distance at which an element should be found.
   */
  ///@{
  template <typename Ct, typename DistanceType>
  inline typename
  enable_if<details::is_compare_builtin<Ct>, typename
            enable_if<std::tr1::is_floating_point<DistanceType>,
                      euclidian_neighbor_iterator<Ct, DistanceType> >::type
            >::type
  euclidian_neighbor_lower_bound
  (Ct& container,
   const typename container_traits<Ct>::key_type& target,
   DistanceType bound)
  {
    return neighbor_lower_bound
      (container,
       euclidian<Ct, DistanceType, typename
                 details::with_builtin_difference<Ct, DistanceType>::type>
       (details::with_builtin_difference<Ct, DistanceType>()(container)),
       target, bound);
  }

  template <typename Ct, typename DistanceType>
  inline typename
  enable_if<details::is_compare_builtin<Ct>, typename
            enable_if<std::tr1::is_floating_point<DistanceType>,
                      euclidian_neighbor_iterator<const Ct, DistanceType>
                      >::type
            >::type
  euclidian_neighbor_lower_bound
  (const Ct& container,
   const typename container_traits<Ct>::key_type& target,
   DistanceType bound)
  {
    return neighbor_lower_bound
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct, double>::type>
       (details::with_builtin_difference<Ct, double>()(container)),
       target, bound);
  }

  template <typename Ct, typename DistanceType>
  inline typename
  enable_if<details::is_compare_builtin<Ct>, typename
            enable_if<std::tr1::is_floating_point<DistanceType>,
                      euclidian_neighbor_iterator<const Ct, DistanceType>
                      >::type
            >::type
  euclidian_neighbor_clower_bound
  (const Ct& container,
   const typename container_traits<Ct>::key_type& target,
   DistanceType bound)
  {
    return neighbor_lower_bound
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct, double>::type>
       (details::with_builtin_difference<Ct, double>()(container)),
       target, bound);
  }
  ///@}

  /**
   *  Returns a \ref euclidian_neighbor_iterator<Ct, double, Diff> pointing
   *  to the closest element to \c target that further than \c bound.
   *
   *  \param container The container to iterate.
   *  \param diff A model of \ref DifferenceConcept.
   *  \param target Search for element in container closest to target.
   *  \param bound The minimum distance at which a neighbor should be found.
   */
  ///@{
  template <typename Ct, typename Diff, typename DistanceType>
  inline typename
  enable_if<std::tr1::is_floating_point<DistanceType>,
            euclidian_neighbor_iterator<Ct, DistanceType, Diff> >::type
  euclidian_neighbor_upper_bound
  (Ct& container, const Diff& diff,
   const typename container_traits<Ct>::key_type& target,
   DistanceType bound)
  {
    return neighbor_upper_bound
      (container, euclidian<Ct, DistanceType, Diff>(diff), target, bound);
  }

  template <typename Ct, typename Diff, typename DistanceType>
  inline typename
  enable_if<std::tr1::is_floating_point<DistanceType>,
            euclidian_neighbor_iterator<const Ct, DistanceType, Diff> >::type
  euclidian_neighbor_upper_bound
  (const Ct& container, const Diff& diff,
   const typename container_traits<Ct>::key_type& target,
   DistanceType bound)
  {
    return neighbor_upper_bound
      (container, euclidian<Ct, DistanceType, Diff>(diff), target, bound);
  }

  template <typename Ct, typename Diff, typename DistanceType>
  inline typename
  enable_if<std::tr1::is_floating_point<DistanceType>,
            euclidian_neighbor_iterator<const Ct, DistanceType, Diff> >::type
  euclidian_neighbor_cupper_bound
  (const Ct& container, const Diff& diff,
   const typename container_traits<Ct>::key_type& target,
   DistanceType bound)
  {
    return neighbor_upper_bound
      (container, euclidian<Ct, DistanceType, Diff>(diff), target, bound);
  }
  ///@}

  /**
   *  Returns a \ref euclidian_neighbor_iterator<Ct, double, Diff> pointing
   *  to the closest element to \c target that is further than \c bound.
   *
   *  \param container The container to iterate.
   *  \param target Search for element in container closest to target.
   *  \param bound The minimum distance at which an element should be found.
   */
  ///@{
  template <typename Ct, typename DistanceType>
  inline typename
  enable_if<details::is_compare_builtin<Ct>, typename
            enable_if<std::tr1::is_floating_point<DistanceType>,
                      euclidian_neighbor_iterator<Ct, DistanceType> >::type
            >::type
  euclidian_neighbor_upper_bound
  (Ct& container,
   const typename container_traits<Ct>::key_type& target,
   DistanceType bound)
  {
    return neighbor_upper_bound
      (container,
       euclidian<Ct, DistanceType, typename
                 details::with_builtin_difference<Ct, DistanceType>::type>
       (details::with_builtin_difference<Ct, DistanceType>()(container)),
       target, bound);
  }

  template <typename Ct, typename DistanceType>
  inline typename
  enable_if<details::is_compare_builtin<Ct>, typename
            enable_if<std::tr1::is_floating_point<DistanceType>,
                      euclidian_neighbor_iterator<const Ct, DistanceType>
                      >::type
            >::type
  euclidian_neighbor_upper_bound
  (const Ct& container,
   const typename container_traits<Ct>::key_type& target,
   DistanceType bound)
  {
    return neighbor_upper_bound
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct, double>::type>
       (details::with_builtin_difference<Ct, double>()(container)),
       target, bound);
  }

  template <typename Ct, typename DistanceType>
  inline typename
  enable_if<details::is_compare_builtin<Ct>, typename
            enable_if<std::tr1::is_floating_point<DistanceType>,
                      euclidian_neighbor_iterator<const Ct, DistanceType>
                      >::type
            >::type
  euclidian_neighbor_cupper_bound
  (const Ct& container,
   const typename container_traits<Ct>::key_type& target,
   DistanceType bound)
  {
    return neighbor_upper_bound
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct, double>::type>
       (details::with_builtin_difference<Ct, double>()(container)),
       target, bound);
  }
  ///@}

  /**
   *  Make a pair of iterators spanning the range of iterable element in \c
   *  container from the closest to the furthest to \c target
   *
   *  \param container The container to iterate.
   *  \param diff A model of \ref DifferenceConcept.
   *  \param target Search for element in container closest to target.
   *
   *  The search will occur in euclidian space, where distance are computed in
   *  double, by default. However distances can be expressed in any floating
   *  point type by simply assigning the result to an similar iterator using a
   *  different distance type:
   *
   *  \code
   *  euclidian_neighbor_iterator_pair<Ct, float, Diff> my_float_iterator_pair
   *    = euclidian_neighbor_range(container, diff(), target);
   *  \endcode
   */
  ///@{
  template <typename Ct, typename Diff>
  inline euclidian_neighbor_iterator_pair<Ct, double, Diff>
  euclidian_neighbor_range
  (Ct& container, const Diff& diff,
   const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_range
      (container, euclidian<Ct, double, Diff>(diff), target);
  }

  template <typename Ct, typename Diff>
  inline euclidian_neighbor_iterator_pair<Ct, double, Diff>
  euclidian_neighbor_range
  (const Ct& container, const Diff& diff,
   const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_range
      (container, euclidian<Ct, double, Diff>(diff), target);
  }

  template <typename Ct, typename Diff>
  inline euclidian_neighbor_iterator_pair<Ct, double, Diff>
  euclidian_neighbor_crange
  (const Ct& container, const Diff& diff,
   const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_range
      (container, euclidian<Ct, double, Diff>(diff), target);
  }
  ///@}

  /**
   *  Make a pair of iterators spanning the range of iterable elements in \c
   *  container from the closest to the furthest to \c target.
   *
   *  \param container The container to iterate.
   *  \param target Search for element in container closest to target.
   *
   *  The search will occur in euclidian space, where distance are computed in
   *  double, by default. However distances can be expressed in any floating
   *  point type by simply assigning the result to an similar iterator using a
   *  different distance type:
   *
   *  \code
   *  euclidian_neighbor_iterator_pair<Ct, float, Diff> my_float_iterator_pair
   *    = euclidian_neighbor_range(container, diff(), target);
   *  \rangecode
   */
  ///@{
  template <typename Ct>
  inline typename
  enable_if<details::is_compare_builtin<Ct>,
            euclidian_neighbor_iterator_pair<Ct, double> >::type
  euclidian_neighbor_range
  (Ct& container,
   const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_range
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct, double>::type>
       (details::with_builtin_difference<Ct, double>()(container)),
       target);
  }

  template <typename Ct>
  inline typename
  enable_if<details::is_compare_builtin<Ct>,
            euclidian_neighbor_iterator_pair<const Ct, double> >::type
  euclidian_neighbor_range
  (const Ct& container,
   const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_range
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct, double>::type>
       (details::with_builtin_difference<Ct, double>()(container)),
       target);
  }

  template <typename Ct>
  inline typename
  enable_if<details::is_compare_builtin<Ct>,
            euclidian_neighbor_iterator_pair<const Ct, double> >::type
  euclidian_neighbor_crange
  (const Ct& container,
   const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_range
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct, double>::type>
       (details::with_builtin_difference<Ct, double>()(container)),
       target);
  }
  ///@}

} // namespace spatial

#endif // SPATIAL_EUCLIDIAN_NEIGHBOR_HPP
