// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_geometry.hpp
 *  @brief  Contains the definition of the default geometries available to use
 *  with the neighbor iterators.
 *
 *  \see neighbor
 */

#ifndef SPATIAL_GEOMETRY_HPP
#define SPATIAL_GEOMETRY_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#include "spatial_math.hpp"

namespace spatial
{
  namespace details
  {
    /**
     *  The internal type auto_difference is used to resolve a compare functor
     *  provided by the library into a difference functor provided by the
     *  library. Will not work for user defined comparators (i.e. they should
     *  define their own geometry).
     */
    //@{
    template<typename Compare, typename Unit>
    struct auto_difference { };

    template<typename Key, typename Unit>
    struct auto_difference<bracket_less<Key>, Unit>
    {
      typedef bracket_minus<Key, Unit> type;
      type operator() (const bracket_less<Key>&) const
      { return type(); }
    };

    template<typename Key, typename Unit>
    struct auto_difference<paren_less<Key>, Unit>
    {
      typedef paren_minus<Key, Unit> type;
      type operator() (const paren_less<Key>&) const
      { return type(); }
    };

    template<typename Key, typename Unit>
    struct auto_difference<iterator_less<Key>, Unit>
    {
      typedef iterator_minus<Key, Unit> type;
      type operator() (const iterator_less<Key>&) const
      { return type(); }
    };

    template<typename Key, typename Unit, typename Accessor>
    struct auto_difference<accessor_less<Accessor, Key>, Unit>
    {
      typedef accessor_minus<Accessor, Key, Unit> type;
      type operator() (const accessor_less<Accessor, Key>& cmp) const
      { return type(cmp.accessor()); }
    };
    //@}

    /**
     *  The function resolve the appropriate difference functor
     *  from the comparators provided by the library are:
     *  \ul \ref bracket_less is resolved to \ref bracket_minus,
     *  \li \ref paren_less is resolved to \ref paren_minus,
     *  \li \ref iterator_less is resolved to \ref iterator_minus,
     *  \li \ref accessor_less is resolved to \ref accessor_minus with the
     *  appropriate accessor functor.
     *  \lu
     *
     *  If the comparator used for the container is not one of the comparators
     *  provided, by the library, the difference_cast function will not compile
     *  since the type \mono type will be non-existant.
     */
    template <typename Compare, typename Unit>
    inline typename auto_difference<Compare, Unit>::type
    auto_difference_cast(const Compare& cmp)
    {
      return auto_difference<Compare, Unit>()(cmp);
    }

    /**
     *  Defines a geometry working on the Euclidian space where distances are
     *  expressed in \c double and the distance between 2 points is calculated
     *  via the functor \c Difference, a concept of \ref ElementDifference.
     *
     *  @concept euclid is a concept of \ref Geometry.
     *
     *  @attention The type returned by \c Difference shall allow casting into
     *  the plain old data type \c double.
     *
     *  @attention \c euclid is written to work on floating types only. It will
     *  not compile if given non-floating types as parameter.
     *
     *  \c euclid attempts to compute distances while limitting loss of
     *  precision due to overflow during the computation. For the \c double
     *  type, \c euclid could be more precise than \ref sqeuclid in some cases,
     *  but it will be slower in all cases.
     */
    //@{
    template<typename Ct, typename Unit, typename Diff,
             typename Enable = void>  // Sink for non floating-point types
    class euclid_geometry { };

    template<typename Ct, typename Unit, typename Diff>
    class euclid_geometry
    <Ct, Unit, Diff,
     typename enable_if<std::tr1::is_floating_point<Unit> >::type>
    {
      /**
       *  The rank_type of the container being used for calculations.
       */
      typedef typename container_traits<Ct>::rank_type rank_type;

      /**
       *  The key_type of the container being used for calculations.
       */
      typedef typename container_traits<Ct>::key_type key_type;

      /**
       *  Placeholder for the 'difference' functor.
       */
      Diff diff_;

    public:
      /**
       *  The distance type being used for distance calculations.
       */
      typedef Unit distance_type;

      //! The constructors allows you to specify a custom difference type.
      euclid_geometry(const Diff& diff = Diff()) : diff_(diff) { }

      /**
       *  Compute the distance between the point of @c origin and the @c key.
       *  @return The resulting distance.
       */
      distance_type
      distance_to_key(const rank_type& rank,
                      const key_type& origin, const key_type& key) const
      {
        return math::euclid_distance_to_key
          <rank_type, key_type, Diff, Unit>(rank, origin, key, diff_);
      }

      /**
       *  The distance between the point of @c origin and the closest point to
       *  the plane orthogonal to the axis of dimension @c dim and crossing @c
       *  key.
       *
       *  Given any 2 points 'origin' and 'key', the result of distance_to_plane
       *  must always be less or equal to the result of distance_to_key.
       *
       *  \return The resulting distance.
       */
      distance_type
      distance_to_plane(const rank_type&, dimension_type dim,
                        const key_type& origin, const key_type& key) const
      {
        return math::euclid_distance_to_plane
          <key_type, Diff, Unit>(dim, origin, key, diff_);
      }
    };
    //@}

    /**
     *  Defines the geometry for the euclidian space where only the square of
     *  the distances are being computed into a scalar value expressed as a
     *  double.
     *
     *  This method of distance calculation is very portable: it works with all
     *  signed and unsigned base types, as well as any user-defined type for
     *  which the basic operations substraction, addition and multiplication
     *  have been defined.
     *
     *  When using this geometry, if you must read the distance value, you must
     *  remember that you are reading the square of the distance, and not the
     *  real distance. Thus, you should use square root to recover the real
     *  distance.
     *
     *  This geometry has one important drawback: if you are working with large
     *  value over the entire range permissible by the value type, then chances
     *  that the computation overflows is non-negligible. To receive an \ref
     *  arithmetic_error exception upon overflow, compile your application with
     *  \c #define \c SPATIAL_SAFER_ARITHEMTICS.
     */
    //@{
    template<typename Ct, typename Unit, typename Diff,
             typename Enable = void> // Sink for non-arithmetic types
    class square_euclid_geometry { };

    template<typename Ct, typename Unit, typename Diff>
    class square_euclid_geometry
    <Ct, Unit, Diff,
     typename enable_if<std::tr1::is_arithmetic<Unit> >::type>
    {
      /**
       *  The rank_type of the container being used for calculations.
       */
      typedef typename container_traits<Ct>::rank_type rank_type;

      /**
       *  The key_type of the container being used for calculations.
       */
      typedef typename container_traits<Ct>::key_type key_type;

      /**
       *  Placeholder for the 'difference' functor.
       */
      Diff diff_;

    public:
      typedef Unit distance_type;

      //! The constructor allows you to specify a custom difference type.
      square_euclid_geometry(const Diff& diff = Diff()) : diff_(diff) { }

      /**
       *  Compute the distance between the point of @c origin and the @c key.
       *  @return The resulting square distance.
       */
      distance_type
      distance_to_key(const rank_type& rank,
                      const key_type& origin, const key_type& key) const
      {
        return math::square_euclid_distance_to_key
          <key_type, Diff, Unit>(rank, origin, key, diff_);
      }

      /**
       *  The distance between the point of @c origin and the closest point to
       *  the plane orthogonal to the axis of dimension @c dim and crossing @c
       *  key.
       *  \return The resulting square distance.
       */
      distance_type
      distance_to_plane(const rank_type&, dimension_type dim,
                        const key_type& origin, const key_type& key) const
      {
        return math::square_euclid_distance_to_plane
          <key_type, Diff, Unit>(dim, origin, key, diff_);
      }
    };
    //@}

    /**
     *  Defines a geometry for the euclidian space where distances are the sum
     *  of all the elements of a vector. Also known as the taxicab geometry.
     *
     *  This method of distance calculation is very portable: it works with all
     *  signed and unsigned base types, as well as any user-defined type for
     *  which the basic operations substraction and addition have been defined.
     *
     *  This geometry has one important drawback: if you are working with large
     *  values over the entire range permissible by the value type, then chances
     *  that the computation overflows is non-negligible. To receive an \ref
     *  arithmetic_error exception upon overflow, compile your application with
     *  \c #define \c SPATIAL_SAFER_ARITHEMTICS.
     */
    //@{
    template<typename Ct, typename Unit, typename Diff,
             typename Enable = void> // Sink for non-arithmetic types
    class manhattan_geometry { };

    template<typename Ct, typename Unit, typename Diff>
    class manhattan_geometry
    <Ct, Unit, Diff,
     typename enable_if<std::tr1::is_arithmetic<Unit> >::type>
    {
      /**
       *  The rank_type of the container being used for calculations.
       */
      typedef typename container_traits<Ct>::rank_type rank_type;

      /**
       *  The key_type of the container being used for calculations.
       */
      typedef typename container_traits<Ct>::key_type key_type;

      /**
       *  Placeholder for the 'difference' functor.
       */
      Diff diff_;

    public:
      typedef Unit distance_type;

      //! A constructor that allows you to specify the Difference type.
      manhattan_geometry(const Diff& diff = Diff()) : diff_(diff) { }

      /**
       *  Compute the distance between the point of @c origin and the @c key.
       *  @return The resulting square distance.
       */
      distance_type
      distance_to_key(const rank_type& rank,
                      const key_type& origin, const key_type& key) const
      {
        return math::manhattan_distance_to_key
          <key_type, Diff, Unit>(rank, origin, key, diff_);
      }

      /**
       *  The distance between the point of @c origin and the closest point to
       *  the plane orthogonal to the axis of dimension @c dim and crossing @c
       *  key.
       *
       *  @return The resulting square distance.
       */
      distance_type
      distance_to_plane(const rank_type&, dimension_type dim,
                        const key_type& origin, const key_type& key) const
      {
        return math::manhattan_distance_to_plane
          <key_type, Diff, Unit>(dim, origin, key, diff_);
      }
    };
    //@}
  } // namespace details

} // namespace spatial

#endif // SPATIAL_GEOMETRY_HPP
