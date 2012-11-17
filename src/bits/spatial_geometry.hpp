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
    // Not sure how else to achieve this...
    template <typename Tp>
    struct arithmetic_constant
    {
      // Know a more generic/portable/type-agnostic way to get values for zero
      // or one? Then, please send me an email...
      static const Tp any_ = Tp();
      static const Tp zero_ = any_ - any_;
      static const Tp& zero() { return zero_; }
      static const Tp& one() { return ++zero_; }
    };
    // specializations...
#define SPATIAL_ARITHMETIC_CONSTANT(Type, Zero, One) \
    template<> struct arithmetic_constant<Type>      \
    {                                                \
      static Type zero() { return Zero; }            \
      static Type one() { return One; }              \
    }
    SPATIAL_ARITHMETIC_CONSTANT(char, 0, 1);
    SPATIAL_ARITHMETIC_CONSTANT(signed char, 0, 1);
    SPATIAL_ARITHMETIC_CONSTANT(unsigned char, 0, 1);
    SPATIAL_ARITHMETIC_CONSTANT(short, 0, 1);
    SPATIAL_ARITHMETIC_CONSTANT(unsigned short, 0, 1);
    SPATIAL_ARITHMETIC_CONSTANT(int, 0, 1);
    SPATIAL_ARITHMETIC_CONSTANT(unsigned int, 0, 1);
    SPATIAL_ARITHMETIC_CONSTANT(long, 0l, 1l);
    SPATIAL_ARITHMETIC_CONSTANT(unsigned long, 0l, 1l);
    // This is C++0x stuffs...
    // SPATIAL_ARITHMETIC_CONSTANT(long long, 0ll, 1ll);
    // SPATIAL_ARITHMETIC_CONSTANT(unsigned long long, 0ll, 1ll);
    SPATIAL_ARITHMETIC_CONSTANT(float, 0.f, 1.f);
    SPATIAL_ARITHMETIC_CONSTANT(double, 0.0, 1.0);
    SPATIAL_ARITHMETIC_CONSTANT(long double, 0.0, 1.0);
#undef SPATIAL_ARITHMETIC_CONSTANT

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
     *  overflow than the first form.
     */
    template <typename Key, typename Difference, typename Distance>
    inline Distance
    euclid_distance_to_key
    (dimension_type rank, Key origin, Key key, Difference diff)
    {
      using namespace std;
      const Distance zero = arithmetic_constant<Distance>::zero();
      const Distance one = arithmetic_constant<Distance>::one();
      // Find a non zero maximum or return 0
      Distance max = zero;
      dimension_type max_dim; // uninitialized on purpose
      for (dimension_type i = 0; i < rank; ++i)
        {
#ifdef SPATIAL_SAFER_ARITHMETICS
          Distance d = except::check_abs(diff(i, origin, key));
#else
          Distance d = abs(diff(i, origin, key));
#endif
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
      return except::check_positive_mul(max, sqrt(one + sum));
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
#ifdef SPATIAL_SAFER_ARITHMETICS
      return except::check_abs(diff(dim, origin, key));
#else
      using namespace std;
      return abs(diff(dim, origin, key));
#endif
    }

    /**
     *  @brief  Compute the distance between the @p origin and the closest point
     *  to the plane orthogonal to the axis of dimension @c dim and passing by
     *  @c key.
     */
    template <typename Key, typename Difference, typename Distance>
    inline Distance
    square_euclid_distance_to_plane
    (dimension_type dim, Key origin, Key key, Difference diff)
    {
      Distance d = diff(dim, origin, key);
#ifdef SPATIAL_SAFER_ARITHMETICS
      return except::check_square(d);
#else
      return d * d;
#endif
    }

    /**
     *  @brief  Compute the square value of the distance between @p origin and
     *  @p key.
     */
    template <typename Key, typename Difference, typename Distance>
    inline Distance
    square_euclid_distance_to_key
    (dimension_type rank, Key origin, Key key, Difference diff)
    {
      Distance sum = static_cast<Distance>(0);
      for (dimension_type i=0; i<rank; ++i)
        {
#ifdef SPATIAL_SAFER_ARITHMETICS
          sum = except::check_positive_add
            (sqeuclid_distance_to_plane<Key, Difference, Distance>
             (i, origin, key, diff), sum);
#else
          sum += square_euclid_distance_to_plane
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
      using namespace std;
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
     *  The function difference_cast resolve the appropriate difference functor
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
    difference_cast(const Compare& cmp)
    {
      auto_difference<Compare, Unit> factory;
      return factory(cmp);
    }

  } // namespace details

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
         typename details::auto_difference<Ct, double>::type>
  make_euclid_auto(const Ct& container)
  {
    return euclid<typename container_traits<Ct>::key_type,
                  typename details::auto_difference<Ct, double>::type>
      (details::difference_cast<Ct, double>(container.key_comp()));
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
          typename details::auto_difference<Ct, float>::type>
  make_euclidf_auto(const Ct& container)
  {
    return euclidf<typename container_traits<Ct>::key_type,
                   typename details::auto_difference<Ct, float>::type>
      (details::difference_cast<typename container_traits<Ct>::key_compare,
                                float>(container.key_comp()));
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
  struct square_euclid
    : private Difference // empty-member optimization
  {
    typedef Distance distance_type;

    //! The constructor allows you to specify a custom difference type.
    square_euclid(const Difference& diff = Difference()) : Difference(diff) { }

    /**
     *  @brief  Compute the distance between the point of @c origin and the @c
     *  key.
     *  @return The resulting square distance.
     */
    distance_type
    distance_to_key(dimension_type rank,
                    const Tp& origin, const Tp& key) const
    {
      return math::square_euclid_distance_to_key<Tp, Difference, Distance>
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
      return math::square_euclid_distance_to_plane<Tp, Difference, Distance>
        (dim, origin, key, *static_cast<const Difference*>(this));
    }
  };

  /**
   *  Facilitates creation of a type \ref sqeuclid from an existing
   *  container.
   */
  template<typename Ct, typename Difference>
  square_euclid<typename container_traits<Ct>::key_type, Difference>
  make_square_euclid(const Ct& container, const Difference& diff)
  {
    return square_euclid<typename container_traits<Ct>::key_type,
                         Difference>(diff);
  }

  /**
   *  Facilitates creation of a type \ref sqeuclid from an existing container
   *  and a provided distance type.
   *
   *  The value of the Distance itself does not matter, therefore it is
   *  recommended to call the function with the default constructor for the
   *  distance type: such as 'int()' or 'double()' or 'MyOwnType()'.
   */
  template<typename Ct, typename Distance>
  square_euclid<typename container_traits<Ct>::key_type,
                typename details::auto_difference<Ct, Distance>::type>
  make_square_euclid(const Ct& container, const Distance&)
  {
    return square_euclid<typename container_traits<Ct>::key_type,
                         typename details::auto_difference<Ct, Distance>::type>
      (details::difference_cast<typename container_traits<Ct>::key_compare,
       Distance>(container.key_comp()));
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
      return math::manhattan_distance_to_key<Tp, Difference, Distance>
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
      return math::manhattan_distance_to_plane<Tp, Difference, Distance>
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
   *  container.
   *
   *  The value of the Distance itself does not matter, therefore it is
   *  recommended to call the function with the default constructor for the
   *  distance type: such as 'int()' or 'double()' or 'MyOwnType()'.
   */
  template<typename Ct, typename Distance>
  manhattan<typename container_traits<Ct>::key_type,
            typename details::auto_difference<Ct, Distance>::type>
  make_manhattan_auto(const Ct& container, const Distance&)
  {
    return manhattan<typename container_traits<Ct>::key_type,
                     typename details::auto_difference<Ct, Distance>::type>
      (details::difference_cast<typename container_traits<Ct>::key_compare,
                                Distance>(container.key_comp()));
  }

} // namespace spatial

#endif // SPATIAL_GEOMETRY_HPP
