// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_test_equal.hpp
 *  All tests for the elements defined in equal_iterator.hpp are
 *  located in this file.
 *  \see equal_iterator.hpp
 */

#ifndef SPATIAL_TEST_EQUAL_HPP
#define SPATIAL_TEST_EQUAL_HPP

#include "../../src/equal_iterator.hpp"

BOOST_AUTO_TEST_CASE_TEMPLATE(test_equal_basics, Tp, every_quad)
{
  Tp fix(0);
  equal_iterator<typename Tp::container_type> a;
  a.node = fix.container.end().node;
  equal_iterator<typename Tp::container_type> b(a);
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

BOOST_AUTO_TEST_CASE_TEMPLATE( test_equal_deference, Tp, double6_maps )
{
  Tp fix(1, same()); // insert 1 key containing (1.0, 1.0, 1.0...)
  double6 model; std::fill(model.begin(), model.end(), 1.0);
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

BOOST_AUTO_TEST_CASE_TEMPLATE( test_equal_minimum, Tp, double6_sets )
{
  {
    Tp fix(100, randomize(-1, 1));
    while (!fix.container.empty())
      {
        // Randomly pick one of the values in the tree and attempt to find it.
        typename Tp::container_type::iterator pick = fix.container.begin();
        std::advance(pick, (ptrdiff_t)
                     ((size_type) std::rand() % fix.container.size()));
        equal_iterator<typename Tp::container_type>
          it = equal_begin(fix.container, *pick);
        // It should always find it!
        BOOST_CHECK(it != equal_end(fix.container, *pick));
        BOOST_CHECK(*it == *pick);
        fix.container.erase(it);
      }
  }
  { // A tree where all elements are the same (= 100.0)!
    Tp fix(100, same());
    double6 k; std::fill(k.begin(), k.end(), 100.0); // leave none out...
    while (!fix.container.empty())
      {
        equal_iterator<typename Tp::container_type>
          it = equal_begin(fix.container, k);
        BOOST_CHECK(it != equal_end(fix.container, k));
        fix.container.erase(it);
      }
  }
  { // test at the limit: a tree with 1 element
    Tp fix(1, same());
    double6 k; std::fill(k.begin(), k.end(), 1.0);
    equal_iterator<typename Tp::container_type>
      it = equal_begin(fix.container, k);
    BOOST_CHECK(it != equal_end(fix.container, k));
  }
  { // test at the limit: an unbalanced tree (i.e. insertions in order)!
    Tp fix(100, increase());
    while (!fix.container.empty())
      {
        // Randomly pick one of the values in the tree and attempt to find it.
        typename Tp::container_type::iterator pick = fix.container.begin();
        std::advance(pick, (ptrdiff_t)
                     ((size_type) std::rand() % fix.container.size()));
        equal_iterator<typename Tp::container_type>
          it = equal_begin(fix.container, *pick);
        // It should always find it!
        BOOST_CHECK(it != equal_end(fix.container, *pick));
        BOOST_CHECK(*it == *pick);
        fix.container.erase(it);
      }
  }
  { // test at the limit: an unbalanced tree (i.e. insertions in order)!
    Tp fix(100, decrease());
    while (!fix.container.empty())
      {
        // Randomly pick one of the values in the tree and attempt to find it.
        typename Tp::container_type::iterator pick = fix.container.begin();
        std::advance(pick, (ptrdiff_t)
                     ((size_type) std::rand() % fix.container.size()));
        equal_iterator<typename Tp::container_type>
          it = equal_begin(fix.container, *pick);
        // It should always find it!
        BOOST_CHECK(it != equal_end(fix.container, *pick));
        BOOST_CHECK(*it == *pick);
        fix.container.erase(it);
      }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE( test_equal_maximum, Tp, double6_sets )
{
  {
    Tp fix(100, randomize(-1, 1));
    while (!fix.container.empty())
      {
        // Randomly pick one of the values in the tree and attempt to find it.
        typename Tp::container_type::iterator pick = fix.container.begin();
        std::advance(pick, (ptrdiff_t)
                     ((size_type) std::rand() % fix.container.size()));
        equal_iterator<typename Tp::container_type>
          it = equal_end(fix.container, *pick);
        --it; // call maximum_equal
        // It should always find it!
        BOOST_CHECK(it != equal_end(fix.container, *pick));
        BOOST_CHECK(*it == *pick);
        fix.container.erase(it);
      }
  }
  { // A tree where all elements are the same (= 100.0)!
    Tp fix(100, same());
    double6 k; std::fill(k.begin(), k.end(), 100.0); // leave none out...
    while (!fix.container.empty())
      {
        equal_iterator<typename Tp::container_type>
          it = equal_end(fix.container, k);
        --it; // call maximum_equal
        BOOST_CHECK(it != equal_end(fix.container, k));
        fix.container.erase(it);
      }
  }
  { // test at the limit: a tree with 1 element
    Tp fix(1, same());
    double6 k; std::fill(k.begin(), k.end(), 1.0);
    equal_iterator<typename Tp::container_type>
      it = equal_end(fix.container, k);
    --it; // call maximum_equal
    BOOST_CHECK(it != equal_end(fix.container, k));
  }
  { // test at the limit: an unbalanced tree (i.e. insertions in order)!
    Tp fix(100, increase());
    while (!fix.container.empty())
      {
        // Randomly pick one of the values in the tree and attempt to find it.
        typename Tp::container_type::iterator pick = fix.container.begin();
        std::advance(pick, (ptrdiff_t)
                     ((size_type) std::rand() % fix.container.size()));
        equal_iterator<typename Tp::container_type>
          it = equal_end(fix.container, *pick);
        --it; // call maximum_equal
        // It should always find it!
        BOOST_CHECK(it != equal_end(fix.container, *pick));
        BOOST_CHECK(*it == *pick);
        fix.container.erase(it);
      }
  }
  { // test at the limit: an unbalanced tree (i.e. insertions in order)!
    Tp fix(100, decrease());
    while (!fix.container.empty())
      {
        // Randomly pick one of the values in the tree and attempt to find it.
        typename Tp::container_type::iterator pick = fix.container.begin();
        std::advance(pick, (ptrdiff_t)
                     ((size_type) std::rand() % fix.container.size()));
        equal_iterator<typename Tp::container_type>
          it = equal_end(fix.container, *pick);
        --it; // call maximum_equal
        // It should always find it!
        BOOST_CHECK(it != equal_end(fix.container, *pick));
        BOOST_CHECK(*it == *pick);
        fix.container.erase(it);
      }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE( test_equal_increment, Tp, quad_sets )
{
  {
    Tp fix(100, randomize(-1, 1)); // tight interval to generate duplicates
    // Prove that you can iterate all N nodes, down to 1 nodes
    while (!fix.container.empty())
      {
        int count_it = 0, count_eq = 0;
        typename Tp::container_type::iterator it = fix.container.begin();
        typename Tp::container_type::iterator pick = it;
        std::advance(pick, (ptrdiff_t)
                     ((size_type) std::rand() % fix.container.size()));
        for (; it != fix.container.end(); ++it)
          {
            if (*it == *pick) ++count_it;
          }
        equal_iterator<typename Tp::container_type>
          eq = equal_begin(fix.container, *pick);
        for (;eq != equal_end(fix.container, *pick); ++eq)
          {
            BOOST_CHECK(*eq == *pick);
            ++count_eq;
          }
        BOOST_CHECK_EQUAL(count_it, count_eq);
        fix.container.erase(pick);
      }
  }
  { // A tree where all elements are the same (= 100.0)!
    Tp fix(100, same());
    // Prove that you can iterate all N nodes, down to 1 nodes
    quad model(100, 100, 100, 100);
    while (!fix.container.empty())
      {
        BOOST_CHECK(std::distance(equal_begin(fix.container, model),
                                  equal_end(fix.container, model))
                    == static_cast<std::ptrdiff_t>(fix.container.size()));
        fix.container.erase(fix.container.begin());
      }
  }
  { // test at the limit: a tree with 1 element with point-box
    Tp fix(1, same());
    quad model(1, 1, 1, 1);
    equal_iterator<const typename Tp::container_type>
      i = equal_begin(fix.container, model),
      j = i;
    BOOST_CHECK(i != equal_end(fix.container, model));
    BOOST_CHECK(++i == equal_end(fix.container, model));
    BOOST_CHECK(j++ != equal_end(fix.container, model));
    BOOST_CHECK(j == equal_end(fix.container, model));
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE( test_equal_decrement, Tp, quad_sets )
{
  {
    Tp fix(100, randomize(-1, 1)); // tight interval to generate duplicates
    // Prove that you can iterate all N nodes, down to 1 nodes
    while (!fix.container.empty())
      {
        int count_it = 0, count_eq = 0;
        typename Tp::container_type::iterator it = fix.container.begin();
        typename Tp::container_type::iterator pick = it;
        std::advance(pick, (ptrdiff_t)
                     ((size_type) std::rand() % fix.container.size()));
        for (; it != fix.container.end(); ++it)
          {
            if (*it == *pick) ++count_it;
          }
        std::reverse_iterator<equal_iterator<typename Tp::container_type> >
          req(equal_end(fix.container, *pick)),
          rend(equal_begin(fix.container, *pick));
        for (;req != rend; ++req)
          {
            BOOST_CHECK(*req == *pick);
            ++count_eq;
          }
        BOOST_CHECK_EQUAL(count_it, count_eq);
        fix.container.erase(pick);
      }
  }
  { // A tree where all elements are the same (= 100.0)!
    Tp fix(100, same());
    // Prove that you can iterate all N nodes, down to 1 nodes
    quad model(100, 100, 100, 100);
    while (!fix.container.empty())
      {
        std::reverse_iterator
          <equal_iterator<const typename Tp::container_type> >
          begin(equal_end(fix.container, model)),
          end(equal_begin(fix.container, model));
        BOOST_CHECK(std::distance(begin, end)
                    == static_cast<std::ptrdiff_t>(fix.container.size()));
        fix.container.erase(--(fix.container.end()));
      }
  }
  { // test at the limit: a tree with 1 element with point-box
    Tp fix(1, same());
    quad model(1, 1, 1, 1);
    equal_iterator<typename Tp::container_type>
      i = equal_end(fix.container, model),
      j = i;
    BOOST_CHECK(--i != equal_end(fix.container, model));
    BOOST_CHECK(i == equal_begin(fix.container, model));
    BOOST_CHECK(j-- == equal_end(fix.container, model));
    BOOST_CHECK(j == equal_begin(fix.container, model));
  }
}

#endif // SPATIAL_TEST_EQUAL_HPP
