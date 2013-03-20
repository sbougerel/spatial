// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_test_neighbor.hpp
 *  @brief  All tests for the elements defined in spatial_neighbor.hpp are
 *  located in this file.
 *  @see spatial_neighbor.hpp
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
          (fix.container.rank(), target, *fix.container.begin());
        for(typename Tp::container_type::iterator
              i = fix.container.begin(); i != fix.container.end(); ++i)
          {
            distance_type tmp = metric.distance_to_key(fix.container.rank(),
                                                       target, *i);
            if (tmp < min_dist) { min_dist = tmp; }
            ++count;
          }
        BOOST_CHECK_EQUAL(count, fix.container.size());
        iter = neighbor_begin(fix.container, target);
        BOOST_CHECK(iter != neighbor_end(fix.container, target));
        BOOST_CHECK_CLOSE(distance(iter), min_dist, 0.000000001);
        neighbor_iterator<typename Tp::container_type> tmp = iter;
        BOOST_CHECK(--tmp == neighbor_end(fix.container, target));
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
    typedef typename neighbor_iterator<typename Tp::container_type>
      ::distance_type distance_type;
    while (!fix.container.empty())
      {
        same()(target, 0, 100);
        iter = neighbor_begin(fix.container, target);
        BOOST_CHECK(iter != neighbor_end(fix.container, target));
        BOOST_CHECK_CLOSE(distance(iter), 0.0, 0.000000001);
        neighbor_iterator<typename Tp::container_type> tmp = iter;
        BOOST_CHECK(--tmp == neighbor_end(fix.container, target));
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
    distance_type dist = metric.distance_to_key(fix.container.rank(),
                                                target, *iter);
    BOOST_CHECK_CLOSE(distance(iter), dist, 0.000000001);
    BOOST_CHECK(*iter == *fix.container.begin());
    neighbor_iterator<typename Tp::container_type> tmp = iter;
    BOOST_CHECK(--tmp == neighbor_end(fix.container, target));
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
          (fix.container.rank(), target, *fix.container.begin());
        for(typename Tp::container_type::iterator
              i = fix.container.begin(); i != fix.container.end(); ++i)
          {
            distance_type tmp = metric.distance_to_key(fix.container.rank(),
                                                       target, *i);
            if (tmp < min_dist) { min_dist = tmp; }
            ++count;
          }
        BOOST_CHECK_EQUAL(count, fix.container.size());
        iter = neighbor_begin(fix.container, target);
        BOOST_CHECK(iter != neighbor_end(fix.container, target));
        BOOST_CHECK_CLOSE(distance(iter), min_dist, 0.000000001);
        neighbor_iterator<typename Tp::container_type> tmp = iter;
        BOOST_CHECK(--tmp == neighbor_end(fix.container, target));
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
          (fix.container.rank(), target, *fix.container.begin());
        for(typename Tp::container_type::iterator
              i = fix.container.begin(); i != fix.container.end(); ++i)
          {
            distance_type tmp = metric.distance_to_key(fix.container.rank(),
                                                       target, *i);
            if (tmp < min_dist) { min_dist = tmp; }
            ++count;
          }
        BOOST_CHECK_EQUAL(count, fix.container.size());
        iter = neighbor_begin(fix.container, target);
        BOOST_CHECK(iter != neighbor_end(fix.container, target));
        BOOST_CHECK_CLOSE(distance(iter), min_dist, 0.000000001);
        neighbor_iterator<typename Tp::container_type> tmp = iter;
        BOOST_CHECK(--tmp == neighbor_end(fix.container, target));
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
          (fix.container.rank(), target, *fix.container.begin());
        for(typename Tp::container_type::iterator
              i = fix.container.begin(); i != fix.container.end(); ++i)
          {
            distance_type tmp = metric.distance_to_key(fix.container.rank(),
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
    typedef typename neighbor_iterator<typename Tp::container_type>
      ::distance_type distance_type;
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
    distance_type dist = metric.distance_to_key(fix.container.rank(),
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
          (fix.container.rank(), target, *fix.container.begin());
        for(typename Tp::container_type::iterator
              i = fix.container.begin(); i != fix.container.end(); ++i)
          {
            distance_type tmp = metric.distance_to_key(fix.container.rank(),
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
          (fix.container.rank(), target, *fix.container.begin());
        for(typename Tp::container_type::iterator
              i = fix.container.begin(); i != fix.container.end(); ++i)
          {
            distance_type tmp = metric.distance_to_key(fix.container.rank(),
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
  // Prove that you can iterate if a tree has a single element
  // Prove that you can iterate equivalent nodes
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_neighbor_decrement, Tp, int2_maps )
{
  // Prove that you can iterate N nodes, down to 1 node
  {
    Tp fix(100, randomize(-20, 20));
    typename neighbor_iterator<typename Tp::container_type>::metric_type
      metric(neighbor_iterator<typename Tp::container_type>().metric());
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
    typename neighbor_iterator<typename Tp::container_type>::metric_type
      metric(neighbor_iterator<typename Tp::container_type>().metric());
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
    typename neighbor_iterator<typename Tp::container_type>::metric_type
      metric(neighbor_iterator<typename Tp::container_type>().metric());
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
  // Prove that you can iterate if a tree has a single element
  // Prove that you can iterate equivalent nodes
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_neighbor_lower_bound, Tp, quad_sets )
{
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_neighbor_upper_bound, Tp, quad_sets )
{
}

#endif // SPATIAL_TEST_NEIGHBOR_HPP
