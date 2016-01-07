#include <iostream>
#include <vector>
#include <sstream>

#include "../../src/point_multiset.hpp"
#include "../../src/idle_point_multiset.hpp"
#include "../../src/equal_iterator.hpp"

#include "chrono.hpp"
#include "point_type.hpp"

double total = 0.;

template <spatial::dimension_type N, typename Point>
void compare_libraries(std::size_t data_size)
{
  std::cout << "\t" << N << " dimensions, " << data_size << " objects:" << std::endl;
  std::vector<Point> data;
  data.reserve(data_size);
  Point p(1.0);
  for (std::size_t i = 0; i < data_size; ++i)
    data.push_back(p);
  {
    spatial::point_multiset<N, Point> cobaye;
    cobaye.insert(data.begin(), data.end());
    std::cout << "\t\tpoint_multiset:\t" << std::flush;
    utils::time_point start = utils::process_timer_now();
    for (spatial::equal_iterator<spatial::point_multiset<N, Point> >
           i = equal_begin(cobaye, p); i != equal_end(cobaye, p); ++i);
    utils::time_point stop = utils::process_timer_now();
    std::cout << (stop - start) << "sec" << std::endl;
    total += stop - start;
    std::cout << "\t\tpoint_multiset (reverse):\t" << std::flush;
    start = utils::process_timer_now();
    spatial::equal_iterator<spatial::point_multiset<N, Point> >
      i = equal_end(cobaye, p), end = equal_begin(cobaye, p);
    for (; i != end; --i);
    stop = utils::process_timer_now();
    std::cout << (stop - start) << "sec" << std::endl;
    total += stop - start;
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

  std::istringstream argbuf(argv[1]);
  std::size_t data_size;
  argbuf >> data_size;

  std::cout << "All points equal:" << std::endl;
  compare_libraries<3, point3_type>(data_size);
  compare_libraries<9, point9_type>(data_size);

  std::cout << "Total: " << total << std::endl;
}
