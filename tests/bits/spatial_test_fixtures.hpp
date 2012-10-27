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

template <typename Manip>
struct to_first
{
  Manip manip;

  explicit to_first(const Manip& m = Manip()) : manip(m) { }

  template<typename Tp1, typename Tp2>
  std::pair<Tp1, Tp2>& operator()
    (std::pair<Tp1, Tp2>& p, int i, int n) const
  {
    p.first = Manip()(p.first, i, n);
    return p;
  }
};

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
  quad& operator()(quad& p, int, int n) const
  {
    p.x = p.y = p.z = p.w = n;
    return p;
  }
  double6& operator()(double6& d, int, int n) const
  {
    d[0] = d[1] = d[2] = d[3] = d[4] = d[5] = static_cast<double>(n);
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
  quad& operator()(quad& p, int i, int) const
  {
    p.x = p.y = p.z = p.w = i;
    return p;
  }
  double6& operator()(double6& d, int i, int) const
  {
    d[0] = d[1] = d[2] = d[3] = d[4] = d[5] = static_cast<double>(i);
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
  quad& operator()(quad& p, int i, int n) const
  {
    p.x = p.y = p.z = p.w = n - i;
    return p;
  }
  double6& operator()(double6& d, int i, int n) const
  {
    d[0] = d[1] = d[2] = d[3] = d[4] = d[5] = static_cast<double>(n - i);
    return d;
  }
};

/**
 *  Define a manipulator that initializes all the elements of the objects to
 *  random values.
 */
struct randomize
{
  int lower;
  int higher;
  randomize(int l = 0, int h = 1) : lower(l), higher(h) { }
  int2& operator()(int2& p, int, int) const
  {
    p[0] = rand() % (higher - lower) + lower;
    p[1] = rand() % (higher - lower) + lower;
    return p;
  }
  quad& operator()(quad& p, int, int) const
  {
    p.x = rand() % (higher - lower) + lower;
    p.y = rand() % (higher - lower) + lower;
    p.z = rand() % (higher - lower) + lower;
    p.w = rand() % (higher - lower) + lower;
    return p;
  }
  double6& operator()(double6& d, int, int) const
  {
    d[0] = drand() * static_cast<double>(higher) + static_cast<double>(lower);
    d[1] = drand() * static_cast<double>(higher) + static_cast<double>(lower);
    d[2] = drand() * static_cast<double>(higher) + static_cast<double>(lower);
    d[3] = drand() * static_cast<double>(higher) + static_cast<double>(lower);
    d[4] = drand() * static_cast<double>(higher) + static_cast<double>(lower);
    d[5] = drand() * static_cast<double>(higher) + static_cast<double>(lower);
    return d;
  }
};

// Implement pair comparison needed for what follows:
template<typename Tp1, typename Tp2>
bool operator==(const std::pair<const Tp1, Tp2>& a, const std::pair<Tp1, Tp2>& b)
{
  return a.first == b.first;
}

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
template<typename Tp, int Dim, typename Container>
struct runtime_fixture
{
  typedef Container container_type;
  container_type container;
  typedef std::vector<Tp> record_type;
  std::vector<Tp> record;
  runtime_fixture() : container(Dim), record() { }
  template<typename Manip>
  runtime_fixture(int n, const Manip& manip = pass())
    : container(Dim), record()
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
template<typename Tp, typename Compare = typename compare_traits<Tp>::type >
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
 *  Defines a boxset container with optionally a specific comparator.
 */
template<typename Tp, typename Compare = typename compare_traits<Tp>::type >
struct boxset_fix
  : basic_fixture<Tp, boxset<dimension_traits<Tp>::half, Tp, Compare> >
{
  boxset_fix() { }
  explicit boxset_fix(int n)
    : basic_fixture<Tp, boxset<dimension_traits<Tp>::half, Tp, Compare> >
      (n, pass()) { }
  template<typename Manip> boxset_fix(int n, const Manip& manip)
    : basic_fixture<Tp, boxset<dimension_traits<Tp>::half, Tp, Compare> >
      (n, manip) { }
};

/**
 *  Defines a pointmap container with optionally a specific comparator.
 */
template<typename Tp, typename Mapped,
         typename Compare = typename compare_traits<Tp>::type >
struct pointmap_fix
  : basic_fixture<std::pair<Tp, Mapped>,
                  pointmap<dimension_traits<Tp>::value, Tp, Mapped, Compare> >
{
  pointmap_fix() { }
  explicit pointmap_fix(int n)
    : basic_fixture<std::pair<Tp, Mapped>,
                    pointmap<dimension_traits<Tp>::value, Tp, Mapped, Compare> >
      (n, to_first<pass>()) { }
  template<typename Manip>
  pointmap_fix(int n, const Manip& manip)
    : basic_fixture<std::pair<Tp, Mapped>,
                    pointmap<dimension_traits<Tp>::value, Tp, Mapped, Compare> >
      (n, to_first<Manip>(manip)) { }
};

/**
 *  Defines a boxmap container with optionally a specific comparator.
 */
template<typename Tp, typename Mapped,
         typename Compare = typename compare_traits<Tp>::type >
struct boxmap_fix
  : basic_fixture<std::pair<Tp, Mapped>,
                  boxmap<dimension_traits<Tp>::half, Tp, Mapped, Compare> >
{
  boxmap_fix() { }
  explicit boxmap_fix(int n)
    : basic_fixture<std::pair<Tp, Mapped>,
                    boxmap<dimension_traits<Tp>::half, Tp, Mapped, Compare> >
      (n, to_first<pass>()) { }
  template<typename Manip>
  boxmap_fix(int n, const Manip& manip)
    : basic_fixture<std::pair<Tp, Mapped>,
                    boxmap<dimension_traits<Tp>::half, Tp, Mapped, Compare> >
      (n, to_first<Manip>(manip)) { }
};

/**
 *  Defines a pointset fixture that is tightly balanced.
 */
template<typename Tp, typename Compare = typename compare_traits<Tp>::type >
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
 *  Defines a boxset fixture that is tightly balanced.
 */
template<typename Tp, typename Compare = typename compare_traits<Tp>::type >
struct tight_boxset_fix
  : basic_fixture<Tp, boxset<dimension_traits<Tp>::half, Tp, Compare,
                             tight_balancing> >
{
  tight_boxset_fix() { }
  explicit tight_boxset_fix(int n)
    : basic_fixture<Tp, boxset<dimension_traits<Tp>::half, Tp, Compare,
                               tight_balancing> >(n, pass()) { }
  template<typename Manip>
  tight_boxset_fix(int n, const Manip& manip)
    : basic_fixture<Tp, boxset<dimension_traits<Tp>::half, Tp, Compare,
                               tight_balancing> >(n, manip) { }
};

/**
 *  Defines a pointmap fixture that is tightly balanced.
 */
template<typename Tp, typename Mapped,
         typename Compare = typename compare_traits<Tp>::type >
struct tight_pointmap_fix
  : basic_fixture<std::pair<Tp, Mapped>,
                  pointmap<dimension_traits<Tp>::value, Tp, Mapped, Compare,
                           tight_balancing> >
{
  tight_pointmap_fix() { }
  explicit tight_pointmap_fix(int n)
    : basic_fixture<std::pair<Tp, Mapped>,
                    pointmap<dimension_traits<Tp>::value, Tp, Mapped, Compare,
                             tight_balancing> >(n, to_first<pass>()) { }
  template<typename Manip>
  tight_pointmap_fix(int n, const Manip& manip)
    : basic_fixture<std::pair<Tp, Mapped>,
                    pointmap<dimension_traits<Tp>::value, Tp, Mapped, Compare,
                             tight_balancing> >(n, to_first<Manip>(manip)) { }
};

/**
 *  Defines a boxmap fixture that is tightly balanced.
 */
template<typename Tp, typename Mapped,
         typename Compare = typename compare_traits<Tp>::type >
struct tight_boxmap_fix
  : basic_fixture<std::pair<Tp, Mapped>,
                  boxmap<dimension_traits<Tp>::half, Tp, Mapped, Compare,
                         tight_balancing> >
{
  tight_boxmap_fix() { }
  explicit tight_boxmap_fix(int n)
    : basic_fixture<std::pair<Tp, Mapped>,
                    boxmap<dimension_traits<Tp>::half, Tp, Mapped, Compare,
                           tight_balancing> >(n, to_first<pass>()) { }
  template<typename Manip>
  tight_boxmap_fix(int n, const Manip& manip)
    : basic_fixture<std::pair<Tp, Mapped>,
                    boxmap<dimension_traits<Tp>::half, Tp, Mapped, Compare,
                           tight_balancing> >(n, to_first<Manip>(manip)) { }
};

/**
 *  Defines a fixture for a runtime_pointset.
 */
template<typename Tp, typename Compare = typename compare_traits<Tp>::type >
struct runtime_pointset_fix
  : runtime_fixture<Tp, dimension_traits<Tp>::value,
                    runtime_pointset<Tp, Compare> >
{
  runtime_pointset_fix() { }
  explicit runtime_pointset_fix(int n)
    : runtime_fixture<Tp, dimension_traits<Tp>::value,
                      runtime_pointset<Tp, Compare> >(n, pass()) { }
  template<typename Manip>
  runtime_pointset_fix(int n, const Manip& manip)
    : runtime_fixture<Tp, dimension_traits<Tp>::value,
                      runtime_pointset<Tp, Compare> >(n, manip) { }
};

/**
 *  Defines a fixture for a runtime_boxset.
 */
template<typename Tp, typename Compare = typename compare_traits<Tp>::type >
struct runtime_boxset_fix
  : runtime_fixture<Tp, dimension_traits<Tp>::half,
                    runtime_boxset<Tp, Compare> >
{
  runtime_boxset_fix() { }
  explicit runtime_boxset_fix(int n)
    : runtime_fixture<Tp, dimension_traits<Tp>::half,
                      runtime_boxset<Tp, Compare> >(n, pass()) { }
  template<typename Manip>
  runtime_boxset_fix(int n, const Manip& manip)
    : runtime_fixture<Tp, dimension_traits<Tp>::half,
                      runtime_boxset<Tp, Compare> >(n, manip) { }
};

/**
 *  Defines a fixture for a runtime_pointmap.
 */
template<typename Tp, typename Mapped,
         typename Compare = typename compare_traits<Tp>::type >
struct runtime_pointmap_fix
  : runtime_fixture<std::pair<Tp, Mapped>, dimension_traits<Tp>::value,
                    runtime_pointmap<Tp, Mapped, Compare> >
{
  runtime_pointmap_fix() { }

  explicit runtime_pointmap_fix(int n)
    : runtime_fixture<std::pair<Tp, Mapped>, dimension_traits<Tp>::value,
                      runtime_pointmap<Tp, Mapped, Compare> >
      (n, to_first<pass>()) { }

  template<typename Manip>
  runtime_pointmap_fix(int n, const Manip& manip)
    : runtime_fixture<std::pair<Tp, Mapped>, dimension_traits<Tp>::value,
                      runtime_pointmap<Tp, Mapped, Compare> >
      (n, to_first<Manip>(manip)) { }
};

/**
 *  Defines a fixture for a runtime_boxmap.
 */
template<typename Tp, typename Mapped,
         typename Compare = typename compare_traits<Tp>::type >
struct runtime_boxmap_fix
  : runtime_fixture<std::pair<Tp, Mapped>, dimension_traits<Tp>::half,
                    runtime_boxmap<Tp, Mapped, Compare> >
{
  runtime_boxmap_fix() { }

  explicit runtime_boxmap_fix(int n)
    : runtime_fixture<std::pair<Tp, Mapped>, dimension_traits<Tp>::half,
                      runtime_boxmap<Tp, Mapped, Compare> >
      (n, to_first<pass>()) { }

  template<typename Manip>
  runtime_boxmap_fix(int n, const Manip& manip)
    : runtime_fixture<std::pair<Tp, Mapped>, dimension_traits<Tp>::half,
                      runtime_boxmap<Tp, Mapped, Compare> >
      (n, to_first<Manip>(manip)) { }
};

/**
 *  Defines a fixture for a idle_poinset.
 */
template<typename Tp, typename Compare = typename compare_traits<Tp>::type >
struct idle_pointset_fix
  : basic_fixture<Tp, idle_pointset<dimension_traits<Tp>::value, Tp,
                                      Compare> >
{
  idle_pointset_fix() { }
  explicit idle_pointset_fix(int n)
    : basic_fixture<Tp, idle_pointset<dimension_traits<Tp>::value, Tp,
                                        Compare> >(n, pass()) { }
  template<typename Manip>
  idle_pointset_fix(int n, const Manip& manip)
    : basic_fixture<Tp, idle_pointset<dimension_traits<Tp>::value, Tp,
                                        Compare> >(n, manip) { }
};

/**
 *  Defines a fixture for a idle_poinset.
 */
template<typename Tp, typename Compare = typename compare_traits<Tp>::type >
struct idle_boxset_fix
  : basic_fixture<Tp, idle_boxset<dimension_traits<Tp>::half, Tp,
                                    Compare> >
{
  idle_boxset_fix() { }
  explicit idle_boxset_fix(int n)
    : basic_fixture<Tp, idle_boxset<dimension_traits<Tp>::half, Tp,
                                      Compare> >(n, pass()) { }
  template<typename Manip>
  idle_boxset_fix(int n, const Manip& manip)
    : basic_fixture<Tp, idle_boxset<dimension_traits<Tp>::half, Tp,
                                      Compare> >(n, manip) { }
};

/**
 *  Defines a fixture for a idle_poinmap.
 */
template<typename Tp, typename Mapped,
         typename Compare = typename compare_traits<Tp>::type >
struct idle_pointmap_fix
  : basic_fixture<std::pair<Tp, Mapped>,
                  idle_pointmap<dimension_traits<Tp>::value,
                                  Tp, Mapped, Compare> >
{
  idle_pointmap_fix() { }
  explicit idle_pointmap_fix(int n)
    : basic_fixture<std::pair<Tp, Mapped>,
                    idle_pointmap<dimension_traits<Tp>::value, Tp,
                                    Mapped, Compare> >
      (n, to_first<pass>()) { }
  template<typename Manip>
  idle_pointmap_fix(int n, const Manip& manip)
    : basic_fixture<std::pair<Tp, Mapped>,
                    idle_pointmap<dimension_traits<Tp>::value, Tp,
                                    Mapped, Compare> >
      (n, to_first<Manip>(manip)) { }
};

/**
 *  Defines a fixture for a idle_poinmap.
 */
template<typename Tp, typename Mapped,
         typename Compare = typename compare_traits<Tp>::type >
struct idle_boxmap_fix
  : basic_fixture<std::pair<Tp, Mapped>,
                  idle_boxmap<dimension_traits<Tp>::half,
                                Tp, Mapped, Compare> >
{
  idle_boxmap_fix() { }
  explicit idle_boxmap_fix(int n)
    : basic_fixture<std::pair<Tp, Mapped>,
                    idle_boxmap<dimension_traits<Tp>::half, Tp,
                                  Mapped, Compare> >
      (n, to_first<pass>()) { }
  template<typename Manip>
  idle_boxmap_fix(int n, const Manip& manip)
    : basic_fixture<std::pair<Tp, Mapped>,
                    idle_boxmap<dimension_traits<Tp>::half, Tp,
                                  Mapped, Compare> >
      (n, to_first<Manip>(manip)) { }
};

/**
 *  Defines a fixture for a \c runtime_idle_pointset.
 */
template<typename Tp, typename Compare = typename compare_traits<Tp>::type >
struct runtime_idle_pointset_fix
  : runtime_fixture<Tp, dimension_traits<Tp>::value,
                    runtime_idle_pointset<Tp, Compare> >
{
  runtime_idle_pointset_fix() { }
  explicit runtime_idle_pointset_fix(int n)
    : runtime_fixture<Tp, dimension_traits<Tp>::value,
                      runtime_idle_pointset<Tp, Compare> >(n, pass()) { }
  template<typename Manip>
  runtime_idle_pointset_fix(int n, const Manip& manip)
    : runtime_fixture<Tp, dimension_traits<Tp>::value,
                      runtime_idle_pointset<Tp, Compare> >(n, manip) { }
};

/**
 *  Defines a fixture for a \c runtime_idle_boxset.
 */
template<typename Tp, typename Compare = typename compare_traits<Tp>::type >
struct runtime_idle_boxset_fix
  : runtime_fixture<Tp, dimension_traits<Tp>::half,
                    runtime_idle_boxset<Tp, Compare> >
{
  runtime_idle_boxset_fix() { }
  explicit runtime_idle_boxset_fix(int n)
    : runtime_fixture<Tp, dimension_traits<Tp>::half,
                      runtime_idle_boxset<Tp, Compare> >(n, pass()) { }
  template<typename Manip>
  runtime_idle_boxset_fix(int n, const Manip& manip)
    : runtime_fixture<Tp, dimension_traits<Tp>::half,
                      runtime_idle_boxset<Tp, Compare> >(n, manip) { }
};

/**
 *  Defines a fixture for a \c runtime_idle_pointmap.
 */
template<typename Tp, typename Mapped,
         typename Compare = typename compare_traits<Tp>::type >
struct runtime_idle_pointmap_fix
  : runtime_fixture<std::pair<Tp, Mapped>, dimension_traits<Tp>::value,
                    runtime_idle_pointmap<Tp, Mapped, Compare> >
{
  runtime_idle_pointmap_fix() { }
  explicit runtime_idle_pointmap_fix(int n)
    : runtime_fixture<std::pair<Tp, Mapped>, dimension_traits<Tp>::value,
                      runtime_idle_pointmap<Tp, Mapped, Compare> >
      (n, to_first<pass>()) { }
  template<typename Manip>
  runtime_idle_pointmap_fix(int n, const Manip& manip)
    : runtime_fixture<std::pair<Tp, Mapped>, dimension_traits<Tp>::value,
                      runtime_idle_pointmap<Tp, Mapped, Compare> >
      (n, to_first<Manip>(manip)) { }
};

/**
 *  Defines a fixture for a \c runtime_idle_boxmap.
 */
template<typename Tp, typename Mapped,
         typename Compare = typename compare_traits<Tp>::type >
struct runtime_idle_boxmap_fix
  : runtime_fixture<std::pair<Tp, Mapped>, dimension_traits<Tp>::half,
                    runtime_idle_boxmap<Tp, Mapped, Compare> >
{
  runtime_idle_boxmap_fix() { }
  explicit runtime_idle_boxmap_fix(int n)
    : runtime_fixture<std::pair<Tp, Mapped>, dimension_traits<Tp>::half,
                      runtime_idle_boxmap<Tp, Mapped, Compare> >
      (n, to_first<pass>()) { }
  template<typename Manip>
  runtime_idle_boxmap_fix(int n, const Manip& manip)
    : runtime_fixture<std::pair<Tp, Mapped>, dimension_traits<Tp>::half,
                      runtime_idle_boxmap<Tp, Mapped, Compare> >
      (n, to_first<Manip>(manip)) { }
};

// Set lists
typedef boost::mpl::list<pointset_fix<int2>,
                         tight_pointset_fix<int2>,
                         idle_pointset_fix<int2>,
                         runtime_pointset_fix<int2>,
                         runtime_idle_pointset_fix<int2>,
                         boxset_fix<int2>,
                         tight_boxset_fix<int2>,
                         idle_boxset_fix<int2>,
                         runtime_boxset_fix<int2>,
                         runtime_idle_boxset_fix<int2> >
int2_sets;

typedef boost::mpl::list<pointset_fix<quad>,
                         tight_pointset_fix<quad>,
                         idle_pointset_fix<quad>,
                         runtime_pointset_fix<quad>,
                         runtime_idle_pointset_fix<quad>,
                         boxset_fix<quad>,
                         tight_boxset_fix<quad>,
                         idle_boxset_fix<quad>,
                         runtime_boxset_fix<quad>,
                         runtime_idle_boxset_fix<quad> >
quad_sets;

typedef boost::mpl::list<pointset_fix<double6>,
                         tight_pointset_fix<double6>,
                         idle_pointset_fix<double6>,
                         runtime_pointset_fix<double6>,
                         runtime_idle_pointset_fix<double6>,
                         boxset_fix<double6>,
                         tight_boxset_fix<double6>,
                         idle_boxset_fix<double6>,
                         runtime_boxset_fix<double6>,
                         runtime_idle_boxset_fix<double6> >
double6_sets;

// Map lists
typedef boost::mpl::list<pointmap_fix<int2, std::string>,
                         tight_pointmap_fix<int2, std::string>,
                         idle_pointmap_fix<int2, std::string>,
                         runtime_pointmap_fix<int2, std::string>,
                         runtime_idle_pointmap_fix<int2, std::string>,
                         boxmap_fix<int2, std::string>,
                         tight_boxmap_fix<int2, std::string>,
                         idle_boxmap_fix<int2, std::string>,
                         runtime_boxmap_fix<int2, std::string>,
                         runtime_idle_boxmap_fix<int2, std::string> >
int2_maps;

typedef boost::mpl::list<pointmap_fix<quad, std::string>,
                         tight_pointmap_fix<quad, std::string>,
                         idle_pointmap_fix<quad, std::string>,
                         runtime_pointmap_fix<quad, std::string>,
                         runtime_idle_pointmap_fix<quad, std::string>,
                         boxmap_fix<quad, std::string>,
                         tight_boxmap_fix<quad, std::string>,
                         idle_boxmap_fix<quad, std::string>,
                         runtime_boxmap_fix<quad, std::string>,
                         runtime_idle_boxmap_fix<quad, std::string> >
quad_maps;

typedef boost::mpl::list<pointmap_fix<double6, std::string>,
                         tight_pointmap_fix<double6, std::string>,
                         idle_pointmap_fix<double6, std::string>,
                         runtime_pointmap_fix<double6, std::string>,
                         runtime_idle_pointmap_fix<double6, std::string>,
                         boxmap_fix<double6, std::string>,
                         tight_boxmap_fix<double6, std::string>,
                         idle_boxmap_fix<double6, std::string>,
                         runtime_boxmap_fix<double6, std::string>,
                         runtime_idle_boxmap_fix<double6, std::string> >
double6_maps;

// Every quad
typedef boost::mpl::list<pointset_fix<quad>,
                         tight_pointset_fix<quad>,
                         idle_pointset_fix<quad>,
                         runtime_pointset_fix<quad>,
                         runtime_idle_pointset_fix<quad>,
                         boxset_fix<quad>,
                         tight_boxset_fix<quad>,
                         idle_boxset_fix<quad>,
                         runtime_boxset_fix<quad>,
                         runtime_idle_boxset_fix<quad>,
                         pointmap_fix<quad, std::string>,
                         tight_pointmap_fix<quad, std::string>,
                         idle_pointmap_fix<quad, std::string>,
                         runtime_pointmap_fix<quad, std::string>,
                         runtime_idle_pointmap_fix<quad, std::string>,
                         boxmap_fix<quad, std::string>,
                         tight_boxmap_fix<quad, std::string>,
                         idle_boxmap_fix<quad, std::string>,
                         runtime_boxmap_fix<quad, std::string>,
                         runtime_idle_boxmap_fix<quad, std::string> >
every_quad;


#endif // SPATIAL_TEST_FIXTURES_HPP
