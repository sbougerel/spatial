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
( test_neighbor_begin, Tp, double6_sets )
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
        randomize(-20, 20)(target, 0, 0);
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
        fix.container.erase(iter);
      }
  }
  // Prove that you can find the min if a tree has a single element
  // Prove that you can find the min in a very unbalanced tree
  // Prove that you can find the min in an opposite unbalanced tree
  /*
    using namespace spatial::details::geometry;
    typedef Empty_Kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef euclidian_double
      <kdtree_type::key_type,
       bracket_cast_accessor<point2d, double> > geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // If tree has one node, it should always return that node.
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, ones, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.begin().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 0);
      BOOST_CHECK_CLOSE(iter.impl_.distance_(), std::sqrt(2.0), .000000000001);
      BOOST_CHECK(iter.impl_.target_() == ones);
    }
    {
      // Find the expected closest at on a left-unblanced tree
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(threes);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, ones, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == ones);
      BOOST_CHECK_EQUAL(iter.distance(), .0);
      BOOST_CHECK(iter.impl_.node_ != fix.kdtree.end().node);
    }
    {
      // Find the expected closest at on a right-unblanced tree
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(threes);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, fours, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == threes);
      BOOST_CHECK_CLOSE(iter.distance(), std::sqrt(2.0), .000000000001);
      BOOST_CHECK(iter.impl_.node_ != fix.kdtree.end().node);
    }
  }
  {
    // Find the closest in a tree with a lot of positions, cross-check
    typedef Hundred_Kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    Hundred_Kdtree_5D_fixture fix;
    geometry_type geometry;
    for(int i=0; i<20; ++i)
      {
        point5d target; // A bit out of the tree interval of points
        target[0] = static_cast<double>(rand() % 22 - 1);
        target[1] = static_cast<double>(rand() % 22 - 1);
        target[2] = static_cast<double>(rand() % 22 - 1);
        target[3] = static_cast<double>(rand() % 22 - 1);
        target[4] = static_cast<double>(rand() % 22 - 1);
        iterator_type min =
          iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                                 geometry, target, 0,
                                 fix.kdtree.end().node->parent);
        BOOST_REQUIRE(min.impl_.node_ != fix.kdtree.end().node);
        // Need to collect the results independantly to cross-check
        kdtree_type::const_iterator iter = fix.kdtree.begin();
        kdtree_type::const_iterator end = fix.kdtree.end();
        point5d best = *iter;
        double best_distance = geometry.distance_to_key(5, target, *iter);
        ++iter;
        for (; iter != end; ++iter)
          {
            double tmp_distance = geometry.distance_to_key(5, target, *iter);
            if (tmp_distance < best_distance)
              {
                best = *iter;
                best_distance = tmp_distance;
              }
          }
        BOOST_CHECK_CLOSE(min.distance(), best_distance, .000000000001);
      }
  }
  */
}
/*
BOOST_AUTO_TEST_CASE( test_neighbor_maximum )
{
  {
    typedef Empty_Kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef euclidian_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // If tree has one node, it should always return that node.
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, ones, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.begin().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 0);
      BOOST_CHECK_CLOSE(iter.impl_.distance_(), std::sqrt(2.0), .000000000001);
      BOOST_CHECK(iter.impl_.target_() == ones);
    }
    {
      // Find the expected furthest on a left-unblanced tree
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(threes);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, ones, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == threes);
      BOOST_CHECK_CLOSE(iter.distance(), std::sqrt(8.0), .000000000001);
      BOOST_CHECK(iter.impl_.node_ != fix.kdtree.end().node);
    }
    {
      // Find the expected furthest on a right-unblanced tree
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(threes);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, fours, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == zeros);
      BOOST_CHECK_CLOSE(iter.distance(), std::sqrt(32.0), .000000000001);
      BOOST_CHECK(iter.impl_.node_ != fix.kdtree.end().node);
    }
  }
  {
    // Find the furthest in a tree with a lot of positions, cross-check
    typedef Hundred_Kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    Hundred_Kdtree_5D_fixture fix;
    geometry_type geometry;
    for(int i=0; i<20; ++i)
      {
        point5d target; // A bit out of the tree interval of points
        target[0] = static_cast<double>(rand() % 22 - 1);
        target[1] = static_cast<double>(rand() % 22 - 1);
        target[2] = static_cast<double>(rand() % 22 - 1);
        target[3] = static_cast<double>(rand() % 22 - 1);
        target[4] = static_cast<double>(rand() % 22 - 1);
        iterator_type max =
          iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                                 geometry, target, 0,
                                 fix.kdtree.end().node->parent);
        BOOST_REQUIRE(max.impl_.node_ != fix.kdtree.end().node);
        // Need to collect the results independantly to cross-check
        kdtree_type::const_iterator iter = fix.kdtree.begin();
        kdtree_type::const_iterator end = fix.kdtree.end();
        point5d best = *iter;
        double best_distance = geometry.distance_to_key(5, target, *iter);
        ++iter;
        for (; iter != end; ++iter)
          {
            double tmp_distance = geometry.distance_to_key(5, target, *iter);
            if (tmp_distance > best_distance)
              {
                best = *iter;
                best_distance = tmp_distance;
              }
          }
        BOOST_CHECK_CLOSE(max.distance(), best_distance, .000000000001);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_neighbor_increment )
{
  {
    typedef Empty_Kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // If tree has one node, it should iterate to the end
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type(fix.kdtree.rank(), fix.kdtree.key_comp(), geometry, zeros,
                      0, static_cast<kdtree_type::node_type*>
                      (fix.kdtree.end().node->parent), 0.0);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.begin().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 0);
      BOOST_CHECK(iter.impl_.target_() == zeros);
      BOOST_CHECK_NO_THROW(++iter);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == zeros);
    }
    {
      // Find the expected nodes on a left-unblanced tree
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(threes);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, threes, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == threes);
      ++iter;
      BOOST_CHECK(*iter == twos);
      BOOST_CHECK(iter.distance() == 2);
      ++iter;
      BOOST_CHECK(*iter == ones);
      BOOST_CHECK(iter.distance() == 8);
      ++iter;
      BOOST_CHECK(*iter == zeros);
      BOOST_CHECK(iter.distance() == 18);
      ++iter;
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == threes);
    }
    {
      // Find the expected furthest on a right-unblanced tree
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(threes);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, zeros, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == zeros);
      ++iter;
      BOOST_CHECK(*iter == ones);
      BOOST_CHECK(iter.distance() == 2);
      ++iter;
      BOOST_CHECK(*iter == twos);
      BOOST_CHECK(iter.distance() == 8);
      ++iter;
      BOOST_CHECK(*iter == threes);
      BOOST_CHECK(iter.distance() == 18);
      ++iter;
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == zeros);
    }
  }
  {
    typedef Hundred_Kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    Hundred_Kdtree_5D_fixture fix;
    geometry_type geometry;
    for(int i=0; i<20; ++i)
      {
        point5d target; // A bit out of the tree interval of points
        target[0] = static_cast<double>(rand() % 22 - 1);
        target[1] = static_cast<double>(rand() % 22 - 1);
        target[2] = static_cast<double>(rand() % 22 - 1);
        target[3] = static_cast<double>(rand() % 22 - 1);
        target[4] = static_cast<double>(rand() % 22 - 1);
        iterator_type iter =
          iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                                 geometry, target, 0,
                                 fix.kdtree.end().node->parent);
        iterator_type end =
          iterator_type(fix.kdtree.rank(), fix.kdtree.key_comp(),
                        geometry, target, 4,
                        static_cast<kdtree_type::node_type*>
                        (fix.kdtree.end().node));
        BOOST_REQUIRE(iter.impl_.node_ != fix.kdtree.end().node);
        double old_distance = geometry.distance_to_key(5, target, *iter);
        BOOST_CHECK_EQUAL(iter.distance(), old_distance);
        ++iter;
        int count = 1;
        for (; iter != end; ++iter, ++count)
          {
            double distance = geometry.distance_to_key(5, target, *iter);
            BOOST_CHECK_EQUAL(iter.distance(), distance);
            BOOST_CHECK(distance >= old_distance);
            old_distance = distance;
          }
        BOOST_CHECK_EQUAL(count, 100);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_neighbor_decrement )
{
  {
    typedef Empty_Kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // If tree has one node, it should iterate to the end
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type(fix.kdtree.rank(), fix.kdtree.key_comp(), geometry, zeros,
                      0, static_cast<kdtree_type::node_type*>
                      (fix.kdtree.end().node->parent), 0.0);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.begin().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 0);
      BOOST_CHECK(iter.impl_.target_() == zeros);
      BOOST_CHECK_NO_THROW(--iter);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == zeros);
    }
    {
      // Find the expected nodes on a left-unblanced tree
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(threes);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, threes, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == zeros);
      --iter;
      BOOST_CHECK(*iter == ones);
      BOOST_CHECK(iter.distance() == 8);
      --iter;
      BOOST_CHECK(*iter == twos);
      BOOST_CHECK(iter.distance() == 2);
      --iter;
      BOOST_CHECK(*iter == threes);
      BOOST_CHECK(iter.distance() == 0);
      --iter;
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == threes);
      --iter;
      BOOST_CHECK(*iter == zeros);
      BOOST_CHECK(iter.distance() == 18);
    }
    {
      // Find the expected furthest on a right-unblanced tree
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(threes);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, threes, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == zeros);
      --iter;
      BOOST_CHECK(*iter == ones);
      BOOST_CHECK(iter.distance() == 8);
      --iter;
      BOOST_CHECK(*iter == twos);
      BOOST_CHECK(iter.distance() == 2);
      --iter;
      BOOST_CHECK(*iter == threes);
      BOOST_CHECK(iter.distance() == 0);
      --iter;
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == threes);
      --iter;
      BOOST_CHECK(*iter == zeros);
      BOOST_CHECK(iter.distance() == 18);
    }
  }
  {
    typedef Hundred_Kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef euclidian_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    Hundred_Kdtree_5D_fixture fix;
    geometry_type geometry;
    for(int i=0; i<20; ++i)
      {
        point5d target; // A bit out of the tree interval of points
        target[0] = static_cast<double>(rand() % 22 - 1);
        target[1] = static_cast<double>(rand() % 22 - 1);
        target[2] = static_cast<double>(rand() % 22 - 1);
        target[3] = static_cast<double>(rand() % 22 - 1);
        target[4] = static_cast<double>(rand() % 22 - 1);
        iterator_type iter =
          iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                                 geometry, target, 0,
                                 fix.kdtree.end().node->parent);
        iterator_type end =
          iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                                 geometry, target, 0,
                                 fix.kdtree.end().node->parent);
        BOOST_REQUIRE(iter.impl_.node_ != fix.kdtree.end().node);
        double old_distance = geometry.distance_to_key(5, target, *iter);
        BOOST_CHECK_EQUAL(iter.distance(), old_distance);
        --iter;
        int count = 1;
        for (; iter != end; --iter, ++count)
          {
            double distance = geometry.distance_to_key(5, target, *iter);
            BOOST_CHECK_EQUAL(iter.distance(), distance);
            BOOST_CHECK(distance <= old_distance);
            old_distance = distance;
          }
        BOOST_CHECK_EQUAL(count, 99);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_neighbor_lower_bound )
{
  {
    // Return the smallest element in set that is greater or equal to limit.
    // Test with high density and oob values.
    typedef Empty_Kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef manhattan
      <kdtree_type::key_type, kdtree_type::key_compare, float> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // Check that there is no failure out of limits
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type it = iterator_type::lower_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 1.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
      // Check that there is no failure in limits.
      fix.kdtree.insert(ones);
      it = iterator_type::lower_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 1.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ != fix.kdtree.end().node);
      BOOST_CHECK(*it == ones);
    }
    {
      // Check that there is no failure in limits
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type it = iterator_type::lower_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 0.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ != fix.kdtree.end().node);
      BOOST_CHECK(*it == zeros);
    }
  }
  {
    // On random sets, check that the appropriate nodes are found.
    typedef Hundred_Kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef manhattan
      <kdtree_type::key_type, kdtree_type::key_compare, float> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    Hundred_Kdtree_5D_fixture fix;
    geometry_type geometry;
    for (int i = 0; i < 100; ++i)
      {
        // manathan distance should not exceed 20.f+20.f
        float limit = (float)(rand() % 42 - 1);
        point5d origin;
        origin[0] = rand() % 22 - 1;
        origin[1] = rand() % 22 - 1;
        origin[2] = rand() % 22 - 1;
        origin[3] = rand() % 22 - 1;
        origin[4] = rand() % 22 - 1;
        iterator_type it = iterator_type::lower_bound
          (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
           origin, limit, 0, fix.kdtree.end().node->parent);
        // dist to value found should be greater or equal to limit or end
        if (it.impl_.node_ != fix.kdtree.end().node)
          { BOOST_CHECK_LE(limit, it.distance()); }
        --it;
        // dist to previous value should be stricly less than limit or end
        if (it.impl_.node_ != fix.kdtree.end().node)
          { BOOST_CHECK_LT(it.distance(), limit); }
      }
  }
}

BOOST_AUTO_TEST_CASE( test_neighbor_upper_bound )
{
  {
    // Return the smallest element in set that is strictly greater than key.
    // Test with high density and oob values.
    typedef Empty_Kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef manhattan
      <kdtree_type::key_type, kdtree_type::key_compare, float> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // Check that there is no failure out of limits
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type it = iterator_type::upper_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 1.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
      // Check that there is no failure in limits.
      fix.kdtree.insert(ones);
      it = iterator_type::upper_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 1.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ != fix.kdtree.end().node);
      BOOST_CHECK(*it == ones);
    }
    {
      // Check that there is no failure in limits
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type it = iterator_type::upper_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 0.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
    }
  }
  {
    // On random sets, check that the appropriate nodes are found.
    typedef Hundred_Kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef manhattan
      <kdtree_type::key_type, kdtree_type::key_compare, float> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    Hundred_Kdtree_5D_fixture fix;
    geometry_type geometry;
    for (int i = 0; i < 100; ++i)
      {
        // manathan distance should not exceed 20.f+20.f
        float limit = (float)(rand() % 42 - 1);
        point5d origin;
        origin[0] = rand() % 22 - 1;
        origin[1] = rand() % 22 - 1;
        origin[2] = rand() % 22 - 1;
        origin[3] = rand() % 22 - 1;
        origin[4] = rand() % 22 - 1;
        iterator_type it = iterator_type::upper_bound
          (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
           origin, limit, 0, fix.kdtree.end().node->parent);
        // dist to value found should be strictly greater than limit or end
        if (it.impl_.node_ != fix.kdtree.end().node)
          { BOOST_CHECK_LT(limit, it.distance()); }
        --it;
        // dist to previous value should be lower or equal limit or end
        if (it.impl_.node_ != fix.kdtree.end().node)
          { BOOST_CHECK_LE(it.distance(), limit); }
      }
  }
}

BOOST_AUTO_TEST_CASE( test_pointset_neighborhood_view )
{
  // Now add some points to pointset and iterate throught these points.
  BOOST_CHECK_MESSAGE(false, "test not implemented");
  pointset<2, point2d> points;
}

BOOST_AUTO_TEST_CASE( test_pointset_neighborhood_other_view )
{
  BOOST_CHECK_MESSAGE(false, "test not implemented");
}

BOOST_AUTO_TEST_CASE( test_relaxed_neighbor_minimum )
{
  {
    using namespace spatial::details::geometry;
    typedef Empty_Relaxed_kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef euclidian_double
      <kdtree_type::key_type,
       bracket_cast_accessor<point2d, double> > geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // If tree has one node, it should always return that node.
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, ones, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.begin().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 0);
      BOOST_CHECK_CLOSE(iter.impl_.distance_(), std::sqrt(2.0), .000000000001);
      BOOST_CHECK(iter.impl_.target_() == ones);
    }
    {
      // Find the expected closest at on a left-unblanced tree
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(threes);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, ones, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == ones);
      BOOST_CHECK_EQUAL(iter.distance(), .0);
      BOOST_CHECK(iter.impl_.node_ != fix.kdtree.end().node);
    }
    {
      // Find the expected closest at on a right-unblanced tree
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(threes);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, fours, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == threes);
      BOOST_CHECK_CLOSE(iter.distance(), std::sqrt(2.0), .000000000001);
      BOOST_CHECK(iter.impl_.node_ != fix.kdtree.end().node);
    }
  }
  {
    // Find the closest in a tree with a lot of positions, cross-check
    typedef Hundred_Relaxed_kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    Hundred_Relaxed_kdtree_5D_fixture fix;
    geometry_type geometry;
    for(int i=0; i<20; ++i)
      {
        point5d target; // A bit out of the tree interval of points
        target[0] = static_cast<double>(rand() % 22 - 1);
        target[1] = static_cast<double>(rand() % 22 - 1);
        target[2] = static_cast<double>(rand() % 22 - 1);
        target[3] = static_cast<double>(rand() % 22 - 1);
        target[4] = static_cast<double>(rand() % 22 - 1);
        iterator_type min =
          iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                                 geometry, target, 0,
                                 fix.kdtree.end().node->parent);
        BOOST_REQUIRE(min.impl_.node_ != fix.kdtree.end().node);
        // Need to collect the results independantly to cross-check
        kdtree_type::const_iterator iter = fix.kdtree.begin();
        kdtree_type::const_iterator end = fix.kdtree.end();
        point5d best = *iter;
        double best_distance = geometry.distance_to_key(5, target, *iter);
        ++iter;
        for (; iter != end; ++iter)
          {
            double tmp_distance = geometry.distance_to_key(5, target, *iter);
            if (tmp_distance < best_distance)
              {
                best = *iter;
                best_distance = tmp_distance;
              }
          }
        BOOST_CHECK_CLOSE(min.distance(), best_distance, .000000000001);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_relaxed_neighbor_maximum )
{
  {
    typedef Empty_Relaxed_kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef euclidian_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // If tree has one node, it should always return that node.
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, ones, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.begin().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 0);
      BOOST_CHECK_CLOSE(iter.impl_.distance_(), std::sqrt(2.0), .000000000001);
      BOOST_CHECK(iter.impl_.target_() == ones);
    }
    {
      // Find the expected furthest on a left-unblanced tree
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(threes);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, ones, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == threes);
      BOOST_CHECK_CLOSE(iter.distance(), std::sqrt(8.0), .000000000001);
      BOOST_CHECK(iter.impl_.node_ != fix.kdtree.end().node);
    }
    {
      // Find the expected furthest on a right-unblanced tree
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(threes);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, fours, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == zeros);
      BOOST_CHECK_CLOSE(iter.distance(), std::sqrt(32.0), .000000000001);
      BOOST_CHECK(iter.impl_.node_ != fix.kdtree.end().node);
    }
  }
  {
    // Find the furthest in a tree with a lot of positions, cross-check
    typedef Hundred_Relaxed_kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    Hundred_Relaxed_kdtree_5D_fixture fix;
    geometry_type geometry;
    for(int i=0; i<20; ++i)
      {
        point5d target; // A bit out of the tree interval of points
        target[0] = static_cast<double>(rand() % 22 - 1);
        target[1] = static_cast<double>(rand() % 22 - 1);
        target[2] = static_cast<double>(rand() % 22 - 1);
        target[3] = static_cast<double>(rand() % 22 - 1);
        target[4] = static_cast<double>(rand() % 22 - 1);
        iterator_type max =
          iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                                 geometry, target, 0,
                                 fix.kdtree.end().node->parent);
        BOOST_REQUIRE(max.impl_.node_ != fix.kdtree.end().node);
        // Need to collect the results independantly to cross-check
        kdtree_type::const_iterator iter = fix.kdtree.begin();
        kdtree_type::const_iterator end = fix.kdtree.end();
        point5d best = *iter;
        double best_distance = geometry.distance_to_key(5, target, *iter);
        ++iter;
        for (; iter != end; ++iter)
          {
            double tmp_distance = geometry.distance_to_key(5, target, *iter);
            if (tmp_distance > best_distance)
              {
                best = *iter;
                best_distance = tmp_distance;
              }
          }
        BOOST_CHECK_CLOSE(max.distance(), best_distance, .000000000001);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_relaxed_neighbor_increment )
{
  {
    typedef Empty_Relaxed_kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // If tree has one node, it should iterate to the end
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type(fix.kdtree.rank(), fix.kdtree.key_comp(), geometry, zeros,
                      0, static_cast<kdtree_type::node_type*>
                      (fix.kdtree.end().node->parent), 0.0);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.begin().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 0);
      BOOST_CHECK(iter.impl_.target_() == zeros);
      BOOST_CHECK_NO_THROW(++iter);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == zeros);
    }
    {
      // Find the expected nodes on a left-unblanced tree
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(threes);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, threes, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == threes);
      ++iter;
      BOOST_CHECK(*iter == twos);
      BOOST_CHECK(iter.distance() == 2);
      ++iter;
      BOOST_CHECK(*iter == ones);
      BOOST_CHECK(iter.distance() == 8);
      ++iter;
      BOOST_CHECK(*iter == zeros);
      BOOST_CHECK(iter.distance() == 18);
      ++iter;
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == threes);
    }
    {
      // Find the expected furthest on a right-unblanced tree
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(threes);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, zeros, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == zeros);
      ++iter;
      BOOST_CHECK(*iter == ones);
      BOOST_CHECK(iter.distance() == 2);
      ++iter;
      BOOST_CHECK(*iter == twos);
      BOOST_CHECK(iter.distance() == 8);
      ++iter;
      BOOST_CHECK(*iter == threes);
      BOOST_CHECK(iter.distance() == 18);
      ++iter;
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == zeros);
    }
  }
  {
    typedef Hundred_Relaxed_kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    Hundred_Relaxed_kdtree_5D_fixture fix;
    geometry_type geometry;
    for(int i=0; i<20; ++i)
      {
        point5d target; // A bit out of the tree interval of points
        target[0] = static_cast<double>(rand() % 22 - 1);
        target[1] = static_cast<double>(rand() % 22 - 1);
        target[2] = static_cast<double>(rand() % 22 - 1);
        target[3] = static_cast<double>(rand() % 22 - 1);
        target[4] = static_cast<double>(rand() % 22 - 1);
        iterator_type iter =
          iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                                 geometry, target, 0,
                                 fix.kdtree.end().node->parent);
        iterator_type end =
          iterator_type(fix.kdtree.rank(), fix.kdtree.key_comp(),
                        geometry, target, 4,
                        static_cast<kdtree_type::node_type*>
                        (fix.kdtree.end().node));
        BOOST_REQUIRE(iter.impl_.node_ != fix.kdtree.end().node);
        double old_distance = geometry.distance_to_key(5, target, *iter);
        BOOST_CHECK_EQUAL(iter.distance(), old_distance);
        ++iter;
        int count = 1;
        for (; iter != end; ++iter, ++count)
          {
            double distance = geometry.distance_to_key(5, target, *iter);
            BOOST_CHECK_EQUAL(iter.distance(), distance);
            BOOST_CHECK(distance >= old_distance);
            old_distance = distance;
          }
        BOOST_CHECK_EQUAL(count, 100);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_relaxed_neighbor_decrement )
{
  {
    typedef Empty_Relaxed_kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // If tree has one node, it should iterate to the end
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type(fix.kdtree.rank(), fix.kdtree.key_comp(), geometry, zeros,
                      0, static_cast<kdtree_type::node_type*>
                      (fix.kdtree.end().node->parent), 0.0);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.begin().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 0);
      BOOST_CHECK(iter.impl_.target_() == zeros);
      BOOST_CHECK_NO_THROW(--iter);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == zeros);
    }
    {
      // Find the expected nodes on a left-unblanced tree
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(threes);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, threes, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == zeros);
      --iter;
      BOOST_CHECK(*iter == ones);
      BOOST_CHECK(iter.distance() == 8);
      --iter;
      BOOST_CHECK(*iter == twos);
      BOOST_CHECK(iter.distance() == 2);
      --iter;
      BOOST_CHECK(*iter == threes);
      BOOST_CHECK(iter.distance() == 0);
      --iter;
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == threes);
      --iter;
      BOOST_CHECK(*iter == zeros);
      BOOST_CHECK(iter.distance() == 18);
    }
    {
      // Find the expected furthest on a right-unblanced tree
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(threes);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, threes, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == zeros);
      --iter;
      BOOST_CHECK(*iter == ones);
      BOOST_CHECK(iter.distance() == 8);
      --iter;
      BOOST_CHECK(*iter == twos);
      BOOST_CHECK(iter.distance() == 2);
      --iter;
      BOOST_CHECK(*iter == threes);
      BOOST_CHECK(iter.distance() == 0);
      --iter;
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == threes);
      --iter;
      BOOST_CHECK(*iter == zeros);
      BOOST_CHECK(iter.distance() == 18);
    }
  }
  {
    typedef Hundred_Relaxed_kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef euclidian_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    Hundred_Relaxed_kdtree_5D_fixture fix;
    geometry_type geometry;
    for(int i=0; i<20; ++i)
      {
        point5d target; // A bit out of the tree interval of points
        target[0] = static_cast<double>(rand() % 22 - 1);
        target[1] = static_cast<double>(rand() % 22 - 1);
        target[2] = static_cast<double>(rand() % 22 - 1);
        target[3] = static_cast<double>(rand() % 22 - 1);
        target[4] = static_cast<double>(rand() % 22 - 1);
        iterator_type iter =
          iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                                 geometry, target, 0,
                                 fix.kdtree.end().node->parent);
        iterator_type end =
          iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                                 geometry, target, 0,
                                 fix.kdtree.end().node->parent);
        BOOST_REQUIRE(iter.impl_.node_ != fix.kdtree.end().node);
        double old_distance = geometry.distance_to_key(5, target, *iter);
        BOOST_CHECK_EQUAL(iter.distance(), old_distance);
        --iter;
        int count = 1;
        for (; iter != end; --iter, ++count)
          {
            double distance = geometry.distance_to_key(5, target, *iter);
            BOOST_CHECK_EQUAL(iter.distance(), distance);
            BOOST_CHECK(distance <= old_distance);
            old_distance = distance;
          }
        BOOST_CHECK_EQUAL(count, 99);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_relaxed_neighbor_lower_bound )
{
  {
    // Return the smallest element in set that is greater or equal to limit.
    // Test with high density and oob values.
    typedef Empty_Relaxed_kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef manhattan
      <kdtree_type::key_type, kdtree_type::key_compare, float> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // Check that there is no failure out of limits
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type it = iterator_type::lower_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 1.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
      // Check that there is no failure in limits.
      fix.kdtree.insert(ones);
      it = iterator_type::lower_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 1.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ != fix.kdtree.end().node);
      BOOST_CHECK(*it == ones);
    }
    {
      // Check that there is no failure in limits
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type it = iterator_type::lower_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 0.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ != fix.kdtree.end().node);
      BOOST_CHECK(*it == zeros);
    }
  }
  {
    // On random sets, check that the appropriate nodes are found.
    typedef Hundred_Relaxed_kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef manhattan
      <kdtree_type::key_type, kdtree_type::key_compare, float> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    Hundred_Relaxed_kdtree_5D_fixture fix;
    geometry_type geometry;
    for (int i = 0; i < 100; ++i)
      {
        // manathan distance should not exceed 20.f+20.f
        float limit = (float)(rand() % 42 - 1);
        point5d origin;
        origin[0] = rand() % 22 - 1;
        origin[1] = rand() % 22 - 1;
        origin[2] = rand() % 22 - 1;
        origin[3] = rand() % 22 - 1;
        origin[4] = rand() % 22 - 1;
        iterator_type it = iterator_type::lower_bound
          (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
           origin, limit, 0, fix.kdtree.end().node->parent);
        // dist to value found should be greater or equal to limit or end
        if (it.impl_.node_ != fix.kdtree.end().node)
          { BOOST_CHECK_LE(limit, it.distance()); }
        --it;
        // dist to previous value should be stricly less than limit or end
        if (it.impl_.node_ != fix.kdtree.end().node)
          { BOOST_CHECK_LT(it.distance(), limit); }
      }
  }
}

BOOST_AUTO_TEST_CASE( test_relaxed_neighbor_upper_bound )
{
  {
    // Return the smallest element in set that is strictly greater than key.
    // Test with high density and oob values.
    typedef Empty_Relaxed_kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef manhattan
      <kdtree_type::key_type, kdtree_type::key_compare, float> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // Check that there is no failure out of limits
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type it = iterator_type::upper_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 1.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
      // Check that there is no failure in limits.
      fix.kdtree.insert(ones);
      it = iterator_type::upper_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 1.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ != fix.kdtree.end().node);
      BOOST_CHECK(*it == ones);
    }
    {
      // Check that there is no failure in limits
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type it = iterator_type::upper_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 0.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
    }
  }
  {
    // On random sets, check that the appropriate nodes are found.
    typedef Hundred_Relaxed_kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef manhattan
      <kdtree_type::key_type, kdtree_type::key_compare, float> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    Hundred_Relaxed_kdtree_5D_fixture fix;
    geometry_type geometry;
    for (int i = 0; i < 100; ++i)
      {
        // manathan distance should not exceed 20.f+20.f
        float limit = (float)(rand() % 42 - 1);
        point5d origin;
        origin[0] = rand() % 22 - 1;
        origin[1] = rand() % 22 - 1;
        origin[2] = rand() % 22 - 1;
        origin[3] = rand() % 22 - 1;
        origin[4] = rand() % 22 - 1;
        iterator_type it = iterator_type::upper_bound
          (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
           origin, limit, 0, fix.kdtree.end().node->parent);
        // dist to value found should be strictly greater than limit or end
        if (it.impl_.node_ != fix.kdtree.end().node)
          { BOOST_CHECK_LT(limit, it.distance()); }
        --it;
        // dist to previous value should be lower or equal limit or end
        if (it.impl_.node_ != fix.kdtree.end().node)
          { BOOST_CHECK_LE(it.distance(), limit); }
      }
  }
}
*/
#endif // SPATIAL_TEST_NEIGHBOR_HPP
