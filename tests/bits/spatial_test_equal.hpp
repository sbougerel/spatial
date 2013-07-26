// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_test_equal.hpp
 *  \brief  All tests for the elements defined in equal_iterator.hpp are
 *  located in this file.
 *  \see equal_iterator.hpp
 */

#ifndef SPATIAL_TEST_EQUAL_HPP
#define SPATIAL_TEST_EQUAL_HPP

#include "../../src/equal_iterator.hpp"

BOOST_AUTO_TEST_CASE_TEMPLATE(test_equal_basics, Tp, every_quad)
{
  Tp fix(0);
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

BOOST_AUTO_TEST_CASE_TEMPLATE( test_region_deference, Tp, double6_maps )
{
  Tp fix(1, same()); // insert 1 key containing (1.0, 1.0, 1.0...)
  double6 model; model.assign(1.0);
  equal_iterator<typename Tp::container_type>
    a(fix.container, model, fix.container.begin());
  equal_iterator<const typename Tp::container_type>
    b(fix.container, model, fix.container.begin());
  BOOST_CHECK((*a).first == fix.container.begin()->first);
  (*a).second = "some string";
  BOOST_CHECK(a->first == fix.container.begin()->first);
  a->second = "some other string";
  BOOST_CHECK((*b).first == fix.container.begin()->first);
  BOOST_CHECK(b->first == fix.container.begin()->first);
  BOOST_CHECK(a.dimension() == fix.container.dimension());
  BOOST_CHECK(b.dimension() == fix.container.dimension());
}

BOOST_AUTO_TEST_CASE_TEMPLATE( test_region_minimum, Tp, double6_sets )
{
  {
    Tp fix(100, randomize(-1, 1));
    // Prove that you can find the min value with N nodes, down to 1 nodes
    double6 l; l.assign(-0.8); // leave a few out...
    double6 h; h.assign(0.8);
    while (!fix.container.empty())
      {
        equal_iterator<typename Tp::container_type>
          it = equal_begin(fix.container, l, h);
        if (it == equal_end(fix.container, l, h)) break;
        // Make sure it is one within [-0.8, 0.8)
        BOOST_CHECK(details::match_all(fix.container.rank(), *it,
                                       make_bounds(fix.container, l, h)));
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
        fix.container.erase(it);
      }
  }
  { // test at the limit: a tree with 0 element
    Tp fix(0);
    double6 k;
    equal_iterator<typename Tp::container_type>
      it = equal_begin(fix.container, k, k);
    BOOST_CHECK(it == equal_end(fix.container, k, k));
  }
  { // test at the limit: a tree with 1 element
    Tp fix(1, same());
    double6 k; k.assign(1.0);
    equal_iterator<typename Tp::container_type>
      it = equal_begin(fix.container, k, k);
    BOOST_CHECK(it != equal_end(fix.container, k, k));
  }
  { // test at the limit: an unbalanced tree (i.e. insertions in order)!
    Tp fix(100, increase());
    // Prove that you can find the min value with N nodes, down to 1 nodes
    double6 l; l.assign(20.0); // leave a few out...
    double6 h; h.assign(80.0);
    while (!fix.container.empty())
      {
        equal_iterator<typename Tp::container_type>
          it = equal_begin(fix.container, l, h);
        if (it == equal_end(fix.container, l, h)) break;
        // Make sure it is one within (10, 90)
        BOOST_CHECK(details::match_all(fix.container.rank(), *it,
                                       make_open_bounds(fix.container, l, h)));
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
        equal_iterator<typename Tp::container_type>
          it = equal_begin(fix.container, l, h);
        if (it == equal_end(fix.container, l, h)) break;
        // Make sure it is one within (10, 90)
        BOOST_CHECK(details::match_all(fix.container.rank(), *it,
                                       make_closed_bounds(fix.container, l, h)));
        fix.container.erase(it);
      }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE( test_region_maximum, Tp, double6_sets )
{
  {
    Tp fix(100, randomize(-1, 1));
    // Prove that you can find the max value with N nodes, down to 1 nodes
    double6 l; l.assign(-0.8); // leave a few out...
    double6 h; h.assign(0.8);
    while (!fix.container.empty())
      {
        equal_iterator<typename Tp::container_type>
          it = equal_end(fix.container, l, h);
        --it;
        if (it == equal_end(fix.container, l, h)) break;
        // Make sure it is one within [-0.8, 0.8)
        BOOST_CHECK(details::match_all(fix.container.rank(), *it,
                                       make_bounds(fix.container, l, h)));
        equal_iterator<typename Tp::container_type> tmp = it;
        ++tmp;
        BOOST_CHECK(tmp == equal_end(fix.container, l, h));
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
    equal_iterator<typename Tp::container_type>
      it = equal_end(fix.container, k, k);
    --it;
    BOOST_CHECK(it != equal_end(fix.container, k, k));
  }
  { // test at the limit: an unbalanced tree (i.e. insertions in order)!
    Tp fix(100, increase());
    // Prove that you can find the min value with N nodes, down to 1 nodes
    double6 l; l.assign(20.0); // leave a few out...
    double6 h; h.assign(80.0);
    while (!fix.container.empty())
      {
        equal_iterator<typename Tp::container_type>
          it = equal_end(fix.container, l, h);
        --it;
        if (it == equal_end(fix.container, l, h)) break;
        // Make sure it is one within (10, 90)
        BOOST_CHECK(details::match_all(fix.container.rank(), *it,
                                       make_open_bounds(fix.container, l, h)));
        equal_iterator<typename Tp::container_type> tmp = it;
        ++tmp;
        BOOST_CHECK(tmp == equal_end(fix.container, l, h));
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
        equal_iterator<typename Tp::container_type>
          it = equal_end(fix.container, l, h);
        --it;
        if (it == equal_end(fix.container, l, h)) break;
        // Make sure it is one within (10, 90)
        BOOST_CHECK(details::match_all(fix.container.rank(), *it,
                                       make_closed_bounds(fix.container, l, h)));
        equal_iterator<typename Tp::container_type> tmp = it;
        ++tmp;
        BOOST_CHECK(tmp == equal_end(fix.container, l, h));
        fix.container.erase(it);
      }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE( test_region_increment, Tp, double6_sets )
{
  {
    Tp fix(100, boximize(-1, 1));
    // Prove that you can iterate all N nodes, down to 1 nodes
    double6 b; b[0] = b[1] = b[2] = -0.8; b[3] = b[4] = b[5] = 0.8;
    while (!fix.container.empty())
      {
        int count_it = 0, count_re = 0;
        enclosed_bounds<double6, bracket_less<double6> >
          eb = make_enclosed_bounds(fix.container, b);
        typename Tp::container_type::iterator it = fix.container.begin();
        for (; it != fix.container.end(); ++it)
          {
            if (details::match_all(fix.container.rank(), *it, eb))
              ++count_it;
          }
        equal_iterator<typename Tp::container_type>
          re = equal_begin(fix.container, b);
        for (;re != equal_end(fix.container, b); ++re)
          {
            BOOST_CHECK(details::match_all(fix.container.rank(), *re, eb));
            ++count_re;
          }
        BOOST_CHECK_EQUAL(count_it, count_re);
        fix.container.erase(fix.container.begin());
      }
  }
  { // A tree where all elements are the same (= 100.0)!
    Tp fix(100, same());
    // Prove that you can iterate all N nodes, down to 1 nodes
    double6 b; b[0] = b[1] = b[2] = 99.0; b[3] = b[4] = b[5] = 101.0;
    while (!fix.container.empty())
      {
        BOOST_CHECK(std::distance(equal_begin(fix.container, b),
                                  equal_end(fix.container, b))
                    == static_cast<std::ptrdiff_t>(fix.container.size()));
        fix.container.erase(fix.container.begin());
      }
  }
  { // test at the limit: a tree with 1 element with point-box
    Tp fix(1, same());
    // Prove that you can iterate all N nodes, down to 1 nodes
    double6 b; b[0] = b[1] = b[2] = b[3] = b[4] = b[5] = 1.0;
    equal_iterator<typename Tp::container_type, lhlh_layout_tag>
      i = equal_begin(fix.container, b, lhlh_layout),
      j = i;
    BOOST_CHECK(i != equal_end(fix.container, b, lhlh_layout));
    BOOST_CHECK(++i == equal_end(fix.container, b, lhlh_layout));
    BOOST_CHECK(j++ != equal_end(fix.container, b, lhlh_layout));
    BOOST_CHECK(j == equal_end(fix.container, b, lhlh_layout));
  }
  { // test at the limit: an unbalanced tree (i.e. insertions in order)!
    Tp fix(100, increase());
    // Prove that you can iterate all N nodes, down to 1 nodes
    double6 l; l[0] = l[1] = l[2] = l[3] = l[4] = l[5] = -20.0;
    double6 h; h[0] = h[1] = h[2] = h[3] = h[4] = h[5] = 60.0;
    while (!fix.container.empty())
      {
        int count_it = 0, count_re = 0;
        open_bounds<double6, bracket_less<double6> >
          orb = make_open_bounds(fix.container, l, h);
        typename Tp::container_type::iterator it = fix.container.begin();
        for (; it != fix.container.end(); ++it)
          {
            if (details::match_all(fix.container.rank(), *it, orb))
              ++count_it;
          }
        equal_iterator<typename Tp::container_type>
          re = equal_begin(fix.container, l, h);
        for (;re != equal_end(fix.container, l, h); ++re)
          {
            BOOST_CHECK(details::match_all(fix.container.rank(), *re, orb));
            ++count_re;
          }
        BOOST_CHECK_EQUAL(count_it, count_re);
        fix.container.erase(fix.container.begin());
      }
  }
  { // test at the limit: an unbalanced tree (i.e. insertions in order)!
    Tp fix(100, decrease());
    // Prove that you can iterate all N nodes, down to 1 nodes
    double6 l; l[0] = l[1] = l[2] = l[3] = l[4] = l[5] = 30.0;
    double6 h; h[0] = h[1] = h[2] = h[3] = h[4] = h[5] = 120.0;
    while (!fix.container.empty())
      {
        int count_it = 0, count_re = 0;
        open_bounds<double6, bracket_less<double6> >
          orb = make_open_bounds(fix.container, l, h);
        typename Tp::container_type::iterator it = fix.container.begin();
        for (; it != fix.container.end(); ++it)
          {
            if (details::match_all(fix.container.rank(), *it, orb))
              ++count_it;
          }
        equal_iterator<typename Tp::container_type>
          re = equal_begin(fix.container, l, h);
        for (;re != equal_end(fix.container, l, h); ++re)
          {
            BOOST_CHECK(details::match_all(fix.container.rank(), *re, orb));
            ++count_re;
          }
        BOOST_CHECK_EQUAL(count_it, count_re);
        fix.container.erase(fix.container.begin());
      }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE( test_region_decrement, Tp, double6_sets )
{
  {
    Tp fix(100, boximize(-1, 1));
    // Prove that you can iterate all N nodes, down to 1 nodes
    double6 b; b[0] = b[1] = b[2] = -0.8; b[3] = b[4] = b[5] = 0.8;
    while (!fix.container.empty())
      {
        int count_it = 0, count_re = 0;
        enclosed_bounds<double6, bracket_less<double6> >
          eb(make_enclosed_bounds(fix.container, b));
        typename Tp::container_type::iterator it = fix.container.begin();
        for (; it != fix.container.end(); ++it)
          {
            if (details::match_all(fix.container.rank(), *it, eb))
              ++count_it;
          }
        std::reverse_iterator
          <equal_iterator<typename Tp::container_type> >
          re(equal_end(fix.container, b)),
          rend(equal_begin(fix.container, b));
        for (;re != rend; ++re)
          {
            BOOST_CHECK(details::match_all(fix.container.rank(), *re, eb));
            ++count_re;
          }
        BOOST_CHECK_EQUAL(count_it, count_re);
        fix.container.erase(fix.container.begin());
      }
  }
  { // A tree where all elements are the same (= 100.0)!
    Tp fix(100, same());
    // Prove that you can iterate all N nodes, down to 1 nodes
    double6 b; b[0] = b[1] = b[2] = 99.0; b[3] = b[4] = b[5] = 101.0;
    while (!fix.container.empty())
      {
        std::reverse_iterator
          <equal_iterator<typename Tp::container_type> >
          begin(equal_end(fix.container, b)),
          end(equal_begin(fix.container, b));
        BOOST_CHECK(std::distance(begin, end)
                    == static_cast<std::ptrdiff_t>(fix.container.size()));
        fix.container.erase(fix.container.begin());
      }
  }
  { // test at the limit: a tree with 1 element with point-box
    Tp fix(1, same());
    // Prove that you can iterate all N nodes, down to 1 nodes
    double6 b; b[0] = b[1] = b[2] = b[3] = b[4] = b[5] = 1.0;
    equal_iterator<typename Tp::container_type, lhlh_layout_tag>
      i = equal_end(fix.container, b, lhlh_layout),
      j = i;
    BOOST_CHECK(i != equal_begin(fix.container, b, lhlh_layout));
    BOOST_CHECK(--i == equal_begin(fix.container, b, lhlh_layout));
    BOOST_CHECK(j-- != equal_begin(fix.container, b, lhlh_layout));
    BOOST_CHECK(j == equal_begin(fix.container, b, lhlh_layout));
  }
  { // test at the limit: an unbalanced tree (i.e. insertions in order)!
    Tp fix(100, increase());
    // Prove that you can iterate all N nodes, down to 1 nodes
    double6 l; l[0] = l[1] = l[2] = l[3] = l[4] = l[5] = -20.0;
    double6 h; h[0] = h[1] = h[2] = h[3] = h[4] = h[5] = 60.0;
    while (!fix.container.empty())
      {
        int count_it = 0, count_re = 0;
        open_bounds<double6, bracket_less<double6> >
          orb = make_open_bounds(fix.container, l, h);
        typename Tp::container_type::iterator it = fix.container.begin();
        for (; it != fix.container.end(); ++it)
          {
            if (details::match_all(fix.container.rank(), *it, orb))
              ++count_it;
          }
        std::reverse_iterator
          <equal_iterator<typename Tp::container_type> >
          re(equal_end(fix.container, l, h)),
          rend(equal_begin(fix.container, l, h));
        for (;re != rend; ++re)
          {
            BOOST_CHECK(details::match_all(fix.container.rank(), *re, orb));
            ++count_re;
          }
        BOOST_CHECK_EQUAL(count_it, count_re);
        fix.container.erase(fix.container.begin());
      }
  }
  { // test at the limit: an unbalanced tree (i.e. insertions in order)!
    Tp fix(100, decrease());
    // Prove that you can iterate all N nodes, down to 1 nodes
    double6 l; l[0] = l[1] = l[2] = l[3] = l[4] = l[5] = 30.0;
    double6 h; h[0] = h[1] = h[2] = h[3] = h[4] = h[5] = 120.0;
    while (!fix.container.empty())
      {
        int count_it = 0, count_re = 0;
        open_bounds<double6, bracket_less<double6> >
          orb = make_open_bounds(fix.container, l, h);
        typename Tp::container_type::iterator it = fix.container.begin();
        for (; it != fix.container.end(); ++it)
          {
            if (details::match_all(fix.container.rank(), *it, orb))
              ++count_it;
          }
        std::reverse_iterator
          <equal_iterator<typename Tp::container_type> >
          re(equal_end(fix.container, l, h)),
          rend(equal_begin(fix.container, l, h));
        for (;re != rend; ++re)
          {
            BOOST_CHECK(details::match_all(fix.container.rank(), *re, orb));
            ++count_re;
          }
        BOOST_CHECK_EQUAL(count_it, count_re);
        fix.container.erase(fix.container.begin());
      }
  }
}

#endif // SPATIAL_TEST_REGION_HPP
