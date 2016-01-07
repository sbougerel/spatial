// -*- C++ -*-

#include <iostream>
#include <sstream>
#include "random.hpp"

template <typename Distrib>
void run_distribution(int size, const Distrib& distrib)
{
  double mean = 0;
  double deviation = 0;
  std::cout << "(" << std::flush;
  for (int i = 0; i < size; ++i, std::cout << ", " << std::flush)
    {
      double sample = distrib();
      std::cout << sample << std::flush;
      mean += sample;
      deviation += sample > 0 ? sample : -sample;
    }
  std::cout << ")" << std::endl;
  std::cout << "\tMean: " << mean / size << std::endl;
  std::cout << "\tDeviation: " << deviation / size << std::endl;
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
  utils::random_engine engine(46728843); // constant seed for repeatable tests

  std::cout << "Uniform distribution (-1.0, 1.0):" << std::endl;
  utils::uniform_double_distribution uniform(engine, -1.0, 1.0);
  run_distribution(data_size, uniform);

  std::cout << "Normal distribution (-1.0, 1.0):" << std::endl;
  utils::normal_double_distribution normal(engine, -1.0, 1.0);
  run_distribution(data_size, normal);

  std::cout << "Narrow normal distribution (-1.0, 1.0):" << std::endl;
  utils::narrow_double_distribution narrow(engine, -1.0, 1.0);
  run_distribution(data_size, narrow);
}
