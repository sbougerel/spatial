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
  template <typename Distribution>
  point3_type(const Distribution& distrib)
  { values[0] = distrib(); values[1] = distrib(); values[2] = distrib(); }
  double operator [] (std::size_t index) const { return values[index]; }
  double values[3];
};

struct point9_type
{
  template <typename Distribution>
  point9_type(const Distribution& distrib)
  {
    values[0] = distrib(); values[1] = distrib(); values[2] = distrib();
    values[3] = distrib(); values[4] = distrib(); values[5] = distrib();
    values[8] = distrib(); values[7] = distrib(); values[8] = distrib();
  }
  double operator [] (std::size_t index) const { return values[index]; }
  double values[9];
};

template <spatial::dimension_type N, typename Point, typename Distribution>
void compare_libraries
(std::size_t data_size, const Distribution& distribution)
{
  std::vector<Point> data;
  data.reserve(data_size);
  for (int i = 0; i < data_size; ++i)
    data.push_back(Point(distribution));

  // Insert into a point_multiset
  std::cout << "\t" << N << " dimensions\t"
            << data_size << " objects\tpoint_multiset:\t" << std::flush;
  spatial::point_multiset<N, Point> first;
  utils::time_point start = utils::process_timer_now();
  first.insert(data.begin(), data.end());
  utils::time_point stop = utils::process_timer_now();
  std::cout << (stop - start) << "sec" << std::endl;

  // Insert into an idle_point_multiset
  std::cout << "\t" << N << " dimensions\t"
            << data_size << " objects\tidle_point_multiset:\t" << std::flush;
  spatial::idle_point_multiset<N, Point> second;
  start = utils::process_timer_now();
  second.insert(data.begin(), data.end());
  second.rebalance();
  stop = utils::process_timer_now();
  std::cout << (stop - start) << "sec" << std::endl;
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
  utils::random_engine engine;

  std::cout << "Uniform distribution" << std::endl;
  utils::uniform_double_distribution uniform(engine, -1.0, 1.0);
  compare_libraries<3, point3_type, utils::uniform_double_distribution>
    (data_size, uniform);
  compare_libraries<9, point9_type, utils::uniform_double_distribution>
    (data_size, uniform);

  std::cout << "Normal distribution" << std::endl;
  utils::normal_double_distribution normal(engine, -1.0, 1.0);
  compare_libraries<3, point3_type, utils::normal_double_distribution>
    (data_size, normal);
  compare_libraries<9, point9_type, utils::normal_double_distribution>
    (data_size, normal);

  std::cout << "Narrow normal distribution" << std::endl;
  utils::narrow_double_distribution narrow(engine, -1.0, 1.0);
  compare_libraries<3, point3_type, utils::narrow_double_distribution>
    (data_size, narrow);
  compare_libraries<9, point9_type, utils::narrow_double_distribution>
    (data_size, narrow);
}
