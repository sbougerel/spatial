// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_metric.hpp
 *  @brief  Contains the definition of the default metrics available to use
 *  with the neighbor iterators.
 *
 *  \see neighbor
 */

#ifndef SPATIAL_METRIC_HPP
#define SPATIAL_METRIC_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#include "spatial_math.hpp"

namespace spatial
{
  namespace details
  {
    /**
     *  Retrieve the builtin difference functor on the condition that the
     *  compare functor used in Container is a builtin comparator.
     *
     *  If you are not using one of the builtin compare functor for your
     *  Container, then you should also provide your user-defined difference
     *  function when using euclidian_neighbor_iterator<> or other similar
     *  iterators.
     *
     *  \tparam Container The container used in the iterator.
     *  \tparam DistanceType The type used to express distances.
     */
    //@{
    template<typename Container, typename DistanceType, typename Enable = void>
    struct with_builtin_difference { }; // sink for not using built-in compare

    template<typename Container, typename DistanceType>
    struct with_builtin_difference
    <Container, DistanceType,
     typename enable_if<is_compare_builtin<Container> >::type>
    {
      /**
       *  This internal type casting is used to resolve a built-in compare
       *  functor (provided by the library) into a built-in difference functor.
       *  It will not work for user defined comparators; it means that if you
       *  are using a user-defined comparator in your container, you should also
       *  use a user-defined metric.
       *
       *  \tparam Compare The comparator used in the container.
       *  \tparam DistanceType The type used to express distances.
       */
      //@{
      template<typename, typename>
      struct builtin_difference { }; // sink type, never used normally

      template<typename Key, typename Tp>
      struct builtin_difference<bracket_less<Key>, Tp>
      {
        typedef bracket_minus<Key, Tp> type;
        type operator() (const bracket_less<Key>&) const
        { return type(); }
      };

      template<typename Key, typename Tp>
      struct builtin_difference<paren_less<Key>, Tp>
      {
        typedef paren_minus<Key, Tp> type;
        type operator() (const paren_less<Key>&) const
        { return type(); }
      };

      template<typename Key, typename Tp>
      struct builtin_difference<iterator_less<Key>, Tp>
      {
        typedef iterator_minus<Key, Tp> type;
        type operator() (const iterator_less<Key>&) const
        { return type(); }
      };

      template<typename Key, typename Tp, typename Accessor>
      struct builtin_difference<accessor_less<Accessor, Key>, Tp>
      {
        typedef accessor_minus<Accessor, Key, Tp> type;
        type operator() (const accessor_less<Accessor, Key>& cmp) const
        { return type(cmp.accessor()); }
      };
      //@}

      typedef typename builtin_difference
      <typename container_traits<Container>::key_compare,
       DistanceType>::type type;

      //! Constructs the difference type from the built-in container's key
      //! compare operator.
      type operator()(const Container& container) const
      {
        return builtin_difference
          <typename container_traits<Container>::key_compare, DistanceType>()
          (container.key_comp());
      }
    };
    //@}
  } // namespace details

  /**
   *  Defines a metric working on the Euclidian space where distances are
   *  expressed in one of C++'s floating point types.
   *
   *  \concept A concept of \ref Metric.
   *
   *  \attention \c This metric works on floating types only. It will fail
   *  to compile if given non-floating types as a parameter for the distance.
   *
   *  \ref euclidian attempts to compute distances while limitting loss of
   *  precision due to overflow during the computation. \ref euclidian could
   *  be more precise than \ref quadrance in some cases, but it will be
   *  slower in all cases.
   */
  //@{
  template<typename Container, typename DistanceType, typename Diff,
           typename Enable = void>  // Sink for non floating-point types
  class euclidian { };

  template<typename Ct, typename DistanceType, typename Diff>
  class euclidian
  <Ct, DistanceType, Diff,
   typename enable_if<std::tr1::is_floating_point<DistanceType> >::type>
  {
    /**
     *  The key_type of the container being used for calculations.
     */
    typedef typename container_traits<Ct>::key_type key_type;

    /**
     *  Placeholder for the 'difference' functor.
     */
    Diff _diff;

  public:
    /**
     *  The distance type being used for distance calculations.
     */
    typedef DistanceType distance_type;

    //! The constructors allows you to specify a custom difference type.
    euclidian(const Diff& diff = Diff()) : _diff(diff) { }

    /**
     *  Compute the distance between the point of @c origin and the @c key.
     *  @return The resulting distance.
     */
    distance_type
    distance_to_key(dimension_type rank,
                    const key_type& origin, const key_type& key) const
    {
      return math::euclid_distance_to_key
        <key_type, Diff, DistanceType>(rank, origin, key, _diff);
    }

    /**
     *  The distance between the point of \c origin and the closest point to
     *  the plane orthogonal to the axis of dimension \c dim and crossing \c
     *  key.
     *
     *  Given any 2 points 'origin' and 'key', the result of distance_to_plane
     *  must always be less or equal to the result of distance_to_key.
     *
     *  \return The resulting distance.
     */
    distance_type
    distance_to_plane(dimension_type, dimension_type dim,
                      const key_type& origin, const key_type& key) const
    {
      return math::euclid_distance_to_plane
        <key_type, Diff, DistanceType>(dim, origin, key, _diff);
    }
  };
  //@}

  /**
   *  Defines a metric in the Euclidian space where only the square of
   *  the distances are being computed into a scalar value expressed with
   *  the DistanceType which is one of C++'s arithmetic types.
   *
   *  \concept A concept of \ref Metric.
   *
   *  This method of distance calculation is more flexible than \ref
   *  euclidian<> since it can support all of the C++'s arithmetic types.
   *
   *  When using this metric and reading the distance value, you must
   *  remember that you are reading the square of the distance, and not the
   *  real distance. Thus, you should use square root to recover the real
   *  distance.
   *
   *  This metric has one important drawback: if you are working with large
   *  value over the entire range permissible by DistanceType, then chances
   *  that the computation overflows is non-negligible. To receive an \ref
   *  arithmetic_error exception upon overflow, compile your application with
   *  \c #define \c SPATIAL_SAFER_ARITHEMTICS.
   *
   */
  //@{
  template<typename Ct, typename DistanceType, typename Diff,
           typename Enable = void> // Sink for non-arithmetic types
  class quadrance { };

  template<typename Ct, typename DistanceType, typename Diff>
  class quadrance
  <Ct, DistanceType, Diff,
   typename enable_if<std::tr1::is_arithmetic<DistanceType> >::type>
  {
    /**
     *  The key_type of the container being used for calculations.
     */
    typedef typename container_traits<Ct>::key_type key_type;

    /**
     *  Placeholder for the 'difference' functor.
     */
    Diff _diff;

  public:
    typedef DistanceType distance_type;

    //! The constructor allows you to specify a custom difference type.
    quadrance(const Diff& diff = Diff()) : _diff(diff) { }

    /**
     *  Compute the distance between the point of @c origin and the @c key.
     *  @return The resulting square distance.
     */
    distance_type
    distance_to_key(dimension_type rank,
                    const key_type& origin, const key_type& key) const
    {
      return math::square_euclid_distance_to_key
        <key_type, Diff, DistanceType>(rank, origin, key, _diff);
    }

    /**
     *  The distance between the point of @c origin and the closest point to
     *  the plane orthogonal to the axis of dimension @c dim and crossing @c
     *  key.
     *  \return The resulting square distance.
     */
    distance_type
    distance_to_plane(dimension_type, dimension_type dim,
                      const key_type& origin, const key_type& key) const
    {
      return math::square_euclid_distance_to_plane
        <key_type, Diff, DistanceType>(dim, origin, key, _diff);
    }
  };
  //@}

  /**
   *  Defines a metric for the a space where distances are the sum
   *  of all the elements of the vector. Also known as the taxicab metric.
   *
   *  \concept A concept of \ref Metric.
   *
   *  \tparam Ct The container used with this metric.
   *  \tparam DistanceType The type used to compute distance values.
   *  \tparam Diff A difference functor that compute the difference between 2
   *  elements of a the Container's key type, along the same dimension.
   *
   *  This method of distance calculation is more flexible than \ref
   *  euclidian<> since it can support all of the C++'s arithmetic types.
   *
   *  This metric is the fastest of all built-in metric, and generally offers
   *  an acceptable approaximation to the euclidian metric. However the
   *  distance calculated by this metric cannot be converted into Euclidian
   *  distances. If you are looking for a fast metric that is convertible in
   *  Euclidian distances, check out \ref quadrance<>.
   *
   *  This metric has one important drawback: if you are working with large
   *  value over the entire range permissible by DistanceType, then chances
   *  that the computation overflows is non-negligible. To receive an \ref
   *  arithmetic_error exception upon overflow, compile your application with
   *  \c #define \c SPATIAL_SAFER_ARITHEMTICS.
   */
  ///@{
  template<typename Ct, typename DistanceType, typename Diff,
           typename Enable = void> // Sink for non-arithmetic types
  class manhattan { };

  template<typename Ct, typename DistanceType, typename Diff>
  class manhattan
  <Ct, DistanceType, Diff,
   typename enable_if<std::tr1::is_arithmetic<DistanceType> >::type>
  {
    /**
     *  The key_type of the container being used for calculations.
     */
    typedef typename container_traits<Ct>::key_type key_type;

    /**
     *  Placeholder for the 'difference' functor.
     */
    Diff _diff;

  public:
    typedef DistanceType distance_type;

    //! A constructor that allows you to specify the Difference type.
    manhattan(const Diff& diff = Diff()) : _diff(diff) { }

    /**
     *  Compute the distance between the point of \c origin and the \c key.
     *  \return The resulting manahattan distance.
     */
    distance_type
    distance_to_key(dimension_type rank,
                    const key_type& origin, const key_type& key) const
    {
      return math::manhattan_distance_to_key
        <key_type, Diff, DistanceType>(rank, origin, key, _diff);
    }

    /**
     *  The distance between the point of \c origin and the closest point to
     *  the plane orthogonal to the axis of dimension \c dim and crossing \c
     *  key.
     *
     *  \return The resulting manhattan distance.
     */
    distance_type
    distance_to_plane(dimension_type, dimension_type dim,
                      const key_type& origin, const key_type& key) const
    {
      return math::manhattan_distance_to_plane
        <key_type, Diff, DistanceType>(dim, origin, key, _diff);
    }
  };
  ///@}

} // namespace spatial

#endif // SPATIAL_METRIC_HPP
