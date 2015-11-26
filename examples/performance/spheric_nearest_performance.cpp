#include <iostream>
#include <vector>
#include <sstream>

#include <spatial/point_multiset.hpp>
#include <spatial/idle_point_multiset.hpp>
#include <spatial/neighbor_iterator.hpp>
#include <kdtree++/kdtree.hpp>

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
      data.push_back(distribution());
      targets.push_back(distribution(0.01));
    }
  {
    // Nearest neighbor begin into an idle_point_multiset
    spatial::idle_point_multiset<N, Point> cobaye;
    cobaye.insert_rebalance(data.begin(), data.end());
    std::cout << "\t\tidle_point_multiset:\t" << std::flush;
    utils::time_point start = utils::process_timer_now();
    for (typename std::vector<Point>::const_iterator
           i = targets.begin(); i != targets.end(); ++i)
      neighbor_begin(cobaye, *i);
    utils::time_point stop = utils::process_timer_now();
    std::cout << (stop - start) << "sec" << std::endl;
  }
  {
    // Nearest neighbor begin into an idle_point_multiset
    spatial::point_multiset<N, Point> cobaye;
    cobaye.insert(data.begin(), data.end());
    std::cout << "\t\tpoint_multiset:\t" << std::flush;
    utils::time_point start = utils::process_timer_now();
    for (typename std::vector<Point>::const_iterator
           i = targets.begin(); i != targets.end(); ++i)
      neighbor_begin(cobaye, *i);
    utils::time_point stop = utils::process_timer_now();
    std::cout << (stop - start) << "sec" << std::endl;
  }
  {
    // Nearest neighbor into an KDtree
    KDTree::KDTree<N, Point> cobaye;
    cobaye.insert(data.begin(), data.end());
    cobaye.optimise();
    std::cout << "\t\tKDtree:\t" << std::flush;
    utils::time_point start = utils::process_timer_now();
    for (typename std::vector<Point>::const_iterator
           i = targets.begin(); i != targets.end(); ++i)
      cobaye.find_nearest(*i);
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

  std::cout << "Uniform sphere distribution:" << std::endl;
  utils::uniform_sphere_distribution<point3_type> uniform(engine);
  compare_libraries<3, point3_type, utils::uniform_sphere_distribution<point3_type> >
    (data_size, uniform);
}
