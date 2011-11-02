// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   fixtures.hpp
 *  @brief  Contains the list of all fixtures required for the test in the
 *  library.
 */

#ifndef SPATIAL_FIXTURES_HPP
#define SPATIAL_FIXTURES_HPP

using details::Node_base;
using details::Relaxed_kdtree_node;
using details::Kdtree
using details::Kdtree_node;

typedef std::tr1::array<int, 2> point2d;
typedef std::pair<int, int> pair_type;

point2d zeros = { { 0, 0 } };
point2d ones = { { 1, 1 } };
point2d twos = { { 2, 2 } };
point2d threes = { { 3, 3 } };
point2d fours = { { 4, 4 } };
point2d fives = { { 5, 5 } };

void swap(point2d& left, point2d& right)
{
  point2d tmp = right;
  right = left;
  left = tmp;
}

// Provides an empty, 2D tree.
struct Empty_2D_fixture
{
  typedef frozen_pointset<2, point2d> kdtree_type;
  kdtree_type kdtree;
  Empty_2D_fixture() : kdtree() { }
};

// Helps to test that details::match* family of functions are working properly
struct closed_test_range
{
  relative_order
  operator()(dimension_type dim, const point2d& point, dimension_type rank)
    const
  {
    if (dim >= rank) throw std::out_of_range("'dim' must be lower than 'rank'");
    return ((point[dim] < 0)
            ? below
            : ((point[dim] > 1)
               ? above
               : matching));
  }
};

struct Node_basic_empty_fixture
{
  Node_base header;
  Node_basic_empty_fixture()
  {
    header.parent = &header;
    header.left = &header;
    header.right = &header;
  }
};

struct Five_Node_basic_fixture
{
  /*         H
             |
             Root
             /   \
             L     R
             /  \
             LL    LR       */
  Node_base header;
  Node_base node_root;
  Node_base node_left;
  Node_base node_left_left;
  Node_base node_left_right;
  Node_base node_right;
  Five_Node_basic_fixture()
  {
    header.parent = &node_root;
    header.left = &header;
    header.right = &node_right;
    node_root.parent = &header;
    node_root.left = &node_left;
    node_root.right = &node_right;
    node_left.parent = &node_root;
    node_left.left = &node_left_left;
    node_left.right = &node_left_right;
    node_right.parent = &node_root;
    node_right.left = 0;
    node_right.right = 0;
    node_left_right.parent = &node_left;
    node_left_right.left = 0;
    node_left_right.right = 0;
    node_left_left.parent = &node_left;
    node_left_left.left = 0;
    node_left_left.right = 0;
  }
};

struct Five_Kdtree_node_fixture
{
  /*         H
             |
             (2,2)
             /   \
             (1,1)   (3,3)
             /  \
             (0,0)  (1,1)   */
  Node_base header;
  Node_base::Base_ptr leftmost;
  Kdtree_node<point2d> node_root;
  Kdtree_node<point2d> node_left;
  Kdtree_node<point2d> node_left_left;
  Kdtree_node<point2d> node_left_right;
  Kdtree_node<point2d> node_right;
  Five_Kdtree_node_fixture()
  {
    header.parent = &node_root;
    header.left = &header;
    header.right = &node_right;
    leftmost = &node_left_left;
    node_root.parent = &header;
    node_root.left = &node_left;
    node_root.right = &node_right;
    node_root.value = twos;
    node_left.parent = &node_root;
    node_left.left = &node_left_left;
    node_left.right = &node_left_right;
    node_left.value = ones;
    node_right.parent = &node_root;
    node_right.left = 0;
    node_right.right = 0;
    node_right.value = threes;
    node_left_right.parent = &node_left;
    node_left_right.left = 0;
    node_left_right.right = 0;
    node_left_right.value = ones;
    node_left_left.parent = &node_left;
    node_left_left.left = 0;
    node_left_left.right = 0;
    node_left_left.value = zeros;
  }
};

struct Five_Relaxed_kdtree_node_fixture
{
  /*         H
             |
           (2,2)5
           /   \
      (1,1)3  (3,3)1
       /  \
  (0,0)1  (1,1)1       */
  Node_base header;
  Node_base::Base_ptr leftmost;
  Relaxed_kdtree_node<point2d> node_root;
  Relaxed_kdtree_node<point2d> node_left;
  Relaxed_kdtree_node<point2d> node_left_left;
  Relaxed_kdtree_node<point2d> node_left_right;
  Relaxed_kdtree_node<point2d> node_right;
  Five_Relaxed_kdtree_node_fixture()
  {
    header.parent = &node_root;
    header.left = &header;
    header.right = &node_right;
    leftmost = &node_left_left;
    node_root.parent = &header;
    node_root.left = &node_left;
    node_root.right = &node_right;
    node_root.weight = 5;
    node_root.value = twos;
    node_left.parent = &node_root;
    node_left.left = &node_left_left;
    node_left.right = &node_left_right;
    node_left.weight = 3;
    node_left.value = ones;
    node_right.parent = &node_root;
    node_right.left = 0;
    node_right.right = 0;
    node_right.weight = 1;
    node_right.value = threes;
    node_left_right.parent = &node_left;
    node_left_right.left = 0;
    node_left_right.right = 0;
    node_left_right.weight = 1;
    node_left_right.value = ones;
    node_left_left.parent = &node_left;
    node_left_left.left = 0;
    node_left_left.right = 0;
    node_left_left.weight = 1;
    node_left_left.value = zeros;
  }
};

struct point2d_paren
  : std::tr1::array<int, 2>
{
  std::tr1::array<int, 2>::reference
  operator()(size_type n)
  { return std::tr1::array<int, 2>::operator[](n); }

  std::tr1::array<int, 2>::const_reference
  operator()(size_type n) const
  { return std::tr1::array<int, 2>::operator[](n); }
};

template <typename Argument, typename Return>
struct at_accessor
{
  Return operator()(dimension_type dim, const Argument& arg) const
  { return arg.at(dim); }

  Return& operator()(dimension_type dim, Argument& arg) const
  { return arg.at(dim); }
};

struct Empty_Kdtree_2D_fixture
{
  typedef Kdtree<details::Dynamic_rank, point2d, point2d,
                 bracket_less<point2d>,
                 std::allocator<point2d> > kdtree_type;
  kdtree_type kdtree;
  Empty_Kdtree_2D_fixture() : kdtree(details::Dynamic_rank(2)) { }
};

struct pair_less
{
  bool
  operator()(dimension_type dim, const pair_type& a, const pair_type& b) const
  {
    switch(dim)
      {
      case 0: return (a.first < b.first);
      case 1: return (a.second < b.second);
      default: throw std::logic_error("dim is greater than 1");
      }
  }
};

struct Pair_kdtree_fixture
{
  typedef runtime_frozen_pointset<pair_type, pair_less> kdtree_type;
  kdtree_type kdtree;
  Pair_kdtree_fixture() : kdtree(2)
  {
    pair_type p(1, 2);
    kdtree.insert(p);
  }
};

struct Unbalanced_right_kdtree_fixture
{
  typedef runtime_frozen_pointset<pair_type, pair_less> kdtree_type;
  kdtree_type kdtree;
  Unbalanced_right_kdtree_fixture() : kdtree(2)
  {
    pair_type a(1, 2); kdtree.insert(a);
    pair_type b(3, 4); kdtree.insert(b);
    pair_type c(5, 6); kdtree.insert(c);
    pair_type d(7, 8); kdtree.insert(d);
  }
};

struct Unbalanced_left_kdtree_fixture
{
  typedef runtime_frozen_pointset<pair_type, pair_less> kdtree_type;
  kdtree_type kdtree;
  Unbalanced_left_kdtree_fixture() : kdtree(2)
  {
    pair_type a(7, 8); kdtree.insert(a);
    pair_type b(5, 6); kdtree.insert(b);
    pair_type c(3, 4); kdtree.insert(c);
    pair_type d(1, 2); kdtree.insert(d);
  }
};

struct Hundred_kdtree_2D_fixture
{
  typedef runtime_frozen_pointset<point2d> kdtree_type;
  kdtree_type kdtree;
  Hundred_kdtree_2D_fixture() : kdtree(2)
  {
    for(int i = 0; i != 100; ++i)
    {
      point2d p;
      p[0] = rand() % 20;
      p[1] = rand() % 20;
      kdtree.insert(p);
    }
  }
};

struct triple
{
  int x; int y; int z;
  triple() { }
  triple(int a, int b, int c) : x(a), y(b), z(c) { }
};

inline bool operator== (const triple& a, const triple& b)
{
  return ( a.x == b.x && a.y == b.y && a.z == b.z );
}

inline bool operator!= (const triple& a, const triple& b)
{
  return !(a == b);
}

struct triple_less
{
  bool
  operator()(dimension_type dim, const triple& a, const triple& b) const
  {
    switch(dim)
      {
      case 0: return (a.x < b.x);
      case 1: return (a.y < b.y);
      case 2: return (a.z < b.z);
      default: throw std::logic_error("dim is greater than 2");
      }
  }
};

struct Twenty_kdtree_3D_fixture
{
  typedef runtime_frozen_pointset<triple, triple_less> kdtree_type;
  kdtree_type kdtree;
  Twenty_kdtree_3D_fixture() : kdtree(3)
  {
    for(int i = 0; i != 20; ++i)
    {
      triple t;
      t.x = rand() % 10000000 - 5000000;
      t.y = rand() % 10000000 - 5000000;
      t.z = rand() % 10000000 - 5000000;
      kdtree.insert(t);
    }
  }
};

struct triple_access
{
  int&
  operator()(dimension_type dim, triple& x) const
  {
    switch(dim)
      {
      case 0: return x.x;
      case 1: return x.y;
      case 2: return x.z;
      default: throw std::logic_error("dim is greater than 2");
      }
  }

  int
  operator()(dimension_type dim, const triple& x) const
  {
    switch(dim)
      {
      case 0: return x.x;
      case 1: return x.y;
      case 2: return x.z;
      default: throw std::logic_error("dim is greater than 2");
      }
  }
};

inline
double
rand_double()
{
  double out = static_cast<double>(rand())/static_cast<double>(RAND_MAX);
  out = (static_cast<double>(rand()) + out)/static_cast<double>(RAND_MAX);
  out = (static_cast<double>(rand()) + out)/static_cast<double>(RAND_MAX);
  out = (static_cast<double>(rand()) + out)/static_cast<double>(RAND_MAX);
  out = (static_cast<double>(rand()) + out)/static_cast<double>(RAND_MAX);
  return out;
}

inline
float
rand_float()
{
  float out = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
  out = (static_cast<float>(rand()) + out)/static_cast<float>(RAND_MAX);
  out = (static_cast<float>(rand()) + out)/static_cast<float>(RAND_MAX);
  return out;
}

typedef std::tr1::array<double, 5> point5d;
typedef std::tr1::array<float, 5> fpoint5d;

struct Hundred_Kdtree_5D_fixture
{
  typedef Kdtree<details::Static_rank<5>, point5d, point5d,
                 bracket_less<point5d>, std::allocator<point5d>, false>
  kdtree_type;
  kdtree_type kdtree;
  Hundred_Kdtree_5D_fixture()
  {
    for(int i = 0; i != 100; ++i)
    {
      point5d p;
      p[0] = static_cast<double>(rand() % 20);
      p[1] = static_cast<double>(rand() % 20);
      p[2] = static_cast<double>(rand() % 20);
      p[3] = static_cast<double>(rand() % 20);
      p[4] = static_cast<double>(rand() % 20);
      kdtree.insert(p);
    }
  }
};

struct Twenty_Relaxed_kdtree_3D_fixture
{
  typedef details::Relaxed_kdtree
  <details::Static_rank<3>, triple, triple, triple_less,
   tight_balancing, std::allocator<triple>, false> kdtree_type;

  kdtree_type kdtree;
  std::vector<triple> mem;
  Twenty_Relaxed_kdtree_3D_fixture() : kdtree(), mem()
  {
    for(int i = 0; i != 20; ++i)
    {
      mem.reserve(20);
      triple t;
      t.x = rand() % 10 - 5; // small range to have colisions
      t.y = rand() % 10 - 5;
      t.z = rand() % 10 - 5;
      mem.push_back(t);
      kdtree_type::const_iterator it = kdtree.insert(t);
      BOOST_CHECK(*it == t);
    }
  }
};

struct Twenty_Relaxed_kdtree_2D_fixture
{
  typedef details::Relaxed_kdtree
  <details::Static_rank<2>, point2d, point2d, bracket_less<point2d>,
   tight_balancing, std::allocator<point2d>, false> kdtree_type;

  kdtree_type kdtree;
  std::vector<point2d> mem;
  Twenty_Relaxed_kdtree_2D_fixture() : kdtree(), mem()
  {
    for(int i = 0; i != 20; ++i)
    {
      mem.reserve(20);
      point2d t;
      t[0] = rand() % 10 - 5; // small range to have colisions
      t[1] = rand() % 10 - 5;
      mem.push_back(t);
      kdtree_type::const_iterator it = kdtree.insert(t);
      BOOST_CHECK(*it == t);
    }
  }
};

struct Grow_Relaxed_Kdtree_2D_fixture
{
  typedef details::Relaxed_kdtree
  <details::Static_rank<2>, point2d, point2d, bracket_less<point2d>,
   tight_balancing, std::allocator<point2d>, false> kdtree_type;

  kdtree_type kdtree;
  Grow_Relaxed_Kdtree_2D_fixture() : kdtree()
  {
    for(int i = 0; i != 20; ++i)
    {
      point2d t;
      t[0] = i;
      t[1] = i;
      kdtree_type::const_iterator it = kdtree.insert(t);
      BOOST_CHECK(*it == t);
    }
  }
};

struct Reduce_Relaxed_Kdtree_2D_fixture
{
  typedef details::Relaxed_kdtree
  <details::Static_rank<2>, point2d, point2d, bracket_less<point2d>,
   tight_balancing, std::allocator<point2d>, false> kdtree_type;

  kdtree_type kdtree;
  Reduce_Relaxed_Kdtree_2D_fixture() : kdtree()
  {
    for(int i = 20; i != 0; --i)
    {
      point2d t;
      t[0] = i;
      t[1] = i;
      kdtree_type::const_iterator it = kdtree.insert(t);
      BOOST_CHECK(*it == t);
    }
  }
};

struct Hundred_Relaxed_kdtree_5D_fixture
{
  typedef details::Relaxed_kdtree
  <details::Static_rank<5>, point5d, point5d, bracket_less<point5d>,
   loose_balancing, std::allocator<point5d>, false> kdtree_type;

  kdtree_type kdtree;
  std::vector<point5d> array;
  Hundred_Relaxed_kdtree_5D_fixture() : kdtree()
  {
    array.reserve(100);
    for(int i = 0; i != 100; ++i)
    {
      point5d p;
      p[0] = rand() % 20 - 10;
      p[1] = rand() % 20 - 10;
      p[2] = rand() % 20 - 10;
      p[3] = rand() % 20 - 10;
      p[4] = rand() % 20 - 10;
      array.push_back(p);
      kdtree_type::const_iterator it = kdtree.insert(p);
      BOOST_CHECK(*it == p);
    }
  }
};

struct Empty_Relaxed_kdtree_2D_fixture
{
  typedef details::Relaxed_kdtree
  <details::Static_rank<2>, point2d, point2d, bracket_less<point2d>,
   loose_balancing, std::allocator<point2d>, false> kdtree_type;

  kdtree_type kdtree;
  Empty_Relaxed_kdtree_2D_fixture() : kdtree()
  { }
};

struct Empty_Relaxed_kdtree_3D_fixture
{
  typedef details::Relaxed_kdtree
  <details::Static_rank<3>, triple, triple, triple_less,
   loose_balancing, std::allocator<triple>, false> kdtree_type;

  kdtree_type kdtree;
  Empty_Relaxed_kdtree_3D_fixture() : kdtree()
  { }
};

struct Seven_Relaxed_kdtree_node_fixture
{
  /*              H
                  |
               (2,2)7
            /          \
      (1,1)3           (3,3)3
       /  \             /
  (0,0)1  (1,1)1     (3,3)2
                       \
                     (3,3)1     */
  Node_base header;
  Node_base::Base_ptr leftmost;
  Relaxed_kdtree_node<point2d> node_root;
  Relaxed_kdtree_node<point2d> node_left;
  Relaxed_kdtree_node<point2d> node_left_left;
  Relaxed_kdtree_node<point2d> node_left_right;
  Relaxed_kdtree_node<point2d> node_right;
  Relaxed_kdtree_node<point2d> node_right_left;
  Relaxed_kdtree_node<point2d> node_right_left_right;
  Seven_Relaxed_kdtree_node_fixture()
  {
    header.parent = &node_root;
    header.left = &header;
    header.right = &node_right;
    leftmost = &node_left_left;
    node_root.parent = &header;
    node_root.left = &node_left;
    node_root.right = &node_right;
    node_root.weight = 7;
    node_root.value = twos;
    node_left.parent = &node_root;
    node_left.left = &node_left_left;
    node_left.right = &node_left_right;
    node_left.weight = 3;
    node_left.value = ones;
    node_right.parent = &node_root;
    node_right.left = &node_right_left;
    node_right.right = 0;
    node_right.weight = 3;
    node_right.value = threes;
    node_right_left.parent = &node_right;
    node_right_left.left = 0;
    node_right_left.right = &node_right_left_right;
    node_right_left.weight = 2;
    node_right_left.value = threes;
    node_right_left_right.parent = &node_right_left;
    node_right_left_right.left = 0;
    node_right_left_right.right = 0;
    node_right_left_right.weight = 1;
    node_right_left_right.value = threes;
    node_left_right.parent = &node_left;
    node_left_right.left = 0;
    node_left_right.right = 0;
    node_left_right.weight = 1;
    node_left_right.value = ones;
    node_left_left.parent = &node_left;
    node_left_left.left = 0;
    node_left_left.right = 0;
    node_left_left.weight = 1;
    node_left_left.value = zeros;
  }
};

struct Dispersed_Relaxed_kdtree_3D_fixture
{
  typedef details::Relaxed_kdtree
  <details::Dynamic_rank, triple, triple, triple_less, tight_balancing,
   std::allocator<triple>, false> kdtree_type;
  kdtree_type kdtree;
  Dispersed_Relaxed_kdtree_3D_fixture() : kdtree(details::Dynamic_rank(3))
  {
    for(int i = 0; i != 20; ++i)
    {
      triple t;
      t.x = rand() % 10000000 - 5000000;
      t.y = rand() % 10000000 - 5000000;
      t.z = rand() % 10000000 - 5000000;
      kdtree.insert(t);
    }
  }
};

struct Hundred_Relaxed_kdtree_2D_fixture
{
  typedef details::Relaxed_kdtree
  <details::Static_rank<2>, point2d, point2d, bracket_less<point2d>,
   loose_balancing, std::allocator<point2d>, false> kdtree_type;

  kdtree_type kdtree;
  Hundred_Relaxed_kdtree_2D_fixture() : kdtree()
  {
    for(int i = 0; i != 100; ++i)
    {
      point2d p;
      p[0] = rand() % 20;
      p[1] = rand() % 20;
      kdtree_type::const_iterator it = kdtree.insert(p);
      BOOST_CHECK(*it == p);
    }
  }
};

#endif // SPATIAL_FIXTURES_HPP
