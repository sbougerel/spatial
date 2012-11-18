// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_test_geometry.hpp
 *  @brief  All tests for the elements defined in spatial_geometry.hpp are
 *  located in this file.
 *  @see spatial_geometry.hpp
 */

#ifndef SPATIAL_TEST_GEOMETRY_HPP
#define SPATIAL_TEST_GEOMETRY_HPP

BOOST_AUTO_TEST_CASE(test_geometry_trait)
{
  check_is_same
    <geometry_traits<square_euclidian<pointset<2, int2>, int,
                                      paren_minus<int2, int> > >::distance_type,
     square_euclidian<pointset<2, int2>, int,
                      paren_minus<int2, int> >::distance_type>();
  check_is_same
    <geometry_traits<square_euclidian<pointmap<6, double6, std::string>,
                                      double> >::distance_type,
     square_euclidian<pointmap<6, double6, std::string>,
                      double>::distance_type>();
  check_is_same
    <geometry_traits<euclidian<boxset<3, double6>, double,
                               bracket_minus<double6, double> > >::distance_type,
     euclidian<boxset<3, double6>, double,
               bracket_minus<double6, double> >::distance_type>();
  check_is_same
    <geometry_traits<euclidian<boxmap<3, double6, std::string>,
                               double> >::distance_type,
     euclidian<boxmap<3, double6, std::string>, double>::distance_type>();
  check_is_same
    <geometry_traits<manhattan<idle_pointset<2, int2>, int,
                               paren_minus<int2, int> > >::distance_type,
     manhattan<idle_pointset<2, int2>, int,
               paren_minus<int2, int> >::distance_type>();
  check_is_same
    <geometry_traits<manhattan<idle_pointmap<2, int2, std::string>,
                               int> >::distance_type,
     manhattan<idle_pointmap<2, int2, std::string>, int>::distance_type>();
}

BOOST_AUTO_TEST_CASE(test_difference_bracket)
{
  details::auto_difference<bracket_less<int2>, int>::type
    diff = details::difference_cast<bracket_less<int2>, int>
    (bracket_less<int2>());
  int2 p(0, 1);
  int2 q(2, 0);
  BOOST_CHECK_EQUAL(diff(0, p, q), -2);
  BOOST_CHECK_EQUAL(diff(1, p, q), 1);
}

BOOST_AUTO_TEST_CASE(test_difference_paren)
{
  details::auto_difference<paren_less<int2>, int>::type
    diff = details::difference_cast<paren_less<int2>, int>(paren_less<int2>());
  int2 p(0, 1);
  int2 q(2, 0);
  BOOST_CHECK_EQUAL(diff(0, p, q), -2);
  BOOST_CHECK_EQUAL(diff(1, p, q), 1);
}

BOOST_AUTO_TEST_CASE(test_difference_iterator)
{
  details::auto_difference<iterator_less<int2>, int>::type
    diff = details::difference_cast<iterator_less<int2>, int>
    (iterator_less<int2>());
  int2 p(0, 1);
  int2 q(2, 0);
  BOOST_CHECK_EQUAL(diff(0, p, q), -2);
  BOOST_CHECK_EQUAL(diff(1, p, q), 1);
}

BOOST_AUTO_TEST_CASE(test_difference_accessor)
{
  details::auto_difference<accessor_less<quad_access, quad>, int>::type
    diff = details::difference_cast<accessor_less<quad_access, quad>, int>
    (accessor_less<quad_access, quad>());
  quad p(0, 1, 0, 0);
  quad q(2, 0, 0, 0);
  BOOST_CHECK_EQUAL(diff(0, p, q), -2);
  BOOST_CHECK_EQUAL(diff(1, p, q), 1);
}

/*
BOOST_AUTO_TEST_CASE( test_euclidian_distance_to_key )
{
  using namespace spatial::details::geometry;
  {
    // distance between 2 points at the same position should be null.
    point2d x = zeros;
    double r = math::euclidian_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
    x = ones;
    r = math::euclidian_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
    x = twos;
    r = math::euclidian_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
    x = threes;
    r = math::euclidian_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
  }
  {
    // 2 point separated by 1 on each dim should return the right amount
    double r = math::euclidian_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, ones, bracket_cast_accessor<point2d, double>());
    using namespace std;
    BOOST_CHECK_CLOSE(r, sqrt(2.0), .000000000001);
  }
  {
    // Distance between 2 points at different positions in 3D
    for (int i=0; i<100; ++i)
      {
        triple p, q;
        p.x = rand() % 80 - 40;
        p.y = rand() % 80 - 40;
        p.z = rand() % 80 - 40;
        q.x = rand() % 80 - 40;
        q.y = rand() % 80 - 40;
        q.z = rand() % 80 - 40;
        double dist = math::euclidian_distance_to_key
          <triple, cast_accessor<triple, double, triple_access>, double>
          (3, p, q, cast_accessor<triple, double, triple_access>());
        using namespace ::std;
        double other_dist = sqrt(static_cast<double>((p.x-q.x)*(p.x-q.x)
                                 +(p.y-q.y)*(p.y-q.y)
                                 +(p.z-q.z)*(p.z-q.z)));
        BOOST_CHECK_CLOSE(dist, other_dist, .000000000001);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_euclidian_distance_to_plane )
{
  using namespace spatial::details::geometry;
  {
    // distance between points and plane at the same position should be null.
    point2d x = zeros;
    float r = math::euclidian_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (0, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
    x = ones;
    r = math::euclidian_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (1, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
    x = twos;
    r = math::euclidian_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (0, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
    x = threes;
    r = math::euclidian_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (1, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
  }
  {
    // Distance between points and plane at different positions in 3D
    for (int i=0; i<100; ++i)
      {
        triple p, q;
        p.x = rand() % 80 - 40;
        p.y = rand() % 80 - 40;
        p.z = rand() % 80 - 40;
        q.x = rand() % 80 - 40;
        q.y = rand() % 80 - 40;
        q.z = rand() % 80 - 40;
        dimension_type dim = static_cast<dimension_type>(rand()) % 3;
        float dist = math::euclidian_distance_to_plane
          <triple, cast_accessor<triple, double, triple_access>, float>
          (dim, p, q, cast_accessor<triple, double, triple_access>());
        using namespace ::std;
        triple_access access;
        float other_dist = abs(static_cast<float>(access(dim, p) - access(dim, q)));
        BOOST_CHECK_CLOSE(dist, other_dist, .0000001f);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_euclidian_square_distance_to_key )
{
  using namespace spatial::details::geometry;
  {
    // distance between 2 points at the same position should be null.
    point2d x = zeros;
    double r = math::euclidian_square_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
    x = ones;
    r = math::euclidian_square_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
    x = twos;
    r = math::euclidian_square_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
    x = threes;
    r = math::euclidian_square_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
  }
  {
    // 2 point separated by 1 on each dim should return the right amount
    double r = math::euclidian_square_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, ones, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, 2.0, .000000000001);
  }
  {
    // Distance between 2 points at different positions in 3D
    for (int i=0; i<100; ++i)
      {
        triple p, q;
        p.x = rand() % 80 - 40;
        p.y = rand() % 80 - 40;
        p.z = rand() % 80 - 40;
        q.x = rand() % 80 - 40;
        q.y = rand() % 80 - 40;
        q.z = rand() % 80 - 40;
        double dist = math::euclidian_square_distance_to_key
          <triple, cast_accessor<triple, double, triple_access>, double>
          (3, p, q, cast_accessor<triple, double, triple_access>());
        double other_dist = (p.x-q.x)*(p.x-q.x) + (p.y-q.y)*(p.y-q.y)
          + (p.z-q.z)*(p.z-q.z);
        BOOST_CHECK_CLOSE(dist, other_dist, .000000000001);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_euclidian_square_distance_to_plane )
{
  using namespace spatial::details::geometry;
  {
    // distance between points and plane at the same position should be null.
    point2d x = zeros;
    float r = math::euclidian_square_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (0, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
    x = ones;
    r = math::euclidian_square_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (1, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
    x = twos;
    r = math::euclidian_square_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (0, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
    x = threes;
    r = math::euclidian_square_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (1, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
  }
  {
    // Distance between points and plane at different positions in 3D
    for (int i=0; i<100; ++i)
      {
        triple p, q;
        p.x = rand() % 80 - 40;
        p.y = rand() % 80 - 40;
        p.z = rand() % 80 - 40;
        q.x = rand() % 80 - 40;
        q.y = rand() % 80 - 40;
        q.z = rand() % 80 - 40;
        dimension_type dim = static_cast<dimension_type>(rand()) % 3;
        float dist = math::euclidian_square_distance_to_plane
          <triple, cast_accessor<triple, double, triple_access>, float>
          (dim, p, q, cast_accessor<triple, double, triple_access>());
        using namespace ::std;
        triple_access access;
        float other_dist
          = static_cast<float>((access(dim, p) - access(dim, q))
                               * (access(dim, p) - access(dim, q)));
        BOOST_CHECK_CLOSE(dist, other_dist, .0000001f);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_manhattan_distance_to_key )
{
  using namespace spatial::details::geometry;
  {
    // distance between 2 points at the same position should be null.
    point2d x = zeros;
    double r = math::manhattan_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
    x = ones;
    r = math::manhattan_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
    x = twos;
    r = math::manhattan_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
    x = threes;
    r = math::manhattan_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
  }
  {
    // 2 point separated by 1 on each dim should return the right amount
    double r = math::manhattan_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, ones, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, 2.0, .000000000001);
  }
  {
    // Distance between 2 points at different positions in 3D
    for (int i=0; i<100; ++i)
      {
        triple p, q;
        p.x = rand() % 80 - 40;
        p.y = rand() % 80 - 40;
        p.z = rand() % 80 - 40;
        q.x = rand() % 80 - 40;
        q.y = rand() % 80 - 40;
        q.z = rand() % 80 - 40;
        double dist = math::manhattan_distance_to_key
          <triple, cast_accessor<triple, double, triple_access>, double>
          (3, p, q, cast_accessor<triple, double, triple_access>());
        using namespace ::std;
        double other_dist = abs(p.x-q.x) + abs(p.y-q.y) + abs(p.z-q.z);
        BOOST_CHECK_CLOSE(dist, other_dist, .000000000001);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_manhattan_distance_to_plane )
{
  using namespace spatial::details::geometry;
  {
    // distance between points and plane at the same position should be null.
    point2d x = zeros;
    float r = math::manhattan_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (0, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
    x = ones;
    r = math::manhattan_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (1, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
    x = twos;
    r = math::manhattan_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (0, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
    x = threes;
    r = math::manhattan_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (1, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
  }
  {
    // Distance between points and plane at different positions in 3D
    for (int i=0; i<100; ++i)
      {
        triple p, q;
        p.x = rand() % 80 - 40;
        p.y = rand() % 80 - 40;
        p.z = rand() % 80 - 40;
        q.x = rand() % 80 - 40;
        q.y = rand() % 80 - 40;
        q.z = rand() % 80 - 40;
        dimension_type dim = static_cast<dimension_type>(rand()) % 3;
        float dist = math::manhattan_distance_to_plane
          <triple, cast_accessor<triple, double, triple_access>, float>
          (dim, p, q, cast_accessor<triple, double, triple_access>());
        using namespace ::std;
        triple_access access;
        float other_dist = abs(static_cast<float>(access(dim, p) - access(dim, q)));
        BOOST_CHECK_CLOSE(dist, other_dist, .0000001f);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_geometry_euclidian_double )
{
  using namespace spatial::details::geometry;
  triple_access access;
  euclidian_double<triple, triple_access> geometry(access);
  // distance_type should be declared
  typedef euclidian_double<triple, triple_access>
    ::distance_type distance_type;
  {
    triple p, q;
    p.x = rand() % 80 - 40;
    p.y = rand() % 80 - 40;
    p.z = rand() % 80 - 40;
    q.x = rand() % 80 - 40;
    q.y = rand() % 80 - 40;
    q.z = rand() % 80 - 40;
    {
      distance_type r = geometry.distance_to_key(3, p, q);
      distance_type s = math::euclidian_distance_to_key
        <triple, triple_access, distance_type>(3, p, q, access);
      BOOST_CHECK_CLOSE(r, s, .000000000001);
    }
    {
      dimension_type dim = static_cast<dimension_type>(rand()) % 3;
      distance_type r = geometry.distance_to_plane(3, dim, p, q);
      distance_type s = math::euclidian_distance_to_plane
        <triple, cast_accessor<triple, distance_type, triple_access>,
         distance_type>(dim, p, q,
                        cast_accessor<triple, distance_type, triple_access>());
      BOOST_CHECK_CLOSE(r, s, .000000000001);
    }
  }
  {
    bracket_cast_accessor<point5d, double> access_double;
    euclidian_double<point5d, bracket_cast_accessor<point5d, double> >
      geometry_double(access_double);
    // Bad attempt to test that given any 2 points, the result of
    // distance_to_plane is always less or equal to distance to key
    {
      for(int i=0; i<10000; ++i)
        {
          point5d p, q;
          p[0] = rand_double();
          p[1] = rand_double();
          p[2] = rand_double();
          p[3] = rand_double();
          p[4] = rand_double();
          q[0] = p[0];
          q[1] = p[1];
          q[2] = -rand_double();
          q[3] = p[3];
          q[4] = p[4];
          distance_type r = geometry_double.distance_to_key(5, p, q);
          distance_type s = geometry_double.distance_to_plane(5, 2, p, q);
          BOOST_REQUIRE_MESSAGE
            (s <= r, std::setprecision(std::numeric_limits<double>::digits10)
             << r << " > " << s);
        }
    }
  }
}

BOOST_AUTO_TEST_CASE( test_geometry_euclidian_float )
{
  using namespace spatial::details::geometry;
  triple_access access;
  euclidian_float<triple, triple_access> geometry (access);
  // distance_type should be declared
  typedef euclidian_float<triple, triple_access>
    ::distance_type distance_type;
  {
    triple p, q;
    p.x = rand() % 80 - 40;
    p.y = rand() % 80 - 40;
    p.z = rand() % 80 - 40;
    q.x = rand() % 80 - 40;
    q.y = rand() % 80 - 40;
    q.z = rand() % 80 - 40;
    {
      distance_type r = geometry.distance_to_key(3, p, q);
      distance_type s = math::euclidian_distance_to_key
        <triple, triple_access, distance_type>(3, p, q, access);
      BOOST_CHECK_CLOSE(r, s, .0000001f);
    }
    {
      dimension_type dim = static_cast<dimension_type>(rand()) % 3;
      distance_type r = geometry.distance_to_plane(3, dim, p, q);
      distance_type s = math::euclidian_distance_to_plane
        <triple, triple_access, distance_type>(dim, p, q, access);
      BOOST_CHECK_CLOSE(r, s, .0000001f);
    }
  }
  {
    bracket_cast_accessor<fpoint5d, float> access_float;
    euclidian_float
      <fpoint5d, bracket_cast_accessor<fpoint5d, float> >
    geometry_float(access_float);
    // Bad attempt to test that given any 2 points, the result of
    // distance_to_plane is always less or equal to distance to key
    {
      for(int i=0; i<10000; ++i)
        {
          fpoint5d p, q;
          p[0] = rand_float();
          p[1] = rand_float();
          p[2] = rand_float();
          p[3] = rand_float();
          p[4] = rand_float();
          q[0] = p[0];
          q[1] = p[1];
          q[2] = -rand_float();
          q[3] = p[3];
          q[4] = p[4];
          distance_type r = geometry_float.distance_to_key(5, p, q);
          distance_type s = geometry_float.distance_to_plane(5, 2, p, q);
          BOOST_REQUIRE_MESSAGE
            (s <= r, std::setprecision(std::numeric_limits<float>::digits10)
             << r << " > " << s);
        }
    }
  }
}

BOOST_AUTO_TEST_CASE( test_geometry_euclidian_square_double )
{
  using namespace spatial::details::geometry;
  triple_access access;
  euclidian_square_double<triple, triple_access> geometry (access);
  // distance_type should be declared
  typedef euclidian_square_double<triple, triple_access>
    ::distance_type distance_type;
  {
    triple p, q;
    p.x = rand() % 80 - 40;
    p.y = rand() % 80 - 40;
    p.z = rand() % 80 - 40;
    q.x = rand() % 80 - 40;
    q.y = rand() % 80 - 40;
    q.z = rand() % 80 - 40;
    {
      distance_type r = geometry.distance_to_key(3, p, q);
      distance_type s = math::euclidian_square_distance_to_key
        <triple, triple_access, distance_type>(3, p, q, access);
      BOOST_CHECK_CLOSE(r, s, .000000000001);
    }
    {
      dimension_type dim = static_cast<dimension_type>(rand()) % 3;
      distance_type r = geometry.distance_to_plane(3, dim, p, q);
      distance_type s = math::euclidian_square_distance_to_plane
        <triple, triple_access, distance_type>(dim, p, q, access);
      BOOST_CHECK_CLOSE(r, s, .000000000001);
    }
  }
  {
    bracket_cast_accessor<point5d, double> access_double;
    euclidian_square_double<point5d, bracket_cast_accessor<point5d, double> >
    geometry_double(access_double);
    // Bad attempt to test that given any 2 points, the result of
    // distance_to_plane is always less or equal to distance to key
    {
      for(int i=0; i<10000; ++i)
        {
          point5d p, q;
          p[0] = rand_double();
          p[1] = rand_double();
          p[2] = rand_double();
          p[3] = rand_double();
          p[4] = rand_double();
          q[0] = p[0];
          q[1] = p[1];
          q[2] = -rand_double();
          q[3] = p[3];
          q[4] = p[4];
          distance_type r = geometry_double.distance_to_key(5, p, q);
          distance_type s = geometry_double.distance_to_plane(5, 2, p, q);
          BOOST_REQUIRE_MESSAGE
            (s <= r, std::setprecision(std::numeric_limits<double>::digits10)
             << r << " > " << s);
        }
    }
  }
}

BOOST_AUTO_TEST_CASE( test_geometry_euclidian_square_float )
{
  using namespace spatial::details::geometry;
  triple_access access;
  euclidian_square_float<triple, triple_access> geometry (access);
  // distance_type should be declared
  typedef euclidian_square_float<triple, triple_access>
    ::distance_type distance_type;
  {
    triple p, q;
    p.x = rand() % 80 - 40;
    p.y = rand() % 80 - 40;
    p.z = rand() % 80 - 40;
    q.x = rand() % 80 - 40;
    q.y = rand() % 80 - 40;
    q.z = rand() % 80 - 40;
    {
      distance_type r = geometry.distance_to_key(3, p, q);
      distance_type s = math::euclidian_square_distance_to_key
        <triple, triple_access, distance_type>(3, p, q, access);
      BOOST_CHECK_CLOSE(r, s, .0000001f);
    }
    {
      dimension_type dim = static_cast<dimension_type>(rand()) % 3;
      distance_type r = geometry.distance_to_plane(3, dim, p, q);
      distance_type s = math::euclidian_square_distance_to_plane
        <triple, triple_access, distance_type>(dim, p, q, access);
      BOOST_CHECK_CLOSE(r, s, .0000001f);
    }
  }
  {
    bracket_cast_accessor<fpoint5d, float> access_float;
    euclidian_square_float<fpoint5d, bracket_cast_accessor<fpoint5d, float> >
      geometry_float(access_float);
    // Bad attempt to test that given any 2 points, the result of
    // distance_to_plane is always less or equal to distance to key
    {
      for(int i=0; i<10000; ++i)
        {
          fpoint5d p, q;
          p[0] = rand_float();
          p[1] = rand_float();
          p[2] = rand_float();
          p[3] = rand_float();
          p[4] = rand_float();
          q[0] = p[0];
          q[1] = p[1];
          q[2] = -rand_float();
          q[3] = p[3];
          q[4] = p[4];
          distance_type r = geometry_float.distance_to_key(5, p, q);
          distance_type s = geometry_float.distance_to_plane(5, 2, p, q);
          BOOST_REQUIRE_MESSAGE
            (s <= r, std::setprecision(std::numeric_limits<float>::digits10)
             << r << " > " << s);
        }
    }
  }
}

BOOST_AUTO_TEST_CASE( test_geometry_manhattan )
{
  using namespace spatial::details::geometry;
  triple_access access;
  manhattan<triple, triple_access, int> geometry (access);
  {
    // distance_type should be declared
    typedef manhattan<triple, triple_access, int>
      ::distance_type distance_type;
    triple p, q;
    p.x = rand() % 80 - 40;
    p.y = rand() % 80 - 40;
    p.z = rand() % 80 - 40;
    q.x = rand() % 80 - 40;
    q.y = rand() % 80 - 40;
    q.z = rand() % 80 - 40;
    {
      distance_type r = geometry.distance_to_key(3, p, q);
      distance_type s = math::manhattan_distance_to_key
        <triple, triple_access, distance_type>(3, p, q, access);
      BOOST_CHECK_EQUAL(r, s);
    }
    {
      dimension_type dim = static_cast<dimension_type>(rand() % 3);
      distance_type r = geometry.distance_to_plane(3, dim, p, q);
      distance_type s = math::manhattan_distance_to_plane
        <triple, triple_access, distance_type>(dim, p, q, access);
      BOOST_CHECK_EQUAL(r, s);
    }
  }
  {
    bracket_cast_accessor<point5d, double> access_double;
    manhattan<point5d, bracket_cast_accessor<point5d, double>, double>
      geometry_double(access_double);
    typedef manhattan<point5d, bracket_cast_accessor<point5d, double>, double>
      ::distance_type distance_type;
    // Bad attempt to test that given any 2 points, the result of
    // distance_to_plane is always less or equal to distance to key
    {
      for(int i=0; i<10000; ++i)
        {
          point5d p, q;
          p[0] = rand_double();
          p[1] = rand_double();
          p[2] = rand_double();
          p[3] = rand_double();
          p[4] = rand_double();
          q[0] = p[0];
          q[1] = p[1];
          q[2] = -rand_double();
          q[3] = p[3];
          q[4] = p[4];
          distance_type r = geometry_double.distance_to_key(5, p, q);
          distance_type s = geometry_double.distance_to_plane(5, 2, p, q);
          BOOST_REQUIRE_MESSAGE
            (s <= r, std::setprecision(std::numeric_limits<double>::digits10)
             << r << " > " << s);
        }
    }
  }
}
*/

#endif // SPATIAL_TEST_GEOMETRY_HPP
