// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_test_neighbor.hpp
 *  Most tests for the elements defined in spatial_neighbor.hpp are
 *  located in this file.
 *  \see spatial_neighbor.hpp
 */

#ifndef SPATIAL_TEST_NEIGHBOR_HPP
#define SPATIAL_TEST_NEIGHBOR_HPP

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_neighbor_default, Tp, every_quad )
{
  Tp fix(0);
  neighbor_iterator
    <typename Tp::container_type,
     euclidian<typename Tp::container_type, double,
               accessor_minus<quad_access, quad, double> > >
    a, b(a);
  neighbor_iterator
    <const typename Tp::container_type,
     euclidian<typename Tp::container_type, double,
               accessor_minus<quad_access, quad, double> > > c;
  c = a;
  neighbor_iterator_pair
    <typename Tp::container_type,
     euclidian<typename Tp::container_type, double,
               accessor_minus<quad_access, quad, double> > >
    p, q(a, b);
  neighbor_iterator_pair
    <const typename Tp::container_type,
     euclidian<typename Tp::container_type, double,
               accessor_minus<quad_access, quad, double> > >
    r, s(c, c), t(p);
  BOOST_CHECK(a == b);
  BOOST_CHECK(!(a != c));
  typename Tp::container_type::iterator i = a;
  BOOST_CHECK(i == a);
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_neighbor_compact, Tp, double6_maps )
{
  Tp fix(0);
  neighbor_iterator<typename Tp::container_type> a, b(a);
  neighbor_iterator<const typename Tp::container_type> c;
  c = a;
  neighbor_iterator_pair<typename Tp::container_type> p, q(a, b);
  neighbor_iterator_pair<const typename Tp::container_type> r, s(c, c), t(p);
  BOOST_CHECK(a == b);
  BOOST_CHECK(!(a != c));
  typename Tp::container_type::iterator i = a;
  BOOST_CHECK(i == a);
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_neighbor_const, Tp, double6_sets )
{
  // Check that all these consts operations are working and compiling cleanly.
  Tp fix(10);
  neighbor_iterator<const typename Tp::container_type>
    a = neighbor_cbegin(fix.container, make_double6(1.0));
  a++;
  a--;
  a = neighbor_cend(fix.container, make_double6(1.0));
  a = neighbor_clower_bound(fix.container, make_double6(1.0), 1.0);
  a = neighbor_cupper_bound(fix.container, make_double6(1.0), 1.0);
  neighbor_iterator_pair<const typename Tp::container_type>
    p = neighbor_crange(fix.container, make_double6(1.0));
  silence_unused(p);
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_euclidian_neighbor_const, Tp, double6_sets )
{
  // Check that all these consts operations are working and compiling cleanly.
  Tp fix(10);
  euclidian_neighbor_iterator<const typename Tp::container_type, double>
    a = euclidian_neighbor_cbegin(fix.container, make_double6(1.0));
  a++;
  a--;
  a = euclidian_neighbor_cend(fix.container, make_double6(1.0));
  a = euclidian_neighbor_clower_bound(fix.container, make_double6(1.0), 1.0);
  a = euclidian_neighbor_cupper_bound(fix.container, make_double6(1.0), 1.0);
  euclidian_neighbor_iterator_pair<const typename Tp::container_type, double>
    p = euclidian_neighbor_crange(fix.container, make_double6(1.0));
  silence_unused(p);
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_quadrance_neighbor_const, Tp, double6_sets )
{
  // Check that all these consts operations are working and compiling cleanly.
  Tp fix(10);
  quadrance_neighbor_iterator<const typename Tp::container_type, double>
    a = quadrance_neighbor_cbegin(fix.container, make_double6(1.0));
  a++;
  a--;
  a = quadrance_neighbor_cend(fix.container, make_double6(1.0));
  a = quadrance_neighbor_clower_bound(fix.container, make_double6(1.0), 1.0);
  a = quadrance_neighbor_cupper_bound(fix.container, make_double6(1.0), 1.0);
  quadrance_neighbor_iterator_pair<const typename Tp::container_type, double>
    p = quadrance_neighbor_crange(fix.container, make_double6(1.0));
  silence_unused(p);
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_manhattan_neighbor_const, Tp, double6_sets )
{
  // Check that all these consts operations are working and compiling cleanly.
  Tp fix(10);
  manhattan_neighbor_iterator<const typename Tp::container_type, double>
    a = manhattan_neighbor_cbegin(fix.container, make_double6(1.0));
  a++;
  a--;
  a = manhattan_neighbor_cend(fix.container, make_double6(1.0));
  a = manhattan_neighbor_clower_bound(fix.container, make_double6(1.0), 1.0);
  a = manhattan_neighbor_cupper_bound(fix.container, make_double6(1.0), 1.0);
  manhattan_neighbor_iterator_pair<const typename Tp::container_type, double>
    p = manhattan_neighbor_crange(fix.container, make_double6(1.0));
  silence_unused(p);
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_neighbor_accessors, Tp, double6_maps )
{
  Tp fix(0);
  neighbor_iterator<typename Tp::container_type>
    a(fix.container, typename neighbor_iterator<typename Tp::container_type>
      ::metric_type(), make_double6(0., 1., 2., 3., 4., 5.),
      fix.container.end());
  BOOST_CHECK(target_key(a) == make_double6(0., 1., 2., 3., 4., 5.));
  // There is no way to force the value of distance, so we just check it's
  // accessible
  silence_unused(distance(a));
  // This will also be checked in others tests below
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_neighbor_deference, Tp, double6_maps )
{
  Tp fix(2);
  neighbor_iterator<typename Tp::container_type>
    a(fix.container, typename neighbor_iterator<typename Tp::container_type>
      ::metric_type(), make_double6(0., 1., 2., 3., 4., 5.),
      fix.container.begin());
  BOOST_CHECK(*a == *fix.container.begin());
  BOOST_CHECK(a->first == fix.container.begin()->first);
  a->second = "Value assignment must work.";
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_neighbor_minimum, Tp, double6_sets )
{
  // Prove that you can find the closest value with N nodes, down to 1 node
  {
    Tp fix(100, randomize(-20, 20));
    neighbor_iterator<typename Tp::container_type> iter;
    typename neighbor_iterator<typename Tp::container_type>::metric_type
      metric(iter.metric());
    double6 target;
    typedef typename neighbor_iterator<typename Tp::container_type>
      ::distance_type distance_type;
    while (!fix.container.empty())
      {
        randomize(-22, 22)(target, 0, 0);
        unsigned int count = 0;
        distance_type min_dist = metric.distance_to_key
          (fix.container.rank()(), target, *fix.container.begin());
        for(typename Tp::container_type::iterator
              i = fix.container.begin(); i != fix.container.end(); ++i)
          {
            distance_type tmp = metric.distance_to_key(fix.container.rank()(),
                                                       target, *i);
            if (tmp < min_dist) { min_dist = tmp; }
            ++count;
          }
        BOOST_CHECK_EQUAL(count, fix.container.size());
        iter = neighbor_begin(fix.container, target);
        BOOST_CHECK(iter != neighbor_end(fix.container, target));
        BOOST_CHECK_CLOSE(distance(iter), min_dist, 0.000000001);
        fix.container.erase(iter);
      }
  }
  // Prove that you can find the min in N nodes, with target = nodes
  {
    Tp fix(100, same());
    neighbor_iterator<typename Tp::container_type> iter;
    typename neighbor_iterator<typename Tp::container_type>::metric_type
      metric(iter.metric());
    double6 target;
    while (!fix.container.empty())
      {
        same()(target, 0, 100);
        iter = neighbor_begin(fix.container, target);
        BOOST_CHECK(iter != neighbor_end(fix.container, target));
        BOOST_CHECK_CLOSE(distance(iter), 0.0, 0.000000001);
        fix.container.erase(iter);
      }
  }
  // Prove that you can find the min if a tree has a single element
  {
    Tp fix(1, same());
    neighbor_iterator<typename Tp::container_type> iter;
    typename neighbor_iterator<typename Tp::container_type>::metric_type
      metric(iter.metric());
    double6 target = make_double6(2.);
    typedef typename neighbor_iterator<typename Tp::container_type>
      ::distance_type distance_type;
    iter = neighbor_begin(fix.container, target);
    BOOST_CHECK(iter != neighbor_end(fix.container, target));
    distance_type dist = metric.distance_to_key(fix.container.rank()(),
                                                target, *iter);
    BOOST_CHECK_CLOSE(distance(iter), dist, 0.000000001);
    BOOST_CHECK(*iter == *fix.container.begin());
  }
  // Prove that you can find the min in a very unbalanced tree
  {
    Tp fix(40, increase());
    neighbor_iterator<typename Tp::container_type> iter;
    typename neighbor_iterator<typename Tp::container_type>::metric_type
      metric(iter.metric());
    double6 target;
    typedef typename neighbor_iterator<typename Tp::container_type>
      ::distance_type distance_type;
    while (!fix.container.empty())
      {
        randomize(0, 40)(target, 0, 0);
        unsigned int count = 0;
        distance_type min_dist = metric.distance_to_key
          (fix.container.rank()(), target, *fix.container.begin());
        for(typename Tp::container_type::iterator
              i = fix.container.begin(); i != fix.container.end(); ++i)
          {
            distance_type tmp = metric.distance_to_key(fix.container.rank()(),
                                                       target, *i);
            if (tmp < min_dist) { min_dist = tmp; }
            ++count;
          }
        BOOST_CHECK_EQUAL(count, fix.container.size());
        iter = neighbor_begin(fix.container, target);
        BOOST_CHECK(iter != neighbor_end(fix.container, target));
        BOOST_CHECK_CLOSE(distance(iter), min_dist, 0.000000001);
        fix.container.erase(iter);
      }
  }
  // Prove that you can find the min in an opposite unbalanced tree
  {
    Tp fix(40, decrease());
    neighbor_iterator<typename Tp::container_type> iter;
    typename neighbor_iterator<typename Tp::container_type>::metric_type
      metric(iter.metric());
    double6 target;
    typedef typename neighbor_iterator<typename Tp::container_type>
      ::distance_type distance_type;
    while (!fix.container.empty())
      {
        randomize(0, 40)(target, 0, 0);
        unsigned int count = 0;
        distance_type min_dist = metric.distance_to_key
          (fix.container.rank()(), target, *fix.container.begin());
        for(typename Tp::container_type::iterator
              i = fix.container.begin(); i != fix.container.end(); ++i)
          {
            distance_type tmp = metric.distance_to_key(fix.container.rank()(),
                                                       target, *i);
            if (tmp < min_dist) { min_dist = tmp; }
            ++count;
          }
        BOOST_CHECK_EQUAL(count, fix.container.size());
        iter = neighbor_begin(fix.container, target);
        BOOST_CHECK(iter != neighbor_end(fix.container, target));
        BOOST_CHECK_CLOSE(distance(iter), min_dist, 0.000000001);
        fix.container.erase(iter);
      }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_neighbor_maximum, Tp, double6_sets )
{
  // Prove that you can find the closest value with N nodes, down to 1 node
  {
    Tp fix(100, randomize(-20, 20));
    neighbor_iterator<typename Tp::container_type> iter;
    typename neighbor_iterator<typename Tp::container_type>::metric_type
      metric(iter.metric());
    double6 target;
    typedef typename neighbor_iterator<typename Tp::container_type>
      ::distance_type distance_type;
    while (!fix.container.empty())
      {
        randomize(-22, 22)(target, 0, 0);
        unsigned int count = 0;
        distance_type max_dist = metric.distance_to_key
          (fix.container.rank()(), target, *fix.container.begin());
        for(typename Tp::container_type::iterator
              i = fix.container.begin(); i != fix.container.end(); ++i)
          {
            distance_type tmp = metric.distance_to_key(fix.container.rank()(),
                                                       target, *i);
            if (tmp > max_dist) { max_dist = tmp; }
            ++count;
          }
        BOOST_CHECK_EQUAL(count, fix.container.size());
        iter = neighbor_end(fix.container, target); --iter;
        BOOST_CHECK(iter != neighbor_end(fix.container, target));
        BOOST_CHECK_CLOSE(distance(iter), max_dist, 0.000000001);
        neighbor_iterator<typename Tp::container_type> tmp = iter;
        BOOST_CHECK(++tmp == neighbor_end(fix.container, target));
        fix.container.erase(iter);
      }
  }
  // Prove that you can find the min in N nodes, with target = nodes
  {
    Tp fix(100, same());
    neighbor_iterator<typename Tp::container_type> iter;
    typename neighbor_iterator<typename Tp::container_type>::metric_type
      metric(iter.metric());
    double6 target;
    while (!fix.container.empty())
      {
        same()(target, 0, 100);
        iter = neighbor_end(fix.container, target); --iter;
        BOOST_CHECK(iter != neighbor_end(fix.container, target));
        BOOST_CHECK_CLOSE(distance(iter), 0.0, 0.000000001);
        neighbor_iterator<typename Tp::container_type> tmp = iter;
        BOOST_CHECK(++tmp == neighbor_end(fix.container, target));
        fix.container.erase(iter);
      }
  }
  // Prove that you can find the min if a tree has a single element
  {
    Tp fix(1, same());
    neighbor_iterator<typename Tp::container_type> iter;
    typename neighbor_iterator<typename Tp::container_type>::metric_type
      metric(iter.metric());
    double6 target = make_double6(2.);
    typedef typename neighbor_iterator<typename Tp::container_type>
      ::distance_type distance_type;
    iter = neighbor_end(fix.container, target); --iter;
    BOOST_CHECK(iter != neighbor_end(fix.container, target));
    distance_type dist = metric.distance_to_key(fix.container.rank()(),
                                                target, *iter);
    BOOST_CHECK_CLOSE(distance(iter), dist, 0.000000001);
    BOOST_CHECK(*iter == *--fix.container.end());
    neighbor_iterator<typename Tp::container_type> tmp = iter;
    BOOST_CHECK(++tmp == neighbor_end(fix.container, target));
  }
  // Prove that you can find the min in a very unbalanced tree
  {
    Tp fix(40, increase());
    neighbor_iterator<typename Tp::container_type> iter;
    typename neighbor_iterator<typename Tp::container_type>::metric_type
      metric(iter.metric());
    double6 target;
    typedef typename neighbor_iterator<typename Tp::container_type>
      ::distance_type distance_type;
    while (!fix.container.empty())
      {
        randomize(0, 40)(target, 0, 0);
        unsigned int count = 0;
        distance_type max_dist = metric.distance_to_key
          (fix.container.rank()(), target, *fix.container.begin());
        for(typename Tp::container_type::iterator
              i = fix.container.begin(); i != fix.container.end(); ++i)
          {
            distance_type tmp = metric.distance_to_key(fix.container.rank()(),
                                                       target, *i);
            if (tmp > max_dist) { max_dist = tmp; }
            ++count;
          }
        BOOST_CHECK_EQUAL(count, fix.container.size());
        iter = neighbor_end(fix.container, target); --iter;
        BOOST_CHECK(iter != neighbor_end(fix.container, target));
        BOOST_CHECK_CLOSE(distance(iter), max_dist, 0.000000001);
        neighbor_iterator<typename Tp::container_type> tmp = iter;
        BOOST_CHECK(++tmp == neighbor_end(fix.container, target));
        fix.container.erase(iter);
      }
  }
  // Prove that you can find the min in an opposite unbalanced tree
  {
    Tp fix(40, decrease());
    neighbor_iterator<typename Tp::container_type> iter;
    typename neighbor_iterator<typename Tp::container_type>::metric_type
      metric(iter.metric());
    double6 target;
    typedef typename neighbor_iterator<typename Tp::container_type>
      ::distance_type distance_type;
    while (!fix.container.empty())
      {
        randomize(0, 40)(target, 0, 0);
        unsigned int count = 0;
        distance_type max_dist = metric.distance_to_key
          (fix.container.rank()(), target, *fix.container.begin());
        for(typename Tp::container_type::iterator
              i = fix.container.begin(); i != fix.container.end(); ++i)
          {
            distance_type tmp = metric.distance_to_key(fix.container.rank()(),
                                                       target, *i);
            if (tmp > max_dist) { max_dist = tmp; }
            ++count;
          }
        BOOST_CHECK_EQUAL(count, fix.container.size());
        iter = neighbor_end(fix.container, target); --iter;
        BOOST_CHECK(iter != neighbor_end(fix.container, target));
        BOOST_CHECK_CLOSE(distance(iter), max_dist, 0.000000001);
        neighbor_iterator<typename Tp::container_type> tmp = iter;
        BOOST_CHECK(++tmp == neighbor_end(fix.container, target));
        fix.container.erase(iter);
      }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_neighbor_increment, Tp, double6_maps )
{
  // Prove that you can iterate N nodes, down to 1 node
  {
    Tp fix(100, randomize(-20, 20));
    neighbor_iterator<typename Tp::container_type> iter;
    typename neighbor_iterator<typename Tp::container_type>::metric_type
      metric(iter.metric());
    double6 target;
    typedef typename neighbor_iterator<typename Tp::container_type>
      ::distance_type distance_type;
    while (!fix.container.empty())
      {
        randomize(-22, 22)(target, 0, 0);
        size_t countdown = fix.container.size();
        iter = neighbor_begin(fix.container, target);
        distance_type min_dist = distance(iter++);
        for(; --countdown != 0
              && iter != neighbor_end(fix.container, target); ++iter)
          {
            distance_type tmp = distance(iter);
            BOOST_CHECK_GE(tmp, min_dist);
            min_dist = tmp;
          }
        BOOST_CHECK_EQUAL(countdown, 0u);
        BOOST_CHECK(iter == neighbor_end(fix.container, target));
        fix.container.erase(fix.container.begin());
      }
  }
  // Prove that you can iterate a very unbalanced tree
  {
    Tp fix(40, increase());
    neighbor_iterator<typename Tp::container_type> iter;
    typename neighbor_iterator<typename Tp::container_type>::metric_type
      metric(iter.metric());
    double6 target;
    typedef typename neighbor_iterator<typename Tp::container_type>
      ::distance_type distance_type;
    while (!fix.container.empty())
      {
        randomize(0, 40)(target, 0, 0);
        size_t countdown = fix.container.size();
        iter = neighbor_begin(fix.container, target);
        distance_type min_dist = distance(iter++);
        for(; --countdown != 0
              && iter != neighbor_end(fix.container, target); ++iter)
          {
            distance_type tmp = distance(iter);
            BOOST_CHECK_GE(tmp, min_dist);
            min_dist = tmp;
          }
        BOOST_CHECK_EQUAL(countdown, 0u);
        BOOST_CHECK(iter == neighbor_end(fix.container, target));
        fix.container.erase(fix.container.begin());
      }
  }
  // Prove that you can iterate an opposite unbalanced tree
  {
    Tp fix(40, decrease());
    neighbor_iterator<typename Tp::container_type> iter;
    typename neighbor_iterator<typename Tp::container_type>::metric_type
      metric(iter.metric());
    double6 target;
    typedef typename neighbor_iterator<typename Tp::container_type>
      ::distance_type distance_type;
    while (!fix.container.empty())
      {
        randomize(0, 40)(target, 0, 0);
        size_t countdown = fix.container.size();
        iter = neighbor_begin(fix.container, target);
        distance_type min_dist = distance(iter++);
        for(; --countdown != 0
              && iter != neighbor_end(fix.container, target); ++iter)
          {
            distance_type tmp = distance(iter);
            BOOST_CHECK_GE(tmp, min_dist);
            min_dist = tmp;
          }
        BOOST_CHECK_EQUAL(countdown, 0u);
        BOOST_CHECK(iter == neighbor_end(fix.container, target));
        fix.container.erase(fix.container.begin());
      }
  }
  // Prove that you can iterate equivalent nodes
  {
    double6 target;
    same()(target, 0, 100);
    Tp fix(100, same());
    neighbor_iterator<typename Tp::container_type>
      iter = neighbor_begin(fix.container, target),
      end = neighbor_end(fix.container, target);
    size_t count = 0;
    for(; iter != end && count < fix.container.size(); ++iter, ++count)
      {
        BOOST_CHECK_CLOSE(distance(iter), 0.0, 0.000000001);
      }
    BOOST_CHECK(iter == end);
    BOOST_CHECK_EQUAL(count, fix.container.size());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_neighbor_lower_bound, Tp, quad_sets )
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
        // use this knowledge to test the lower bound
        neighbor_iterator_type i
          = neighbor_lower_bound(fix.container, metric, target, min_dist - 1);
        BOOST_CHECK(i == neighbor_begin(fix.container, metric, target));
        BOOST_CHECK_EQUAL(min_dist, distance(i));
        i = neighbor_lower_bound(fix.container, metric, target, max_dist);
        BOOST_CHECK(i != neighbor_end(fix.container, metric, target));
        BOOST_CHECK_EQUAL(max_dist, distance(i));
        BOOST_CHECK(i == neighbor_begin(fix.container, metric, target)
                    || distance(--i) < max_dist);
        i = neighbor_lower_bound(fix.container, metric, target, avg_dist);
        BOOST_CHECK(i != neighbor_end(fix.container, metric, target));
        BOOST_CHECK_GE(distance(i), avg_dist);
        neighbor_iterator_type tmp = i;
        BOOST_CHECK(tmp == neighbor_begin(fix.container, metric, target)
                    || distance(--tmp) < avg_dist);
        fix.container.erase(i);
      }
  }
  // Prove that you can find the lower bound when node and target are same
  {
    Tp fix(100, same());
    metric_type metric;
    quad target;
    same()(target, 0, 100);
    // All points and targets are the same.
    while (!fix.container.empty())
      {
        neighbor_iterator_type i
          = neighbor_lower_bound(fix.container, metric, target, 1);
        BOOST_CHECK(i == neighbor_end(fix.container, metric, target));
        i = neighbor_lower_bound(fix.container, metric, target, 0);
        BOOST_CHECK(i == neighbor_begin(fix.container, metric, target));
        BOOST_CHECK_EQUAL(0, distance(i));
        fix.container.erase(i);
      }
  }
  // Prove that you can find the lower bound in an unbalanced tree
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
        // Use this knowledge to test the lower bound
        neighbor_iterator_type i
          = neighbor_lower_bound(fix.container, metric, target, min_dist - 1);
        BOOST_CHECK(i == neighbor_begin(fix.container, metric, target));
        BOOST_CHECK_EQUAL(min_dist, distance(i));
        i = neighbor_lower_bound(fix.container, metric, target, max_dist);
        BOOST_CHECK(i != neighbor_end(fix.container, metric, target));
        BOOST_CHECK_EQUAL(max_dist, distance(i));
        BOOST_CHECK(i == neighbor_begin(fix.container, metric, target)
                    || distance(--i) < max_dist);
        i = neighbor_lower_bound(fix.container, metric, target, avg_dist);
        BOOST_CHECK(i != neighbor_end(fix.container, metric, target));
        BOOST_CHECK_GE(distance(i), avg_dist);
        neighbor_iterator_type tmp = i;
        BOOST_CHECK(tmp == neighbor_begin(fix.container, metric, target)
                    || distance(--tmp) < avg_dist);
        fix.container.erase(i);
      }
  }
  // Prove that you can find the lower bound in an unbalanced tree
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
        // Use this knowledge to test the lower bound
        neighbor_iterator_type i
          = neighbor_lower_bound(fix.container, metric, target, min_dist - 1);
        BOOST_CHECK(i == neighbor_begin(fix.container, metric, target));
        BOOST_CHECK_EQUAL(min_dist, distance(i));
        i = neighbor_lower_bound(fix.container, metric, target, max_dist);
        BOOST_CHECK(i != neighbor_end(fix.container, metric, target));
        BOOST_CHECK_EQUAL(max_dist, distance(i));
        BOOST_CHECK(i == neighbor_begin(fix.container, metric, target)
                    || distance(--i) < max_dist);
        i = neighbor_lower_bound(fix.container, metric, target, avg_dist);
        BOOST_CHECK(i != neighbor_end(fix.container, metric, target));
        BOOST_CHECK_GE(distance(i), avg_dist);
        neighbor_iterator_type tmp = i;
        BOOST_CHECK(tmp == neighbor_begin(fix.container, metric, target)
                    || distance(--tmp) < avg_dist);
        fix.container.erase(i);
      }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE
(test_euclidian_neighbor, Tp, double6_maps)
{
  Tp fix(3, randomize(-2, 2));
  double6 target; same()(target, 0, 2);
  {
    // With default diff with float
    euclidian_neighbor_iterator<typename Tp::container_type, float>
      i,
      j = euclidian_neighbor_begin(fix.container, target),
      k = euclidian_neighbor_end(fix.container, target),
      l = euclidian_neighbor_lower_bound(fix.container, target, 0.0),
      m = euclidian_neighbor_upper_bound(fix.container, target, 0.0);
    i = j;
    BOOST_CHECK(i == j);
    BOOST_CHECK(j != k);
    BOOST_CHECK(i == l);
    BOOST_CHECK(m == l);
  }
  {
    // With custom diff and const
    euclidian_neighbor_iterator<const typename Tp::container_type,
                                float, double6_diff>
      i,
      j = euclidian_neighbor_cbegin(fix.container, double6_diff(), target),
      k = euclidian_neighbor_cend(fix.container, double6_diff(), target),
      l = euclidian_neighbor_clower_bound(fix.container, double6_diff(),
                                          target, 0.0),
      m = euclidian_neighbor_cupper_bound(fix.container, double6_diff(),
                                          target, 0.0);
    i = j;
    BOOST_CHECK(i == j);
    BOOST_CHECK(j != k);
    BOOST_CHECK(i == l);
    BOOST_CHECK(m == l);
  }
  {
    // With default diff with float and a pair
    euclidian_neighbor_iterator_pair<typename Tp::container_type, float>
      i,
      j = euclidian_neighbor_range(fix.container, target);
    i = j;
    BOOST_CHECK(i == j);
  }
  {
    // With custom diff and const
    euclidian_neighbor_iterator_pair<const typename Tp::container_type,
                                     float, double6_diff>
      i,
      j = euclidian_neighbor_crange(fix.container, double6_diff(), target);
    i = j;
    BOOST_CHECK(i == j);
  }
  // Need to test the pair
}

BOOST_AUTO_TEST_CASE_TEMPLATE
(test_quadrance_neighbor, Tp, double6_maps)
{
  Tp fix(3, randomize(-2, 2));
  double6 target; same()(target, 0, 2);
  {
    // With default diff with float
    quadrance_neighbor_iterator<typename Tp::container_type, float>
      i,
      j = quadrance_neighbor_begin(fix.container, target),
      k = quadrance_neighbor_end(fix.container, target),
      l = quadrance_neighbor_lower_bound(fix.container, target, 0.0),
      m = quadrance_neighbor_upper_bound(fix.container, target, 0.0);
    i = j;
    BOOST_CHECK(i == j);
    BOOST_CHECK(j != k);
    BOOST_CHECK(i == l);
    BOOST_CHECK(m == l);
  }
  {
    // With custom diff and const
    quadrance_neighbor_iterator<const typename Tp::container_type,
                                float, double6_diff>
      i,
      j = quadrance_neighbor_cbegin(fix.container, double6_diff(), target),
      k = quadrance_neighbor_cend(fix.container, double6_diff(), target),
      l = quadrance_neighbor_clower_bound(fix.container, double6_diff(),
                                          target, 0.0),
      m = quadrance_neighbor_cupper_bound(fix.container, double6_diff(),
                                          target, 0.0);
    i = j;
    BOOST_CHECK(i == j);
    BOOST_CHECK(j != k);
    BOOST_CHECK(i == l);
    BOOST_CHECK(m == l);
  }
  {
    // With default diff with float and a pair
    quadrance_neighbor_iterator_pair<typename Tp::container_type, long>
      i,
      j = quadrance_neighbor_range(fix.container, target);
    i = j;
    BOOST_CHECK(i == j);
  }
  {
    // With custom diff and const
    quadrance_neighbor_iterator_pair<const typename Tp::container_type,
                                     long, double6_diff>
      i,
      j = quadrance_neighbor_crange(fix.container, double6_diff(), target);
    i = j;
    BOOST_CHECK(i == j);
  }
  // Need to test the pair
}

#endif // SPATIAL_TEST_NEIGHBOR_HPP
