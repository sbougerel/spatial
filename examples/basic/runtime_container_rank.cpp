#include <iostream>
#include <tr1/array> // TR1 array
#include <spatial/point_multiset.hpp>

// Using a rank of 0 for the container, allows you to determine the dimension at
// runtime, as a parameter of the constructor of the conatiner.
int main(int argc, char** argv)
{
  // Dimension of 0: This container's rank (or dimension) will be determined at
  // runtime
  typedef spatial::point_multiset<0, std::tr1::array<int, 3> >
    runtime_container;

  std::cout << "Enter a dimension for container: " << std::flush;
  spatial::dimension_type dim;
  std::cin >> dim;

  // If we are not interested in dealing with ranks larger than 20
  if (dim >= 20) throw spatial::invalid_dimension("dim");

  runtime_container container(dim); // Note: if dim was equal to 0,
                                    // spatial::invalid_dimension would be
                                    // thrown

  std::cout << "container rank is: " << container.dimension() << std::endl;
  return 0;
}
