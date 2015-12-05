// -*- C++ -*-
#ifndef SPATIAL_EXAMPLE_UTILS_CHRONO_HPP
#define SPATIAL_EXAMPLE_UTILS_CHRONO_HPP

#include <ctime>

namespace utils
{
  /// Manage a time point and compute its difference
  class time_point
  {
  public:
    /// \defctor
    time_point() { }

    /// Initialized from the clock
    time_point(::clock_t instant) : system_time(instant) { }

    /// The difference between 2 time points
    typedef double difference_type;

    /// Compute the difference between 2 time points
    difference_type difference (const time_point& rhs) const
    {
      return static_cast<double>(system_time - rhs.system_time)
        / CLOCKS_PER_SEC;
    }

  private:
    /// The time_point value
    ::clock_t system_time;
  };

  /// Defines the difference between 2 time with the minus operator
  inline time_point::difference_type operator-
  (const time_point& lhs, const time_point& rhs)
  { return lhs.difference(rhs); }

  /// Return the process time
  inline time_point process_timer_now()
  {
    return time_point(::clock());
  }
}

#endif // SPATIAL_EXAMPLE_UTILS_CHRONO_HPP
