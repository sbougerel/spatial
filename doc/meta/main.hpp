/**
\mainpage Spatial Manual

- \ref intro
- \subpage changelog
- \subpage install
- \subpage structure
- \subpage tutorial

\prev \ref intro \next \ref changelog \endprevnext

\section intro Introduction

\quote
Man's mind, once stretched by a new idea, never regains its original
dimensions. <br> -- Oliver Wendell Holmes, Jr.
\endquote

After getting involved with \libkdtree, I had quite a number of new
ideas. \Spatial was started from scratch and it aims at providing a library that
retains all the great benefits of \libkdtree while broadening its horizons.

Like \libkdtree, \Spatial is a header-only C++ library providing template
multi-dimension in-memory containers that should be reusable and that should be
tuned to offer good overall performance in various situations. Like it's parent,
\Spatial borrows heavily from the \STL, and strive to follow \STL's interfaces
whenever possible.

Just like the \STL containers \c ::std::multiset and \c ::std::multimap,
\Spatial was designed to handle element insertion and removal from its
containers in logarithmic amortized time. Because of this characteristics,
\Spatial is particularly well-suited for tracking analysis, real-time
positioning, or geographic information systems.

Features include:

- multi-dimension template containers with unlimited number of dimensions to
  store points in space,
- dimensions may be of different types,
- static or dynamic time determination of the number of dimensions,
- <i>O(log(n))</i> class of complexity on insertion and removal,
- use custom predicate to define a range of elements,
- \STL-compatible iterators that go over that range of elements,
- flexible definition of a metric space for elements in the container,
- \STL-compatible iterators going from the nearest to the furthest neighbor
  of a point according to the metric space applied.

\prev \ref intro \next \ref changelog \endprevnext
*/

/**
\page changelog Change Log

- \ref ver1

\prev \ref intro \next \ref install \endprevnext

\section ver1 Version 1.0.0

- First version with:
  - multi-dimension template containers with unlimited number of dimensions to
    store points in space,
  - dimensions may be of different types,
  - static or dynamic time determination of the number of dimensions,
  - <i>O(log(n))</i> complexity on insertion and removal,
  - use custom predicate to define a range of elements,
  - \STL-compatible iterators that go over that range of elements,
  - flexible definition of a metric space for elements in the container,
  - \STL-compatible iterators going from the nearest to the furthest neighbor
    of a point according to the metric space applied.
- Documentation
- Examples

\prev \ref intro \next \ref install \endprevnext
*/

/**
\page install Installing the library

- \ref windows
- \ref linux
- \ref other_platforms

\prev \ref changelog \next \ref structure \endprevnext

\Spatial is a header only library, and all you have to do is to copy these
headers in the appropriate header directory for your project. Generally, you
must copy the contents of the \c src folder in a folder named \c spatial on your
computer.

If you choose to use the library for only one project, the license authorizes
you to copy its files directly in your project as long as you respect the
copyrights. See the \license.

I have no experience with Borland or Macs, and so any input is welcomed.

\section windows Windows and Visual Studio

If you are using the library for multiple shared projects and you wish to
upgrade to subsequent versions of the library, you can copy the \c src folder in
any directory that you find suitable, rename it to \c spatial, and add that
directory in the <em>Additional Include Directories</em> of the
<em>General</em>, <em>C/C++</em> properties of your project. You can now use the
library for your project by including in the source file:

\code
#include "spatial/poinset.hpp"
\endcode

If you wish to install the library in one of the default include directories of
Visual Studio, then copy the content of the \c src folder into:

\code
%Visual Studio location%\VC\include\
\endcode

Rename the folder to \c spatial and you're good to go. You can now use the
library for your project by including in the source file:

\code
#include <spatial/poinset.hpp>
\endcode

\section linux Linux with autotools/cmake

If you are using the library for several project and youe want the library to
reside in a different directory from your projects, such as \c "/usr/local/include/",
just copy the \c src folder of the library in there and rename it to \c
spatial. Then you must configure your \c "configure.ac" or \c "CMakeList.txt" file
to add \c "-I$HOME/include/" to the \c CPPFLAGS variable of autoconf or modify the
\c "include_directories()" directive of cmake.

You can now use the
library for your project by including in the source file:

\code
#include <spatial/poinset.hpp>
\endcode

\section other_platforms Other platforms

Copy the directory \c src and its content to the default C++ header location for
your compilation environement. You can now use the library for your project by
including in the source file:

\code
#include <spatial/pointset.hpp>
\endcode

Or copy the directory 'spatial' and its content to your own project and use the
library by including in the source file:

\code
#include "spatial/pointset.hpp"
\endcode

\prev \ref changelog \next \ref structure \endprevnext
*/

/**
\page structure Structure of the Library

- \ref nsspatial
- \ref nsview

\prev \ref install \next \ref tutorial \endprevnext

A few basic definitions will help you to gets through the examples of the \ref
tutorial "next section".

\section nsspatial Namespace ::spatial

- \c pointset: a container that stores points along a number of dimensions fixed
  at compile time.
- \c dynamic_pointset: a container that store points along a number of
  dimensions determined at run-time, upon user input for example. Additionally,
  declaring a variable of type \c pointset with a dimension of 0 is equivalent
  to using a \c dynamic_pointset.

These 2 containers provide you with only the very basic set of iterators. They
are fast, but they only iterate through each of the elements in the containers,
and are not following any order. To perform nearest neighbor search or
orthogonal range search, you must use views.

\section nsview Namespace ::spatial::view

View are providing the user with a wide range of additional operations that you
would normally expect on spatial containers, such as nearest neighbor search or
orthogonal range search, or even mapping all elements in the container to a
particular dimension.

There are several views provided in the library and only the most important are
presented here, for more information refer to the section on \ref view "views".

- \c view::mapping offers iterators that iterate through all the element of the
  containers ordered along one particular dimension. Did you ever felt that
  \Boost::bimap was not enough for you? That you needed a trimap or a quadmap?
  With this view, you basically transform your container in a \e n-map. You can
  store element over multiple dimensions in one container, but treat it has if
  you had a separate \c ::std::multiset container for each dimension, for example.
- \c view::range offers iterators that arbitrarily iterate through all the
  points contained within a <i>k</i>-dimensional region of space, defined by a
  \e low and a \e high component. \c view::range has a number of siblings such
  as \c view::close_range, or One of them, \c view::range_predicate is notable
  for allowing you to set your own predicate. These views provide orthogonal
  range search functionality over the containers.
- \c view::neighbor_euclidian offers iterators that iterate in order through all
  points that are nearest to a point of origin, from nearest to furthest and
  vice-versa. This view provide nearest neighbor search functionality over the
  containers, including finding the <i>n</i>-nearest element from the point of
  origin.

With this basic knowledge of the view and the iterators, you know enough to
get started.

\prev \ref install \next \ref tutorial \endprevnext
*/

/**
\page tutorial Getting Started

- \ref tutorial_include
- \ref tutorial_step1
- \ref tutorial_step2
- \ref tutorial_step3
- \ref tutorial_end

\prev \ref structure \next \ref tutorial_end \endprevnext

If you haven't already done so, I recommend you go through the section \ref
structure. This section will walk you through several basic usage scenarios
assuming you already have a minimum knowledge of what kind of \ref container
"containers" and \ref view "views" are provided by the library.

\section tutorial_include Include Spatial Header

After you have \ref install "installed" the library, you can begin using it by
including its header in your code.

\code
#include <spatial/poinset.hpp>
// ...

using namespace spatial;
\endcode

There is only one header to include in order to get access to \c pointset and \c
dynamic_pointset and the views that they support. In the future there should be
more header, but at the moment, it is the only one necessary.

If you are going to declare several \Spatial objects in your file, it's also
good to include the <tt>using namespace spatial;</tt> directive so you don't
have to prefix your each object with the namespace all the time. Nevertheless,
the rest of this guide will prefix each declaration with this namespace, as if
you did not use the \c using directive.

\section tutorial_step1 Step 1: Declaring the right container for your objects

Object that can be expressed through multi-dimension needs to define an accessor
to these dimensions. By default, if an object has overloaded the bracket operator
to access each of its dimension, then it can be inserted in the \c poinset,
right out of the box.

\code
#include <tr1/array>
#include <iostream>

typedef std::tr1::array<int, 3> point3d;

// Creation of a 3-dimensional container of point3d:
spatial::pointset<3, point3d> container;

// Creation of a few points:
point3d origin = { { 0, 0, 0 } };
point3d p1 = { { 432, 65, -32 } };
point3d p2 = { { 84, -2, -35 } };

// insertion of these points in the container
container.insert(origin);
container.insert(p1);
container.insert(p2);

std::cout << "There are " << container.size() << "elements in space." << std::end;
\endcode

\Spatial supports by default objects with overloaded bracket operator, because
the comparator \c spatial::bracket_less is the default comparator on the
containers. In the example above, you can also see how to insert or count
elements present in the container.

However, most objects do not overload the bracket operator. \Spatial supports 2
others ways of accessing dimensions in an object right out-of-the-box: if you
overloaded the parenthesis operator instead, or if you have iterators to access
each of your dimensions one by one. To declare a container for both cases, you
would use a \c spatial::paren_less or an \c spatial::iterator_less comparator.

\code
#include <tr1/array>
typedef std::tr1::array<int, 3> point3d;

// Access the dimensions in the point3d through the iterator
spatial::pointset<3, point3d, spatial::iterator_less<point3d> > container;

// Create a new type of point with overloaded parenthesis operator
struct other_point3d : point3d
{
  // ...
  int operator() (const size_t& index) const { return point3d::operator[](index); }
};

// Access the dimension in other_point3d through the parenthesis operator
spatial::pointset<3, point3d, spatial::paren_less<point3d> > other_container;
\endcode

That provides more flexibility so you can use containers such as
\c std::list inside the \Spatial container itself.

But in some cases this is still not enough: the only way to access your
dimensions may be through the use of independent members. In such case, you must
use the \c spatial::accessor_less comparator.

\code
// A point whose dimensions are accessed directly through it's members:
struct point3d
{
  int x;
  int y;
  int z;
};

#include <stdexcept> // for std::out_of_range()

// Remedy: define an accessor that takes a dimension_type as a parameter:
struct point3d_accessor
{
  int operator() (spatial::dimension_type dim) const
  {
    switch(dim)
      {
      case 0: return x;
      case 1: return y;
      case 2; return z;
      default: throw std::out_of_range();
      }
  }
};

// Now declare the container with the user-defined accessor
spatial::pointset<3, point3d,
		  spatial::accessor_less<point3d_accessor, point3d>
		  > other_container;
\endcode

Now you start to see that you can use a wide range of objects, inside your
container. However, one case is still not covered. What if you have an object
with different types for each dimension? You have to define your own
comparator. Did you notice that each of \c spatial::bracket_less, \c
spatial::paren_less, or \c spatial::iterator_less is actually a comparator
that wraps an accessor? If an accessor cannot be defined easily, then we
can create a new comparator, as presented below.

\code
#include <string>

// We want to create a library of books, according to several dimensions:
struct book
{
  std::string author;
  std::string title;
  int date;
};

// We define a comparator that compares 2 objects along one dimension
struct compare_book
{
  bool operator() (spatial::dimension_type n, const Book& x, const Book& y) const
  {
    switch(n)
      {
      case 0: return x.author < y.author;
      case 1: return x.title < y.title;
      case 2; return x.date < y.date;
      default: throw std::out_of_range();
      }
  }
};

// Now we declare our library
spatial::pointset<3, book, compare_book> library;
\endcode

With this covered, you can now store any kind of objects, in any number of
dimension that you wish inside your \Spatial containers. It doesn't take much
to understand how it all works if you are already familiar with the \STL.


\section tutorial_step2 Step 2: Basic operations on containers

Briefly, at the beginning of the last section, \ref tutorial_step1, we showed
how to insert elements inside the container. Now let's figure out how to
iterate through all the inserted elements, or finding out if the container
already contains an particular element.

\code
typedef spatial::pointset<3, point3d> container_type;
container_type container;

// ...
// let's suppose that plenty of point3d have been inserted in the container
// ...

for( container_type::const_iterator iter = container.begin();
     iter != container.end(); ++iter )
  {
    // do something with iter
  }
\endcode

Again, it works like any other container from the \STL, so finding out if a
particular element is present in the container or not, should not surprise
you either.

\code
// continued from above...

point3d point_to_find = { { 1, 2, 3 } };

if ( container.find(point_to_find) != container.end() )
  {
    // point_to_find is in the container!
  }
else
  {
    // humm... point_to_find is not in the container...
  }
\endcode

What about erasing a point then? As you would expect...

\code
// continued from above...

point3d point_to_erase = { { 6, 6, 6 } }; // better erase the beast...

if ( container.erase(point_to_erase) != 0 )
  {
    // erased all points that matched point_to_erase
  }
\endcode

Pretty straight-forward. Now while most basic operation closely
resemble the \STL containers, there are a few that are only meaniful for
the \Spatial containers.

One of them is the spatial::pointset::rebalance() function that, as its name
implies, balances all the element in the container. The subtility here, is when
do you actually call this function? If you are permanently inserting or erasing
elements in the container, and if you do a lot of searches in the container,
especially nearest neighbor searches, then you are out to call the rebalance()
function quite often, but not each time. It's a tricky business to find when to
call it.

Note that in the very near future I'll be completing the self-balancing
implementation, and when I do so you wont need to call this function anymore.


\section tutorial_step3 Step 3: Advanced operations on containers

Where the interesting stuff basically begin. Obviously, if you have used a
\Spatial container, it's because you probably wanted to do one of these things:
- find all objects within a certain range,
- find the nearest neighbor to some point,
- treat all dimensions as if they where a different way to index each points.

\subsection tutorial_range Orthogonal range search

There are several way to do orthogonal range search with \Spatial. In this
tutorial, we introduce only 2 ways, in the hope it give enough insight to
understand how to use the related orthogonal range search \ref view "views".

The most simple way is to use the spatial::view::range view. This view will let
you specify a \e low and a \e high value, and all points that satisfy the
following condition can be iterated in this view:

\f[
\forall _{i = 0 \to n} , low _i \leq x _i < high _i
\f]

This condition is the multi-dimension equivalent of an half closed interval,
commonly used when programing, and thus, it is the one you are most likely to be
needing.

\code
typedef spatial::pointset<3, point3d> container_type;
container_type container;
// ... let's imagine a lot of points have been inserted in the container

// define a range to explore
point3d low = { { -2, -2, -2 } };
point3d high = { { 2, 2, 2 } };
spatial::view::range<container_type> sample(container, low, high);

// find all the points between low and high
for (spatial::view::range<container_type>::iterator iter = sample.begin();
     iter != sample.end(); ++iter)
{
  // now do something with these points...
}
\endcode

As shown above above, we are defining a view on the container. All the access to
advanced functionalities is done through view which are declared similarly. It
doesn't take long to understand that most of the time, what we just want is to
iterate through all the points to do something with it, or copy them to a
separate container. There is a short cut for that, it works like std::for_each,
as highlighted below.

\code
typedef spatial::pointset<3, point3d> container_type;
container_type container;
// ... let's imagine a lot of points have been inserted in the container

// define a range to explore
point3d low = { { -2, -2, -2 } };
point3d high = { { 2, 2, 2 } };

// create a functor that is gonna do something with each points
struct functor
{
  void operator() (const point3d& p) const
  {
    // now do something with these points...
  }
};

spatial::for_each_in_range(container, low, high, functor);
\endcode

Now, both pieces of code are doing strictly the same thing, but depending on your
needs and your existing code, you might find one more useful than the other.

But what if we need to find all the points within a very particular orthogonal
range? For example we want all the points that satisfy a peculiar requirement such
as this one:

\f[
-1 < x _0 < 1,
-\infty < x _1 < \infty,
-\infty < x _2 < 2
\f]

Then we must define a predicate for this particular requirement and use it in the
orthogonal range search.

\code
typedef spatial::pointset<3, point3d> container_type;
container_type container;
// ... let's imagine again that a lot of points have been inserted in the container

// Now we define our predicate, it returns a spatial::relative_order type,
struct predicate
{
  spatial::relative_order
  operator()(dimension_type dim, const point3d& x) const
  {
    switch(dim)
      {
	// for dimension 0, only the interval ]-1, 1[ matches...
      case 0: return x[0] <= -1 ? spatial::below
                     : (x[0] >= 1 ? spatial::above : spatial::matching );
	// for dimension 1, it's always a match...
      case 1: return spatial::matching;
	// for dimension 2, matches unless it's equal or above 2...
      case 2: return x[2] < 2 ? spatial::matching : spatial::above;
	// else we must be out of range...
      default: throw std::out_of_range();
      }
  }
};

spatial::view::range_predicate<container_type, predicate>
  sample(container, predicate());

// find all the points that satisfy the predicate
for (spatial::view::range_predicate<container_type, predicate>
     ::iterator iter = sample.begin(); iter != sample.end(); ++iter)
{
  // now do something with these points...
}
\endcode

As you can see, the predicate must return a spatial::relative_order type.
spatial::relative_order defines 3 values: below, matching and above. These
values tell the algorithm the current postion of the key with regard to the
dimension that is being studied: is the key \b below the interval being
studied? Is it \b above? Or it is \b matching with the interval being studied?

While the predicate may not be intuitive to define, it give complete control
over the orthogonal range search that is being done on the container.

\Spatial comes with several orthogonal range search views predefined:
- spatial::view::range, as seen above, works on half closed intervals,
- spatial::view::closed_range is similar but works on closed intervals,
  both \a low and \a high are included in the search,
- spatial::view::opened_range is the exact opposite of
  spatial::view::closed_range, neither \a low nor \a high are included in the
  search

\subsection tutorial_ann Nearest neighbor search

Another kind of search that is commonly done on multi-dimension containers are
nearest neighbor search. \Spatial provide views with iterators that allow you to
iterate through the <i>n</i>-nearest points of an origin.

In order to find the nearest neighbor of a point, one must first apply a
geometry to the container. The container only orders point in a
multi-dimensional space, but it has no notion of distances. The geometry
provides metric and angles for all points in the container.

Note that since the container is independent of the geometry,
several different geometries can be applied to the same set of points,
therefore, without modifying the content of the container, you will be able
to compute nearest neighbor searches through with manhattan metric,
euclidian metric or even curved riemannian spaces (though no geometry
is currently available for these spaces, I'll be working on an Earth
geometry soon, for latitude-longitude-altitude computations)

At the moment, 3 different geometries are definied and each have a
corresponding view:

- spatial::view::neighbor_euclidian, uses the familiar euclidan metric and expect
  that all elements of the point can be cast into a double,
- spatial::view::neighbor_euclidian_float, is similar but uses float instead of
  doubles,
- spatial::view::neighbor_euclidian_square, also uses the familiar euclidian
  metric, but does not compute the square root of the distance (it leaves it
  for you to do, if you need to. This metric is just as precise and much faster
  than the first one, only you must remember that all distances are expressed
  at the square of their values,
- spatial::view::neighbor_euclidian_square_float, is self-explanatory
- spatial::view::neighbor_manhattan uses the famous Manhattan or taxicab metric
  (basically compute distances using orthogonal lines), and distances can be
  computed with any types (not only doubles or float) as long as they overload
  the assignment, addition, substraction and comparison operators.

Since all these \ref View "views" work in the same way, only one is presented
below. It is also possible to define your own geometry. If you wish to do so,
refer to the more complete explanation for the namespace
spatial::view::geometry.

\code



\endcode

\subsection tutorial_mapping Mapping of the container onto one dimension

Mapping makes your multi-dimension container behave as if it was only a
single-dimension container such as ::std::multiset.

Being able to map each dimension of the container independently gives you
the power of <i>n</i> containers merged into one, and it starts shifting
the perception of the library from a multi-dimension container to a embryo
of in-memory database, where every single field of your records are indexed
and can be searched by range of elements.

The spatial::view::mapping gives you just this power, and to illustrate it
better, we can start again from the earlier example of the book library.

\code
#include <string>

// We want to create a library of books, according to several dimensions:
struct book
{
  std::string author;
  std::string title;
  int date;
};

// We define a comparator that compares 2 objects along one dimension
struct compare_book
{
  bool operator() (spatial::dimension_type n, const Book& x, const Book& y) const
  {
    switch(n)
      {
      case 0: return x.author < y.author;
      case 1: return x.title < y.title;
      case 2; return x.date < y.date;
      default: throw std::out_of_range();
      }
  }
};

// Now we declare our library
spatial::pointset<3, book, compare_book> library;
\endcode

Now lets start doing a few searches. For exemples, getting all the books that
are written within 1981 to 1993, from the first to the last.

\code

\endcode

More similar queries can be done in this way, so you could find the authors
in lexicographic order, or the titles in lexicographic order, etc...

\section tutorial_end I need more examples...

I wanna do \a this, and it's not explained \a how? Well, you can try to have a
look at the \c example directory in the source of the library, it contains more
complete programs that would actually execute, but it is a work in progress and
you may not find what you are looking for. What about sending me a mail? I may
even take this opportunity to write an additional example, so that other may
benefit of your question.

There is a documentation for each example too, in the related pages.

\prev \ref structure \next \ref tutorial_end \endprevnext

*/

