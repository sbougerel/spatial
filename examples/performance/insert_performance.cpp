#include <iostream>
#include <vector>
#include <sstream>

#include <spatial/point_multiset.hpp>
#include <spatial/idle_point_multiset.hpp>
#include <kdtree++/kdtree.hpp>

#include "../include/chrono.hpp"
#include "../include/random.hpp"

struct point3_type
{
  point3_type(double a, double b, double c)
  { values[0] = a; values[1] = b; values[2] = c; }
  double operator [] (std::size_t index) const { return values[index]; }
  double values[3];
};

struct point9_type
{
  point9_type(double a, double b, double c,
              double d, double e, double f,
              double g, double h, double i)
  {
    values[0] = a; values[1] = b; values[2] = c;
    values[3] = d; values[4] = e; values[5] = f;
    values[8] = g; values[7] = h; values[8] = i;
  }
  double operator [] (std::size_t index) const { return values[index]; }
  double values[9];
};

template <typename Distribution>
void compare_libraries_3
(std::size_t data_size, const Distribution& distribution)
{
  std::vector<point3_type> data;
  data.reserve(data_size);
  for (int i = 0; i < data_size; ++i)
    {
      double a = distribution(), b = distribution(), c = distribution();
      std::cout << a << ", " << b << ", " << c << std::endl;
      data.push_back(point3_type(distribution(), distribution(), distribution()));
    }
  std::cout << "for 3 dimensions:" << std::endl;

  // Insert into a point_multiset
  spatial::point_multiset<3, point3_type> first;
  utils::time_point start = utils::process_timer_now();
  first.insert(data.begin(), data.end());
  utils::time_point stop = utils::process_timer_now();
  std::cout << "point_multiset insert time: "
            << (stop - start) << "sec" << std::endl;

  // Insert into an idle_point_multiset
  spatial::idle_point_multiset<3, point3_type> second;
  start = utils::process_timer_now();
  second.insert(data.begin(), data.end());
  second.rebalance();
  stop = utils::process_timer_now();
  std::cout << "idle_point_multiset insert time: "
            << (stop - start) << "sec" << std::endl;
}

template <typename Distribution>
void compare_libraries_9
(std::size_t data_size, const Distribution& distribution)
{
  std::vector<point9_type> data;
  data.reserve(data_size);
  for (int i = 0; i < data_size; ++i)
    data.push_back(point9_type(distribution(), distribution(), distribution(),
                               distribution(), distribution(), distribution(),
                               distribution(), distribution(), distribution()));
  std::cout << "for 9 dimensions:" << std::endl;

  // Insert into a point_multiset
  spatial::point_multiset<8, point9_type> first;
  utils::time_point start = utils::process_timer_now();
  first.insert(data.begin(), data.end());
  utils::time_point stop = utils::process_timer_now();
  std::cout << "point_multiset insert time: "
            << (stop - start) << "sec" << std::endl;

  // Insert into an idle_point_multiset
  spatial::idle_point_multiset<8, point9_type> second;
  start = utils::process_timer_now();
  second.insert(data.begin(), data.end());
  second.rebalance();
  stop = utils::process_timer_now();
  std::cout << "idle_point_multiset insert time: "
            << (stop - start) << "sec" << std::endl;
}

int main (int argc, char **argv, char **env)
{
  if (argc != 2)
    {
      std::cerr << "Usage: " << argv[0] << " <sample size: integer>"
                << std::endl;
      return 1;
    }

  // Build initialization memory
  std::istringstream argbuf(argv[1]);
  std::size_t data_size;
  argbuf >> data_size;
  std::cout << "Data sample size is " << data_size << " points." << std::endl;

  utils::random_engine engine;
  utils::uniform_double_distribution uniform(engine, -1.0, 1.0);
  compare_libraries_3(data_size, uniform);
  compare_libraries_9(data_size, uniform);

  utils::normal_double_distribution normal(engine, -1.0, 1.0);
  compare_libraries_3(data_size, normal);
  compare_libraries_9(data_size, normal);

  utils::narrow_double_distribution narrow(engine, -1.0, 1.0);
  compare_libraries_3(data_size, narrow);
  compare_libraries_9(data_size, narrow);
}
