// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_test_ordered.hpp
 *  @brief  All tests for the elements defined in spatial_ordered.hpp are
 *  located in this file.
 *  @see spatial_ordered.hpp
 */

#ifndef SPATIAL_TEST_ORDERED_HPP
#define SPATIAL_TEST_ORDERED_HPP

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
        ordered_iterator<typename Tp::container_type> tmp(iter);
        BOOST_CHECK(--tmp == ordered_end(fix.container));
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
        ordered_iterator<typename Tp::container_type> tmp(iter);
        BOOST_CHECK(--tmp == ordered_end(fix.container));
        fix.container.erase(iter);
      }
  }
  { // test at the limit: a tree with 1 element
    Tp fix(1, same());
    ordered_iterator<const typename Tp::container_type> iter;
    iter = ordered_cbegin(fix.container);
    BOOST_CHECK(*iter == int2(1, 1));
    BOOST_CHECK(--iter == ordered_end(fix.container));
  }
  { // test at the limit: an unbalanced tree (i.e. insertions in order)!
    Tp fix(100, decrease());
    ordered_iterator<const typename Tp::container_type>
      iter(ordered_cbegin(fix.container));
    BOOST_CHECK(*iter == int2(1, 1));
    BOOST_CHECK(--iter == ordered_end(fix.container));
  }
  { // test at the limit: an unbalanced tree (i.e insertions in order)!
    Tp fix(100, increase());
    ordered_iterator<const typename Tp::container_type>
      iter(ordered_cbegin(fix.container));
    BOOST_CHECK(*iter == int2(0, 0));
    BOOST_CHECK(--iter == ordered_end(fix.container));
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
/*
BOOST_AUTO_TEST_CASE_TEMPLATE
( test_ordered_increment, Tp, double6_sets )
{
  { // test the invarient of the increment
    Tp fix(100, randomize(-1, 1));
    for (dimension_type ordered_dim = 0; ordered_dim < 6;
         ++ordered_dim)
      {
        ordered_iterator<typename Tp::container_type>
          iter = ordered_begin(fix.container, ordered_dim),
          end = ordered_end(fix.container, ordered_dim);
        int count = 0;
        double tmp = (*iter)[ordered_dim];
        for (; iter != end; ++iter)
          {
            BOOST_CHECK_LE(tmp, (*iter)[ordered_dim]);
            tmp = (*iter)[ordered_dim];
            if (++count > 100) break;
          }
        BOOST_CHECK_EQUAL(count, 100);
      }
  }
  { // test at the limit: a tree where all elements are the same
    Tp fix(100, same());
    for (dimension_type ordered_dim = 0; ordered_dim < 6;
         ++ordered_dim)
      {
        ordered_iterator<typename Tp::container_type>
          iter = ordered_begin(fix.container, ordered_dim),
          end = ordered_end(fix.container, ordered_dim);
        int count = 0;
        for (; iter != end; ++iter)
          {
            BOOST_CHECK_EQUAL(100, (*iter)[ordered_dim]);
            if (++count > 100) break;
          }
        BOOST_CHECK_EQUAL(count, 100);
      }
  }
  { // test at the limit: a tree with 2 elements
    Tp fix(2, same());
    for (dimension_type ordered_dim = 0; ordered_dim < 6;
         ++ordered_dim)
      {
        ordered_iterator<const typename Tp::container_type>
          pre = ordered_cbegin(fix.container, ordered_dim),
          post = ordered_cbegin(fix.container, ordered_dim),
          end = ordered_cend(fix.container, ordered_dim);
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
    for (dimension_type ordered_dim = 0; ordered_dim < 6;
         ++ordered_dim)
      {
        ordered_iterator<typename Tp::container_type>
          iter = ordered_begin(fix.container, ordered_dim),
          end = ordered_end(fix.container, ordered_dim);
        int count = 0;
        double tmp = (*iter)[ordered_dim];
        for (; iter != end; ++iter)
          {
            BOOST_CHECK_LE(tmp, (*iter)[ordered_dim]);
            tmp = (*iter)[ordered_dim];
            if (++count > 100) break;
          }
        BOOST_CHECK_EQUAL(count, 100);
      }
  }
  { // test at the limit: a left-unbalanced tree (post-increment)
    Tp fix(100, decrease());
    for (dimension_type ordered_dim = 0; ordered_dim < 6;
         ++ordered_dim)
      {
        ordered_iterator<typename Tp::container_type>
          iter = ordered_begin(fix.container, ordered_dim),
          end = ordered_end(fix.container, ordered_dim);
        int count = 0;
        double tmp = (*iter)[ordered_dim];
        for (; iter != end; iter++)
          {
            BOOST_CHECK_LE(tmp, (*iter)[ordered_dim]);
            tmp = (*iter)[ordered_dim];
            if (++count > 100) break;
          }
        BOOST_CHECK_EQUAL(count, 100);
      }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_ordered_decrement, Tp, double6_maps )
{
  { // test the invarient of the increment
    Tp fix(100, randomize(-1, 1));
    for (dimension_type ordered_dim = 0; ordered_dim < 6;
         ++ordered_dim)
      {
        std::reverse_iterator
              <ordered_iterator<typename Tp::container_type> >
          iter(ordered_end(fix.container, ordered_dim)),
          end(ordered_begin(fix.container, ordered_dim));
        int count = 0;
        double tmp = iter->first[ordered_dim];
        for (; iter != end; ++iter)
          {
            BOOST_CHECK_GE(tmp, iter->first[ordered_dim]);
            tmp = iter->first[ordered_dim];
            if (++count > 100) break;
          }
        BOOST_CHECK_EQUAL(count, 100);
      }
  }
  { // test at the limit: a tree where all elements are the same
    Tp fix(100, same());
    for (dimension_type ordered_dim = 0; ordered_dim < 6;
         ++ordered_dim)
      {
        std::reverse_iterator
                  <ordered_iterator<typename Tp::container_type> >
          iter(ordered_end(fix.container, ordered_dim)),
          end(ordered_begin(fix.container, ordered_dim));
        int count = 0;
        for (; iter != end; ++iter)
          {
            BOOST_CHECK_EQUAL(100, iter->first[ordered_dim]);
            if (++count > 100) break;
          }
        BOOST_CHECK_EQUAL(count, 100);
      }
  }
  { // test at the limit: a tree with 2 elements
    Tp fix(2, same());
    for (dimension_type ordered_dim = 0; ordered_dim < 6;
         ++ordered_dim)
      {
        ordered_iterator<const typename Tp::container_type>
          pre = ordered_cend(fix.container, ordered_dim),
          post = ordered_cend(fix.container, ordered_dim),
          begin = ordered_cbegin(fix.container, ordered_dim);
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
    for (dimension_type ordered_dim = 0; ordered_dim < 6;
         ++ordered_dim)
      {
        std::reverse_iterator
                  <ordered_iterator<typename Tp::container_type> >
          iter(ordered_end(fix.container, ordered_dim)),
          end(ordered_begin(fix.container, ordered_dim));
        int count = 0;
        double tmp = iter->first[ordered_dim];
        for (; iter != end; ++iter)
          {
            BOOST_CHECK_GE(tmp, iter->first[ordered_dim]);
            tmp = iter->first[ordered_dim];
            if (++count > 100) break;
          }
        BOOST_CHECK_EQUAL(count, 100);
      }
  }
  { // test at the limit: a left-unbalanced tree (post-increment)
    Tp fix(100, decrease());
    for (dimension_type ordered_dim = 0; ordered_dim < 6;
         ++ordered_dim)
      {
                std::reverse_iterator
          <ordered_iterator<typename Tp::container_type> >
          iter(ordered_end(fix.container, ordered_dim)),
          end(ordered_begin(fix.container, ordered_dim));
        int count = 0;
        double tmp = iter->first[ordered_dim];
        for (; iter != end; iter++)
          {
            BOOST_CHECK_GE(tmp, iter->first[ordered_dim]);
            tmp = iter->first[ordered_dim];
            if (++count > 100) break;
          }
        BOOST_CHECK_EQUAL(count, 100);
      }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_ordered_lower_bound, Tp, quad_sets )
{
  { // find the smallest element that is greater or equal to key
    Tp fix(100, randomize(-2, 2));
    quad lower (-2, -2, -2, -2);
    quad in (1, 1, 1, 1);
    quad upper (2, 2, 2, 2);
    for (dimension_type ordered_dim = 0; ordered_dim < 4;
         ++ordered_dim)
      {
                ordered_iterator<typename Tp::container_type>
                  iter (ordered_lower_bound(fix.container, ordered_dim, in));
                BOOST_CHECK(iter == ordered_end(fix.container, ordered_dim)
                                || !quad_less()(ordered_dim, *iter, in));
                BOOST_CHECK(iter == ordered_begin(fix.container, ordered_dim)
                                || quad_less()(ordered_dim, *--iter, in));
                iter = ordered_lower_bound(fix.container, ordered_dim, lower);
                BOOST_CHECK(iter == ordered_begin(fix.container, ordered_dim));
                iter = ordered_lower_bound(fix.container, ordered_dim, upper);
                BOOST_CHECK(iter == ordered_end(fix.container, ordered_dim));
      }
  }
  { // same test with a tree filled with similar values
    Tp fix(100, same());
    quad lower (99, 99, 99, 99);
    quad in (100, 100, 100, 100);
        quad upper (101, 101, 101, 101);
    for (dimension_type ordered_dim = 0; ordered_dim < 4;
         ++ordered_dim)
      {
                ordered_iterator<typename Tp::container_type>
                  iter (ordered_lower_bound(fix.container, ordered_dim, lower));
                BOOST_CHECK(iter == ordered_begin(fix.container, ordered_dim));
                iter = ordered_lower_bound(fix.container, ordered_dim, in);
                BOOST_CHECK(iter == ordered_begin(fix.container, ordered_dim));
                iter = ordered_lower_bound(fix.container, ordered_dim, upper);
                BOOST_CHECK(iter == ordered_end(fix.container, ordered_dim));
      }
  }
  { // test at the limit: tree with 1 value
    Tp fix(1, same());
    quad lower (0, 0, 0, 0);
    quad in (1, 1, 1, 1);
        quad upper (2, 2, 2, 2);
    for (dimension_type ordered_dim = 0; ordered_dim < 4;
         ++ordered_dim)
      {
                ordered_iterator<const typename Tp::container_type>
                  iter (ordered_clower_bound(fix.container, ordered_dim, lower));
                BOOST_CHECK(iter == ordered_cbegin(fix.container, ordered_dim));
                iter = ordered_clower_bound(fix.container, ordered_dim, in);
                BOOST_CHECK(iter == ordered_cbegin(fix.container, ordered_dim));
                iter = ordered_clower_bound(fix.container, ordered_dim, upper);
                BOOST_CHECK(iter == ordered_cend(fix.container, ordered_dim));
      }
  }
  { // test at the limit: tree filled with decreasing values
    Tp fix(100, decrease()); // first (100, 100, 100, 100), last (1, 1, 1, 1)
        quad lower(1, 1, 1, 1);
        quad in (100, 100, 100, 100);
        quad upper(101, 101, 101, 101);
    for (dimension_type ordered_dim = 0; ordered_dim < 4;
         ++ordered_dim)
      {
                ordered_iterator<typename Tp::container_type>
                  iter (ordered_lower_bound(fix.container, ordered_dim, lower));
                BOOST_CHECK(iter == ordered_begin(fix.container, ordered_dim));
                iter = ordered_lower_bound(fix.container, ordered_dim, in);
                BOOST_CHECK(iter != ordered_end(fix.container, ordered_dim)
                        && ++iter == ordered_end(fix.container, ordered_dim));
                iter = ordered_lower_bound(fix.container, ordered_dim, upper);
                BOOST_CHECK(iter == ordered_end(fix.container, ordered_dim));
      }
  }
  { // test at the limit: tree filled with increasing values
    Tp fix(100, increase()); // first (0, 0, 0, 0), last (99, 99, 99, 99)
        quad lower(0, 0, 0, 0);
        quad in(99, 99, 99, 99);
        quad upper (100, 100, 100, 100);
    for (dimension_type ordered_dim = 0; ordered_dim < 4;
         ++ordered_dim)
      {
                ordered_iterator<typename Tp::container_type>
                  iter (ordered_lower_bound(fix.container, ordered_dim, lower));
                BOOST_CHECK(iter == ordered_begin(fix.container, ordered_dim));
                iter = ordered_lower_bound(fix.container, ordered_dim, in);
                BOOST_CHECK(iter != ordered_end(fix.container, ordered_dim)
                        && ++iter == ordered_end(fix.container, ordered_dim));
                iter = ordered_lower_bound(fix.container, ordered_dim, upper);
                BOOST_CHECK(iter == ordered_end(fix.container, ordered_dim));
      }
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
    for (dimension_type ordered_dim = 0; ordered_dim < 4;
         ++ordered_dim)
      {
                ordered_iterator<typename Tp::container_type>
                  iter (ordered_upper_bound(fix.container, ordered_dim, in));
                BOOST_CHECK(iter == ordered_end(fix.container, ordered_dim)
                                || quad_less()(ordered_dim, in, iter->first));
                BOOST_CHECK(iter == ordered_begin(fix.container, ordered_dim)
                                || !quad_less()(ordered_dim, (--iter)->first, in));
                iter = ordered_upper_bound(fix.container, ordered_dim, lower);
                BOOST_CHECK(iter == ordered_begin(fix.container, ordered_dim));
                iter = ordered_upper_bound(fix.container, ordered_dim, upper);
                BOOST_CHECK(iter == ordered_end(fix.container, ordered_dim));
      }
  }
  { // same test with a tree filled with similar values
    Tp fix(100, same());
    quad lower (99, 99, 99, 99);
    quad in (100, 100, 100, 100);
        quad upper (101, 101, 101, 101);
    for (dimension_type ordered_dim = 0; ordered_dim < 4;
         ++ordered_dim)
      {
                ordered_iterator<typename Tp::container_type>
                  iter (ordered_upper_bound(fix.container, ordered_dim, lower));
                BOOST_CHECK(iter == ordered_begin(fix.container, ordered_dim));
                iter = ordered_upper_bound(fix.container, ordered_dim, in);
                BOOST_CHECK(iter == ordered_end(fix.container, ordered_dim));
                iter = ordered_upper_bound(fix.container, ordered_dim, upper);
                BOOST_CHECK(iter == ordered_end(fix.container, ordered_dim));
      }
  }
  { // test at the limit: tree with 1 value
    Tp fix(1, same());
    quad lower (0, 0, 0, 0);
    quad in (1, 1, 1, 1);
        quad upper (2, 2, 2, 2);
    for (dimension_type ordered_dim = 0; ordered_dim < 4;
         ++ordered_dim)
      {
                ordered_iterator<const typename Tp::container_type>
                  iter (ordered_cupper_bound(fix.container, ordered_dim, lower));
                BOOST_CHECK(iter == ordered_cbegin(fix.container, ordered_dim));
                iter = ordered_cupper_bound(fix.container, ordered_dim, in);
                BOOST_CHECK(iter == ordered_cend(fix.container, ordered_dim));
                iter = ordered_cupper_bound(fix.container, ordered_dim, upper);
                BOOST_CHECK(iter == ordered_cend(fix.container, ordered_dim));
      }
  }
  { // test at the limit: tree filled with decreasing values
    Tp fix(100, decrease()); // first (100, 100, 100, 100), last (1, 1, 1, 1)
        quad lower(0, 0, 0, 0);
        quad in (99, 99, 99, 99);
        quad upper(100, 100, 100, 100);
    for (dimension_type ordered_dim = 0; ordered_dim < 4;
         ++ordered_dim)
      {
                ordered_iterator<typename Tp::container_type>
                  iter (ordered_upper_bound(fix.container, ordered_dim, lower));
                BOOST_CHECK(iter == ordered_begin(fix.container, ordered_dim));
                iter = ordered_upper_bound(fix.container, ordered_dim, in);
                BOOST_CHECK(iter != ordered_end(fix.container, ordered_dim)
                        && ++iter == ordered_end(fix.container, ordered_dim));
                iter = ordered_upper_bound(fix.container, ordered_dim, upper);
                BOOST_CHECK(iter == ordered_end(fix.container, ordered_dim));
      }
  }
  { // test at the limit: tree filled with increasing values
    Tp fix(100, increase()); // first (0, 0, 0, 0), last (99, 99, 99, 99)
        quad lower(-1, -1, -1, -1);
        quad in(98, 98, 98, 98);
        quad upper (99, 99, 99, 99);
    for (dimension_type ordered_dim = 0; ordered_dim < 4;
         ++ordered_dim)
      {
                ordered_iterator<typename Tp::container_type>
                  iter (ordered_upper_bound(fix.container, ordered_dim, lower));
                BOOST_CHECK(iter == ordered_begin(fix.container, ordered_dim));
                iter = ordered_upper_bound(fix.container, ordered_dim, in);
                BOOST_CHECK(iter != ordered_end(fix.container, ordered_dim)
                        && ++iter == ordered_end(fix.container, ordered_dim));
                iter = ordered_upper_bound(fix.container, ordered_dim, upper);
                BOOST_CHECK(iter == ordered_end(fix.container, ordered_dim));
      }
  }
}

BOOST_AUTO_TEST_CASE( test_ordered_dimension )
{
  { // with mutable iterator
        pointset_fix<double6> fix;
        ordered_iterator<typename pointset_fix<double6>::container_type> iter;
        iter = ordered_begin(fix.container, 5u);
        BOOST_CHECK_EQUAL(ordered_dimension(iter), 5u);
        ordered_dimension(iter, 3u);
        BOOST_CHECK_EQUAL(ordered_dimension(iter), 3u);
        ordered_iterator<const typename pointset_fix<double6>
                             ::container_type> citer;
        citer = ordered_cbegin(fix.container, 5u);
        BOOST_CHECK_EQUAL(ordered_dimension(citer), 5u);
        ordered_dimension(citer, 3u);
        BOOST_CHECK_EQUAL(ordered_dimension(citer), 3u);
  }
  { // Check invalid dimension exception
        pointset_fix<double6> fix;
        ordered_iterator<typename pointset_fix<double6>::container_type>
          iter(ordered_begin(fix.container, 5u));
        BOOST_CHECK_THROW(ordered_dimension(iter, 6u), invalid_dimension);
        BOOST_CHECK_THROW(ordered_begin(fix.container, 6u),
                              invalid_dimension);
        BOOST_CHECK_THROW(ordered_end(fix.container, 6u),
                                  invalid_dimension);
        BOOST_CHECK_THROW(ordered_lower_bound(fix.container, 6u, double6()),
                              invalid_dimension);
        BOOST_CHECK_THROW(ordered_upper_bound(fix.container, 6u, double6()),
                              invalid_dimension);
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_ordered_range, Tp, every_quad )
{
  Tp fix(20, randomize(-100, 100));
  { // non-const
        ordered_iterator_pair<typename Tp::container_type>
      pair(ordered_range(fix.container, 2u));
        BOOST_CHECK(pair.first ==  ordered_begin(fix.container, 2u));
        BOOST_CHECK(pair.second == ordered_end(fix.container, 2u));
        ordered_iterator_pair<typename Tp::container_type> pair2;
        pair2 = ordered_range(fix.container, 3u);
        BOOST_CHECK(pair2.first ==  ordered_begin(fix.container, 3u));
        BOOST_CHECK(pair2.second == ordered_end(fix.container, 3u));
  }
  { // const
        ordered_iterator_pair<const typename Tp::container_type>
      pair0(ordered_range(fix.container, 1u)); // cast constructor
        BOOST_CHECK(pair0.first ==  ordered_begin(fix.container, 1u));
        BOOST_CHECK(pair0.second == ordered_end(fix.container, 1u));
        ordered_iterator_pair<const typename Tp::container_type>
      pair1(ordered_crange(fix.container, 2u)); // copy constructor
        BOOST_CHECK(pair1.first ==  ordered_begin(fix.container, 2u));
        BOOST_CHECK(pair1.second == ordered_end(fix.container, 2u));
        ordered_iterator_pair<const typename Tp::container_type> pair2;
        pair2 = ordered_crange(fix.container, 3u); // assignment operator
        BOOST_CHECK(pair2.first ==  ordered_cbegin(fix.container, 3u));
        BOOST_CHECK(pair2.second == ordered_cend(fix.container, 3u));
  }
}
*/
#endif // SPATIAL_TEST_ORDERED_HPP
