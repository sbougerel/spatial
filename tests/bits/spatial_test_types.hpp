// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_test_fixtures.hpp
 *
 *  @brief  Defines the types that will be used in the unit tests.
 */

#ifndef SPATIAL_TEST_TYPES_HPP
#define SPATIAL_TEST_TYPES_HPP

/**
 *  The dimension_traits resolve a given type into a constant dimension value;
 *  that will be used by the fixtures.
 */
template<typename Tp> struct dimension_traits { };

/**
 *  An helper macro to specialize dimension_traits for a particular type.
 */
#define define_dimension(Tp, dim)               \
  template<> struct dimension_traits<Tp>        \
  { enum { value = dim, half = dim >> 1 }; }

/**
 *  The compare_traits resolves a given type into a type used for comparison in
 *  the fixture.
 */
template <typename Tp> struct compare_traits { };

/**
 *  An helper macro to define the comparison function for the key types.
 */
#define define_compare(Tp, Cmp)                 \
  template<> struct compare_traits<Tp>          \
  { typedef Cmp type; }

/**
 *  The unit_traits resolves a given type into a type used for
 *  neighbor_iterators
 */
template <typename Tp> struct unit_traits { };

/**
 *  An helper macro to define the unit of key types.
 */
#define define_unit(Tp, Unit)                   \
  template<> struct unit_traits<Tp>             \
  { typedef Unit type; }

/**
 *  Reports an error if type Tp1 differs from type Tp2.
 */
//@{
template<typename Tp1, typename Tp2>
struct check_is_same
{
  check_is_same() { BOOST_CHECK_MESSAGE(false, "Types differ"); }
};
template<typename Tp>
struct check_is_same<Tp, Tp>
{
  check_is_same() { }
};
//@}

// Definitions of int2 below, a simple array of simple type

/**
 *  Defines int2, an array type that contains 2 integrers.
 *
 *  In int2, elements of the array can be accessed either via at(), [], or ().
 */
struct int2 : std::tr1::array<int, 2>
{
  int2 () { }
  int2 (int first, int second)
  {
    std::tr1::array<int, 2>::operator[](0) = first;
    std::tr1::array<int, 2>::operator[](1) = second;
  }
  std::tr1::array<int, 2>::reference
  operator()(size_type n)
  { return int2::operator[](n); }

  std::tr1::array<int, 2>::const_reference
  operator()(size_type n) const
  { return int2::operator[](n); }
};
define_dimension(int2, 2);
define_compare(int2, bracket_less<int2>);
define_unit(int2, int);

// int2 declaration for usual value
const int2 zeros(0, 0);
const int2 ones(1, 1);
const int2 twos(2, 2);
const int2 threes(3, 3);
const int2 fours(4, 4);
const int2 fives(5, 5);

// Definition of double6 below, a larger array of double type

//! A type that contains an array of 6 doubles.
typedef std::tr1::array<double, 6> double6;
define_dimension(double6, 6);
define_compare(double6, bracket_less<double6>);
define_unit(double6, double);

//! Quick initialization of double6
inline double6
make_double6(double a) { double6 x; x.assign(a); return x; }
inline double6
make_double6(double a, double b, double c, double d, double e, double f)
{
  double6 x;
  x[0] = a; x[1] = b; x[2] = c; x[3] = d; x[4] = e; x[5] = f;
  return x;
}

//! Used in ordered_iterator tests
struct double6_ordered_less
{
  bool operator() (const double6& a, const double6& b) const
  {
    double6::const_iterator i(a.begin()), j(b.begin());
    return (*i < *j
            || (*i == *j // 0
                && (*++i < *++j
                    || (*i == *j // 1
                        && (*++i < *++j
                            || (*i == *j // 2
                                && (*++i < *++j
                                    || (*i == *j // 3
                                        && (*++i < *++j
                                            || (*i == *j // 4
                                                && *++i < *++j)))))))))); // 5
  }
};

// Definition of quad below, a structure of simple type

/**
 *  A type that contains 4 elements of type int accessed exclusively via their
 *  variable names: x, y, z, w.
 */
struct quad
{
  int x; int y; int z; int w;
  quad() { }
  quad(int a, int b, int c, int d) : x(a), y(b), z(c), w(d) { }
};
define_dimension(quad, 4);
define_unit(quad, int);

//! \return true if 2 quads contain the same values
inline bool operator== (const quad& a, const quad& b)
{
  return ( a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w );
}

//! \return false if 2 quads contain the same values
inline bool operator!= (const quad& a, const quad& b)
{
  return !(a == b);
}

//! A comparator for the type quad.
struct quad_less
{
  bool
  operator()(dimension_type dim, const quad& a, const quad& b) const
  {
    switch(dim)
      {
      case 0: return (a.x < b.x);
      case 1: return (a.y < b.y);
      case 2: return (a.z < b.z);
      case 3: return (a.w < b.w);
      default: throw std::out_of_range("argument 'dim' is greater than 3");
      }
  }
  bool operator()
  (dimension_type a, const quad& x, dimension_type b, const quad& y) const
  {
    int e1; int e2;
    switch(a)
      {
      case 0: e1 = x.x; break;
      case 1: e1 = x.y; break;
      case 2: e1 = x.z; break;
      case 3: e1 = x.w; break;
      default: throw std::out_of_range("argument 'dim' is greater than 3");
      }
    switch(b)
      {
      case 0: e2 = y.x; break;
      case 1: e2 = y.y; break;
      case 2: e2 = y.z; break;
      case 3: e2 = y.w; break;
      default: throw std::out_of_range("argument 'dim' is greater than 3");
      }
    return (e1 < e2);
  }
};
define_compare(quad, quad_less);

//! Used in ordered_iterator tests
struct quad_ordered_less
{
  bool operator() (const quad& a, const quad& b) const
  {
    return (a.x < b.x
            || (a.x == b.x
                && (a.y < b.y
                    || (a.y == b.y
                        && (a.z < b.z
                            || (a.z == b.z
                                && a.w < b.w))))));
  }
};

//! An accessor for the type quad
struct quad_access
{
  int
  operator()(dimension_type dim, const quad& x) const
  {
    switch(dim)
      {
      case 0: return x.x;
      case 1: return x.y;
      case 2: return x.z;
      case 3: return x.w;
      default: throw std::out_of_range("argument 'dim' is greater than 3");
      }
  }
};

// Definition of accessors and comparators below...

//! An accessor that retreives the elements of a type via the at() accessor.
struct at_accessor
{
  int operator()(dimension_type dim, const int2& arg) const
  { return arg.at(dim); }
};

// Helps to test that details::match* family of functions are working properly
struct closed_test_range
{
  relative_order
  operator()(dimension_type dim, dimension_type rank, const int2& point)
    const
  {
    if (dim >= rank)
      throw std::out_of_range("argument 'dim' must be lower than 'rank'");
    return ((point[dim] < 0)
            ? below
            : ((point[dim] > 1)
               ? above
               : matching));
  }
};

#endif // SPATIAL_TEST_TYPES_HPP
