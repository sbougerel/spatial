#include <exception>
#include <spatial/point_multiset.hpp>

// A point whose dimensions are accessed directly through it's members:
struct point3d
{
  int x;
  int y;
  int z;
};

// Remedy: define an accessor that takes a dimension_type as a parameter:
struct point3d_accessor
{
  int operator() (spatial::dimension_type dim, const point3d p) const
  {
    switch(dim)
      {
      case 0: return p.x;
      case 1: return p.y;
      case 2: return p.z;
      default: throw std::out_of_range("dim");
      }
  }
};

int main ()
{
  // Now declare the container with the user-defined accessor
  spatial::point_multiset<3, point3d,
                          spatial::accessor_less<point3d_accessor, point3d>
                          > point3d_container;
  point3d a = { 0, 1, 2 };
  point3d b = { 1, 2, 3 };
  point3d_container.insert(a);
  point3d_container.insert(b);
  return 0;
}
