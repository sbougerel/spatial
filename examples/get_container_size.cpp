#include <tr1/array>
#include <iostream>
#include <spatial/point_multiset.hpp>

int main()
{
  typedef std::tr1::array<int, 3> point3d;

  // Creation of a 3-dimensional container of point3d:
  spatial::point_multiset<3, point3d> container;

  // Creation of a few points:
  point3d origin = { { 0, 0, 0 } };
  point3d p1 = { { 432, 65, -32 } };
  point3d p2 = { { 84, -2, -35 } };

  // insertion of these points in the container
  container.insert(origin);
  container.insert(p1);
  container.insert(p2);

  std::cout << "There are " << container.size()
            << " elements in space." << std::endl;
  return 0;
}
