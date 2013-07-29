// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_test_ordered.hpp
 *  All tests for the elements defined in spatial_ordered.hpp are
 *  located in this file.
 *  \see spatial_ordered.hpp
 */

#ifndef SPATIAL_TEST_ORDERED_HPP
#define SPATIAL_TEST_ORDERED_HPP

#include "../../src/ordered_iterator.hpp"

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_ordered_basics, Tp, every_quad )
{
  Tp fix(0);
  ordered_iterator<typename Tp::container_type> a, b(a);
  ordered_iterator<const typename Tp::container_type> c;
  c = a;
  ordered_iterator_pair<typename Tp::container_type> p, q(a, b);
  ordered_iterator_pair<const typename Tp::container_type>
    r, s(c, c), t(p);
  BOOST_CHECK(a == b);
  BOOST_CHECK(!(a != c));
  typename Tp::container_type::iterator i = a;
  BOOST_CHECK(i == a);
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_ordered_dereference, Tp, quad_maps )
{
  Tp fix(1, same());
  ordered_iterator<typename Tp::container_type>
    a(fix.container, fix.container.begin());
  ordered_iterator<const typename Tp::container_type>
    b(fix.container, fix.container.begin());
  BOOST_CHECK((*a).first == fix.container.begin()->first);
  (*a).second = "some string";
  BOOST_CHECK(a->first == fix.container.begin()->first);
  a->second = "some other string";
  BOOST_CHECK((*b).first == fix.container.begin()->first);
  BOOST_CHECK(b->first == fix.container.begin()->first);
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_ordered_minimum, Tp, int2_sets )
{
  {
    Tp fix(100, randomize(-20, 20));
    // Prove that you can find the min value with N nodes, down to 1 nodes
    while (!fix.container.empty())
      {
        unsigned int count = 0;
        int2 min_value = *fix.container.begin();
        for(typename Tp::container_type::iterator
              i = fix.container.begin(); i != fix.container.end(); ++i)
          {
            // Follow the same ordering invarient as ordered_iterator
            if ((*i)[0] < min_value[0]
                || ((*i)[0] == min_value[0] && (*i)[1] < min_value[1]))
              min_value = *i;
            ++count;
          }
        BOOST_CHECK_EQUAL(count, fix.container.size());
        ordered_iterator<typename Tp::container_type> iter;
        iter = ordered_begin(fix.container);
        BOOST_CHECK(*iter == min_value);
        fix.container.erase(iter);
      }
  }
  { // A tree where all elements are the same!
    Tp fix(100, same());
    // Prove that you can find the min value with N nodes, down to 1 nodes
    while (!fix.container.empty())
      {
        ordered_iterator<typename Tp::container_type>
          iter(ordered_begin(fix.container));
        BOOST_CHECK(*iter == int2(100, 100));
        fix.container.erase(iter);
      }
  }
  { // test at the limit: a tree with 1 element
    Tp fix(1, same());
    ordered_iterator<const typename Tp::container_type> iter;
    iter = ordered_cbegin(fix.container);
    BOOST_CHECK(*iter == int2(1, 1));
  }
  { // test at the limit: an unbalanced tree (i.e. insertions in order)!
    Tp fix(100, decrease());
    ordered_iterator<const typename Tp::container_type>
      iter(ordered_cbegin(fix.container));
    BOOST_CHECK(*iter == int2(1, 1));
  }
  { // test at the limit: an unbalanced tree (i.e insertions in order)!
    Tp fix(100, increase());
    ordered_iterator<const typename Tp::container_type>
      iter(ordered_cbegin(fix.container));
    BOOST_CHECK(*iter == int2(0, 0));
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_ordered_maximum, Tp, int2_sets )
{
  {
    Tp fix(100, randomize(-20, 20));
    // Prove that you can find the max value with N nodes, down to 1 nodes
    while (!fix.container.empty())
      {
        unsigned int count = 0;
        int2 max_value = *fix.container.begin();
        for(typename Tp::container_type::iterator
              i = fix.container.begin(); i != fix.container.end(); ++i)
          {
            // Follow the same ordering invarient as ordered_iterator
            if ((*i)[0] > max_value[0]
                || ((*i)[0] == max_value[0] && (*i)[1] > max_value[1]))
              max_value = *i;
            ++count;
          }
        BOOST_CHECK_EQUAL(count, fix.container.size());
        ordered_iterator<typename Tp::container_type> iter;
        iter = ordered_end(fix.container);
        --iter; // When at the end, this call the 'maximum' function
        BOOST_REQUIRE_EQUAL(*iter, max_value);
        ordered_iterator<typename Tp::container_type> tmp(iter);
        BOOST_CHECK(++tmp == ordered_end(fix.container));
        fix.container.erase(iter);
      }
  }
  { // A tree where all elements are the same!
    Tp fix(100, same());
    // Prove that you can find the max value with N nodes, down to 1 nodes
    while (!fix.container.empty())
      {
        ordered_iterator<typename Tp::container_type>
          iter(ordered_end(fix.container)); --iter;
        BOOST_CHECK(*iter == int2(100, 100));
        ordered_iterator<typename Tp::container_type> tmp(iter);
        BOOST_CHECK(++tmp == ordered_end(fix.container));
        fix.container.erase(iter);
      }
  }
  { // test at the limit: a tree with 1 element
    Tp fix(1, same());
    ordered_iterator<const typename Tp::container_type> iter;
    iter = ordered_cend(fix.container); --iter;
    BOOST_CHECK(*iter == int2(1, 1));
    BOOST_CHECK(++iter == ordered_end(fix.container));
  }
  { // test at the limit: an unbalanced tree!
    Tp fix(100, decrease());
    ordered_iterator<const typename Tp::container_type>
      iter(ordered_cend(fix.container)); --iter;
    BOOST_CHECK(*iter == int2(100, 100));
    BOOST_CHECK(++iter == ordered_end(fix.container));
  }
  { // test at the limit: an unbalanced tree!
    Tp fix(100, increase());
    ordered_iterator<const typename Tp::container_type>
      iter(ordered_cend(fix.container)); --iter;
    BOOST_CHECK(*iter == int2(99, 99));
    BOOST_CHECK(++iter == ordered_end(fix.container));
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_ordered_increment, Tp, double6_sets )
{
  { // test the invarient of the increment
    Tp fix(100, randomize(-1, 1));
    ordered_iterator<typename Tp::container_type>
      iter = ordered_begin(fix.container),
      end = ordered_end(fix.container);
    int count = 0;
    double6 tmp = *iter;
    for (; iter != end; ++iter)
      {
        BOOST_CHECK(!double6_ordered_less()(*iter, tmp));
        tmp = *iter;
        if (++count > 100) break;
      }
    BOOST_CHECK_EQUAL(count, 100);
  }
  { // test at the limit: a tree where all elements are the same
    Tp fix(100, same());
    ordered_iterator<typename Tp::container_type>
      iter = ordered_begin(fix.container),
      end = ordered_end(fix.container);
    int count = 0;
    double6 expected; expected.assign(100.0);
    for (; iter != end; ++iter)
      {
        BOOST_CHECK(*iter == expected);
        if (++count > 100) break;
      }
    BOOST_CHECK_EQUAL(count, 100);
  }
  { // test at the limit: a tree with 2 elements
    Tp fix(2, same());
    ordered_iterator<const typename Tp::container_type>
      pre = ordered_cbegin(fix.container),
      post = ordered_cbegin(fix.container),
      end = ordered_cend(fix.container);
    BOOST_CHECK(pre != end);
    BOOST_CHECK(++pre != post++);
    BOOST_CHECK(pre == post);
    BOOST_CHECK(post++ != end);
    BOOST_CHECK(++pre == end);
    BOOST_CHECK(post == end);
  }
  { // test at the limit: a right-unbalanced tree (pre-increment)
    Tp fix(100, increase());
    ordered_iterator<const typename Tp::container_type>
      iter(ordered_cbegin(fix.container)),
      end(ordered_cend(fix.container));
    int count = 1;
    double6 tmp = *(iter++);
    for (; iter != end; ++iter)
      {
        BOOST_CHECK(double6_ordered_less()(tmp, *iter));
        tmp = *iter;
        if (++count > 100) break;
      }
    BOOST_CHECK_EQUAL(count, 100);
  }
  { // test at the limit: a left-unbalanced tree (post-increment)
    Tp fix(100, decrease());
    ordered_iterator<const typename Tp::container_type>
      iter(ordered_begin(fix.container)),
      end(ordered_end(fix.container));
    int count = 1;
    double6 tmp = *(iter++);
    for (; iter != end; ++iter)
      {
        BOOST_CHECK(double6_ordered_less()(tmp, *iter));
        tmp = *iter;
        if (++count > 100) break;
      }
    BOOST_CHECK_EQUAL(count, 100);
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_ordered_decrement, Tp, double6_sets )
{
  { // test the invarient of the increment
    Tp fix(100, randomize(-1, 1));
    std::reverse_iterator<ordered_iterator<typename Tp::container_type> >
      iter(ordered_end(fix.container)),
      end(ordered_begin(fix.container));
    int count = 0;
    double6 tmp = *iter;
    for (; iter != end; ++iter)
      {
        BOOST_CHECK(!double6_ordered_less()(tmp, *iter));
        tmp = *iter;
        if (++count > 100) break;
      }
    BOOST_CHECK_EQUAL(count, 100);
  }
  { // test at the limit: a tree where all elements are the same
    Tp fix(100, same());
    std::reverse_iterator<ordered_iterator<typename Tp::container_type> >
      iter(ordered_end(fix.container)),
      end(ordered_begin(fix.container));
    int count = 0;
    double6 expected; expected.assign(100.0);
    for (; iter != end; ++iter)
      {
        BOOST_CHECK(*iter == expected);
        if (++count > 100) break;
      }
    BOOST_CHECK_EQUAL(count, 100);
  }
  { // test at the limit: a tree with 2 elements
    Tp fix(2, same());
    ordered_iterator<const typename Tp::container_type>
      pre = ordered_cend(fix.container),
      post = ordered_cend(fix.container),
      end = ordered_cbegin(fix.container);
    BOOST_CHECK(pre != end);
    BOOST_CHECK(--pre != post--);
    BOOST_CHECK(pre == post);
    BOOST_CHECK(post-- != end);
    BOOST_CHECK(--pre == end);
    BOOST_CHECK(post == end);
  }
  { // test at the limit: a right-unbalanced tree (pre-increment)
    Tp fix(100, increase());
    std::reverse_iterator<ordered_iterator<const typename Tp::container_type> >
      iter(ordered_cend(fix.container)),
      end(ordered_cbegin(fix.container));
    int count = 1;
    double6 tmp = *(iter++);
    for (; iter != end; ++iter)
      {
        BOOST_CHECK(double6_ordered_less()(*iter, tmp));
        tmp = *iter;
        if (++count > 100) break;
      }
    BOOST_CHECK_EQUAL(count, 100);
  }
  { // test at the limit: a left-unbalanced tree (post-increment)
    Tp fix(100, decrease());
    std::reverse_iterator<ordered_iterator<const typename Tp::container_type> >
      iter(ordered_end(fix.container)),
      end(ordered_begin(fix.container));
    int count = 1;
    double6 tmp = *(iter++);
    for (; iter != end; ++iter)
      {
        BOOST_CHECK(double6_ordered_less()(*iter, tmp));
        tmp = *iter;
        if (++count > 100) break;
      }
    BOOST_CHECK_EQUAL(count, 100);
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_ordered_lower_bound, Tp, quad_maps )
{
  { // find the smallest element that is greater or equal to key
    Tp fix(100, randomize(-2, 2));
    quad lower (-2, -2, -2, -2);
    quad in (1, 1, 1, 1);
    quad upper (2, 2, 2, 2);
    ordered_iterator<typename Tp::container_type>
      iter (ordered_lower_bound(fix.container, in));
    BOOST_CHECK(iter == ordered_end(fix.container)
                || !quad_ordered_less()(iter->first, in));
    BOOST_CHECK(iter == ordered_begin(fix.container)
                || quad_ordered_less()((--iter)->first, in));
    iter = ordered_lower_bound(fix.container, lower);
    BOOST_CHECK(iter == ordered_begin(fix.container));
    iter = ordered_lower_bound(fix.container, upper);
    BOOST_CHECK(iter == ordered_end(fix.container));
  }
  { // same test with a tree filled with similar values
    Tp fix(100, same());
    quad lower (99, 99, 99, 99);
    quad in (100, 100, 100, 100);
    quad upper (101, 101, 101, 101);
    ordered_iterator<typename Tp::container_type>
      iter (ordered_lower_bound(fix.container, lower));
    BOOST_CHECK(iter == ordered_begin(fix.container));
    iter = ordered_lower_bound(fix.container, in);
    BOOST_CHECK(iter == ordered_begin(fix.container));
    iter = ordered_lower_bound(fix.container, upper);
    BOOST_CHECK(iter == ordered_end(fix.container));
  }
  { // test at the limit: tree with 1 value
    Tp fix(1, same());
    quad lower (0, 0, 0, 0);
    quad in (1, 1, 1, 1);
    quad upper (2, 2, 2, 2);
    ordered_iterator<const typename Tp::container_type>
      iter (ordered_clower_bound(fix.container, lower));
    BOOST_CHECK(iter == ordered_cbegin(fix.container));
    iter = ordered_clower_bound(fix.container, in);
    BOOST_CHECK(iter == ordered_cbegin(fix.container));
    iter = ordered_clower_bound(fix.container, upper);
    BOOST_CHECK(iter == ordered_cend(fix.container));
  }
  { // test at the limit: tree filled with decreasing values
    Tp fix(100, decrease()); // first (100, 100, 100, 100), last (1, 1, 1, 1)
    quad lower(1, 1, 1, 1);
    quad in (100, 100, 100, 100);
    quad upper(101, 101, 101, 101);
    ordered_iterator<typename Tp::container_type>
      iter (ordered_lower_bound(fix.container, lower));
    BOOST_CHECK(iter == ordered_begin(fix.container));
    iter = ordered_lower_bound(fix.container, in);
    BOOST_CHECK(iter != ordered_end(fix.container)
                && ++iter == ordered_end(fix.container));
    iter = ordered_lower_bound(fix.container, upper);
    BOOST_CHECK(iter == ordered_end(fix.container));
  }
  { // test at the limit: tree filled with increasing values
    Tp fix(100, increase()); // first (0, 0, 0, 0), last (99, 99, 99, 99)
    quad lower(0, 0, 0, 0);
    quad in(99, 99, 99, 99);
    quad upper (100, 100, 100, 100);
    ordered_iterator<typename Tp::container_type>
      iter (ordered_lower_bound(fix.container, lower));
    BOOST_CHECK(iter == ordered_begin(fix.container));
    iter = ordered_lower_bound(fix.container, in);
    BOOST_CHECK(iter != ordered_end(fix.container)
                && ++iter == ordered_end(fix.container));
    iter = ordered_lower_bound(fix.container, upper);
    BOOST_CHECK(iter == ordered_end(fix.container));
  }
}


BOOST_AUTO_TEST_CASE_TEMPLATE
( test_ordered_upper_bound, Tp, quad_maps )
{
  { // find the smallest element that is greater than key
    Tp fix(100, randomize(-2, 2));
    quad lower (-3, -3, -3, -3);
    quad in (-1, -1, -1, -1);
    quad upper (1, 1, 1, 1);
    ordered_iterator<typename Tp::container_type>
      iter (ordered_upper_bound(fix.container, in));
    BOOST_CHECK(iter == ordered_end(fix.container)
                || quad_ordered_less()(in, iter->first));
    BOOST_CHECK(iter == ordered_begin(fix.container)
                || !quad_ordered_less()((--iter)->first, in));
    iter = ordered_upper_bound(fix.container, lower);
    BOOST_CHECK(iter == ordered_begin(fix.container));
    iter = ordered_upper_bound(fix.container, upper);
    BOOST_CHECK(iter == ordered_end(fix.container));
  }
  { // same test with a tree filled with similar values
    Tp fix(100, same());
    quad lower (99, 99, 99, 99);
    quad in (100, 100, 100, 100);
    quad upper (101, 101, 101, 101);
    ordered_iterator<typename Tp::container_type>
      iter (ordered_upper_bound(fix.container, lower));
    BOOST_CHECK(iter == ordered_begin(fix.container));
    iter = ordered_upper_bound(fix.container, in);
    BOOST_CHECK(iter == ordered_end(fix.container));
    iter = ordered_upper_bound(fix.container, upper);
    BOOST_CHECK(iter == ordered_end(fix.container));
  }
  { // test at the limit: tree with 1 value
    Tp fix(1, same());
    quad lower (0, 0, 0, 0);
    quad in (1, 1, 1, 1);
    quad upper (2, 2, 2, 2);
    ordered_iterator<const typename Tp::container_type>
      iter (ordered_cupper_bound(fix.container, lower));
    BOOST_CHECK(iter == ordered_cbegin(fix.container));
    iter = ordered_cupper_bound(fix.container, in);
    BOOST_CHECK(iter == ordered_cend(fix.container));
    iter = ordered_cupper_bound(fix.container, upper);
    BOOST_CHECK(iter == ordered_cend(fix.container));
  }
  { // test at the limit: tree filled with decreasing values
    Tp fix(100, decrease()); // first (100, 100, 100, 100), last (1, 1, 1, 1)
    quad lower(0, 0, 0, 0);
    quad in (99, 99, 99, 99);
    quad upper(100, 100, 100, 100);
    ordered_iterator<typename Tp::container_type>
      iter (ordered_upper_bound(fix.container, lower));
    BOOST_CHECK(iter == ordered_begin(fix.container));
    iter = ordered_upper_bound(fix.container, in);
    BOOST_CHECK(iter != ordered_end(fix.container)
                && ++iter == ordered_end(fix.container));
    iter = ordered_upper_bound(fix.container, upper);
    BOOST_CHECK(iter == ordered_end(fix.container));
  }
  { // test at the limit: tree filled with increasing values
    Tp fix(100, increase()); // first (0, 0, 0, 0), last (99, 99, 99, 99)
    quad lower(-1, -1, -1, -1);
    quad in(98, 98, 98, 98);
    quad upper (99, 99, 99, 99);
    ordered_iterator<typename Tp::container_type>
      iter (ordered_upper_bound(fix.container, lower));
    BOOST_CHECK(iter == ordered_begin(fix.container));
    iter = ordered_upper_bound(fix.container, in);
    BOOST_CHECK(iter != ordered_end(fix.container)
                && ++iter == ordered_end(fix.container));
    iter = ordered_upper_bound(fix.container, upper);
    BOOST_CHECK(iter == ordered_end(fix.container));
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_ordered_range, Tp, every_quad )
{
  Tp fix(20, randomize(-100, 100));
  { // non-const
    ordered_iterator_pair<typename Tp::container_type>
      pair(ordered_range(fix.container));
    BOOST_CHECK(pair.first ==  ordered_begin(fix.container));
    BOOST_CHECK(pair.second == ordered_end(fix.container));
    ordered_iterator_pair<typename Tp::container_type> pair2;
    pair2 = ordered_range(fix.container);
    BOOST_CHECK(pair2.first ==  ordered_begin(fix.container));
    BOOST_CHECK(pair2.second == ordered_end(fix.container));
  }
  { // const
    ordered_iterator_pair<const typename Tp::container_type>
      pair0(ordered_range(fix.container)); // cast constructor
    BOOST_CHECK(pair0.first ==  ordered_begin(fix.container));
    BOOST_CHECK(pair0.second == ordered_end(fix.container));
    ordered_iterator_pair<const typename Tp::container_type>
      pair1(ordered_crange(fix.container)); // copy constructor
    BOOST_CHECK(pair1.first ==  ordered_begin(fix.container));
    BOOST_CHECK(pair1.second == ordered_end(fix.container));
    ordered_iterator_pair<const typename Tp::container_type> pair2;
    pair2 = ordered_crange(fix.container); // assignment operator
    BOOST_CHECK(pair2.first ==  ordered_cbegin(fix.container));
    BOOST_CHECK(pair2.second == ordered_cend(fix.container));
  }
}

#endif // SPATIAL_TEST_ORDERED_HPP
