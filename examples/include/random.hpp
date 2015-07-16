// -*- C++ -*-
#ifndef SPATIAL_EXAMPLE_UTILS_RANDOM_HPP
#define SPATIAL_EXAMPLE_UTILS_RANDOM_HPP

#include <cstdlib>
#include <ctime>
#include <cmath>

namespace utils
{
  namespace details
  {
    /// Return a random number for the type double.
    double randomize(double min, double max)
    {
      return static_cast<double>(::rand()) / static_cast<double>(RAND_MAX)
        * (max - min) + min;
    }

    /// Return a random number for the type int.
    int randomize(int min, int max)
    { return min + ::rand() % (max - min); }
  } // namespace details

  /// Seeds the generator
  struct random_engine
  {
    random_engine () { ::srand(::time(0)); }
  };

  /**
   *  This generator makes random doubles out of thin air.
   *
   *  The generator contains a single function designed to return a
   *  value of the type given in input.
   */
  class uniform_double_distribution
  {
  public:

    /// \defctor
    uniform_double_distribution
    (random_engine = random_engine(), double min = 0.0, double max = 1.0)
      : _min(min), _max(max) { }

    /// Uniformly pick a number
    double operator() () const
    {
      return details::randomize(_min, _max);
    }

  private:
    /// The lower bound
    double _min;

    /// The upper bound
    double _max;
  };

  /**
   *  This distribution returns number that approximate a normal distribution.
   */
  class normal_double_distribution
  {
  public:
    /// \defctor
    normal_double_distribution
    (random_engine = random_engine(), double min = 0.0, double max = 1.0)
      : _median((max + min) / 2),
        _radius((max - min) / 2) { }

    /// Generate the next pick
    double operator() () const
    {
      double a = details::randomize(-1.0, 1.0);
      double b = details::randomize(-1.0, 1.0);
      return (a * b * _radius) + _median;
    }

  private:
    /// The lower bound
    double _median;

    /// The upper bound
    double _radius;
  };

  /**
   *  This distribution returns numbers that follow a very narrow normal distribution.
   */
  class narrow_double_distribution
  {
  public:
    /// \defctor
    narrow_double_distribution
    (random_engine = random_engine(), double min = 0.0, double max = 1.0)
      : _median((max + min) / 2),
        _radius((max - min) / 2) { }

    /// Generate the next pick
    double operator() () const
    {
      double a = details::randomize(-1.0, 1.0);
      double b = details::randomize(-1.0, 1.0);
      double c = details::randomize(-1.0, 1.0);
      return (a * b * c * _radius) + _median;
    }

  private:
    /// The lower bound
    double _median;

    /// The upper bound
    double _radius;
  };

  template<N, Tp>
  class uniform_sphere_point
  {
  public:
    /// \defctor
    uniform_sphere_point
    (random_engine = random_engine(), double scaling_factor = 1.0)
      : _scale(scaling_factor) { }

    /// Generate the next pick using Marsaglia (1972) method which I found on
    /// mathworld.wolfram
    double operator() () const
    {
      Tp value; // must have bracket operator...
      double x1;
      double x2;
      do
        {
          x1 = detail::randomize(-1, 1);
          x2 = detail::randomize(-1, 1);
        }
      while (x1 * x1 + x2 * x2 >= 1);
      value[0] = 2 * x1 * std::sqrt(1 - x1 * x1 - x2 * x2);
      value[1] = 2 * x2 * std::sqrt(1 - x1 * x1 - x2 * x2);
      value[2] = 1 - 2 * (x1 * x1 + x2 * x2);
      return value;
    }

  private:
    /// The scaling factor
    double _scale;
  };
}

#endif // SPATIAL_EXAMPLE_UTILS_RANDOM_HPP
