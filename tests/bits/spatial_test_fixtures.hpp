// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   test_fixtures.hpp
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

#include "test_types.hpp"

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
  int2& operator()(int2& p, int i, int n) const
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
      container_type::const_iterator it = container.insert(manip(tp, i, n));
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
      container_type::const_iterator it = container.insert(manip(tp, i, n));
      BOOST_CHECK(*it == tp);
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
  template<typename Manip> pointset_fix(int n, const Manip& manip = pass())
    : basic_fixture<Tp, poinset<dimension_traits<Tp>::value, Tp, Compare> >
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
  template<typename Manip>
  tight_pointset_fix(int n, const Manip& manip = identical())
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
  template<typename Manip>
  runtime_pointset_fix(int n, const Manip& manip = identical())
    : runtime_fixture<Tp, runtime_poinset<Tp, Compare> >(n, manip) { }
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
  template<typename Manip>
  frozen_pointset_fix(int n, const Manip& manip = identical())
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
  template<typename Manip>
  runtime_frozen_pointset_fix(int n, const Manip& manip = identical())
    : runtime_fixture<Tp, runtime_frozen_pointset<Tp, Compare> >(n, manip) { }
};

struct int2_node_fixture
{
  /*           H
               |
             (2,2)
             /   \
         (1,1)   (3,3)
         /  \
     (0,0)  (1,1)   */
  typedef Node<Kdtree_link<int2, int2> > node_type;
  node_type header;
  node_type::ptr leftmost;
  node_type node_root;
  node_type node_left;
  node_type node_left_left;
  node_type node_left_right;
  node_type node_right;

  int2_node_fixture()
  {
    header.parent = &node_root;
    header.left = &header;
    header.right = &node_right;
    leftmost = &node_left_left;
    node_root.parent = &header;
    node_root.left = &node_left;
    node_root.right = &node_right;
    value(node_root) = twos;
    node_left.parent = &node_root;
    node_left.left = &node_left_left;
    node_left.right = &node_left_right;
    value(node_left) = ones;
    node_right.parent = &node_root;
    node_right.left = 0;
    node_right.right = 0;
    value(node_right) = threes;
    node_left_right.parent = &node_left;
    node_left_right.left = 0;
    node_left_right.right = 0;
    value(node_left_right) = ones;
    node_left_left.parent = &node_left;
    node_left_left.left = 0;
    node_left_left.right = 0;
    value(node_left_left) = zeros;
  }
};

struct int2_relaxed_node_fixture
{
  /*         H
             |
           (2,2)5
           /   \
      (1,1)3  (3,3)1
       /  \
  (0,0)1  (1,1)1       */
  typedef Node<Relaxed_kdtree_link<int2, int2> > node_type;
  node_type header;
  node_type::ptr leftmost;
  node_type node_root;
  node_type node_left;
  node_type node_left_left;
  node_type node_left_right;
  node_type node_right;

  int2_relaxed_node_fixture()
  {
    header.parent = &node_root;
    header.left = &header;
    header.right = &node_right;
    leftmost = &node_left_left;
    node_root.parent = &header;
    node_root.left = &node_left;
    node_root.right = &node_right;
    link(node_root).weight = 5;
    value(node_root) = twos;
    node_left.parent = &node_root;
    node_left.left = &node_left_left;
    node_left.right = &node_left_right;
    link(node_left).weight = 3;
    value(node_left) = ones;
    node_right.parent = &node_root;
    node_right.left = 0;
    node_right.right = 0;
    link(node_right).weight = 1;
    value(node_right) = threes;
    node_left_right.parent = &node_left;
    node_left_right.left = 0;
    node_left_right.right = 0;
    link(node_left_right).weight = 1;
    value(node_left_right) = ones;
    node_left_left.parent = &node_left;
    node_left_left.left = 0;
    node_left_left.right = 0;
    link(node_left_left).weight = 1;
    value(node_left_left) = zeros;
  }
};

struct large_int2_relaxed_node_fixture
{
  /*              H
                  |
               (2,2)7
            /          \
      (1,1)3           (3,3)3
       /  \             /
  (0,0)1  (1,1)1     (3,3)2
                       \
                     (3,3)1     */
  typedef Node<Relaxed_kdtree_link<int2, int2> > node_type;
  node_type header;
  node_type::ptr leftmost;
  node_type node_root;
  node_type node_left;
  node_type node_left_left;
  node_type node_left_right;
  node_type node_right;
  node_type node_right_left;
  node_type node_right_left_right;
  large_int2_relaxed_node_fixture()
  {
    header.parent = &node_root;
    header.left = &header;
    header.right = &node_right;
    leftmost = &node_left_left;
    node_root.parent = &header;
    node_root.left = &node_left;
    node_root.right = &node_right;
    link(node_root).weight = 7;
    value(node_root) = twos;
    node_left.parent = &node_root;
    node_left.left = &node_left_left;
    node_left.right = &node_left_right;
    link(node_left).weight = 3;
    value(node_left) = ones;
    node_right.parent = &node_root;
    node_right.left = &node_right_left;
    node_right.right = 0;
    link(node_right).weight = 3;
    value(node_right) = threes;
    node_right_left.parent = &node_right;
    node_right_left.left = 0;
    node_right_left.right = &node_right_left_right;
    link(node_right_left).weight = 2;
    value(node_right_left) = threes;
    node_right_left_right.parent = &node_right_left;
    node_right_left_right.left = 0;
    node_right_left_right.right = 0;
    link(node_right_left_right).weight = 1;
    value(node_right_left_right) = threes;
    node_left_right.parent = &node_left;
    node_left_right.left = 0;
    node_left_right.right = 0;
    link(node_left_right).weight = 1;
    value(node_left_right) = ones;
    node_left_left.parent = &node_left;
    node_left_left.left = 0;
    node_left_left.right = 0;
    link(node_left_left).weight = 1;
    value(node_left_left) = zeros;
  }
};

#endif // SPATIAL_TEST_FIXTURES_HPP
