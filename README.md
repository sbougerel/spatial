# Spatial C++ Library

Generic multi-dimensional containers for C++, with iterators
over orthogonal ranges or nearest neighbors, modeled after the STL.

Web: https://sourceforge.net/projects/spatial/


## Feature overview

- Static (compile-time) or dynamic (run-time) determination of the number of
  dimensions

- Generic design that mimic STL containers, and iterators

- Feature rebalancing k-d tree preventing worst-case run time on many
  operations

- Iteration over orthogonal range with custom predicates and support for points
  or boxes

- Iteration over neighbors, from nearest to furthest with custom metrics

- Iteration over a single dimension, independantly from others

- Heavily unit-tested and production ready



## Installation

Spatial is a header-only library and all you have to do is to copy the content
of the 'src' folder in a folder named 'spatial' under the appropriate header
directory for your project or your environment.

Typically on Linux, this just means:

```
$ sudo cp -a src/* /usr/local/include/spatial/
```

Using MSVC++ on Windows, you may install them under:
```
%Visual Studio location%\VC\include\spatial\
```

Once the above is done, you should be able to simply use the library by adding:

```
#include <spatial/pointset.hpp>
```

#
