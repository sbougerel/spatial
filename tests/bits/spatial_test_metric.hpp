// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_test_metric.hpp
 *  @brief  All tests for the elements defined in spatial_metric.hpp are
 *  located in this file.
 *  @see spatial_metric.hpp
 */

#ifndef SPATIAL_TEST_METRIC_HPP
#define SPATIAL_TEST_METRIC_HPP

BOOST_AUTO_TEST_CASE(test_metric_trait)
{
  using namespace spatial::details;
  check_is_same
    <metric_traits<quadrance
                     <pointset<2, int2>, int,
                      paren_minus<int2, int> > >::distance_type,
     quadrance<pointset<2, int2>, int,
                            paren_minus<int2, int> >::distance_type>();
  check_is_same
    <metric_traits<euclidian
                     <boxset<3, double6>, double,
                      bracket_minus<double6, double> > >::distance_type,
     euclidian<boxset<3, double6>, double,
                     bracket_minus<double6, double> >::distance_type>();
  // The following will not compile, but I comment it out to test enable_if
  //check_is_same
  //  <metric_traits<euclidian
  //                   <boxmap<2, quad, std::string>, int,
  //                    accessor_minus<quad_access, quad, int> > >
  //                   ::distance_type,
  //   euclidian<boxmap<2, quad, std::string>, int,
  //                   accessor_minus<quad_access, quad, int> >
  //                   ::distance_type>();
  check_is_same
    <metric_traits<manhattan
                     <idle_pointset<2, int2>, int,
                      paren_minus<int2, int> > >::distance_type,
     manhattan<idle_pointset<2, int2>, int,
                        paren_minus<int2, int> >::distance_type>();
}

BOOST_AUTO_TEST_CASE(test_difference_bracket)
{
  details::auto_difference<bracket_less<int2>, int>::type
    diff = details::auto_difference_cast<bracket_less<int2>, int>
    (bracket_less<int2>());
  int2 p(0, 1);
  int2 q(2, 0);
  BOOST_CHECK_EQUAL(diff(0, p, q), -2);
  BOOST_CHECK_EQUAL(diff(1, p, q), 1);
}

BOOST_AUTO_TEST_CASE(test_difference_paren)
{
  details::auto_difference<paren_less<int2>, int>::type
    diff = details::auto_difference_cast<paren_less<int2>, int>
    (paren_less<int2>());
  int2 p(0, 1);
  int2 q(2, 0);
  BOOST_CHECK_EQUAL(diff(0, p, q), -2);
  BOOST_CHECK_EQUAL(diff(1, p, q), 1);
}

BOOST_AUTO_TEST_CASE(test_difference_iterator)
{
  details::auto_difference<iterator_less<int2>, int>::type
    diff = details::auto_difference_cast<iterator_less<int2>, int>
    (iterator_less<int2>());
  int2 p(0, 1);
  int2 q(2, 0);
  BOOST_CHECK_EQUAL(diff(0, p, q), -2);
  BOOST_CHECK_EQUAL(diff(1, p, q), 1);
}

BOOST_AUTO_TEST_CASE(test_difference_accessor)
{
  details::auto_difference<accessor_less<quad_access, quad>, int>::type
    diff = details::auto_difference_cast<accessor_less<quad_access, quad>, int>
    (accessor_less<quad_access, quad>());
  quad p(0, 1, 0, 0);
  quad q(2, 0, 0, 0);
  BOOST_CHECK_EQUAL(diff(0, p, q), -2);
  BOOST_CHECK_EQUAL(diff(1, p, q), 1);
}

BOOST_AUTO_TEST_CASE(test_euclid_distance_to_key)
{
  {
    // distance between 2 points at the same position should be null.
    double6 x; x.assign(.0);
    double r = math::euclid_distance_to_key
      <details::Static_rank<6>, double6, bracket_minus<double6, double>, double>
      (details::Static_rank<6>(), x, x, bracket_minus<double6, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
    x.assign(-1.);
    r = math::euclid_distance_to_key
      <details::Static_rank<6>, double6, bracket_minus<double6, double>, double>
      (details::Static_rank<6>(), x, x, bracket_minus<double6, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
    x.assign(1.);
    r = math::euclid_distance_to_key
      <details::Static_rank<6>, double6, bracket_minus<double6, double>, double>
      (details::Static_rank<6>(), x, x, bracket_minus<double6, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
  }
  {
    // Distance between 2 points at different positions in 3D
    for (int i=0; i<100; ++i)
      {
        double6 p = make_double6(drand(), drand(), drand(),
                                 drand(), drand(), drand());
        double6 q = make_double6(drand(), drand(), drand(),
                                 drand(), drand(), drand());
        double dist = math::euclid_distance_to_key
          <details::Static_rank<6>, double6, bracket_minus<double6, double>, double>
          (details::Static_rank<6>(), p, q, bracket_minus<double6, double>());
        using namespace ::std;
        double other_dist = sqrt((p[0] - q[0]) * (p[0] - q[0])
                                 + (p[1] - q[1]) * (p[1] - q[1])
                                 + (p[2] - q[2]) * (p[2] - q[2])
                                 + (p[3] - q[3]) * (p[3] - q[3])
                                 + (p[4] - q[4]) * (p[4] - q[4])
                                 + (p[5] - q[5]) * (p[5] - q[5]));
        BOOST_CHECK_CLOSE(dist, other_dist, .000000000001);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_euclidian_square_distance_to_key )
{
  {
    // distance between 2 points at the same position should be null.
    quad x(0, 0, 0, 0);
    int r = math::square_euclid_distance_to_key
      <details::Static_rank<4>, quad, accessor_minus<quad_access, quad, int>, int>
      (details::Static_rank<4>(), x, x, accessor_minus<quad_access, quad, int>());
    BOOST_CHECK_EQUAL(r, 0);
    x = quad(1, 1, 1, 1);
    r = math::square_euclid_distance_to_key
      <details::Static_rank<4>, quad, accessor_minus<quad_access, quad, int>, int>
      (details::Static_rank<4>(), x, x, accessor_minus<quad_access, quad, int>());
    BOOST_CHECK_EQUAL(r, 0);
    x = quad(-1, -1, -1, -1);
    r = math::square_euclid_distance_to_key
      <details::Static_rank<4>, quad, accessor_minus<quad_access, quad, int>, int>
      (details::Static_rank<4>(), x, x, accessor_minus<quad_access, quad, int>());
    BOOST_CHECK_EQUAL(r, 0);
  }
  {
    // Distance between 2 points at different positions in 3D
    for (int i=0; i<100; ++i)
      {
        quad p, q;
        p.x = rand() % 80 - 40;
        p.y = rand() % 80 - 40;
        p.z = rand() % 80 - 40;
        p.w = rand() % 80 - 40;
        q.x = rand() % 80 - 40;
        q.y = rand() % 80 - 40;
        q.z = rand() % 80 - 40;
        q.w = rand() % 80 - 40;
        int dist = math::square_euclid_distance_to_key
          <details::Static_rank<4>, quad, accessor_minus<quad_access, quad, int>, int>
          (details::Static_rank<4>(), p, q, accessor_minus<quad_access, quad, int>());
        int other_dist = (p.x-q.x)*(p.x-q.x) + (p.y-q.y)*(p.y-q.y)
          + (p.z-q.z)*(p.z-q.z) + (p.w-q.w)*(p.w-q.w);
        BOOST_CHECK_EQUAL(dist, other_dist);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_manhattan_distance_to_key )
{
  {
    // distance between 2 points at the same position should be null.
    quad x(0, 0, 0, 0);
    int r = math::manhattan_distance_to_key
      <details::Static_rank<4>, quad, accessor_minus<quad_access, quad, int>, int>
      (details::Static_rank<4>(), x, x, accessor_minus<quad_access, quad, int>());
    BOOST_CHECK_EQUAL(r, 0);
    x = quad(1, 1, 1, 1);
    r = math::manhattan_distance_to_key
      <details::Static_rank<4>, quad, accessor_minus<quad_access, quad, int>, int>
      (details::Static_rank<4>(), x, x, accessor_minus<quad_access, quad, int>());
    BOOST_CHECK_EQUAL(r, 0);
    x = quad(-1, -1, -1, -1);
    r = math::manhattan_distance_to_key
      <details::Static_rank<4>, quad, accessor_minus<quad_access, quad, int>, int>
      (details::Static_rank<4>(), x, x, accessor_minus<quad_access, quad, int>());
    BOOST_CHECK_EQUAL(r, 0);
  }
  {
    // Distance between 2 points at different positions in 3D
    for (int i=0; i<100; ++i)
      {
        quad p, q;
        p.x = rand() % 80 - 40;
        p.y = rand() % 80 - 40;
        p.z = rand() % 80 - 40;
        p.w = rand() % 80 - 40;
        q.x = rand() % 80 - 40;
        q.y = rand() % 80 - 40;
        q.z = rand() % 80 - 40;
        q.w = rand() % 80 - 40;
        int dist = math::manhattan_distance_to_key
          <details::Static_rank<4>, quad, accessor_minus<quad_access, quad, int>, int>
          (details::Static_rank<4>(), p, q, accessor_minus<quad_access, quad, int>());
        using namespace ::std;
        int other_dist = abs(p.x-q.x) + abs(p.y-q.y)
          + abs(p.z-q.z) + abs(p.w-q.w);
        BOOST_CHECK_EQUAL(dist, other_dist);
      }
  }
}

#endif // SPATIAL_TEST_METRIC_HPP
