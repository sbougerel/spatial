#include <iostream>
#include <vector>
#include <sstream>

#include <spatial/point_multiset.hpp>
#include <spatial/idle_point_multiset.hpp>
#include <spatial/neighbor_iterator.hpp>

#include "../include/chrono.hpp"
#include "../include/random.hpp"
#include "../include/point_type.hpp"

template <spatial::dimension_type N, typename Point, typename Distribution>
void compare_libraries
(std::size_t data_size, const Distribution& distribution)
{
  std::cout << "\t" << N << " dimensions, " << data_size << " objects:" << std::endl;
  std::vector<Point> data;
  std::vector<Point> targets;
  data.reserve(data_size);
  targets.reserve(data_size);
  for (std::size_t i = 0; i < data_size; ++i)
    {
      data.push_back(Point(distribution));
      targets.push_back(Point(distribution));
    }
  {
    // Nearest neighbor begin into an idle_point_multiset
    std::cout << "\t\tidle_point_multiset:\t" << std::flush;
    spatial::idle_point_multiset<N, Point> cobaye;
    cobaye.insert_rebalance(data.begin(), data.end());
    utils::time_point start = utils::process_timer_now();
    for (typename std::vector<Point>::const_iterator
           i = targets.begin(); i != targets.end(); ++i)
      neighbor_lower_bound(cobaye, *i, (*i)[0]);
    utils::time_point stop = utils::process_timer_now();
    std::cout << (stop - start) << "sec" << std::endl;
  }
  {
    // Nearest neighbor begin into an idle_point_multiset
    std::cout << "\t\tpoint_multiset:\t" << std::flush;
    spatial::point_multiset<N, Point> cobaye;
    cobaye.insert(data.begin(), data.end());
    utils::time_point start = utils::process_timer_now();
    for (typename std::vector<Point>::const_iterator
           i = targets.begin(); i != targets.end(); ++i)
      neighbor_lower_bound(cobaye, *i, (*i)[0]);
    utils::time_point stop = utils::process_timer_now();
    std::cout << (stop - start) << "sec" << std::endl;
  }
}

int main (int argc, char **argv)
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

  std::cout << "Uniform distribution:" << std::endl;
  utils::uniform_double_distribution uniform(engine, -1.0, 1.0);
  compare_libraries<3, point3_type, utils::uniform_double_distribution>
    (data_size, uniform);
  compare_libraries<9, point9_type, utils::uniform_double_distribution>
    (data_size, uniform);

  std::cout << "Normal distribution:" << std::endl;
  utils::normal_double_distribution normal(engine, -1.0, 1.0);
  compare_libraries<3, point3_type, utils::normal_double_distribution>
    (data_size, normal);
  compare_libraries<9, point9_type, utils::normal_double_distribution>
    (data_size, normal);

  std::cout << "Narrow normal distribution:" << std::endl;
  utils::narrow_double_distribution narrow(engine, -1.0, 1.0);
  compare_libraries<3, point3_type, utils::narrow_double_distribution>
    (data_size, narrow);
  compare_libraries<9, point9_type, utils::narrow_double_distribution>
    (data_size, narrow);
}
