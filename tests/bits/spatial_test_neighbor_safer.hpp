// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_test_neighbor.hpp
 *  Remaining tests for the elements defined in spatial_neighbor.hpp are
 *  here. These are compiled with SPATIAL_SAFER_ARITHMETICS enabled.
 *  \see spatial_neighbor.hpp
 */

#ifndef SPATIAL_TEST_NEIGHBOR_SAFER_HPP
#define SPATIAL_TEST_NEIGHBOR_SAFER_HPP

#include "../../src/neighbor_iterator.hpp"

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_neighbor_decrement, Tp, int2_maps )
{
  // Prove that you can iterate N nodes, down to 1 node
  {
    Tp fix(100, randomize(-20, 20));
    int2 target;
    typedef typename neighbor_iterator<typename Tp::container_type>
      ::distance_type distance_type;
    while (!fix.container.empty())
      {
        randomize(-22, 22)(target, 0, 0);
        size_t countdown = fix.container.size();
        std::reverse_iterator<neighbor_iterator<typename Tp::container_type> >
          iter(--neighbor_end(fix.container, target)),
          end(neighbor_begin(fix.container, target));
        distance_type max_dist = distance(iter.base());
        for(; --countdown != 0 && iter != end; ++iter)
          {
            distance_type tmp = distance(iter.base());
            BOOST_CHECK_LE(tmp, max_dist);
            max_dist = tmp;
          }
        BOOST_CHECK_EQUAL(countdown, 0u);
        BOOST_CHECK(iter == end);
        fix.container.erase(fix.container.begin());
      }
  }
  // Prove that you can iterate a very unbalanced tree
  {
    Tp fix(40, increase());
    int2 target;
    typedef typename neighbor_iterator<typename Tp::container_type>
      ::distance_type distance_type;
    while (!fix.container.empty())
      {
        randomize(0, 40)(target, 0, 0);
        size_t countdown = fix.container.size();
        std::reverse_iterator<neighbor_iterator<typename Tp::container_type> >
          iter(--neighbor_end(fix.container, target)),
          end(neighbor_begin(fix.container, target));
        distance_type max_dist = distance(iter.base());
        for(; --countdown != 0 && iter != end; ++iter)
          {
            distance_type tmp = distance(iter.base());
            BOOST_CHECK_LE(tmp, max_dist);
            max_dist = tmp;
          }
        BOOST_CHECK_EQUAL(countdown, 0u);
        BOOST_CHECK(iter == end);
        fix.container.erase(fix.container.begin());
      }
  }
  // Prove that you can iterate an opposite unbalanced tree
  {
    Tp fix(40, decrease());
    int2 target;
    typedef typename neighbor_iterator<typename Tp::container_type>
      ::distance_type distance_type;
    while (!fix.container.empty())
      {
        randomize(0, 40)(target, 0, 0);
        size_t countdown = fix.container.size();
        std::reverse_iterator<neighbor_iterator<typename Tp::container_type> >
          iter(--neighbor_end(fix.container, target)),
          end(neighbor_begin(fix.container, target));
        distance_type max_dist = distance(iter.base());
        for(; --countdown != 0 && iter != end; ++iter)
          {
            distance_type tmp = distance(iter.base());
            BOOST_CHECK_LE(tmp, max_dist);
            max_dist = tmp;
          }
        BOOST_CHECK_EQUAL(countdown, 0u);
        BOOST_CHECK(iter == end);
        fix.container.erase(fix.container.begin());
      }
  }
  // Prove that you can iterate equivalent nodes
  {
    int2 target;
    same()(target, 0, 100);
    Tp fix(100, same());
    std::reverse_iterator<neighbor_iterator<typename Tp::container_type> >
      iter(neighbor_end(fix.container, target)),
      end(neighbor_begin(fix.container, target));
    size_t count = 1;
    ++iter;
    for(; iter != end && count < fix.container.size(); ++iter, ++count)
      {
        BOOST_CHECK_CLOSE(distance(iter.base()), 0.0, 0.000000001);
      }
    BOOST_CHECK(iter == end);
    BOOST_CHECK_EQUAL(count, fix.container.size());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_neighbor_upper_bound, Tp, quad_sets )
{
  typedef quadrance<typename Tp::container_type, int, quad_diff> metric_type;
  typedef typename metric_type::distance_type distance_type;
  typedef neighbor_iterator<typename Tp::container_type, metric_type>
    neighbor_iterator_type;
  // Prove that you can find lower bound with N nodes, down to 1 node
  {
    Tp fix(100, randomize(-20, 20));
    metric_type metric;
    quad target;
    while (!fix.container.empty())
      {
        randomize(-22, 22)(target, 0, 0);
        // Find min and max dist first
        distance_type min_dist;
        distance_type max_dist;
        typename Tp::container_type::iterator it = fix.container.begin();
        min_dist = max_dist
          = metric.distance_to_key(fix.container.rank()(), *it, target);
        ++it;
        for (; it != fix.container.end(); ++it)
          {
            distance_type tmp
              = metric.distance_to_key(fix.container.rank()(), *it, target);
            if (tmp < min_dist) min_dist = tmp;
            if (tmp > max_dist) max_dist = tmp;
          }
        distance_type avg_dist = (min_dist + max_dist) / 2;
        // use this knowledge to test the upper bound
        neighbor_iterator_type i
          = neighbor_upper_bound(fix.container, metric, target, min_dist - 1);
        BOOST_CHECK(i == neighbor_begin(fix.container, metric, target));
        BOOST_CHECK_EQUAL(min_dist, distance(i));
        i = neighbor_upper_bound(fix.container, metric, target, max_dist);
        BOOST_CHECK(i == neighbor_end(fix.container, metric, target));
        i = neighbor_upper_bound(fix.container, metric, target, avg_dist);
        if (i != neighbor_end(fix.container, metric, target))
          {
            BOOST_CHECK_GT(distance(i), avg_dist);
          }
        BOOST_CHECK(i == neighbor_begin(fix.container, metric, target)
                    || distance(--i) <= avg_dist);
        fix.container.erase(i);
      }
  }
  // Prove that you can find the upper bound when node and target are same
  {
    Tp fix(100, same());
    metric_type metric;
    quad target;
    same()(target, 0, 100);
    // All points and targets are the same.
    while (!fix.container.empty())
      {
        neighbor_iterator_type i
          = neighbor_upper_bound(fix.container, metric, target, 1);
        BOOST_CHECK(i == neighbor_end(fix.container, metric, target));
        i = neighbor_upper_bound(fix.container, metric, target, 0);
        BOOST_CHECK(i == neighbor_end(fix.container, metric, target));
        fix.container.erase(--i);
      }
  }
  // Prove that you can find the upper bound in an unbalanced tree
  {
    Tp fix(100, increase());
    metric_type metric;
    quad target;
    while (!fix.container.empty())
      {
        randomize(0, 100)(target, 0, 0);
        // Find min and max dist first
        distance_type min_dist;
        distance_type max_dist;
        typename Tp::container_type::iterator it = fix.container.begin();
        min_dist = max_dist
          = metric.distance_to_key(fix.container.rank()(), *it, target);
        ++it;
        for (; it != fix.container.end(); ++it)
          {
            distance_type tmp
              = metric.distance_to_key(fix.container.rank()(), *it, target);
            if (tmp < min_dist) min_dist = tmp;
            if (tmp > max_dist) max_dist = tmp;
          }
        distance_type avg_dist = (min_dist + max_dist) / 2;
        // Use this knowledge to test the upper bound
        neighbor_iterator_type i
          = neighbor_upper_bound(fix.container, metric, target, min_dist - 1);
        BOOST_CHECK(i == neighbor_begin(fix.container, metric, target));
        BOOST_CHECK_EQUAL(min_dist, distance(i));
        i = neighbor_upper_bound(fix.container, metric, target, max_dist);
        BOOST_CHECK(i == neighbor_end(fix.container, metric, target));
        i = neighbor_upper_bound(fix.container, metric, target, avg_dist);
        if (i != neighbor_end(fix.container, metric, target))
          {
            BOOST_CHECK_GT(distance(i), avg_dist);
          }
        BOOST_CHECK(i == neighbor_begin(fix.container, metric, target)
                    || distance(--i) <= avg_dist);
        fix.container.erase(i);
      }
  }
  // Prove that you can find the upper bound in an unbalanced tree
  {
    Tp fix(100, decrease());
    metric_type metric;
    quad target;
    while (!fix.container.empty())
      {
        randomize(0, 100)(target, 0, 0);
        // Find min and max dist first
        distance_type min_dist;
        distance_type max_dist;
        typename Tp::container_type::iterator it = fix.container.begin();
        min_dist = max_dist
          = metric.distance_to_key(fix.container.rank()(), *it, target);
        ++it;
        for (; it != fix.container.end(); ++it)
          {
            distance_type tmp
              = metric.distance_to_key(fix.container.rank()(), *it, target);
            if (tmp < min_dist) min_dist = tmp;
            if (tmp > max_dist) max_dist = tmp;
          }
        distance_type avg_dist = (min_dist + max_dist) / 2;
        // Use this knowledge to test the upper bound
        neighbor_iterator_type i
          = neighbor_upper_bound(fix.container, metric, target, 0);
        BOOST_CHECK(i == neighbor_begin(fix.container, metric, target));
        BOOST_CHECK_EQUAL(min_dist, distance(i));
        i = neighbor_upper_bound(fix.container, metric, target, max_dist);
        BOOST_CHECK(i == neighbor_end(fix.container, metric, target));
        i = neighbor_upper_bound(fix.container, metric, target, avg_dist);
        if (i != neighbor_end(fix.container, metric, target))
          {
            BOOST_CHECK_GT(distance(i), avg_dist);
          }
        if (i != neighbor_begin(fix.container, metric, target))
          {
            BOOST_CHECK_GE(avg_dist, distance(--i));
          }
        fix.container.erase(i);
      }
  }
}


BOOST_AUTO_TEST_CASE_TEMPLATE
(test_manhattan_neighbor, Tp, double6_maps)
{
  Tp fix(3, randomize(-2, 2));
  double6 target; same()(target, 0, 2);
  {
    // With default diff with float
    manhattan_neighbor_iterator<typename Tp::container_type, float>
      i,
      j = manhattan_neighbor_begin(fix.container, target),
      k = manhattan_neighbor_end(fix.container, target),
      l = manhattan_neighbor_lower_bound(fix.container, target, 0.0),
      m = manhattan_neighbor_upper_bound(fix.container, target, 0.0);
    i = j;
    BOOST_CHECK(i == j);
    BOOST_CHECK(j != k);
    BOOST_CHECK(i == l);
    BOOST_CHECK(m == l);
  }
  {
    // With custom diff and const
    manhattan_neighbor_iterator<const typename Tp::container_type,
                                float, double6_diff>
      i,
      j = manhattan_neighbor_cbegin(fix.container, double6_diff(), target),
      k = manhattan_neighbor_cend(fix.container, double6_diff(), target),
      l = manhattan_neighbor_clower_bound(fix.container, double6_diff(),
                                          target, 0.0),
      m = manhattan_neighbor_cupper_bound(fix.container, double6_diff(),
                                          target, 0.0);
    i = j;
    BOOST_CHECK(i == j);
    BOOST_CHECK(j != k);
    BOOST_CHECK(i == l);
    BOOST_CHECK(m == l);
  }
  {
    // With default diff with float and a pair
    manhattan_neighbor_iterator_pair<typename Tp::container_type, long>
      i,
      j = manhattan_neighbor_range(fix.container, target);
    i = j;
    BOOST_CHECK(i == j);
  }
  {
    // With custom diff and const
    manhattan_neighbor_iterator_pair<const typename Tp::container_type,
                                     long, double6_diff>
      i,
      j = manhattan_neighbor_crange(fix.container, double6_diff(), target);
    i = j;
    BOOST_CHECK(i == j);
  }
  // Need to test the pair
}

#endif // SPATIAL_TEST_NEIGHBOR_SAFER_HPP
