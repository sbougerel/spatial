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

namespace spatial
{
  namespace math
  {
    /**
     *  @brief  Uses hypot algorithm in order to compute the distance: minimize
     *  possibilities of loss of precision due to overflow and underflow.
     *  @test   test_euclidian_distance_to_key
     *
     *  The trick is to find the maximum value among all the component of the
     *  Key and then divide all other component with this one.
     *
     *  Here is the rationale for the distance:
     *  sqrt( x^2 + y^2 + z^2 + ... ) = abs(x) * sqrt( 1 + (y/x)^2 + (z/x)^2 )
     *
     *  Providing x statisfies x>y, x>z, etc, the second form is less likely to
     *  overflow or underflow than the first form.
     */
    template <typename Key, typename Difference, typename Distance>
    inline Distance
    euclid_distance_to_key
    (dimension_type rank, Key origin, Key key, Difference diff)
    {
      const Distance zero = static_cast<Distance>(0);
      const Distance one = static_cast<Distance>(1);
      // Find a non zero maximum or return 0
      Distance max = zero;
      dimension_type max_dim; // uninitialized on purpose
      using namespace ::std;
      for (dimension_type i=0; i<rank; ++i)
	{
	  Distance d = abs(difference(i, origin, key));
	  if (d == zero) continue;
	  if (max == zero || d > max) { max = d; max_dim = i; }
	}
      if (max == zero) return zero; // they're all zero!
      // Compute the distance
      Distance sum = zero;
      for (dimension_type i=0; i<rank; ++i)
	{
	  if (i == max_dim) continue;
	  Distance div = diff(i, origin, key) / max;
	  sum += div * div;
	}
#ifdef SPATIAL_SAFER_ARITHMETICS
      return ::spatial::except::check_positive_mul(max, sqrt(one + sum));
#else
      return max * sqrt(one + sum);
#endif
    }

    /**
     *  @brief  Compute the distance between the @p origin and the closest point
     *  to the plane orthogonal to the axis of dimension @c dim and passing by
     *  @c key.
     */
    template <typename Key, typename Difference, typename Distance>
    inline Distance
    euclid_distance_to_plane
    (dimension_type dim, Key origin, Key key, Difference diff)
    {
      using namespace ::std;
      return abs(diff(dim, origin, key));
    }

    /**
     *  @brief  Compute the distance between the @p origin and the closest point
     *  to the plane orthogonal to the axis of dimension @c dim and passing by
     *  @c key.
     */
    template <typename Key, typename Difference, typename Distance>
    inline Distance
    sqeuclid_distance_to_plane
    (dimension_type dim, Key origin, Key key, Difference diff)
    {
#ifdef SPATIAL_SAFER_ARITHMETICS
      Distance d = diff(dim, origin, key);
      return ::spatial::except::check_square(d);
#else
      Distance d = diff(dim, origin, key);
      return d * d;
#endif
    }

    /**
     *  @brief  Compute the square value of the distance between @p origin and
     *  @p key.
     */
    template <typename Key, typename Difference, typename Distance>
    inline Distance
    sqeuclid_distance_to_key
    (dimension_type rank, Key origin, Key key, Difference diff)
    {
      Distance sum = static_cast<Distance>(0);
      for (dimension_type i=0; i<rank; ++i)
	{
#ifdef SPATIAL_SAFER_ARITHMETICS
	  sum = ::spatial::except::check_positive_add
	    (sqeuclid_distance_to_plane<Key, Difference, Distance>
	     (i, origin, key, diff), sum);
#else
	  sum += sqeuclid_distance_to_plane
	    <Key, Difference, Distance>(i, origin, key, diff);
#endif
	}
      return sum;
    }

    /**
     *  @brief  Compute the distance between the @p origin and the closest point
     *  to the plane orthogonal to the axis of dimension @c dim and passing by
     *  @c key.
     */
    template <typename Key, typename Difference, typename Distance>
    inline Distance
    manhattan_distance_to_plane
    (dimension_type dim, Key origin, Key key, Difference diff)
    {
      using namespace ::std;
      return abs(diff(dim, origin, key));
    }

    /**
     *  @brief  Compute the manhattan distance between @p origin and @p key.
     */
    template <typename Key, typename Difference, typename Distance>
    inline Distance
    manhattan_distance_to_key
    (dimension_type rank, Key origin, Key key, Difference diff)
    {
      Distance sum = static_cast<Distance>(0);
      for (dimension_type i=0; i<rank; ++i)
	{
#ifdef SPATIAL_SAFER_ARITHMETICS
	  sum = ::spatial::except::check_positive_add
	    (manhattan_distance_to_plane<Key, Difference, Distance>
	     (i, origin, key, diff), sum);
#else
	  sum += manhattan_distance_to_plane
	    <Key, Difference, Distance>(i, origin, key, diff);
#endif
	}
      return sum;
    }

    /*
      // For a future implementation where we take earth-like spheroid as an
      // example for non-euclidian spaces, or manifolds.

      math::great_circle_distance_to_key
      math::great_circle_distance_to_plane
      math::vincenty_distance_to_key
      math::vincenty_distance_to_plane
    */
  } // namespace math

  namespace details
  {
    /**
     *  An internal type used by auto_difference to resolve a compare
     *  functor into a difference functor.
     */
    //@{
    template<typename Compare, typename Unit>
    struct auto_difference_resolver { typedef void type; };

    template<typename Tp, typename Unit>
    struct auto_difference_resolver<bracket_less<Tp>, Unit>
    {
      typedef bracket_minus<Tp, Unit> type;
      type make(const bracket_less<Tp>&) const { return type(); }
    };

    template<typename Tp, typename Unit>
    struct auto_difference_resolver<paren_less<Tp>, Unit>
    {
      typedef paren_minus<Tp, Unit> type;
      type make(const paren_less<Tp>&) const { return type(); }
    };

    template<typename Tp, typename Unit>
    struct auto_difference_resolver<iterator_less<Tp>, Unit>
    {
      typedef iterator_minus<Tp, Unit> type;
      type make(const iterator_less<Tp>&) const { return type(); }
    };

    template<typename Accessor, typename Tp, typename Unit>
    struct auto_difference_resolver<accessor_less<Accessor, Tp>, Unit>
    {
      typedef accessor_minus<Accessor, Tp, Unit> type;
      type make(const accessor_less<Accessor, Tp>& cmp) const
      { return type(static_cast<const Accessor&>(cmp)); }
    };
    //@}

  } // namespace details

  /**
   *  The type auto_difference resolve the appropriate difference functor to
   *  use if the container Ct was created with one of the comparators provided
   *  by the library:
   *  \ul \ref bracket_less is resolved to \ref bracket_minus,
   *  \li \ref paren_less is resolved to \ref paren_minus,
   *  \li \ref iterator_less is resolved to \ref iterator_minus,
   *  \li \ref accessor_less is resolved to \ref accessor_minus with the
   *  appropriate accessor functor.
   *  \lu
   *
   *  If the comparator used for the container is not one of the comparators
   *  provided, by the library, the auto_difference function resolves into void.
   */
  template <typename Ct, typename Unit>
  struct auto_difference
  {
    //! The \c type that is resolved into the difference functor.
    typedef typename details::auto_difference_resolver
      <typename container_traits<Ct>::key_compare, Unit> type;

    //! A factory that builds the difference functor from the compare functor.
    static type make(const typename container_traits<Ct>::key_compare& cmp)
    {
      details::auto_difference_resolver
      <typename container_traits<Ct>::key_compare, Unit> res;
      return res.make(cmp);
    }
  };

  /**
   *  @brief  Defines a geometry working on the Euclidian space where distances
   *  are expressed in \c double and the distance between 2 points is calculated
   *  via the functor \c Difference, a concept of \ref ElementDifference.
   *
   *  @concept euclid is a concept of \ref Geometry.
   *
   *  @attention The type returned by \c Difference shall allow casting into the
   *  plain old data type \c double.
   *
   *  @attention \c euclid is written to work on doubles. It will not work
   *  on integral types, not even approximately. If you are looking computing
   *  distances in Euclidian geometry using integers, consider using \ref
   *  sqeuclid. It is very fast because it omits the square root calculation
   *  and it is as precise as the integral types permits.
   *
   *  \c euclid attempts to compute distances while limitting loss of
   *  precision due to overflow during the computation. For the \c double type,
   *  \c euclid could be more precise than \ref sqeuclid in some cases,
   *  but it will be slower in all cases.
   *
   *  \c euclid computes distances that are expressed in \c double. If you
   *  need to express your distances in \c float, the geometry \ref euclidf
   *  is working on \c float.
   */
  template<typename Tp, typename Difference>
  struct euclid
    : private Difference // empty member optimization
  {
    /**
     *  @brief  The distance type being used for distance calculations.
     */
    typedef double distance_type;

    //! The constructors allows you to specify a custom difference type.
    euclid(const Difference& diff = Difference()) : Difference(diff) { }

    /**
     *  @brief  Compute the distance between the point of @c origin and the @c
     *  key.
     *  @return  The resulting distance.
     */
    distance_type
    distance_to_key(dimension_type rank,
		    const Tp& origin, const Tp& key) const
    {
      return math::euclid_distance_to_key<Tp, Difference, double>
	(rank, origin, key, *static_cast<const Difference*>(this));
    }

    /**
     *  @brief  The distance between the point of @c origin and the closest
     *  point to the plane orthogonal to the axis of dimension @c dim and
     *  crossing @c key.
     *  @return The resulting distance.
     *
     *  Given any 2 points 'origin' and 'key', the result of distance_to_plane
     *  must always be less or equal to the result of distance_to_key.
     */
    distance_type
    distance_to_plane(dimension_type, dimension_type dim,
		      const Tp& origin, const Tp& key) const
    {
      return math::euclid_distance_to_plane<Tp, Difference, double>
	(dim, origin, key, *static_cast<const Difference*>(this));
    }
  };

  /**
   *  Facilitates creation of a type \ref euclid from an existing container.
   */
  template<typename Ct, typename Difference>
  euclid<typename container_traits<Ct>::key_type, Difference>
  make_euclid(const Ct& container, const Difference& diff)
  {
    return euclid<typename container_traits<Ct>::key_type,
                      Difference>(diff);
  }

  /**
   *  Facilitates creation of a type \ref euclid from an existing container.
   */
  template<typename Ct>
  euclid<typename container_traits<Ct>::key_type,
             typename auto_difference<Ct, double>::type>
  make_euclid_auto(const Ct& container)
  {
    return euclid<typename container_traits<Ct>::key_type,
                      typename auto_difference<Ct, double>::type>
		      (auto_difference<Ct, double>::make(diff));
  }

  /**
   *  @brief  Defines a geometry working on the Euclidian space where distances
   *  are expressed in \c float and the distance between 2 points is calculated
   *  via the functor \c Difference, a concept of \ref ElementDifference.
   *
   *  @concept euclidf is a concept of \ref Geometry.
   *
   *  @attention The type returned by \c Difference shall allow casting into the
   *  plain old data type \c float.
   *
   *  @attention \c euclidf is written to work on floats. It will not work
   *  on integral types, not even approximately. If you are looking computing
   *  distances in Euclidian geometry using integers, consider using \ref
   *  sqeuclid. It is very fast because it omits the square root calculation
   *  and it is as precise as the integral types permits, also it may overflow
   *  during calculation.
   *
   *  \c euclidf attempts to compute distances while limitting loss of
   *  precision due to overflow during the computation. For the \c float type,
   *  \c euclid could be more precise than \ref sqeuclid in some cases,
   *  but it will be slower in all cases.
   *
   *  \c euclidf computes distances that are expressed in \c float. If you
   *  need to express your distances in \c double, the geometry \ref euclid
   *  is working on \c double.
   */
  template<typename Tp, typename Difference>
  struct euclidf
    : private Difference
  {
    /**
     *  @brief  The distance type being used for distance calculations.
     */
    typedef float distance_type;

    //! The constructor allows you to specify a custom difference type.
    euclidf(const Difference& diff = Difference()) : Difference(diff) { }

    /**
     *  @brief  Compute the distance between the point of @c origin and the @c
     *  key.
     *  @return  The resulting distance.
     */
    distance_type
    distance_to_key(dimension_type rank,
		    const Tp& origin, const Tp& key) const
    {
      return math::euclid_distance_to_key<Tp, Difference, float>
	(rank, origin, key, *static_cast<const Difference*>(this));
    }

    /**
     *  @brief  The distance between the point of @c origin and the closest
     *  point to the plane orthogonal to the axis of dimension @c dim and
     *  crossing @c key.
     *  @return The resulting distance.
     *
     *  Given any 2 points 'origin' and 'key', the result of distance_to_plane
     *  must always be less or equal to the result of distance_to_key.
     */
    distance_type
    distance_to_plane(dimension_type, dimension_type dim,
		      const Tp& origin, const Tp& key) const
    {
      return math::euclid_distance_to_plane<Tp, Difference, float>
	(dim, origin, key, *static_cast<const Difference*>(this));
    }
  };

  /**
   *  Facilitates creation of a type \ref euclidf from an existing container.
   */
  template<typename Ct, typename Difference>
  euclidf<typename container_traits<Ct>::key_type, Difference>
  make_euclidf(const Ct& container, const Difference& diff)
  {
    return euclidf<typename container_traits<Ct>::key_type,
                   Difference>(diff);
  }

  /**
   *  Facilitates creation of a type \ref euclidf from an existing container.
   */
  template<typename Ct>
  euclidf<typename container_traits<Ct>::key_type,
              typename auto_difference<Ct, float>::type>
  make_euclidf_auto(const Ct& container)
  {
    return euclidf<typename container_traits<Ct>::key_type,
                   typename auto_difference<Ct, float>::type>
		   (auto_difference<Ct, float>::make(diff));
  }

  /**
   *  @brief  Defines the geometry for the euclidian space where only the square
   *  of the distances are being computed into a scalar value expressed as a
   *  double.
   *
   *  This method of distance calculation is very portable: it works with
   *  all signed and unsigned base types, as well as any user-defined type for
   *  which the basic operations substraction, addition and multiplication have
   *  been defined.
   *
   *  When using this geometry, if you must read the distance value, you must
   *  remember that you are reading the square of the distance, and not the
   *  real distance. Thus, you should use square root to recover the real
   *  distance.
   *
   *  This geometry has one important drawback: if you are working with large
   *  value over the entire range permissible by the value type, then chances
   *  that the computation overflows is non-negligible. To receive an
   *  \ref arithmetic_error exception upon overflow, compile your application
   *  with \c #define \c SPATIAL_SAFER_ARITHEMTICS.
   */
  template<typename Tp, typename Difference,
           typename Distance = typename Difference::difference_type>
  struct sqeuclid
    : private Difference // empty-member optimization
  {
    typedef Distance distance_type;

    //! The constructor allows you to specify a custom difference type.
    sqeuclid(const Difference& diff = Difference()) : Difference(diff) { }

    /**
     *  @brief  Compute the distance between the point of @c origin and the @c
     *  key.
     *  @return The resulting square distance.
     */
    distance_type
    distance_to_key(dimension_type rank,
		    const Tp& origin, const Tp& key) const
    {
      return math::sqeuclid_distance_to_key<Tp, Difference, Distance>
	(rank, origin, key, *static_cast<const Difference*>(this));
    }

    /**
     *  @brief  The distance between the point of @c origin and the closest
     *  point to the plane orthogonal to the axis of dimension @c dim and
     *  crossing @c key.
     *  @return The resulting square distance.
     */
    distance_type
    distance_to_plane(dimension_type, dimension_type dim,
		      const Tp& origin, const Tp& key) const
    {
      return math::sqeuclid_distance_to_plane<Tp, Difference, Distance>
	(dim, origin, key, *static_cast<const Difference*>(this));
    }
  };

  /**
   *  Facilitates creation of a type \ref sqeuclid from an existing
   *  container.
   */
  template<typename Ct, typename Difference>
  sqeuclid<typename container_traits<Ct>::key_type, Difference>
  make_sqeuclid(const Ct& container, const Difference& diff)
  {
    return sqeuclid<typename container_traits<Ct>::key_type,
                    Difference>(diff);
  }

  /**
   *  Facilitates creation of a type \ref sqeuclid from an existing
   *  container and if you want to use a type for distance that is different
   *  from the type given in the \c Difference functor.
   *
   *  Use of function is a little particular. It needs a value of type
   *  \c Distance in order to guess what type is Distance, but it does not use
   *  the value. By convention just put 1, 1.0, 1.f, 1u, 1l, 1ll or any other
   *  expression of "one" as an input to this function.
   */
  template<typename Ct, typename Difference, typename Distance>
  sqeuclid<typename container_traits<Ct>::key_type, Difference, Distance>
  make_sqeuclid(const Ct& container, const Difference& diff, Distance)
  {
    return sqeuclid<typename container_traits<Ct>::key_type,
                    Difference, Distance>(diff);
  }

  /**
   *  Facilitates creation of a type \ref sqeuclid from an existing
   *  container.
   *
   *  Use of function is a little particular. It needs a value of type
   *  \c Distance in order to guess what type is Distance, but it does not use
   *  the value. By convention just put 1, 1.0, 1.f, 1u, 1l, 1ll or any other
   *  expression of "one" as an input to this function.
   */
  template<typename Ct, typename Distance>
  sqeuclid<typename container_traits<Ct>::key_type,
               typename auto_difference<Ct, Distance>::type, Distance>
  make_sqeuclid_auto(const Ct& container, Distance)
  {
    return sqeuclid<typename container_traits<Ct>::key_type,
                    typename auto_difference<Ct, Distance>::type, Distance>
		    (auto_difference<Ct, Distance>::make(diff));
  }

  /**
   *  @brief  Defines a geometry for the euclidian space where distances are the
   *  sum of all the elements of a vector. Also known as the taxicab geometry.
   *
   *  This method of distance calculation is very portable: it works with
   *  all signed and unsigned base types, as well as any user-defined type for
   *  which the basic operations substraction and addition have been defined.
   *
   *  This geometry has one important drawback: if you are working with large
   *  values over the entire range permissible by the value type, then chances
   *  that the computation overflows is non-negligible. To receive an
   *  \ref arithmetic_error exception upon overflow, compile your application
   *  with \c #define \c SPATIAL_SAFER_ARITHEMTICS.
   */
  template<typename Tp, typename Difference,
           typename Distance = typename Difference::difference_type>
  struct manhattan
    : private Distance // empty-member optimization
  {
    typedef Distance distance_type;

    //! A constructor that allows you to specify the Difference type.
    manhattan (const Difference& diff = Difference()) : Difference(diff) { }

    /**
     *  @brief  Compute the distance between the point of @c origin and the @c
     *  key.
     *  @return The resulting square distance.
     */
    distance_type
    distance_to_key(dimension_type rank,
		    const Tp& origin, const Tp& key) const
    {
      return math::manhattan_distance_to_key<Key, Difference, Distance>
	(rank, origin, key, *static_cast<const Difference*>(this));
    }

    /**
     *  @brief  The distance between the point of @c origin and the closest
     *  point to the plane orthogonal to the axis of dimension @c dim and
     *  crossing @c key.
     *  @return The resulting square distance.
     */
    distance_type
    distance_to_plane(dimension_type, dimension_type dim,
		      const Tp& origin, const Tp& key) const
    {
      return math::manhattan_distance_to_plane<Key, Difference, Distance>
	(dim, origin, key, *static_cast<const Difference*>(this));
    }
  };

  /**
   *  Facilitates creation of a type \ref manhattan from an existing
   *  container.
   */
  template<typename Ct, typename Difference>
  manhattan<typename container_traits<Ct>::key_type, Difference>
  make_manhattan(const Ct& container, const Difference& diff)
  {
    return manhattan<typename container_traits<Ct>::key_type,
                     Difference>(diff);
  }

  /**
   *  Facilitates creation of a type \ref manhattan from an existing
   *  container and if you want to use a type for distance that is different
   *  from the type given in the \c Difference functor.
   *
   *  Use of function is a little particular. It needs a value of type
   *  \c Distance in order to guess what type is Distance, but it does not use
   *  the value. By convention just put 1, 1.0, 1.f, 1u, 1l, 1ll or any other
   *  expression of "one" as an input to this function.
   */
  template<typename Ct, typename Difference, typename Distance>
  manhattan<typename container_traits<Ct>::key_type, Difference, Distance>
  make_manhattan(const Ct& container, const Difference& diff, Distance)
  {
    return manhattan<typename container_traits<Ct>::key_type,
                     Difference, Distance>(diff);
  }

  /**
   *  Facilitates creation of a type \ref manhattan from an existing
   *  container.
   *
   *  Use of function is a little particular. It needs a value of type
   *  \c Distance in order to guess what type is Distance, but it does not use
   *  the value. By convention just put 1, 1.0, 1.f, 1u, 1l, 1ll or any other
   *  expression of "one" as an input to this function.
   */
  template<typename Ct, typename Distance>
  manhattan<typename container_traits<Ct>::key_type,
                typename auto_difference<Ct, Distance>::type, Distance>
  make_manhattan_auto(const Ct& container, Distance)
  {
    return manhattan<typename container_traits<Ct>::key_type,
                     typename auto_difference<Ct, Distance>::type, Distance>
		     (auto_difference<Ct, Distance>::make(diff));
  }

} // namespace spatial

#endif // SPATIAL_GEOMETRY_HPP
