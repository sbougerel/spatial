// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_test_fixtures.hpp
 *  @brief  Desfines the types that will be used in the unit tests.
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
  { enum { value = dim }; }

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

// Definitions of int2 below...

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

// int2 declaration for usual value
int2 zeros(0, 0);
int2 ones(1, 1);
int2 twos(2, 2);
int2 threes(3, 3);
int2 fours(4, 4);
int2 fives(5, 5);

// Definitions of int_pair below...

/**
 *  A type that also contains 2 integers stored in a pair, accessed through
 *  first, second.
 */
typedef std::pair<int, int> int_pair;
define_dimension(int_pair, 2);

/**
 *  The comparator for int_pair.
 */
struct int_pair_less
{
  bool
  operator()(dimension_type dim, const int_pair& a, const int_pair& b) const
  {
    switch(dim)
      {
      case 0: return (a.first < b.first);
      case 1: return (a.second < b.second);
      default: throw std::out_of_range("arugment 'dim' is greater than 1");
      }
  }
};
define_compare(int_pair, int_pair_less);

// Definition of triple below...

/**
 *  A type that contains 3 elements of type int accessed exclusively via their
 *  variable names: x, y, z.
 */
struct triple
{
  int x; int y; int z;
  triple() { }
  triple(int a, int b, int c) : x(a), y(b), z(c) { }
};
define_dimension(triple, 3);

//! \return true if 2 triples contain the same values
inline bool operator== (const triple& a, const triple& b)
{
  return ( a.x == b.x && a.y == b.y && a.z == b.z );
}

//! \return false if 2 triples contain the same values
inline bool operator!= (const triple& a, const triple& b)
{
  return !(a == b);
}

//! A comparator for the type triple.
struct triple_less
{
  bool
  operator()(dimension_type dim, const triple& a, const triple& b) const
  {
    switch(dim)
      {
      case 0: return (a.x < b.x);
      case 1: return (a.y < b.y);
      case 2: return (a.z < b.z);
      default: throw std::out_of_range("argument 'dim' is greater than 2");
      }
  }
};
define_compare(triple, triple_less);

//! An accessor for the type triple
struct triple_access
{
  int&
  operator()(dimension_type dim, triple& x) const
  {
    switch(dim)
      {
      case 0: return x.x;
      case 1: return x.y;
      case 2: return x.z;
      default: throw std::out_of_range("argument 'dim' is greater than 2");
      }
  }

  int
  operator()(dimension_type dim, const triple& x) const
  {
    switch(dim)
      {
      case 0: return x.x;
      case 1: return x.y;
      case 2: return x.z;
      default: throw std::out_of_range("argument 'dim' is greater than 2");
      }
  }
};

// Definition of double5 below...

//! A type that contains an array of 5 doubles.
typedef std::tr1::array<double, 5> double5;
define_dimension(double5, 5);
define_compare(double5, bracket_less<int2>);

// Definition of double5 below...

//! A type that contains an array of 5 floats.
typedef std::tr1::array<float, 5> float5;
define_dimension(float5, 5);
define_compare(float5, bracket_less<int2>);

// Definition of accessors and comparators below...

//! An accessor that retreives the elements of a type via the at() accessor.
struct at_accessor
{
  int operator()(dimension_type dim, const int2& arg) const
  { return arg.at(dim); }
  int& operator()(dimension_type dim, int2& arg) const
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
