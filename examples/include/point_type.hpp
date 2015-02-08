// -*- C++ -*-
#ifndef SPATIAL_EXAMPLE_POINT_TYPE_HPP
#define SPATIAL_EXAMPLE_POINT_TYPE_HPP

#include <algorithm> // std::fill

struct point3_type
{
  typedef double value_type;
  point3_type() { }
  explicit point3_type (double value)
  { std::fill(values, values + 9, value); }
  template <typename Distribution>
  explicit point3_type(const Distribution& distrib)
  { values[0] = distrib(); values[1] = distrib(); values[2] = distrib(); }
  double operator [] (std::size_t index) const { return values[index]; }
  double values[3];
};

struct point9_type
{
  typedef double value_type;
  point9_type() { }
  explicit point9_type (double value)
  { std::fill(values, values + 9, value); }
  template <typename Distribution>
  explicit point9_type(const Distribution& distrib)
  {
    values[0] = distrib(); values[1] = distrib(); values[2] = distrib();
    values[3] = distrib(); values[4] = distrib(); values[5] = distrib();
    values[6] = distrib(); values[7] = distrib(); values[8] = distrib();
  }
  double operator [] (std::size_t index) const { return values[index]; }
  double values[9];
};

#endif // SPATIAL_EXAMPLE_POINT_TYPE_HPP
