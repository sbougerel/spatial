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

BOOST_AUTO_TEST_CASE( test_cast_accessor )
{
  using namespace spatial::details::geometry;
  cast_accessor<triple, int, triple_access> accessor;
  triple p(0, 1, 2);
  BOOST_CHECK_EQUAL(accessor(0, p), p.x);
  BOOST_CHECK_EQUAL(accessor(1, p), p.y);
  triple q(2, 3, 4);
  accessor(0, p, q);
  accessor(1, p, q);
  accessor(2, p, q);
  BOOST_CHECK_EQUAL(q.x, p.x);
  BOOST_CHECK_EQUAL(q.y, p.y);
  BOOST_CHECK_EQUAL(q.z, p.z);
}

BOOST_AUTO_TEST_CASE( test_bracket_cast_accessor )
{
  using namespace spatial::details::geometry;
  bracket_cast_accessor<point2d, int> accessor;
  point2d p = { { 0, 1 } };
  BOOST_CHECK_EQUAL(accessor(0, p), p[0]);
  BOOST_CHECK_EQUAL(accessor(1, p), p[1]);
  point2d q = { { 1, 0 } };
  accessor(0, p, q);
  accessor(1, p, q);
  BOOST_CHECK_EQUAL(q[0], p[0]);
  BOOST_CHECK_EQUAL(q[1], p[1]);
}

BOOST_AUTO_TEST_CASE( test_paren_cast_accessor )
{
  using namespace spatial::details::geometry;
  paren_cast_accessor<point2d_paren, int> accessor;
  point2d_paren p; p[0] = 0; p[1] = 1;
  BOOST_CHECK_EQUAL(accessor(0, p), p[0]);
  BOOST_CHECK_EQUAL(accessor(1, p), p[1]);
  point2d_paren q; q[0] = 1; q[1] = 0;
  accessor(0, p, q);
  accessor(1, p, q);
  BOOST_CHECK_EQUAL(q[0], p[0]);
  BOOST_CHECK_EQUAL(q[1], p[1]);
}

BOOST_AUTO_TEST_CASE( test_iterator_cast_accessor )
{
  using namespace spatial::details::geometry;
  iterator_cast_accessor<point2d, int> accessor;
  point2d p = { { 0, 1 } };
  BOOST_CHECK_EQUAL(accessor(0, p), p[0]);
  BOOST_CHECK_EQUAL(accessor(1, p), p[1]);
  point2d q = { { 1, 0 } };
  accessor(0, p, q);
  accessor(1, p, q);
  BOOST_CHECK_EQUAL(q[0], p[0]);
  BOOST_CHECK_EQUAL(q[1], p[1]);
}

BOOST_AUTO_TEST_CASE( test_accessor_rebind )
{
  using namespace spatial::details::geometry;
  {
    rebind<point2d, double,
                    accessor_less<at_accessor<point2d,
                                              int>, point2d> >::type
      must_compile;
  }
  {
    rebind<point2d, double,
                    bracket_less<point2d> >::type must_compile;
  }
  {
    rebind<point2d, double,
                    paren_less<point2d> >::type must_compile;
  }
  {
    rebind<point2d, double,
                    iterator_less<point2d> >::type must_compile;
  }
}

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

BOOST_AUTO_TEST_CASE( test_euclidian_distance_to_box_edge )
{
  using namespace spatial::details::geometry;
  {
    // distance between a point and a center at the same position should be null
    double r = math::euclidian_distance_to_box_edge
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, zeros, zeros, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
  }
  {
    // 2 point separated by 1 on each dim should return the right amount
    double r = math::euclidian_distance_to_box_edge
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, ones, ones, bracket_cast_accessor<point2d, double>());
    using namespace std;
    BOOST_CHECK_CLOSE(r, sqrt(2.0), .000000000001);
    r = math::euclidian_distance_to_box_edge
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, zeros, ones, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, sqrt(2.0), .000000000001);
  }
  {
    // Distance between 2 points at different positions in 3D
    for (int i=0; i<100; ++i)
      {
        triple p, l, h;
        p.x = rand() % 80 - 40;
        p.y = rand() % 80 - 40;
        p.z = rand() % 80 - 40;
        l.x = rand() % 80 - 40;
        l.y = rand() % 80 - 40;
        l.z = rand() % 80 - 40;
        h.x = rand() % 80 - 40;
        h.y = rand() % 80 - 40;
        h.z = rand() % 80 - 40;
        if (h.x < l.x) std::swap(h.x, l.x);
        if (h.y < l.y) std::swap(h.y, l.y);
        if (h.z < l.z) std::swap(h.z, l.z);
        double dist = math::euclidian_distance_to_box_edge
          <triple, cast_accessor<triple, double, triple_access>, double>
          (3, p, l, h, cast_accessor<triple, double, triple_access>());
        using namespace ::std;
        double dist_x
          = abs(static_cast<double>(p.x)-(static_cast<double>(l.x+h.x)/2.0))
          + (static_cast<double>(h.x-l.x)/2.0);
        double dist_y
          = abs(static_cast<double>(p.y)-(static_cast<double>(l.y+h.y)/2.0))
          + (static_cast<double>(h.y-l.y)/2.0);
        double dist_z
          = abs(static_cast<double>(p.z)-(static_cast<double>(l.z+h.z)/2.0))
          + (static_cast<double>(h.z-l.z)/2.0);
        double other_dist
          = sqrt(dist_x * dist_x + dist_y * dist_y + dist_z * dist_z);
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

BOOST_AUTO_TEST_CASE( test_euclidian_square_distance_to_box_edge )
{
  using namespace spatial::details::geometry;
  {
    // distance between a point and a box at the same position should be null
    double r = math::euclidian_square_distance_to_box_edge
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, zeros, zeros, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
  }
  {
    // point and box separated by 1 on each dim should return the right amount
    double r = math::euclidian_square_distance_to_box_edge
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, ones, ones, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, 2.0, .000000000001);
    r = math::euclidian_square_distance_to_box_edge
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, zeros, ones, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, 2.0, .000000000001);
  }
  {
    // Distance between 2 points at different positions in 3D
    for (int i=0; i<100; ++i)
      {
        triple p, l, h;
        p.x = rand() % 80 - 40;
        p.y = rand() % 80 - 40;
        p.z = rand() % 80 - 40;
        l.x = rand() % 80 - 40;
        l.y = rand() % 80 - 40;
        l.z = rand() % 80 - 40;
        h.x = rand() % 80 - 40;
        h.y = rand() % 80 - 40;
        h.z = rand() % 80 - 40;
        if (h.x < l.x) std::swap(h.x, l.x);
        if (h.y < l.y) std::swap(h.y, l.y);
        if (h.z < l.z) std::swap(h.z, l.z);
        double dist = math::euclidian_square_distance_to_box_edge
          <triple, cast_accessor<triple, double, triple_access>, double>
          (3, p, l, h, cast_accessor<triple, double, triple_access>());
        using namespace ::std;
        double dist_x
          = abs(static_cast<double>(p.x)-(static_cast<double>(l.x+h.x)/2.0))
          + (static_cast<double>(h.x-l.x)/2.0);
        double dist_y
          = abs(static_cast<double>(p.y)-(static_cast<double>(l.y+h.y)/2.0))
          + (static_cast<double>(h.y-l.y)/2.0);
        double dist_z
          = abs(static_cast<double>(p.z)-(static_cast<double>(l.z+h.z)/2.0))
          + (static_cast<double>(h.z-l.z)/2.0);
        double other_dist
          = dist_x * dist_x + dist_y * dist_y + dist_z * dist_z;
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

BOOST_AUTO_TEST_CASE( test_manhattan_distance_to_box_edge )
{
  using namespace spatial::details::geometry;
  {
    // distance between a point and a box at the same position should be null
    double r = math::manhattan_distance_to_box_edge
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, zeros, zeros, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
  }
  {
    // point and box separated by 1 on each dim should return the right amount
    double r = math::manhattan_distance_to_box_edge
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, ones, ones, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, 2.0, .000000000001);
    r = math::manhattan_distance_to_box_edge
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, zeros, ones, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, 2.0, .000000000001);
  }
  {
    // Distance between 2 points at different positions in 3D
    for (int i=0; i<100; ++i)
      {
        triple p, l, h;
        p.x = rand() % 80 - 40;
        p.y = rand() % 80 - 40;
        p.z = rand() % 80 - 40;
        l.x = rand() % 80 - 40;
        l.y = rand() % 80 - 40;
        l.z = rand() % 80 - 40;
        h.x = rand() % 80 - 40;
        h.y = rand() % 80 - 40;
        h.z = rand() % 80 - 40;
        if (h.x < l.x) std::swap(h.x, l.x);
        if (h.y < l.y) std::swap(h.y, l.y);
        if (h.z < l.z) std::swap(h.z, l.z);
        double dist = math::manhattan_distance_to_box_edge
          <triple, cast_accessor<triple, double, triple_access>, double>
          (3, p, l, h, cast_accessor<triple, double, triple_access>());
        using namespace ::std;
        double other_dist
          = abs(static_cast<double>(p.x)-(static_cast<double>(l.x+h.x)/2.0))
          + abs(static_cast<double>(h.x-l.x)/2.0)
          + abs(static_cast<double>(p.y)-(static_cast<double>(l.y+h.y)/2.0))
          + abs(static_cast<double>(h.y-l.y)/2.0)
          + abs(static_cast<double>(p.z)-(static_cast<double>(l.z+h.z)/2.0))
          + abs(static_cast<double>(h.z-l.z)/2.0);
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

#endif // SPATIAL_TEST_GEOMETRY_HPP
