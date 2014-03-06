// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_test_mapping.hpp
 *  All tests for the elements defined in spatial_mapping.hpp are
 *  located in this file.
 *  \see spatial_mapping.hpp
 */

#ifndef SPATIAL_TEST_MAPPING_HPP
#define SPATIAL_TEST_MAPPING_HPP

#include "../../src/mapping_iterator.hpp"

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_mapping_basics, Tp, every_quad )
{
  Tp fix(0);
  mapping_iterator<typename Tp::container_type> a, b(a);
  mapping_iterator<const typename Tp::container_type> c;
  c = a;
  mapping_iterator_pair<typename Tp::container_type> p, q(a, b);
  mapping_iterator_pair<const typename Tp::container_type>
    r, s(c, c), t(p);
  BOOST_CHECK(a == b);
  BOOST_CHECK(!(a != c));
  typename Tp::container_type::iterator i = a;
  BOOST_CHECK(i == a);
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_mapping_dereference, Tp, quad_maps )
{
  Tp fix(1, same());
  mapping_iterator<typename Tp::container_type>
    a(fix.container, 0, fix.container.begin());
  mapping_iterator<const typename Tp::container_type>
    b(fix.container, 0, fix.container.begin());
  BOOST_CHECK((*a).first == fix.container.begin()->first);
  (*a).second = "some string";
  BOOST_CHECK(a->first == fix.container.begin()->first);
  a->second = "some other string";
  BOOST_CHECK((*b).first == fix.container.begin()->first);
  BOOST_CHECK(b->first == fix.container.begin()->first);
  BOOST_CHECK(a.dimension() == fix.container.dimension());
  BOOST_CHECK(b.dimension() == fix.container.dimension());
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_mapping_minimum, Tp, int2_sets )
{
  {
    Tp fix(100, randomize(-20, 20));
    // Prove that you can find the min value with N nodes, down to 1 nodes
    while (!fix.container.empty())
      {
        unsigned int count = 0;
        int min_value_0 = (*fix.container.begin())[0];
        int min_value_1 = (*fix.container.begin())[1];
        for(typename Tp::container_type::iterator
              i = fix.container.begin(); i != fix.container.end(); ++i)
          {
            int tmp = (*i)[0];
            if (tmp < min_value_0) { min_value_0 = tmp; }
            tmp = (*i)[1];
            if (tmp < min_value_1) { min_value_1 = tmp; }
            ++count;
          }
        BOOST_CHECK_EQUAL(count, fix.container.size());
        mapping_iterator<typename Tp::container_type> iter;
        dimension_type mapping_dim = 0;
        iter = mapping_begin(fix.container, mapping_dim);
        BOOST_CHECK_EQUAL((*iter)[mapping_dim], min_value_0);
        mapping_dim = 1;
        iter = mapping_begin(fix.container, mapping_dim);
        BOOST_CHECK_EQUAL((*iter)[mapping_dim], min_value_1);
        fix.container.erase(iter);
      }
  }
  { // A tree where all elements are the same!
    Tp fix(100, same());
    // Prove that you can find the min value with N nodes, down to 1 nodes
    while (!fix.container.empty())
      {
        unsigned int count = 0;
        for(typename Tp::container_type::iterator
              i = fix.container.begin(); i != fix.container.end(); ++i)
          { ++count; }
        BOOST_CHECK_EQUAL(count, fix.container.size());
        mapping_iterator<typename Tp::container_type> iter;
        iter = mapping_begin(fix.container, 0);
        BOOST_CHECK_EQUAL((*iter)[0], 100);
        iter = mapping_begin(fix.container, 1);
        BOOST_CHECK_EQUAL((*iter)[1], 100);
        fix.container.erase(iter);
      }
  }
  { // test at the limit: a tree with 1 element
    Tp fix(1, same());
    mapping_iterator<const typename Tp::container_type> iter;
    iter = mapping_cbegin(fix.container, 0);
    BOOST_CHECK_EQUAL((*iter)[0], 1); // should be (1, 1);
    BOOST_CHECK_EQUAL((*iter)[1], 1);
    iter = mapping_cbegin(fix.container, 1);
    BOOST_CHECK_EQUAL((*iter)[0], 1); // should be (1, 1);
    BOOST_CHECK_EQUAL((*iter)[1], 1);
  }
  { // test at the limit: an unbalanced tree (i.e. insertions in order)!
    Tp fix(100, decrease());
    mapping_iterator<typename Tp::container_type> iter;
    dimension_type mapping_dim = 0;
    iter = mapping_begin(fix.container, mapping_dim);
    BOOST_CHECK_EQUAL((*iter)[0], 1); // should be (1, 1);
    BOOST_CHECK_EQUAL((*iter)[1], 1);
  }
  { // test at the limit: an unbalanced tree (i.e insertions in order)!
    Tp fix(100, increase());
    mapping_iterator<typename Tp::container_type> iter;
    dimension_type mapping_dim = 1;
    iter = mapping_begin(fix.container, mapping_dim);
    BOOST_CHECK_EQUAL((*iter)[0], 0); // should be (0, 0);
    BOOST_CHECK_EQUAL((*iter)[1], 0);
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_mapping_maximum, Tp, int2_sets )
{
  {
    Tp fix(100, randomize(-20, 20));
    // Prove that you can find the max value with N nodes, down to 1 nodes
    while (!fix.container.empty())
      {
        unsigned int count = 0;
        int max_value_0 = (*fix.container.begin())[0];
        int max_value_1 = (*fix.container.begin())[1];
        for(typename Tp::container_type::iterator
              i = fix.container.begin(); i != fix.container.end(); ++i)
          {
            int tmp = (*i)[0];
            if (tmp > max_value_0) { max_value_0 = tmp; }
            tmp = (*i)[1];
            if (tmp > max_value_1) { max_value_1 = tmp; }
            ++count;
          }
        BOOST_CHECK_EQUAL(count, fix.container.size());
        mapping_iterator<typename Tp::container_type> iter;
        iter = mapping_end(fix.container, 0);
        --iter; // When at the end, this call the 'maximum' function
        BOOST_REQUIRE_EQUAL((*iter)[0], max_value_0);
        iter = mapping_end(fix.container, 1); --iter;
        BOOST_REQUIRE_EQUAL((*iter)[1], max_value_1);
        fix.container.erase(iter);
      }
  }
  { // A tree where all elements are the same!
    Tp fix(100, same());
    // Prove that you can find the max value with N nodes, down to 1 nodes
    while (!fix.container.empty())
      {
        unsigned int count = 0;
        for(typename Tp::container_type::iterator
              i = fix.container.begin(); i != fix.container.end(); ++i)
          { ++count; }
        BOOST_CHECK_EQUAL(count, fix.container.size());
        mapping_iterator<typename Tp::container_type> iter;
        iter = mapping_end(fix.container, 0); --iter;
        BOOST_CHECK_EQUAL((*iter)[0], 100);
        iter = mapping_end(fix.container, 1); --iter;
        BOOST_CHECK_EQUAL((*iter)[1], 100);
        fix.container.erase(iter);
      }
  }
  { // test at the limit: a tree with 1 element
    Tp fix(1, same());
    mapping_iterator<const typename Tp::container_type> iter;
    iter = mapping_cend(fix.container, 0); --iter;
    BOOST_CHECK_EQUAL((*iter)[0], 1); // should be (1, 1);
    BOOST_CHECK_EQUAL((*iter)[1], 1);
    iter = mapping_cend(fix.container, 1); --iter;
    BOOST_CHECK_EQUAL((*iter)[0], 1); // should be (1, 1);
    BOOST_CHECK_EQUAL((*iter)[1], 1);
  }
  { // test at the limit: an unbalanced tree!
    Tp fix(100, decrease());
    mapping_iterator<typename Tp::container_type> iter;
    iter = mapping_end(fix.container, 0); --iter;
    BOOST_CHECK_EQUAL((*iter)[0], 100); // should be (100, 100);
    BOOST_CHECK_EQUAL((*iter)[1], 100);
  }
  { // test at the limit: an unbalanced tree!
    Tp fix(100, increase());
    mapping_iterator<typename Tp::container_type> iter;
    iter = mapping_end(fix.container, 1); --iter;
    BOOST_CHECK_EQUAL((*iter)[0], 99); // should be (99, 99);
    BOOST_CHECK_EQUAL((*iter)[1], 99);
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_mapping_increment, Tp, double6_sets )
{
  { // test the invarient of the increment
    Tp fix(100, randomize(-1, 1));
    for (dimension_type mapping_dim = 0; mapping_dim < 6;
         ++mapping_dim)
      {
        mapping_iterator<typename Tp::container_type>
          iter = mapping_begin(fix.container, mapping_dim),
          end = mapping_end(fix.container, mapping_dim);
        int count = 0;
        double tmp = (*iter)[mapping_dim];
        for (; iter != end; ++iter)
          {
            BOOST_CHECK_LE(tmp, (*iter)[mapping_dim]);
            tmp = (*iter)[mapping_dim];
            if (++count > 100) break;
          }
        BOOST_CHECK_EQUAL(count, 100);
      }
  }
  { // test at the limit: a tree where all elements are the same
    Tp fix(100, same());
    for (dimension_type mapping_dim = 0; mapping_dim < 6;
         ++mapping_dim)
      {
        mapping_iterator<typename Tp::container_type>
          iter = mapping_begin(fix.container, mapping_dim),
          end = mapping_end(fix.container, mapping_dim);
        int count = 0;
        for (; iter != end; ++iter)
          {
            BOOST_CHECK_EQUAL(100, (*iter)[mapping_dim]);
            if (++count > 100) break;
          }
        BOOST_CHECK_EQUAL(count, 100);
      }
  }
  { // test at the limit: a tree with 2 elements
    Tp fix(2, same());
    for (dimension_type mapping_dim = 0; mapping_dim < 6;
         ++mapping_dim)
      {
        mapping_iterator<const typename Tp::container_type>
          pre = mapping_cbegin(fix.container, mapping_dim),
          post = mapping_cbegin(fix.container, mapping_dim),
          end = mapping_cend(fix.container, mapping_dim);
        BOOST_CHECK(pre != end);
        BOOST_CHECK(++pre != post++);
        BOOST_CHECK(pre == post);
        BOOST_CHECK(post++ != end);
        BOOST_CHECK(++pre == end);
        BOOST_CHECK(post == end);
      }
  }
  { // test at the limit: a right-unbalanced tree (pre-increment)
    Tp fix(100, increase());
    for (dimension_type mapping_dim = 0; mapping_dim < 6;
         ++mapping_dim)
      {
        mapping_iterator<typename Tp::container_type>
          iter = mapping_begin(fix.container, mapping_dim),
          end = mapping_end(fix.container, mapping_dim);
        int count = 0;
        double tmp = (*iter)[mapping_dim];
        for (; iter != end; ++iter)
          {
            BOOST_CHECK_LE(tmp, (*iter)[mapping_dim]);
            tmp = (*iter)[mapping_dim];
            if (++count > 100) break;
          }
        BOOST_CHECK_EQUAL(count, 100);
      }
  }
  { // test at the limit: a left-unbalanced tree (post-increment)
    Tp fix(100, decrease());
    for (dimension_type mapping_dim = 0; mapping_dim < 6;
         ++mapping_dim)
      {
        mapping_iterator<typename Tp::container_type>
          iter = mapping_begin(fix.container, mapping_dim),
          end = mapping_end(fix.container, mapping_dim);
        int count = 0;
        double tmp = (*iter)[mapping_dim];
        for (; iter != end; iter++)
          {
            BOOST_CHECK_LE(tmp, (*iter)[mapping_dim]);
            tmp = (*iter)[mapping_dim];
            if (++count > 100) break;
          }
        BOOST_CHECK_EQUAL(count, 100);
      }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_mapping_decrement, Tp, double6_maps )
{
  { // test the invarient of the increment
    Tp fix(100, randomize(-1, 1));
    for (dimension_type mapping_dim = 0; mapping_dim < 6;
         ++mapping_dim)
      {
        std::reverse_iterator
          <mapping_iterator<typename Tp::container_type> >
          iter(mapping_end(fix.container, mapping_dim)),
          end(mapping_begin(fix.container, mapping_dim));
        int count = 0;
        double tmp = iter->first[mapping_dim];
        for (; iter != end; ++iter)
          {
            BOOST_CHECK_GE(tmp, iter->first[mapping_dim]);
            tmp = iter->first[mapping_dim];
            if (++count > 100) break;
          }
        BOOST_CHECK_EQUAL(count, 100);
      }
  }
  { // test at the limit: a tree where all elements are the same
    Tp fix(100, same());
    for (dimension_type mapping_dim = 0; mapping_dim < 6;
         ++mapping_dim)
      {
        std::reverse_iterator
          <mapping_iterator<typename Tp::container_type> >
          iter(mapping_end(fix.container, mapping_dim)),
          end(mapping_begin(fix.container, mapping_dim));
        int count = 0;
        for (; iter != end; ++iter)
          {
            BOOST_CHECK_EQUAL(100, iter->first[mapping_dim]);
            if (++count > 100) break;
          }
        BOOST_CHECK_EQUAL(count, 100);
      }
  }
  { // test at the limit: a tree with 2 elements
    Tp fix(2, same());
    for (dimension_type mapping_dim = 0; mapping_dim < 6;
         ++mapping_dim)
      {
        mapping_iterator<const typename Tp::container_type>
          pre = mapping_cend(fix.container, mapping_dim),
          post = mapping_cend(fix.container, mapping_dim),
          begin = mapping_cbegin(fix.container, mapping_dim);
        BOOST_CHECK(pre != begin);
        BOOST_CHECK(--pre != post--);
        BOOST_CHECK(pre == post);
        BOOST_CHECK(post-- != begin);
        BOOST_CHECK(--pre == begin);
        BOOST_CHECK(post == begin);
      }
  }
  { // test at the limit: a right-unbalanced tree (pre-increment)
    Tp fix(100, increase());
    for (dimension_type mapping_dim = 0; mapping_dim < 6;
         ++mapping_dim)
      {
        std::reverse_iterator
          <mapping_iterator<typename Tp::container_type> >
          iter(mapping_end(fix.container, mapping_dim)),
          end(mapping_begin(fix.container, mapping_dim));
        int count = 0;
        double tmp = iter->first[mapping_dim];
        for (; iter != end; ++iter)
          {
            BOOST_CHECK_GE(tmp, iter->first[mapping_dim]);
            tmp = iter->first[mapping_dim];
            if (++count > 100) break;
          }
        BOOST_CHECK_EQUAL(count, 100);
      }
  }
  { // test at the limit: a left-unbalanced tree (post-increment)
    Tp fix(100, decrease());
    for (dimension_type mapping_dim = 0; mapping_dim < 6;
         ++mapping_dim)
      {
        std::reverse_iterator
          <mapping_iterator<typename Tp::container_type> >
          iter(mapping_end(fix.container, mapping_dim)),
          end(mapping_begin(fix.container, mapping_dim));
        int count = 0;
        double tmp = iter->first[mapping_dim];
        for (; iter != end; iter++)
          {
            BOOST_CHECK_GE(tmp, iter->first[mapping_dim]);
            tmp = iter->first[mapping_dim];
            if (++count > 100) break;
          }
        BOOST_CHECK_EQUAL(count, 100);
      }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_mapping_lower_bound, Tp, quad_sets )
{
  { // find the smallest element that is greater or equal to key
    Tp fix(100, randomize(-2, 2));
    quad lower (-2, -2, -2, -2);
    quad in (1, 1, 1, 1);
    quad upper (2, 2, 2, 2);
    for (dimension_type mapping_dim = 0; mapping_dim < 4;
         ++mapping_dim)
      {
        mapping_iterator<typename Tp::container_type>
          iter (mapping_lower_bound(fix.container, mapping_dim, in));
        BOOST_CHECK(iter == mapping_end(fix.container, mapping_dim)
                    || !quad_less()(mapping_dim, *iter, in));
        BOOST_CHECK(iter == mapping_begin(fix.container, mapping_dim)
                    || quad_less()(mapping_dim, *--iter, in));
        iter = mapping_lower_bound(fix.container, mapping_dim, lower);
        BOOST_CHECK(iter == mapping_begin(fix.container, mapping_dim));
        iter = mapping_lower_bound(fix.container, mapping_dim, upper);
        BOOST_CHECK(iter == mapping_end(fix.container, mapping_dim));
      }
  }
  { // same test with a tree filled with similar values
    Tp fix(100, same());
    quad lower (99, 99, 99, 99);
    quad in (100, 100, 100, 100);
    quad upper (101, 101, 101, 101);
    for (dimension_type mapping_dim = 0; mapping_dim < 4;
         ++mapping_dim)
      {
        mapping_iterator<typename Tp::container_type>
          iter (mapping_lower_bound(fix.container, mapping_dim, lower));
        BOOST_CHECK(iter == mapping_begin(fix.container, mapping_dim));
        iter = mapping_lower_bound(fix.container, mapping_dim, in);
        BOOST_CHECK(iter == mapping_begin(fix.container, mapping_dim));
        iter = mapping_lower_bound(fix.container, mapping_dim, upper);
        BOOST_CHECK(iter == mapping_end(fix.container, mapping_dim));
      }
  }
  { // test at the limit: tree with 1 value
    Tp fix(1, same());
    quad lower (0, 0, 0, 0);
    quad in (1, 1, 1, 1);
    quad upper (2, 2, 2, 2);
    for (dimension_type mapping_dim = 0; mapping_dim < 4;
         ++mapping_dim)
      {
        mapping_iterator<const typename Tp::container_type>
          iter (mapping_clower_bound(fix.container, mapping_dim, lower));
        BOOST_CHECK(iter == mapping_cbegin(fix.container, mapping_dim));
        iter = mapping_clower_bound(fix.container, mapping_dim, in);
        BOOST_CHECK(iter == mapping_cbegin(fix.container, mapping_dim));
        iter = mapping_clower_bound(fix.container, mapping_dim, upper);
        BOOST_CHECK(iter == mapping_cend(fix.container, mapping_dim));
      }
  }
  { // test at the limit: tree filled with decreasing values
    Tp fix(100, decrease()); // first (100, 100, 100, 100), last (1, 1, 1, 1)
    quad lower(1, 1, 1, 1);
    quad in (100, 100, 100, 100);
    quad upper(101, 101, 101, 101);
    for (dimension_type mapping_dim = 0; mapping_dim < 4;
         ++mapping_dim)
      {
        mapping_iterator<typename Tp::container_type>
          iter (mapping_lower_bound(fix.container, mapping_dim, lower));
        BOOST_CHECK(iter == mapping_begin(fix.container, mapping_dim));
        iter = mapping_lower_bound(fix.container, mapping_dim, in);
        BOOST_CHECK(iter != mapping_end(fix.container, mapping_dim)
                    && ++iter == mapping_end(fix.container, mapping_dim));
        iter = mapping_lower_bound(fix.container, mapping_dim, upper);
        BOOST_CHECK(iter == mapping_end(fix.container, mapping_dim));
      }
  }
  { // test at the limit: tree filled with increasing values
    Tp fix(100, increase()); // first (0, 0, 0, 0), last (99, 99, 99, 99)
    quad lower(0, 0, 0, 0);
    quad in(99, 99, 99, 99);
    quad upper (100, 100, 100, 100);
    for (dimension_type mapping_dim = 0; mapping_dim < 4;
         ++mapping_dim)
      {
        mapping_iterator<typename Tp::container_type>
          iter (mapping_lower_bound(fix.container, mapping_dim, lower));
        BOOST_CHECK(iter == mapping_begin(fix.container, mapping_dim));
        iter = mapping_lower_bound(fix.container, mapping_dim, in);
        BOOST_CHECK(iter != mapping_end(fix.container, mapping_dim)
                    && ++iter == mapping_end(fix.container, mapping_dim));
        iter = mapping_lower_bound(fix.container, mapping_dim, upper);
        BOOST_CHECK(iter == mapping_end(fix.container, mapping_dim));
      }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_mapping_upper_bound, Tp, quad_maps )
{
  { // find the smallest element that is greater than key
    Tp fix(100, randomize(-2, 2));
    quad lower (-3, -3, -3, -3);
    quad in (-1, -1, -1, -1);
    quad upper (1, 1, 1, 1);
    for (dimension_type mapping_dim = 0; mapping_dim < 4;
         ++mapping_dim)
      {
        mapping_iterator<typename Tp::container_type>
          iter (mapping_upper_bound(fix.container, mapping_dim, in));
        BOOST_CHECK(iter == mapping_end(fix.container, mapping_dim)
                    || quad_less()(mapping_dim, in, iter->first));
        BOOST_CHECK(iter == mapping_begin(fix.container, mapping_dim)
                    || !quad_less()(mapping_dim, (--iter)->first, in));
        iter = mapping_upper_bound(fix.container, mapping_dim, lower);
        BOOST_CHECK(iter == mapping_begin(fix.container, mapping_dim));
        iter = mapping_upper_bound(fix.container, mapping_dim, upper);
        BOOST_CHECK(iter == mapping_end(fix.container, mapping_dim));
      }
  }
  { // same test with a tree filled with similar values
    Tp fix(100, same());
    quad lower (99, 99, 99, 99);
    quad in (100, 100, 100, 100);
    quad upper (101, 101, 101, 101);
    for (dimension_type mapping_dim = 0; mapping_dim < 4;
         ++mapping_dim)
      {
        mapping_iterator<typename Tp::container_type>
          iter (mapping_upper_bound(fix.container, mapping_dim, lower));
        BOOST_CHECK(iter == mapping_begin(fix.container, mapping_dim));
        iter = mapping_upper_bound(fix.container, mapping_dim, in);
        BOOST_CHECK(iter == mapping_end(fix.container, mapping_dim));
        iter = mapping_upper_bound(fix.container, mapping_dim, upper);
        BOOST_CHECK(iter == mapping_end(fix.container, mapping_dim));
      }
  }
  { // test at the limit: tree with 1 value
    Tp fix(1, same());
    quad lower (0, 0, 0, 0);
    quad in (1, 1, 1, 1);
    quad upper (2, 2, 2, 2);
    for (dimension_type mapping_dim = 0; mapping_dim < 4;
         ++mapping_dim)
      {
        mapping_iterator<const typename Tp::container_type>
          iter (mapping_cupper_bound(fix.container, mapping_dim, lower));
        BOOST_CHECK(iter == mapping_cbegin(fix.container, mapping_dim));
        iter = mapping_cupper_bound(fix.container, mapping_dim, in);
        BOOST_CHECK(iter == mapping_cend(fix.container, mapping_dim));
        iter = mapping_cupper_bound(fix.container, mapping_dim, upper);
        BOOST_CHECK(iter == mapping_cend(fix.container, mapping_dim));
      }
  }
  { // test at the limit: tree filled with decreasing values
    Tp fix(100, decrease()); // first (100, 100, 100, 100), last (1, 1, 1, 1)
    quad lower(0, 0, 0, 0);
    quad in (99, 99, 99, 99);
    quad upper(100, 100, 100, 100);
    for (dimension_type mapping_dim = 0; mapping_dim < 4;
         ++mapping_dim)
      {
        mapping_iterator<typename Tp::container_type>
          iter (mapping_upper_bound(fix.container, mapping_dim, lower));
        BOOST_CHECK(iter == mapping_begin(fix.container, mapping_dim));
        iter = mapping_upper_bound(fix.container, mapping_dim, in);
        BOOST_CHECK(iter != mapping_end(fix.container, mapping_dim)
                    && ++iter == mapping_end(fix.container, mapping_dim));
        iter = mapping_upper_bound(fix.container, mapping_dim, upper);
        BOOST_CHECK(iter == mapping_end(fix.container, mapping_dim));
      }
  }
  { // test at the limit: tree filled with increasing values
    Tp fix(100, increase()); // first (0, 0, 0, 0), last (99, 99, 99, 99)
    quad lower(-1, -1, -1, -1);
    quad in(98, 98, 98, 98);
    quad upper (99, 99, 99, 99);
    for (dimension_type mapping_dim = 0; mapping_dim < 4;
         ++mapping_dim)
      {
        mapping_iterator<typename Tp::container_type>
          iter (mapping_upper_bound(fix.container, mapping_dim, lower));
        BOOST_CHECK(iter == mapping_begin(fix.container, mapping_dim));
        iter = mapping_upper_bound(fix.container, mapping_dim, in);
        BOOST_CHECK(iter != mapping_end(fix.container, mapping_dim)
                    && ++iter == mapping_end(fix.container, mapping_dim));
        iter = mapping_upper_bound(fix.container, mapping_dim, upper);
        BOOST_CHECK(iter == mapping_end(fix.container, mapping_dim));
      }
  }
}

BOOST_AUTO_TEST_CASE( test_mapping_dimension )
{
  { // with mutable iterator
    pointset_fix<double6> fix;
    mapping_iterator<pointset_fix<double6>::container_type> iter;
    iter = mapping_begin(fix.container, 5u);
    BOOST_CHECK_EQUAL(mapping_dimension(iter), 5u);
    mapping_dimension(iter, 3u);
    BOOST_CHECK_EQUAL(mapping_dimension(iter), 3u);
    mapping_iterator<const pointset_fix<double6>
                     ::container_type> citer;
    citer = mapping_cbegin(fix.container, 5u);
    BOOST_CHECK_EQUAL(mapping_dimension(citer), 5u);
    mapping_dimension(citer, 3u);
    BOOST_CHECK_EQUAL(mapping_dimension(citer), 3u);
  }
  { // Check invalid dimension exception
    pointset_fix<double6> fix;
    mapping_iterator<pointset_fix<double6>::container_type>
      iter(mapping_begin(fix.container, 5u));
    BOOST_CHECK_THROW(mapping_dimension(iter, 6u), invalid_dimension);
    BOOST_CHECK_THROW(mapping_begin(fix.container, 6u),
                      invalid_dimension);
    BOOST_CHECK_THROW(mapping_end(fix.container, 6u),
                      invalid_dimension);
    BOOST_CHECK_THROW(mapping_lower_bound(fix.container, 6u, double6()),
                      invalid_dimension);
    BOOST_CHECK_THROW(mapping_upper_bound(fix.container, 6u, double6()),
                      invalid_dimension);
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_mapping_range, Tp, every_quad )
{
  Tp fix(20, randomize(-100, 100));
  { // non-const
    mapping_iterator_pair<typename Tp::container_type>
      pair(mapping_range(fix.container, 2u));
    BOOST_CHECK(pair.first ==  mapping_begin(fix.container, 2u));
    BOOST_CHECK(pair.second == mapping_end(fix.container, 2u));
    mapping_iterator_pair<typename Tp::container_type> pair2;
    pair2 = mapping_range(fix.container, 3u);
    BOOST_CHECK(pair2.first ==  mapping_begin(fix.container, 3u));
    BOOST_CHECK(pair2.second == mapping_end(fix.container, 3u));
  }
  { // const
    mapping_iterator_pair<const typename Tp::container_type>
      pair0(mapping_range(fix.container, 1u)); // cast constructor
    BOOST_CHECK(pair0.first ==  mapping_begin(fix.container, 1u));
    BOOST_CHECK(pair0.second == mapping_end(fix.container, 1u));
    mapping_iterator_pair<const typename Tp::container_type>
      pair1(mapping_crange(fix.container, 2u)); // copy constructor
    BOOST_CHECK(pair1.first ==  mapping_begin(fix.container, 2u));
    BOOST_CHECK(pair1.second == mapping_end(fix.container, 2u));
    mapping_iterator_pair<const typename Tp::container_type> pair2;
    pair2 = mapping_crange(fix.container, 3u); // assignment operator
    BOOST_CHECK(pair2.first ==  mapping_cbegin(fix.container, 3u));
    BOOST_CHECK(pair2.second == mapping_cend(fix.container, 3u));
  }
}

#endif // SPATIAL_TEST_MAPPING_HPP
