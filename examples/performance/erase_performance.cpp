#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

#include <spatial/point_multiset.hpp>
#include <spatial/idle_point_multiset.hpp>
#include <kdtree++/kdtree.hpp>

#include "../include/chrono.hpp"
#include "../include/random.hpp"
#include "../include/point_type.hpp"

/**
 *  \file erase_performance.cpp Compare the different libraries on erasing
 *  points by value; \point_multiset, \idle_point_multiset and KDTree containers
 *  from libkdtree++.
 *
 *  It is expected here that \idle_point_multiset is the best performer while
 *  \point_multiset is the worst performer. KDTree containers should be in
 *  between.
 *
 *  \point_multiset should perform worse than both \idle_point_multiset and
 *  KDTree. Firstly, both are optimized (ideally balanced) before all points are
 *  being erased one by one, which should naturally reduce the amount of tree
 *  walking necessary to find the next node to erase, at the
 *  beginning. Additionally, on every erase, \point_multiset will attempt to
 *  re-balance itself while the other 2 containers won't. Here we use the
 *  default rebalancing strategy (\ref spatial::loose_balancing) for the
 *  comparison.
 *
 *  \idle_point_multiset is expected to perform better. It is a better
 *  performer than the other 2 containers to locate a node by value, which is
 *  the largest task when erasing; unless you also need to rebalance.
 *
 *  Finally, on this test, KDTree is at a slight advantage than the 2 other
 *  containers. Both \idle_point_multiset and \point_multiset attempt to erase
 *  \em all elements matching the value given; similarly to
 *  <tt>std::multiset</tt>. On the contrary, KDTree will erase \em any first
 *  element it finds matching the key. This constraint means
 *  \idle_point_multiset and \point_multiset must do a little more work at each
 *  call of erase, trying to find if there is another match in the tree.
 */

int random_integer (int i) { return std::rand()%i;}

template <spatial::dimension_type N, typename Point, typename Distribution>
void compare_libraries
(std::size_t data_size, const Distribution& distribution)
{
  std::cout << "\t" << N << " dimensions, " << data_size << " objects:" << std::endl;
  std::vector<Point> data;
  data.reserve(data_size);
  for (size_t i = 0; i < data_size; ++i)
    data.push_back(Point(distribution));
  {
    // Erase into a point_multiset
    std::cout << "\t\tpoint_multiset:\t" << std::flush;
    spatial::point_multiset<N, Point> cobaye;
    cobaye.insert(data.begin(), data.end());
    std::random_shuffle(data.begin(), data.end(), random_integer);
    utils::time_point start = utils::process_timer_now();
    for (typename std::vector<Point>::const_iterator i = data.begin();
         i != data.end(); ++i)
      cobaye.erase(*i);
    utils::time_point stop = utils::process_timer_now();
    std::cout << (stop - start) << "sec" << std::endl;
  }
  {
    // Erase into an idle_point_multiset
    std::cout << "\t\tidle_point_multiset:\t" << std::flush;
    spatial::idle_point_multiset<N, Point> cobaye;
    cobaye.insert_rebalance(data.begin(), data.end());
    std::random_shuffle(data.begin(), data.end(), random_integer);
    utils::time_point start = utils::process_timer_now();
    for (typename std::vector<Point>::const_iterator i = data.begin();
         i != data.end(); ++i)
      cobaye.erase(*i);
    utils::time_point stop = utils::process_timer_now();
    std::cout << (stop - start) << "sec" << std::endl;
  }
  {
    // Insert into a KDtree
    std::cout << "\t\tKDtree:\t" << std::flush;
    KDTree::KDTree<N, Point> cobaye;
    cobaye.insert(data.begin(), data.end());
    cobaye.optimise();
    std::random_shuffle(data.begin(), data.end(), random_integer);
    utils::time_point start = utils::process_timer_now();
    for (typename std::vector<Point>::const_iterator i = data.begin();
         i != data.end(); ++i)
      cobaye.erase(*i);
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
