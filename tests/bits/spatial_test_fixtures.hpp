// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_test_fixtures.hpp
 *  @brief  Contains the list of all fixtures required for the test in the
 *  library.
 *
 *  This file defines 2 type of components: fixtures and manipulators. Fixtures
 *  initialize and keep track of containers and copy of elements. Manipulators
 *  are used to create the values of all the elements inside the container and
 *  copy type.
 */

#ifndef SPATIAL_TEST_FIXTURES_HPP
#define SPATIAL_TEST_FIXTURES_HPP

#include "spatial_test_types.hpp"

using namespace ::spatial;
using namespace ::spatial::details;

/**
 *  Creates a random double between 0 and 1.
 */
inline
double
drand()
{
  double out = static_cast<double>(rand())/static_cast<double>(RAND_MAX);
  out = (static_cast<double>(rand()) + out)/static_cast<double>(RAND_MAX);
  out = (static_cast<double>(rand()) + out)/static_cast<double>(RAND_MAX);
  out = (static_cast<double>(rand()) + out)/static_cast<double>(RAND_MAX);
  return out;
}

/**
 *  Creates a random float between 0 and 1.
 */
inline
float
frand()
{
  float out = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
  out = (static_cast<float>(rand()) + out)/static_cast<float>(RAND_MAX);
  return out;
}

/**
 *  Defines a manipulator that does nothing.
 */
struct pass
{
  template<typename Tp>
  Tp& operator()(Tp& p, int, int) const { return p; }
};

/**
 *  Defines a manipulator that initializes all the elements of the objects to
 *  the value of \c n.
 */
struct same
{
  int2& operator()(int2& p, int, int n) const
  {
    p[0] = p[1] = n;
    return p;
  }
  int_pair& operator()(int_pair& p, int, int n) const
  {
    p.first = p.second = n;
    return p;
  }
  double5& operator()(double5& d, int, int n) const
  {
    d[0] = d[1] = d[2] = d[3] = d[4] = static_cast<double>(n);
    return d;
  }
  float5& operator()(float5& d, int, int n) const
  {
    d[0] = d[1] = d[2] = d[3] = d[4] = static_cast<float>(n);
    return d;
  }
};

/**
 *  Defines a manipulator that intilizes all the elements of the objects to the
 *  value of \c i.
 */
struct increase
{
  int2& operator()(int2& p, int i, int) const
  {
    p[0] = p[1] = i;
    return p;
  }
  int_pair& operator()(int_pair& p, int i, int) const
  {
    p.first = p.second = i;
    return p;
  }
  double5& operator()(double5& d, int i, int) const
  {
    d[0] = d[1] = d[2] = d[3] = d[4] = static_cast<double>(i);
    return d;
  }
  float5& operator()(float5& d, int i, int) const
  {
    d[0] = d[1] = d[2] = d[3] = d[4] = static_cast<float>(i);
    return d;
  }
};

/**
 *  Defines a manipulator that intilizes all the elements of the objects to the
 *  value of <tc>n - i</tc>.
 */
struct decrease
{
  int2& operator()(int2& p, int i, int n) const
  {
    p[0] = p[1] = n - i;
    return p;
  }
  int_pair& operator()(int_pair& p, int i, int n) const
  {
    p.first = p.second = n - i;
    return p;
  }
  double5& operator()(double5& d, int i, int n) const
  {
    d[0] = d[1] = d[2] = d[3] = d[4] = static_cast<double>(n - i);
    return d;
  }
  float5& operator()(float5& d, int i, int n) const
  {
    d[0] = d[1] = d[2] = d[3] = d[4] = static_cast<float>(n - i);
    return d;
  }
};

/**
 *  Define a manipulator that initializes all the elements of the objects to
 *  random values.
 */
struct random
{
  int lower;
  int higher;
  random(int l = 0, int h = 1) : lower(l), higher(h) { }
  int2& operator()(int2& p, int, int) const
  {
    p[0] = rand() % (higher - lower) + lower;
    p[1] = rand() % (higher - lower) + lower;
    return p;
  }
  int_pair& operator()(int_pair& p, int, int) const
  {
    p.first = rand() % (higher - lower) + lower;
    p.second = rand() % (higher - lower) + lower;
    return p;
  }
  double5& operator()(double5& d, int, int) const
  {
    d[0] = drand() * static_cast<double>(higher) + static_cast<double>(lower);
    d[1] = drand() * static_cast<double>(higher) + static_cast<double>(lower);
    d[2] = drand() * static_cast<double>(higher) + static_cast<double>(lower);
    d[3] = drand() * static_cast<double>(higher) + static_cast<double>(lower);
    d[4] = drand() * static_cast<double>(higher) + static_cast<double>(lower);
    return d;
  }
  float5& operator()(float5& d, int, int) const
  {
    d[0] = frand() * static_cast<float>(higher) + static_cast<float>(lower);
    d[1] = frand() * static_cast<float>(higher) + static_cast<float>(lower);
    d[2] = frand() * static_cast<float>(higher) + static_cast<float>(lower);
    d[3] = frand() * static_cast<float>(higher) + static_cast<float>(lower);
    d[4] = frand() * static_cast<float>(higher) + static_cast<float>(lower);
    return d;
  }
};

/**
 *  Creates a special fixture for a spatial container that makes it possible to
 *  define the number of elements to insert in the container and to specify a
 *  manipulator to build the element in a certain fashion. This fixture also
 *  provides a record of all the element inserted in the container at
 *  construction via a record type.
 */
template<typename Tp, typename Container>
struct basic_fixture
{
  typedef Container container_type;
  container_type container;
  typedef std::vector<Tp> record_type;
  std::vector<Tp> record;
  basic_fixture() : container(), record() { }
  template<typename Manip>
  basic_fixture(int n, const Manip& manip = pass()) : container(), record()
  {
    for (int i = 0; i < n; ++i)
    {
      Tp tp;
      typename container_type::const_iterator it
        = container.insert(manip(tp, i, n));
      BOOST_CHECK(*it == tp);
      record.push_back(tp);
    }
  }
};

/**
 *  Creates a special fixture for a spatial container that makes it possible to
 *  define the number of elements to insert in the container and to specify a
 *  manipulator to build the element in a certain fashion. This fixture is
 *  dedicated to spatial container whose dimensions are set at runtime.
 */
template<typename Tp, typename Container>
struct runtime_fixture
{
  typedef Container container_type;
  container_type container;
  typedef std::vector<Tp> record_type;
  std::vector<Tp> record;
  runtime_fixture() : container(dimension_traits<Tp>::value), record() { }
  template<typename Manip>
  runtime_fixture(int n, const Manip& manip = pass())
    : container(dimension_traits<Tp>::value), record()
  {
    for (int i = 0; i < n; ++i)
    {
      Tp tp;
      typename container_type::const_iterator
        it = container.insert(manip(tp, i, n));
      BOOST_CHECK(*it == tp);
      record.push_back(tp);
    }
  }
};

/**
 *  Defines a pointset container with optionally a specific comparator.
 */
template<typename Tp, typename Compare = bracket_less<Tp> >
struct pointset_fix
  : basic_fixture<Tp, pointset<dimension_traits<Tp>::value, Tp, Compare> >
{
  pointset_fix() { }
  explicit pointset_fix(int n)
    : basic_fixture<Tp, pointset<dimension_traits<Tp>::value, Tp, Compare> >
  (n, pass()) { }
  template<typename Manip> pointset_fix(int n, const Manip& manip)
    : basic_fixture<Tp, pointset<dimension_traits<Tp>::value, Tp, Compare> >
    (n, manip) { }
};

/**
 *  Defines a pointset fixture that is tightly balanced.
 */
template<typename Tp, typename Compare = bracket_less<Tp> >
struct tight_pointset_fix
  : basic_fixture<Tp, pointset<dimension_traits<Tp>::value, Tp, Compare,
  tight_balancing> >
{
  tight_pointset_fix() { }
  explicit tight_pointset_fix(int n)
    : basic_fixture<Tp, pointset<dimension_traits<Tp>::value, Tp, Compare,
                                 tight_balancing> >(n, pass()) { }
  template<typename Manip>
  tight_pointset_fix(int n, const Manip& manip)
    : basic_fixture<Tp, pointset<dimension_traits<Tp>::value, Tp, Compare,
                                 tight_balancing> >(n, manip) { }
};

/**
 *  Defines a fixture for a runtime_pointset.
 */
template<typename Tp, typename Compare = bracket_less<Tp> >
struct runtime_pointset_fix
  : runtime_fixture<Tp, runtime_pointset<Tp, Compare> >
{
  runtime_pointset_fix() { }
  explicit runtime_pointset_fix(int n)
    : runtime_fixture<Tp, runtime_pointset<Tp, Compare> >(n, pass()) { }
  template<typename Manip>
  runtime_pointset_fix(int n, const Manip& manip)
    : runtime_fixture<Tp, runtime_pointset<Tp, Compare> >(n, manip) { }
};

/**
 *  Defines a fixture for a frozen_poinset.
 */
template<typename Tp, typename Compare = bracket_less<Tp> >
struct frozen_pointset_fix
  : basic_fixture<Tp, frozen_pointset<dimension_traits<Tp>::value, Tp,
  Compare> >
{
  frozen_pointset_fix() { }
  explicit frozen_pointset_fix(int n)
    : basic_fixture<Tp, frozen_pointset<dimension_traits<Tp>::value, Tp,
                                        Compare> >(n, pass()) { }
  template<typename Manip>
  frozen_pointset_fix(int n, const Manip& manip)
    : basic_fixture<Tp, frozen_pointset<dimension_traits<Tp>::value, Tp,
    Compare> >(n, manip) { }
};

/**
 *  Defines a fixture for a \c runtime_frozen_pointset.
 */
template<typename Tp, typename Compare = bracket_less<Tp> >
struct runtime_frozen_pointset_fix
  : runtime_fixture<Tp, runtime_frozen_pointset<Tp, Compare> >
{
  runtime_frozen_pointset_fix() { }
  explicit runtime_frozen_pointset_fix(int n)
    : runtime_fixture<Tp, runtime_frozen_pointset<Tp, Compare> >(n, pass()) { }
  template<typename Manip>
  runtime_frozen_pointset_fix(int n, const Manip& manip)
    : runtime_fixture<Tp, runtime_frozen_pointset<Tp, Compare> >(n, manip) { }
};

#endif // SPATIAL_TEST_FIXTURES_HPP
