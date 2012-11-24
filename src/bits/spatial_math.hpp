// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_math.hpp
 *  @brief  Gather in one file all the mathematical operations, mainly for the
 *  geometry types.
 *
 *  Most of the mathematical operations written in this file are not very well
 *  optimized, and for a given compiler or architecture it would be easy to
 *  write more efficient algorithms. Therefore, if you are really looking to
 *  increase the speed of your computation, you might want to write your own
 *  geometry.
 *
 *  \see neighbor
 */

#ifndef SPATIAL_MATH_HPP
#define SPATIAL_MATH_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#include <stdexcept>
#include <limits>
#include <cmath>

#include "spatial_check_concept.hpp"

namespace spatial
{
  /**
   *  Thrown to report that an negative distance has been passed as a parameter
   *  while distances are expected to be positive.
   *
   *  \see check_addition(), check_multiplication()
   */
  struct negative_distance : std::logic_error
  {
    explicit negative_distance(const std::string& arg)
      : std::logic_error(arg) { }
  };

  /**
   *  Thrown to report that an arithmetic error has occured during a
   *  calculation. It could be an overflow, or another kind of error.
   *
   *  \see check_addition(), check_multiplication()
   */
  struct arithmetic_error : std::logic_error
  {
    explicit arithmetic_error(const std::string& arg)
      : std::logic_error(arg) { }
  };

  namespace except
  {
    /**
     *  Check that the distance given in \x has a positive value.
     *  \throws negative_distance
     */
    template<typename Tp>
    inline typename enable_if<std::tr1::is_arithmetic<Tp> >::type
    check_positive_distance(const Tp& x)
    {
      if (x < Tp()) // Tp() == 0 by convention
        throw negative_distance("distance is negative");
    }

    /**
     *  This arithmetic check is only used when the macro
     *  SPATIAL_SAFER_ARITHMETICS is defined. Check that the absolute of an
     *  element has not led to an error such as an overflow, by forcing the
     *  error itself.
     *
     *  This check is not the best check for arithmetic errors. There are ways
     *  to make it faster, but it is intended to be portable and provide users
     *  with the possibility to quickly check the architmectics during
     *  computation with little efforts from their part.
     */
    template <typename Tp>
    inline typename enable_if<std::tr1::is_arithmetic<Tp>, Tp>::type
    check_abs(const Tp& x)
    {
      using namespace std;
      if (x >= Tp()) return x; // Tp() == 0
      Tp a = abs(x);
      if (x != -a)
        throw arithmetic_error
          ("Absolute of an element resulted in an arithmetic error");
      return a;
    }

    /**
     *  This arithmetic check is only used when the macro
     *  SPATIAL_SAFER_ARITHMETICS is defined. Check that the addtion of 2
     *  elements of positive value has not led to an error such as an overflow,
     *  by forcing the error itself.
     *
     *  This check is not the best check for arithmetic errors. There are ways
     *  to make it faster, but it is intended to be portable and provide users
     *  with the possibility to quickly check the architmectics during
     *  computation with little efforts from their part.
     *
     *  In particular, if \c Tp is not a base type, the author of the type must
     *  define the numeric limits \c numeric_limits<Tp>::max() for that type.
     */
    template <typename Tp>
    inline typename enable_if<std::tr1::is_arithmetic<Tp>, Tp>::type
    check_positive_add(const Tp& x, const Tp& y)
    {
      using namespace std;
      // The additional bracket is to avoid conflict with MSVC min/max macros
      if (((numeric_limits<Tp>::max)() - x) < y)
        throw arithmetic_error
          ("Addition of two elements resulted in an arithmetic error");
      return x + y;
    }

    /**
     *  This arithmetic check is only used when the macro
     *  SPATIAL_SAFER_ARITHMETICS is defined. Check that the computation of the
     *  square of an element has not overflown.
     *
     *  This check is not the best check for arithmetic errors. There are ways
     *  to make it faster, but it is intended to be portable and provide users
     *  with the possibility to quickly check the architmectics during
     *  computation with little efforts from their part.
     *
     *  In particular, if \c Tp is not a base type, the author of the type must
     *  define the numeric limits \c numeric_limits<Tp>::max() for that type.
     */
    template <typename Tp>
    inline typename enable_if<std::tr1::is_arithmetic<Tp>, Tp>::type
    check_square(const Tp& x)
    {
      using namespace std;
      Tp zero = Tp(); // get 0
      if (x != zero)
        {
          if (x > zero)
            {
              if (((numeric_limits<Tp>::max)() / x) > x)
                throw arithmetic_error
                  ("Square value of element resulted in an arithmetic error");
            }
          else
            {
              if (((numeric_limits<Tp>::max)() / x) < x)
                throw arithmetic_error
                  ("Square value of element resulted in an arithmetic error");
            }
          return x * x;
        }
      return zero;
    }

    /**
     *  This arithmetic check is only used when the macro
     *  SPATIAL_SAFER_ARITHMETICS is defined. Check that the multiplication of 2
     *  positive elements has not resulted into an arithmetic error such as
     *  overflown.
     *
     *  This check will only work for 2 positive element x and y. This check is
     *  not the best check for arithmetic errors. There are ways to make it
     *  better, but it's hard to make it more portable.
     *
     *  In particular, if \c Tp is not a base type, the author of the type must
     *  define the numeric limits \c ::std::numeric_limits<Tp>::max() for that
     *  type.
     */
    template <typename Tp>
    inline typename enable_if<std::tr1::is_arithmetic<Tp>, Tp>::type
    check_positive_mul(const Tp& x, const Tp& y)
    {
      using namespace std;
      Tp zero = Tp(); // get 0
      if (x != zero)
        {
          if (((numeric_limits<Tp>::max)() / x) > y)
            throw arithmetic_error
              ("Square value of element resulted in an arithmetic error");
          return x * y;
        }
      return zero;
    }
  } // spatial except

  namespace math
  {
    /**
     *  Uses the hypot() algorithm in order to compute the distance: minimize
     *  possibilities of loss of precision due to overflow and underflow.
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
    template <typename Rank, typename Key, typename Difference, typename Unit>
    inline typename enable_if<std::tr1::is_floating_point<Unit>, Unit>::type
    euclid_distance_to_key
    (const Rank& rank, Key origin, Key key, Difference diff)
    {
      using namespace std;
      // Find a non zero maximum or return 0
      Unit max = abs(diff(0, origin, key));
      dimension_type max_dim = 0;
      for (dimension_type i = 1; i < rank(); ++i)
        {
          Unit d = abs(diff(i, origin, key));
          if (d > max) { max = d; max_dim = i; }
        }
      const Unit zero = Unit();
      if (max == zero) return zero; // they're all zero!
      // Compute the distance
      Unit sum = zero;
      for (dimension_type i = 0; i < rank(); ++i)
        {
          if (i == max_dim) continue;
          Unit div = diff(i, origin, key) / max;
          sum += div * div;
        }
      const Unit one = ((Unit) 1.0);
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
    template <typename Key, typename Difference, typename Unit>
    inline typename enable_if<std::tr1::is_floating_point<Unit>, Unit>::type
    euclid_distance_to_plane
    (dimension_type dim, Key origin, Key key, Difference diff)
    {
      using namespace std;
      return abs(diff(dim, origin, key));
    }

    /**
     *  @brief  Compute the distance between the @p origin and the closest point
     *  to the plane orthogonal to the axis of dimension @c dim and passing by
     *  @c key.
     */
    template <typename Key, typename Difference, typename Unit>
    inline typename enable_if<std::tr1::is_arithmetic<Unit>, Unit>::type
    square_euclid_distance_to_plane
    (dimension_type dim, Key origin, Key key, Difference diff)
    {
      Unit d = diff(dim, origin, key);
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
    template <typename Rank, typename Key, typename Difference, typename Unit>
    inline typename enable_if<std::tr1::is_arithmetic<Unit>, Unit>::type
    square_euclid_distance_to_key
    (const Rank& rank, Key origin, Key key, Difference diff)
    {
      Unit sum = square_euclid_distance_to_plane<Key, Difference, Unit>
        (0, origin, key, diff);
      for (dimension_type i = 1; i < rank(); ++i)
        {
#ifdef SPATIAL_SAFER_ARITHMETICS
          sum = except::check_positive_add
            (sqeuclid_distance_to_plane<Key, Difference, Unit>
             (i, origin, key, diff), sum);
#else
          sum += square_euclid_distance_to_plane
            <Key, Difference, Unit>(i, origin, key, diff);
#endif
        }
      return sum;
    }

    /**
     *  @brief  Compute the distance between the @p origin and the closest point
     *  to the plane orthogonal to the axis of dimension @c dim and passing by
     *  @c key.
     */
    template <typename Key, typename Difference, typename Unit>
    inline typename enable_if<std::tr1::is_arithmetic<Unit>, Unit>::type
    manhattan_distance_to_plane
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
     *  @brief  Compute the manhattan distance between @p origin and @p key.
     */
    template <typename Rank, typename Key, typename Difference, typename Unit>
    inline typename enable_if<std::tr1::is_arithmetic<Unit>, Unit>::type
    manhattan_distance_to_key
    (const Rank& rank, Key origin, Key key, Difference diff)
    {
      Unit sum = manhattan_distance_to_plane<Key, Difference, Unit>
        (0, origin, key, diff);
      for (dimension_type i = 1; i < rank(); ++i)
        {
#ifdef SPATIAL_SAFER_ARITHMETICS
          sum = ::spatial::except::check_positive_add
            (manhattan_distance_to_plane<Key, Difference, Unit>
             (i, origin, key, diff), sum);
#else
          sum += manhattan_distance_to_plane
            <Key, Difference, Unit>(i, origin, key, diff);
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
}

#endif // SPATIAL_MATH_HPP
