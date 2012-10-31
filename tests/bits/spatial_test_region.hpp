// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_test_region.hpp
 *  @brief  All tests for the elements defined in spatial_region.hpp are
 *  located in this file.
 *  @see spatial_region.hpp
 */

#ifndef SPATIAL_TEST_REGION_HPP
#define SPATIAL_TEST_REGION_HPP

BOOST_AUTO_TEST_CASE( test_equal_bounds )
{
  int2 t(1, 1);
  int2 x(0, 1);
  int2 y(1, 0);
  int2 z(0, 2);
  int2 w(2, 0);
  pointset_fix<int2> fix(0);
  // Checking this compiles
  equal_bounds<int2, bracket_less<int2> > bounds
    = make_equal_bounds(fix.container, t);
  BOOST_CHECK(bounds(0, 2, t) == matching);
  BOOST_CHECK(bounds(1, 2, t) == matching);
  BOOST_CHECK(bounds(0, 2, x) == below);
  BOOST_CHECK(bounds(1, 2, x) == matching);
  BOOST_CHECK(bounds(0, 2, y) == matching);
  BOOST_CHECK(bounds(1, 2, y) == below);
  BOOST_CHECK(bounds(0, 2, z) == below);
  BOOST_CHECK(bounds(1, 2, z) == above);
  BOOST_CHECK(bounds(0, 2, w) == above);
  BOOST_CHECK(bounds(1, 2, w) == below);
}

BOOST_AUTO_TEST_CASE( test_open_bounds )
{
  int2 l(1, 1);
  int2 h(3, 3);
  int2 x(2, 1);
  int2 y(3, 2);
  pointset_fix<int2> fix(0);
  // Checking this compiles
  open_bounds<int2, bracket_less<int2> > bounds
    = make_open_bounds(fix.container, l, h);
  BOOST_CHECK(bounds(0, 2, l) == below);
  BOOST_CHECK(bounds(1, 2, l) == below);
  BOOST_CHECK(bounds(0, 2, h) == above);
  BOOST_CHECK(bounds(1, 2, h) == above);
  BOOST_CHECK(bounds(0, 2, x) == matching);
  BOOST_CHECK(bounds(1, 2, x) == below);
  BOOST_CHECK(bounds(0, 2, y) == above);
  BOOST_CHECK(bounds(1, 2, y) == matching);
}

BOOST_AUTO_TEST_CASE( test_bounds )
{
  int2 l(1, 1);
  int2 h(3, 3);
  int2 x(2, 0);
  int2 y(3, 2);
  int2 z(0, 0);
  pointset_fix<int2> fix(0);
  // Checking this compiles
  bounds<int2, bracket_less<int2> > bounds
    = make_bounds(fix.container, l, h);
  BOOST_CHECK(bounds(0, 2, l) == matching);
  BOOST_CHECK(bounds(1, 2, l) == matching);
  BOOST_CHECK(bounds(0, 2, h) == above);
  BOOST_CHECK(bounds(1, 2, h) == above);
  BOOST_CHECK(bounds(0, 2, x) == matching);
  BOOST_CHECK(bounds(1, 2, x) == below);
  BOOST_CHECK(bounds(0, 2, y) == above);
  BOOST_CHECK(bounds(1, 2, y) == matching);
  BOOST_CHECK(bounds(0, 2, z) == below);
  BOOST_CHECK(bounds(1, 2, z) == below);
}

BOOST_AUTO_TEST_CASE( test_closed_bounds )
{
  int2 l(1, 1);
  int2 h(3, 3);
  int2 x(2, 0);
  int2 y(4, 2);
  int2 z(0, 0);
  int2 w(4, 4);
  pointset_fix<int2> fix(0);
  // Checking this compiles
  closed_bounds<int2, bracket_less<int2> > bounds
    = make_closed_bounds(fix.container, l, h);
  BOOST_CHECK(bounds(0, 2, l) == matching);
  BOOST_CHECK(bounds(1, 2, l) == matching);
  BOOST_CHECK(bounds(0, 2, h) == matching);
  BOOST_CHECK(bounds(1, 2, h) == matching);
  BOOST_CHECK(bounds(0, 2, x) == matching);
  BOOST_CHECK(bounds(1, 2, x) == below);
  BOOST_CHECK(bounds(0, 2, y) == above);
  BOOST_CHECK(bounds(1, 2, y) == matching);
  BOOST_CHECK(bounds(0, 2, z) == below);
  BOOST_CHECK(bounds(1, 2, z) == below);
  BOOST_CHECK(bounds(0, 2, w) == above);
  BOOST_CHECK(bounds(1, 2, w) == above);
}

BOOST_AUTO_TEST_CASE_TEMPLATE( test_overlap_bounds, Tp, every_quad )
{
  Tp fix(0);
  { // test for llhh_layout_tag
    quad a(0, 0, 2, 2);
    overlap_bounds<quad, quad_less, llhh_layout_tag> bounds
      = make_overlap_bounds(fix.container, a);
    // A region must overlap itself (all it's element must match)
    BOOST_CHECK(details::match_all(fix.container.rank(), a, bounds));
    // A region must overlap a point at its center
    quad p(1, 1, 1, 1);
    BOOST_CHECK(details::match_all(fix.container.rank(), p, bounds));
    // A region must overlap a larger region than itself
    quad b(-1, -1, 3, 3);
    BOOST_CHECK(details::match_all(fix.container.rank(), b, bounds));
    // A region must *not* overlap another region whose corner only contact
    quad c(-1, -1, 0, 3);
    BOOST_CHECK(!details::match_all(fix.container.rank(), c, bounds));
    quad d(-1, -1, 3, 0);
    BOOST_CHECK(!details::match_all(fix.container.rank(), d, bounds));
    quad e(2, -1, 3, 3);
    BOOST_CHECK(!details::match_all(fix.container.rank(), e, bounds));
    quad f(-1, 2, 3, 3);
    BOOST_CHECK(!details::match_all(fix.container.rank(), f, bounds));
  }
  { // test for lhlh_layout_tag
    quad a(0, 2, 0, 2);
    overlap_bounds<quad, quad_less, lhlh_layout_tag> bounds
      = make_overlap_bounds(fix.container, a, lhlh_layout);
    // A region must overlap itself (all it's element must match)
    BOOST_CHECK(details::match_all(fix.container.rank(), a, bounds));
    // A region must overlap a point at its center
    quad p(1, 1, 1, 1);
    BOOST_CHECK(details::match_all(fix.container.rank(), p, bounds));
    // A region must overlap a larger region than itself
    quad b(-1, 3, -1, 3);
    BOOST_CHECK(details::match_all(fix.container.rank(), b, bounds));
    // A region must *not* overlap another region whose corner only contact
    quad c(-1, 0, -1, 3);
    BOOST_CHECK(!details::match_all(fix.container.rank(), c, bounds));
    quad d(-1, 3, -1, 0);
    BOOST_CHECK(!details::match_all(fix.container.rank(), d, bounds));
    quad e(2, 3, -1, 3);
    BOOST_CHECK(!details::match_all(fix.container.rank(), e, bounds));
    quad f(-1, 3, 2, 3);
    BOOST_CHECK(!details::match_all(fix.container.rank(), f, bounds));
  }
  { // test for hhll_layout_tag
    quad a(2, 2, 0, 0);
    overlap_bounds<quad, quad_less, hhll_layout_tag> bounds
      = make_overlap_bounds(fix.container, a, hhll_layout);
    // A region must overlap itself (all it's element must match)
    BOOST_CHECK(details::match_all(fix.container.rank(), a, bounds));
    // A region must overlap a point at its center
    quad p(1, 1, 1, 1);
    BOOST_CHECK(details::match_all(fix.container.rank(), p, bounds));
    // A region must overlap a larger region than itself
    quad b(3, 3, -1, -1);
    BOOST_CHECK(details::match_all(fix.container.rank(), b, bounds));
    // A region must *not* overlap another region whose corner only contact
    quad c(0, 3, -1, -1);
    BOOST_CHECK(!details::match_all(fix.container.rank(), c, bounds));
    quad d(3, 0, -1, -1);
    BOOST_CHECK(!details::match_all(fix.container.rank(), d, bounds));
    quad e(3, 3, 2, -1);
    BOOST_CHECK(!details::match_all(fix.container.rank(), e, bounds));
    quad f(3, 3, -1, 2);
    BOOST_CHECK(!details::match_all(fix.container.rank(), f, bounds));
  }
  { // test for hlhl_layout_tag
    quad a(2, 0, 2, 0);
    overlap_bounds<quad, quad_less, hlhl_layout_tag> bounds
      = make_overlap_bounds(fix.container, a, hlhl_layout);
    // A region must overlap itself (all it's element must match)
    BOOST_CHECK(details::match_all(fix.container.rank(), a, bounds));
    // A region must overlap a point at its center
    quad p(1, 1, 1, 1);
    BOOST_CHECK(details::match_all(fix.container.rank(), p, bounds));
    // A region must overlap a larger region than itself
    quad b(3, -1, 3, -1);
    BOOST_CHECK(details::match_all(fix.container.rank(), b, bounds));
    // A region must *not* overlap another region whose corner only contact
    quad c(0, -1, 3, -1);
    BOOST_CHECK(!details::match_all(fix.container.rank(), c, bounds));
    quad d(3, -1, 0, -1);
    BOOST_CHECK(!details::match_all(fix.container.rank(), d, bounds));
    quad e(3, 2, 3, -1);
    BOOST_CHECK(!details::match_all(fix.container.rank(), e, bounds));
    quad f(3, -1, 3, 2);
    BOOST_CHECK(!details::match_all(fix.container.rank(), f, bounds));
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE( test_enclosed_bounds, Tp, every_quad )
{
  Tp fix(0);
  { // test for llhh_layout_tag
    quad a(0, 0, 3, 3);
    enclosed_bounds<quad, quad_less> bounds
      = make_enclosed_bounds(fix.container, a);
    // A region must enclose itself (all it's element must match)
    BOOST_CHECK(details::match_all(fix.container.rank(), a, bounds));
    // A region must enclose a point at its center
    quad p(1, 1, 1, 1);
    BOOST_CHECK(details::match_all(fix.container.rank(), p, bounds));
    // A region must enclose a smaller region than itself
    quad b(1, 1, 2, 2);
    BOOST_CHECK(details::match_all(fix.container.rank(), b, bounds));
    // A region must *not* enclose another larger region
    quad c(-1, 0, 3, 3);
    BOOST_CHECK(!details::match_all(fix.container.rank(), c, bounds));
    quad d(0, -1, 3, 3);
    BOOST_CHECK(!details::match_all(fix.container.rank(), d, bounds));
    quad e(0, 0, 4, 3);
    BOOST_CHECK(!details::match_all(fix.container.rank(), e, bounds));
    quad f(0, 0, 3, 4);
    BOOST_CHECK(!details::match_all(fix.container.rank(), f, bounds));
  }
  { // test for lhlh_layout_tag
    quad a(0, 3, 0, 3);
    enclosed_bounds<quad, quad_less, lhlh_layout_tag> bounds
      = make_enclosed_bounds(fix.container, a, lhlh_layout);
    // A region must enclose itself (all it's element must match)
    BOOST_CHECK(details::match_all(fix.container.rank(), a, bounds));
    // A region must enclose a point at its center
    quad p(1, 1, 1, 1);
    BOOST_CHECK(details::match_all(fix.container.rank(), p, bounds));
    // A region must enclose a smaller region than itself
    quad b(1, 2, 1, 2);
    BOOST_CHECK(details::match_all(fix.container.rank(), b, bounds));
    // A region must *not* enclose another larger region
    quad c(-1, 3, 0,  3);
    BOOST_CHECK(!details::match_all(fix.container.rank(), c, bounds));
    quad d(0, 3, -1, 3);
    BOOST_CHECK(!details::match_all(fix.container.rank(), d, bounds));
    quad e(0, 4, 0, 3);
    BOOST_CHECK(!details::match_all(fix.container.rank(), e, bounds));
    quad f(0, 3, 0, 4);
    BOOST_CHECK(!details::match_all(fix.container.rank(), f, bounds));
  }
  { // test for hhll_layout_tag
    quad a(3, 3, 0, 0);
    enclosed_bounds<quad, quad_less, hhll_layout_tag> bounds
      = make_enclosed_bounds(fix.container, a, hhll_layout);
    // A region must enclose itself (all it's element must match)
    BOOST_CHECK(details::match_all(fix.container.rank(), a, bounds));
    // A region must enclose a point at its center
    quad p(1, 1, 1, 1);
    BOOST_CHECK(details::match_all(fix.container.rank(), p, bounds));
    // A region must enclose a smaller region than itself
    quad b(2, 2, 1, 1);
    BOOST_CHECK(details::match_all(fix.container.rank(), b, bounds));
    // A region must *not* enclose another larger region
    quad c(3, 3, -1, 0);
    BOOST_CHECK(!details::match_all(fix.container.rank(), c, bounds));
    quad d(3, 3, 0, -1);
    BOOST_CHECK(!details::match_all(fix.container.rank(), d, bounds));
    quad e(4, 3, 0, 0);
    BOOST_CHECK(!details::match_all(fix.container.rank(), e, bounds));
    quad f(3, 4, 0, 0);
    BOOST_CHECK(!details::match_all(fix.container.rank(), f, bounds));
  }
  { // test for hlhl_layout_tag
    quad a(3, 0, 3, 0);
    enclosed_bounds<quad, quad_less, hlhl_layout_tag> bounds
      = make_enclosed_bounds(fix.container, a, hlhl_layout);
    // A region must enclose itself (all it's element must match)
    BOOST_CHECK(details::match_all(fix.container.rank(), a, bounds));
    // A region must enclose a point at its center
    quad p(1, 1, 1, 1);
    BOOST_CHECK(details::match_all(fix.container.rank(), p, bounds));
    // A region must enclose a smaller region than itself
    quad b(2, 1, 2, 1);
    BOOST_CHECK(details::match_all(fix.container.rank(), b, bounds));
    // A region must *not* enclose another larger region
    quad c(3, -1, 3, 0);
    BOOST_CHECK(!details::match_all(fix.container.rank(), c, bounds));
    quad d(3, 0, 3, -1);
    BOOST_CHECK(!details::match_all(fix.container.rank(), d, bounds));
    quad e(4, 0, 3, 0);
    BOOST_CHECK(!details::match_all(fix.container.rank(), e, bounds));
    quad f(3, 0, 4, 0);
    BOOST_CHECK(!details::match_all(fix.container.rank(), f, bounds));
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE( test_region_basics, Tp, every_quad )
{
  Tp fix(0);
  {
    region_iterator<typename Tp::container_type> a, b(a);
    region_iterator<const typename Tp::container_type> c;
    c = a;
    region_iterator_pair<typename Tp::container_type> p, q(a, b);
    region_iterator_pair<const typename Tp::container_type>
      r, s(c, c), t(p);
    BOOST_CHECK(a == b);
    BOOST_CHECK(!(a != c));
    typename Tp::container_type::iterator i = a;
    BOOST_CHECK(i == a);
  }
  {
    equal_iterator<typename Tp::container_type> a, b(a);
    equal_iterator<const typename Tp::container_type> c;
    c = a;
    equal_iterator_pair<typename Tp::container_type> p, q(a, b);
    equal_iterator_pair<const typename Tp::container_type>
      r, s(c, c), t(p);
    BOOST_CHECK(a == b);
    BOOST_CHECK(!(a != c));
    typename Tp::container_type::iterator i = a;
    BOOST_CHECK(i == a);
  }
  {
    open_region_iterator<typename Tp::container_type> a, b(a);
    open_region_iterator<const typename Tp::container_type> c;
    c = a;
    open_region_iterator_pair<typename Tp::container_type> p, q(a, b);
    open_region_iterator_pair<const typename Tp::container_type>
      r, s(c, c), t(p);
    BOOST_CHECK(a == b);
    BOOST_CHECK(!(a != c));
    typename Tp::container_type::iterator i = a;
    BOOST_CHECK(i == a);
  }
  {
    closed_region_iterator<typename Tp::container_type> a, b(a);
    closed_region_iterator<const typename Tp::container_type> c;
    c = a;
    closed_region_iterator_pair<typename Tp::container_type> p, q(a, b);
    closed_region_iterator_pair<const typename Tp::container_type>
      r, s(c, c), t(p);
    BOOST_CHECK(a == b);
    BOOST_CHECK(!(a != c));
    typename Tp::container_type::iterator i = a;
    BOOST_CHECK(i == a);
  }
  {
    overlap_iterator<typename Tp::container_type> a, b(a);
    overlap_iterator<const typename Tp::container_type> c;
    c = a;
    overlap_iterator_pair<typename Tp::container_type> p, q(a, b);
    overlap_iterator_pair<const typename Tp::container_type>
      r, s(c, c), t(p);
    BOOST_CHECK(a == b);
    BOOST_CHECK(!(a != c));
    typename Tp::container_type::iterator i = a;
    BOOST_CHECK(i == a);
  }
  {
    enclosed_iterator<typename Tp::container_type> a, b(a);
    enclosed_iterator<const typename Tp::container_type> c;
    c = a;
    enclosed_iterator_pair<typename Tp::container_type> p, q(a, b);
    enclosed_iterator_pair<const typename Tp::container_type>
      r, s(c, c), t(p);
    BOOST_CHECK(a == b);
    BOOST_CHECK(!(a != c));
    typename Tp::container_type::iterator i = a;
    BOOST_CHECK(i == a);
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE( test_region_deference, Tp, double6_maps )
{
  Tp fix(1, same());
  double6 l; l.assign(0.0);
  double6 h; h.assign(1.0);
  region_iterator<typename Tp::container_type>
    a(fix.container, make_bounds(fix.container, l, h), fix.container.begin());
  region_iterator<const typename Tp::container_type>
    b(fix.container, make_bounds(fix.container, l, h), fix.container.begin());
  BOOST_CHECK((*a).first == fix.container.begin()->first);
  (*a).second = "some string";
  BOOST_CHECK(a->first == fix.container.begin()->first);
  a->second = "some other string";
  BOOST_CHECK((*b).first == fix.container.begin()->first);
  BOOST_CHECK(b->first == fix.container.begin()->first);
  BOOST_CHECK(a.dimension() == fix.container.dimension());
  BOOST_CHECK(b.dimension() == fix.container.dimension());
}

BOOST_AUTO_TEST_CASE_TEMPLATE( test_region_minimum_range, Tp, double6_sets )
{
  {
    Tp fix(100, randomize(-1, 1));
    // Prove that you can find the min value with N nodes, down to 1 nodes
    double6 l; l.assign(-0.8); // leave a few out...
    double6 h; h.assign(0.8);
    while (!fix.container.empty())
      {
        region_iterator<typename Tp::container_type>
          it = region_begin(fix.container, l, h);
        if (it == region_end(fix.container, l, h)) break;
        // Make sure it is one within [-0.8, 0.8)
        BOOST_CHECK(details::match_all(fix.container.rank(), *it,
                                       make_bounds(fix.container, l, h)));
        region_iterator<typename Tp::container_type> tmp = it;
        --tmp;
        BOOST_CHECK(tmp == region_end(fix.container, l, h));
        fix.container.erase(it);
      }
  }
  { // A tree where all elements are the same (= 100.0)!
    Tp fix(100, same());
    double6 k; k.assign(100.0); // leave none out...
    while (!fix.container.empty())
      {
        equal_iterator<typename Tp::container_type>
          it = equal_begin(fix.container, k);
        BOOST_CHECK(it != equal_end(fix.container, k));
        equal_iterator<typename Tp::container_type> tmp = it;
        --tmp;
        BOOST_CHECK(tmp == equal_end(fix.container, k));
        fix.container.erase(it);
      }
  }
  { // test at the limit: a tree with 0 element
    Tp fix(0);
    double6 k;
    closed_region_iterator<typename Tp::container_type>
      it = closed_region_begin(fix.container, k, k);
    BOOST_CHECK(it == closed_region_end(fix.container, k, k));
  }
  { // test at the limit: a tree with 1 element
    Tp fix(1, same());
    double6 k; k.assign(1.0);
    closed_region_iterator<typename Tp::container_type>
      it = closed_region_begin(fix.container, k, k);
    BOOST_CHECK(it != closed_region_end(fix.container, k, k));
  }
  { // test at the limit: an unbalanced tree (i.e. insertions in order)!
    Tp fix(100, increase());
    // Prove that you can find the min value with N nodes, down to 1 nodes
    double6 l; l.assign(20.0); // leave a few out...
    double6 h; h.assign(80.0);
    while (!fix.container.empty())
      {
        open_region_iterator<typename Tp::container_type>
          it = open_region_begin(fix.container, l, h);
        if (it == open_region_end(fix.container, l, h)) break;
        // Make sure it is one within (10, 90)
        BOOST_CHECK(details::match_all(fix.container.rank(), *it,
                                       make_open_bounds(fix.container, l, h)));
        open_region_iterator<typename Tp::container_type> tmp = it;
        --tmp;
        BOOST_CHECK(tmp == open_region_end(fix.container, l, h));
        fix.container.erase(it);
      }
  }
  { // test at the limit: an unbalanced tree (i.e. insertions in order)!
    Tp fix(100, decrease());
    // Prove that you can find the min value with N nodes, down to 1 nodes
    double6 l; l.assign(20.0); // leave a few out...
    double6 h; h.assign(80.0);
    while (!fix.container.empty())
      {
        closed_region_iterator<typename Tp::container_type>
          it = closed_region_begin(fix.container, l, h);
        if (it == closed_region_end(fix.container, l, h)) break;
        // Make sure it is one within (10, 90)
        BOOST_CHECK(details::match_all(fix.container.rank(), *it,
                                       make_closed_bounds(fix.container, l, h)));
        closed_region_iterator<typename Tp::container_type> tmp = it;
        --tmp;
        BOOST_CHECK(tmp == closed_region_end(fix.container, l, h));
        fix.container.erase(it);
      }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE( test_region_iterator_maximum, Tp, double6_sets )
{
  {
    Tp fix(100, randomize(-1, 1));
    // Prove that you can find the max value with N nodes, down to 1 nodes
    double6 l; l.assign(-0.8); // leave a few out...
    double6 h; h.assign(0.8);
    while (!fix.container.empty())
      {
        region_iterator<typename Tp::container_type>
          it = region_end(fix.container, l, h);
        --it;
        if (it == region_end(fix.container, l, h)) break;
        // Make sure it is one within [-0.8, 0.8)
        BOOST_CHECK(details::match_all(fix.container.rank(), *it,
                                       make_bounds(fix.container, l, h)));
        region_iterator<typename Tp::container_type> tmp = it;
        ++tmp;
        BOOST_CHECK(tmp == region_end(fix.container, l, h));
        fix.container.erase(it);
      }
  }
  { // A tree where all elements are the same (= 100.0)!
    Tp fix(100, same());
    double6 k; k.assign(100.0); // leave none out...
    while (!fix.container.empty())
      {
        equal_iterator<typename Tp::container_type>
          it = equal_end(fix.container, k);
        BOOST_CHECK(equal_begin(fix.container, k) != it);
        --it;
        BOOST_CHECK(it != equal_end(fix.container, k));
        equal_iterator<typename Tp::container_type> tmp = it;
        ++tmp;
        BOOST_CHECK(tmp == equal_end(fix.container, k));
        fix.container.erase(it);
      }
  }
  { // test at the limit: a tree with 1 element
    Tp fix(1, same());
    double6 k; k.assign(1.0);
    closed_region_iterator<typename Tp::container_type>
      it = closed_region_end(fix.container, k, k);
    --it;
    BOOST_CHECK(it != closed_region_end(fix.container, k, k));
  }
  { // test at the limit: an unbalanced tree (i.e. insertions in order)!
    Tp fix(100, increase());
    // Prove that you can find the min value with N nodes, down to 1 nodes
    double6 l; l.assign(20.0); // leave a few out...
    double6 h; h.assign(80.0);
    while (!fix.container.empty())
      {
        open_region_iterator<typename Tp::container_type>
          it = open_region_end(fix.container, l, h);
        --it;
        if (it == open_region_end(fix.container, l, h)) break;
        // Make sure it is one within (10, 90)
        BOOST_CHECK(details::match_all(fix.container.rank(), *it,
                                       make_open_bounds(fix.container, l, h)));
        open_region_iterator<typename Tp::container_type> tmp = it;
        ++tmp;
        BOOST_CHECK(tmp == open_region_end(fix.container, l, h));
        fix.container.erase(it);
      }
  }
  { // test at the limit: an unbalanced tree (i.e. insertions in order)!
    Tp fix(100, decrease());
    // Prove that you can find the min value with N nodes, down to 1 nodes
    double6 l; l.assign(20.0); // leave a few out...
    double6 h; h.assign(80.0);
    while (!fix.container.empty())
      {
        closed_region_iterator<typename Tp::container_type>
          it = closed_region_end(fix.container, l, h);
        --it;
        if (it == closed_region_end(fix.container, l, h)) break;
        // Make sure it is one within (10, 90)
        BOOST_CHECK(details::match_all(fix.container.rank(), *it,
                                       make_closed_bounds(fix.container, l, h)));
        closed_region_iterator<typename Tp::container_type> tmp = it;
        ++tmp;
        BOOST_CHECK(tmp == closed_region_end(fix.container, l, h));
        fix.container.erase(it);
      }
  }
}

/*
BOOST_AUTO_TEST_CASE( test_region_iterator_pre_increment )
{
  {
    typedef closed_region_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Region_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> region_iterator;
    typedef Hundred_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order region iteration.
    Hundred_kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    region_iterator region_min = region_iterator::minimum
      (fix.kdtree.rank(), whole_tree_box, 0,
       fix.kdtree.end().node->parent);
    const_iterator min; min.node = region_min.impl_.node_;
    const_iterator end(fix.kdtree.end());
    for(; min != end; ++min)
      {
        if (spatial::details::match_all
            (fix.kdtree.rank(), *min, whole_tree_box))
          {
            BOOST_CHECK(region_min.impl_.node_ == min.node);
            ++region_min;
          }
      }
    BOOST_CHECK(region_min.impl_.node_ == min.node); // at end!
  }
  {
    typedef open_region_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Region_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> region_iterator;
    typedef Hundred_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order region iteration.
    Hundred_kdtree_2D_fixture fix;
    // bounds encloses only a small part of the tree..
    point2d low = { {7, 8} };
    point2d high = { {11, 12} };
    bounds_type small_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    region_iterator region_min = region_iterator::minimum
      (fix.kdtree.rank(), small_box, 0,
       fix.kdtree.end().node->parent);
    const_iterator min; min.node = region_min.impl_.node_;
    const_iterator end(fix.kdtree.end());
    for(; min != end; ++min)
      {
        if (spatial::details::match_all
            (fix.kdtree.rank(), *min, small_box))
          {
            BOOST_CHECK(region_min.impl_.node_ == min.node);
            ++region_min;
          }
      }
    BOOST_CHECK(region_min.impl_.node_ == min.node); // at end!
  }
}

BOOST_AUTO_TEST_CASE( test_region_iterator_pre_decrement )
{
  {
    typedef closed_region_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Region_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> region_iterator;
    typedef Hundred_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order region iteration.
    Hundred_kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    region_iterator region_max = region_iterator
      (fix.kdtree.rank(), whole_tree_box, 1,
       static_cast<Hundred_kdtree_2D_fixture::kdtree_type::node_type *>
       (fix.kdtree.end().node));
    const_iterator max; max.node = region_max.impl_.node_;
    const_iterator begin(fix.kdtree.begin());
    for(; max != begin; --max)
      {
        if (spatial::details::match_all
            (fix.kdtree.rank(), *max, whole_tree_box))
          {
            --region_max;
            BOOST_CHECK(region_max.impl_.node_ == max.node);
          }
      }
  }
  {
    typedef open_region_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Region_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> region_iterator;
    typedef Hundred_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order region iteration.
    Hundred_kdtree_2D_fixture fix;
    // bounds encloses only a small part of the tree..
    point2d low = { {7, 7} };
    point2d high = { {12, 12} };
    bounds_type small_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    region_iterator region_max = region_iterator
      (fix.kdtree.rank(), small_box, 1,
       static_cast<Hundred_kdtree_2D_fixture::kdtree_type::node_type *>
       (fix.kdtree.end().node));
    const_iterator max; max.node = region_max.impl_.node_;
    const_iterator begin(fix.kdtree.begin());
    for(; max != begin; --max)
      {
        if (spatial::details::match_all
            (fix.kdtree.rank(), *max, small_box))
          {
            --region_max;
            BOOST_CHECK(region_max.impl_.node_ == max.node);
          }
      }
  }
}

BOOST_AUTO_TEST_CASE( test_region_iterator_post_decrement )
{
    typedef closed_region_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Region_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> region_iterator;
    typedef Hundred_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order region iteration.
    Hundred_kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    region_iterator test = region_iterator
      (fix.kdtree.rank(), whole_tree_box, 1,
       static_cast<Hundred_kdtree_2D_fixture::kdtree_type::node_type *>
       (fix.kdtree.end().node));
    region_iterator before = test;
    region_iterator after = test--;
    BOOST_CHECK(before == after);
    --after;
    BOOST_CHECK(after == test);
}

BOOST_AUTO_TEST_CASE( test_region_iterator_post_increment )
{
    typedef closed_region_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Region_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> region_iterator;
    typedef Hundred_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order region iteration.
    Hundred_kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    region_iterator test = region_iterator::minimum
      (fix.kdtree.rank(), whole_tree_box,
       0, fix.kdtree.end().node->parent);
    region_iterator before = test;
    region_iterator after = test++;
    BOOST_CHECK(before == after);
    ++after;
    BOOST_CHECK(after == test);
}
*/

#endif // SPATIAL_TEST_REGION_HPP
