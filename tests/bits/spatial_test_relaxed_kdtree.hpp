// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_test_relaxed_kdtree.hpp
 *  All tests for the elements defined in spatial_relaxed_kdtree.hpp are
 *  located in this file.
 *  \see spatial_relaxed_kdtree.hpp
 */

#ifndef SPATIAL_TEST_RELAXED_KDTREE_HPP
#define SPATIAL_TEST_RELAXED_KDTREE_HPP

BOOST_AUTO_TEST_CASE( test_loose_balancing )
{
  details::Dynamic_rank rank(2);
  loose_balancing test;
  // A leaf node is always balanced!
  BOOST_CHECK_EQUAL(test(rank, 0, 0), false);
  // rebalance even if no right.
  BOOST_CHECK_EQUAL(test(rank, 4, 0), true);
  // reblance even if no left.
  BOOST_CHECK_EQUAL(test(rank, 0, 4), true);
  // should fail cause we are right under loose balancing conditions
  BOOST_CHECK_EQUAL(test(rank, 7, 2), false);
  // should fail cause we are right under loose balancing conditions
  BOOST_CHECK_EQUAL(test(rank, 9, 3), false);
  // should pass cause we are above loose balancing conditions
  BOOST_CHECK_EQUAL(test(rank, 2, 8), true);
  // should pass cause we are above loose balancing conditions
  BOOST_CHECK_EQUAL(test(rank, 8, 2), true);
}

BOOST_AUTO_TEST_CASE( test_tight_balancing )
{
  {
    details::Dynamic_rank rank(2);
    tight_balancing test;
    // A leaf node is always balanced!
    BOOST_CHECK_EQUAL(test(rank, 0, 0), false);
    // rebalance even if no right.
    BOOST_CHECK_EQUAL(test(rank, 9, 0), true);
    // rebalance even if no left.
    BOOST_CHECK_EQUAL(test(rank, 0, 9), true);
    // should fail cause we are right under tight balancing conditions
    BOOST_CHECK_EQUAL(test(rank, 4, 2), false);
    // should fail cause we are right under tight balancing conditions
    BOOST_CHECK_EQUAL(test(rank, 2, 4), false);
    // should pass cause we are above tight balancing conditions
    BOOST_CHECK_EQUAL(test(rank, 3, 6), true);
    // should pass cause we are above tight balancing conditions
    BOOST_CHECK_EQUAL(test(rank, 6, 3), true);
  }
  {
    details::Dynamic_rank rank(9);
    tight_balancing test;
    // high dimension prevent rebalance.
    BOOST_CHECK_EQUAL(test(rank, 1, 8), false);
    BOOST_CHECK_EQUAL(test(rank, 8, 1), false);
  }
}

BOOST_AUTO_TEST_CASE( test_perfect_balancing )
{
  {
    details::Dynamic_rank rank(2);
    perfect_balancing test;
    // A leaf node is always balanced!
    BOOST_CHECK_EQUAL(test(rank, 0, 0), false);
    // rebalance even if no right.
    BOOST_CHECK_EQUAL(test(rank, 4, 1), true);
    // rebalance even if no left.
    BOOST_CHECK_EQUAL(test(rank, 2, 5), true);
    // should fail cause we are right under perfect balancing conditions
    BOOST_CHECK_EQUAL(test(rank, 2, 0), false);
    // should fail cause we are right under perfect balancing conditions
    BOOST_CHECK_EQUAL(test(rank, 0, 2), false);
  }
  {
    details::Dynamic_rank rank(9);
    perfect_balancing test;
    // high dimension does not change balancing
    BOOST_CHECK_EQUAL(test(rank, 0, 2), false);
    BOOST_CHECK_EQUAL(test(rank, 2, 0), false);
    BOOST_CHECK_EQUAL(test(rank, 0, 3), true);
    BOOST_CHECK_EQUAL(test(rank, 3, 0), true);
  }
}

BOOST_AUTO_TEST_CASE( test_relaxed_kdtree_ctor )
{
  typedef details::Relaxed_kdtree
    <details::Dynamic_rank, int2, int2, bracket_less<int2>,
     tight_balancing, std::allocator<int2> > kdtree_type;
  kdtree_type instance_one;
  kdtree_type instance_two(details::Dynamic_rank(2));
  kdtree_type instance_three(details::Dynamic_rank(2), bracket_less<int2>());
  kdtree_type instance_four(details::Dynamic_rank(2), bracket_less<int2>(),
                            tight_balancing());
  kdtree_type instance_five(details::Dynamic_rank(2), bracket_less<int2>(),
                            tight_balancing(), std::allocator<int2>());
  // Trees should be empty.
  BOOST_CHECK(instance_one.empty());
  BOOST_CHECK(instance_two.empty());
  BOOST_CHECK(instance_three.empty());
  BOOST_CHECK(instance_four.empty());
  BOOST_CHECK(instance_five.empty());
  // That also mean that beginning equal end for each of them
  BOOST_CHECK(instance_one.begin() == instance_one.end());
  BOOST_CHECK(instance_two.begin() == instance_two.end());
  BOOST_CHECK(instance_three.begin() == instance_three.end());
  BOOST_CHECK(instance_four.begin() == instance_four.end());
  BOOST_CHECK(instance_five.begin() == instance_five.end());
  // And that also mean that have a size of 0
  BOOST_CHECK_EQUAL(instance_one.size(), 0);
  BOOST_CHECK_EQUAL(instance_two.size(), 0);
  BOOST_CHECK_EQUAL(instance_three.size(), 0);
  BOOST_CHECK_EQUAL(instance_four.size(), 0);
  BOOST_CHECK_EQUAL(instance_five.size(), 0);
}

BOOST_AUTO_TEST_CASE( test_relaxed_kdtree_insert_tight )
{
  typedef details::Relaxed_kdtree
    <details::Static_rank<2>, int2, int2, bracket_less<int2>,
     tight_balancing, std::allocator<int2> > kdtree_type;
  kdtree_type tree;
  kdtree_type::const_iterator it;
  BOOST_CHECK_NO_THROW(it = tree.insert(zeros));
  BOOST_CHECK(*it == zeros);
  BOOST_CHECK(!tree.empty());
  BOOST_CHECK(tree.begin() != tree.end());
  BOOST_CHECK_EQUAL(tree.size(), 1);
  BOOST_CHECK(zeros == *tree.begin());
  BOOST_CHECK(++tree.begin() == tree.end());
  BOOST_CHECK(tree.begin() == --tree.end());
}

BOOST_AUTO_TEST_CASE( test_relaxed_kdtree_insert_perfect )
{
  typedef details::Relaxed_kdtree
    <details::Static_rank<2>, int2, int2, bracket_less<int2>,
     perfect_balancing, std::allocator<int2> > kdtree_type;
  kdtree_type tree;
  kdtree_type::const_iterator it;
  BOOST_CHECK_NO_THROW(it = tree.insert(zeros));
  BOOST_CHECK(*it == zeros);
  BOOST_CHECK(!tree.empty());
  BOOST_CHECK(tree.begin() != tree.end());
  BOOST_CHECK_EQUAL(tree.size(), 1);
  BOOST_CHECK(zeros == *tree.begin());
  BOOST_CHECK(++tree.begin() == tree.end());
  BOOST_CHECK(tree.begin() == --tree.end());
}

BOOST_AUTO_TEST_CASE( test_relaxed_kdtree_insert_loose )
{
  typedef details::Relaxed_kdtree
    <details::Static_rank<2>, int2, int2, bracket_less<int2>,
     loose_balancing, std::allocator<int2> > kdtree_type;
  kdtree_type tree;
  kdtree_type::const_iterator it;
  BOOST_CHECK_NO_THROW(it = tree.insert(zeros));
  BOOST_CHECK(*it == zeros);
  BOOST_CHECK(!tree.empty());
  BOOST_CHECK(tree.begin() != tree.end());
  BOOST_CHECK_EQUAL(tree.size(), 1);
  BOOST_CHECK(zeros == *tree.begin());
  BOOST_CHECK(++tree.begin() == tree.end());
  BOOST_CHECK(tree.begin() == --tree.end());
}

BOOST_AUTO_TEST_CASE( test_relaxed_kdtree_insert_growing )
{
  tight_pointset_fix<int2> fix(100, increase());
  // because the tree is ordered on all dimensions and nodes are all sequential
  // values, iteration should be ordered as well.
  int i = 0;
  for (tight_pointset_fix<int2>::container_type::iterator it
         = fix.container.begin(); it != fix.container.end(); ++it, ++i)
    {
      BOOST_CHECK_EQUAL((*it)[0], i);
      BOOST_CHECK_EQUAL((*it)[1], i);
    }
}

BOOST_AUTO_TEST_CASE( test_relaxed_kdtree_insert_same )
{
  perfect_pointset_fix<int2> fix(100, increase());
  // because the tree is ordered on all dimensions and nodes are all sequential
  // values, iteration should be ordered as well.
  int i = 0;
  for (perfect_pointset_fix<int2>::container_type::iterator it
         = fix.container.begin(); it != fix.container.end(); ++it, ++i)
    {
      BOOST_CHECK_EQUAL((*it)[0], i);
      BOOST_CHECK_EQUAL((*it)[1], i);
    }
}

BOOST_AUTO_TEST_CASE( test_relaxed_kdtree_insert_reduce )
{
  pointset_fix<int2> fix(100, decrease());
  // because the tree is ordered on all dimensions and nodes are all sequential
  // values, iteration should be ordered as well.
  int i = 1;
  for (pointset_fix<int2>::container_type::iterator it
         = fix.container.begin(); it != fix.container.end(); ++it, ++i)
    {
      BOOST_CHECK_EQUAL((*it)[0], i);
      BOOST_CHECK_EQUAL((*it)[1], i);
    }
}

BOOST_AUTO_TEST_CASE( test_relaxed_kdtree_insert_lots )
{
  // build repeatitively a lot of them, should not fail.
  // This tree has multiple similar node, by construction
  for (int i=0; i<100; ++i)
    {
      pointset_fix<int2> fix(100, randomize(-4, 4));
      int count = 0;
      for (pointset_fix<int2>::container_type::iterator it
             = fix.container.begin(); it != fix.container.end(); ++it)
        {
          if ((*it)[0] >= -4 && (*it)[0] < 4
              && (*it)[1] >= -4 && (*it)[1] < 4) ++count;
        }
      BOOST_CHECK_EQUAL(count, 100);
    }
}

BOOST_AUTO_TEST_CASE( test_relaxed_kdtree_copy )
{
  // Copy a kdtree of quad and iterate over it. Both tree should have leaf nodes
  // at the same location
  runtime_pointset_fix<quad> fix(40, randomize(-10, 10));
  runtime_pointset_fix<quad>::container_type copy(fix.container);
  BOOST_CHECK_EQUAL(fix.container.size(), copy.size());
  typedef runtime_pointset_fix<quad>::container_type::iterator iterator_type;
  iterator_type orig_begin = fix.container.begin();
  iterator_type orig_end = fix.container.end();
  iterator_type copy_begin = copy.begin();
  iterator_type copy_end = copy.end();
  for (; orig_begin != orig_end && copy_begin != copy_end;
       ++copy_begin, ++orig_begin)
    {
      BOOST_CHECK(*orig_begin == *copy_begin);
      if (orig_begin.node->left == 0) BOOST_CHECK(copy_begin.node->left == 0);
      if (orig_begin.node->right == 0) BOOST_CHECK(copy_begin.node->right == 0);
    }
}

BOOST_AUTO_TEST_CASE( test_relaxed_kdtree_assignment )
{
  // Copy an 3d kdtree and iterate over it. Both iterators should yield same
  // results and exhibit same properties.
  runtime_pointset_fix<quad> fix(40, randomize(-10, 10));
  runtime_pointset_fix<quad> copy(40, randomize(-10, 10));
  BOOST_CHECK_EQUAL(fix.container.size(), copy.container.size());
  copy.container = fix.container;
  typedef runtime_pointset_fix<quad>::container_type::iterator iterator_type;
  iterator_type orig_begin = fix.container.begin();
  iterator_type orig_end = fix.container.end();
  iterator_type copy_begin = copy.container.begin();
  iterator_type copy_end = copy.container.end();
  for (; orig_begin != orig_end && copy_begin != copy_end;
       ++copy_begin, ++orig_begin)
    {
      BOOST_CHECK(*orig_begin == *copy_begin);
      if (orig_begin.node->left == 0) BOOST_CHECK(copy_begin.node->left == 0);
      if (orig_begin.node->right == 0) BOOST_CHECK(copy_begin.node->right == 0);
    }
}

BOOST_AUTO_TEST_CASE( test_relaxed_kdtree_bulk_insert )
{
  // reuse test_kdtree_insert_100_iterate_forward
  typedef details::Relaxed_kdtree
    <details::Static_rank<2>, int2, int2, bracket_less<int2>,
     loose_balancing, std::allocator<int2> > kdtree_type;
  kdtree_type kdtree;
  std::tr1::array<int2, 100> points;
  for(std::tr1::array<int2, 100>::iterator i
        = points.begin(); i != points.end(); ++i)
    {
      (*i)[0] = std::rand() % 20;
      (*i)[1] = std::rand() % 20;
    }
  BOOST_REQUIRE_NO_THROW(kdtree.insert(points.begin(), points.end()));
  BOOST_CHECK(kdtree.empty() == false);
  BOOST_REQUIRE_EQUAL(kdtree.size(), 100);
  BOOST_CHECK_EQUAL(kdtree.count(), 100);
  int count = 0;
  for(kdtree_type::iterator i = kdtree.begin(); i != kdtree.end(); ++i)
    {
      std::tr1::array<int2, 100>::iterator match;
      BOOST_REQUIRE((match = std::find(points.begin(), points.end(), *i))
                    != points.end());
      (*match)[0] = -1; // Prevent the same point from being found twice.
      (*match)[1] = -1;
      BOOST_REQUIRE_LE(++count, 100);
    }
  BOOST_CHECK_EQUAL(count, 100);
}

BOOST_AUTO_TEST_CASE( test_relaxed_kdtree_erase_key )
{
  {
    // Erase on empty tree should return 0.
    pointset_fix<int2> fix;
    int2 some_value(0, 0);
    size_type count = fix.container.erase(some_value);
    BOOST_CHECK_EQUAL(count, 0);
    BOOST_CHECK(fix.container.end() == fix.container.begin());
    BOOST_CHECK(fix.container.empty());
  }
  {
    // Erase one value of a tree should return 1 and leave an empty tree.
    tight_pointset_fix<int2> fix;
    int2 some_value(0, 0);
    fix.container.insert(some_value);
    BOOST_CHECK(!fix.container.empty());
    size_type count = fix.container.erase(some_value);
    BOOST_CHECK_EQUAL(count, 1);
    BOOST_CHECK(fix.container.end() == fix.container.begin());
    BOOST_CHECK(fix.container.empty());
  }
  {
    // Erase a value not in the tree should leave the tree untouched.
    runtime_pointset_fix<int2> fix;
    int2 some_value(0, 0);
    int2 other_value(1, 1);
    fix.container.insert(some_value);
    runtime_pointset_fix<int2>::container_type::iterator
      begin_before = fix.container.begin();
    runtime_pointset_fix<int2>::container_type::iterator
      end_before = fix.container.end();
    size_type count = fix.container.erase(other_value);
    BOOST_CHECK_EQUAL(count, 0);
    BOOST_CHECK(!fix.container.empty());
    BOOST_CHECK(fix.container.end() != fix.container.begin());
    BOOST_CHECK(fix.container.end() == end_before);
    BOOST_CHECK(fix.container.begin() == begin_before);
  }
  {
    // Should be able to erase multiple values
    typedef details::Relaxed_kdtree
      <details::Dynamic_rank, int2, int2, bracket_less<int2>,
       tight_balancing, std::allocator<int2> >
      kdtree_type;
    // 1D tree where we store 2d objects...
    kdtree_type tree(details::Dynamic_rank(1));
    int2 one(1, 0);
    int2 two(2, 2);
    int2 four(4, 0);
    tree.insert(one);
    tree.insert(two);
    tree.insert(two);
    tree.insert(four);
    // ... And equal should take the second dimension into account.
    BOOST_REQUIRE_EQUAL(tree.count(), 4);
    size_type count = tree.erase(two);
    BOOST_CHECK_EQUAL(count, 2);
    BOOST_REQUIRE_EQUAL(tree.count(), 2);
    BOOST_CHECK(tree.find(one) != tree.end());
    BOOST_CHECK(tree.find(four) != tree.end());
    BOOST_CHECK(tree.find(two) == tree.end());
    kdtree_type::iterator iter = tree.begin();
    BOOST_REQUIRE(iter != tree.end());
    BOOST_REQUIRE_NO_THROW(++iter);
    BOOST_REQUIRE(iter != tree.end());
    BOOST_REQUIRE_NO_THROW(++iter);
    BOOST_REQUIRE(iter == tree.end());
  }
}

BOOST_AUTO_TEST_CASE( test_relaxed_kdtree_erase_iterator )
{
  // check that erase at edge preserve basic iterators
  {
    point_multimap_fix<quad, std::string> fix(20, randomize(0, 1));
    typedef point_multimap_fix<quad, std::string>::container_type
      ::iterator iter_type;
    size_type track_size = fix.container.size();
    while (fix.container.size() != 0)
      {
        iter_type iter = fix.container.begin();
        BOOST_REQUIRE_NO_THROW(fix.container.erase(iter));
        BOOST_CHECK_EQUAL(fix.container.size(), --track_size);
        BOOST_CHECK(iter != fix.container.begin());
        int count = 0;
        for(iter_type i = fix.container.begin();
            i != fix.container.end(); ++i, ++count);
        BOOST_CHECK_EQUAL(count, track_size);
      }
  }
  {
    point_multimap_fix<quad, std::string> fix(20, randomize(0, 1));
    typedef point_multimap_fix<quad, std::string>::container_type
      ::iterator iter_type;
    typedef point_multimap_fix<quad, std::string>::container_type
      ::reverse_iterator riter_type;
    size_type track_size = fix.container.size();
    while (fix.container.size() != 0)
      {
        iter_type iter = --fix.container.end();
        BOOST_REQUIRE_NO_THROW(fix.container.erase(iter));
        BOOST_CHECK_EQUAL(fix.container.size(), --track_size);
        BOOST_CHECK(iter != (--fix.container.end()));
        int count = 0;
        for(riter_type i = fix.container.rbegin();
            i != fix.container.rend(); ++i, ++count);
        BOOST_CHECK_EQUAL(count, track_size);
      }
  }
  // erase all and check that total ordering is preserved.
  {
    typedef pointset_fix<double6>::container_type::iterator iterator;
    typedef mapping_iterator<pointset_fix<double6>::container_type>
      mapping_iterator;
    pointset_fix<double6> fix(100, randomize(0, 1));
    size_type track_size = fix.container.size();
    while (fix.container.size() != 0)
      {
        iterator eraser = fix.container.begin();
        std::advance(eraser, static_cast<size_type>(rand()) % fix.container.size());
        mapping_iterator begin_0(fix.container, 0, 0,
                                 fix.container.end().node->parent);
        mapping_iterator end_0(fix.container, 0,
                               details::decr_dim(fix.container.rank(), 0),
                               fix.container.end().node);
        mapping_iterator begin_1(fix.container, 1, 0,
                                 fix.container.end().node->parent);
        mapping_iterator end_1(fix.container, 1,
                               details::decr_dim(fix.container.rank(), 0),
                               fix.container.end().node);
        begin_0 = details::minimum_mapping(begin_0);
        begin_1 = details::minimum_mapping(begin_1);
        size_type count = 0;
        for(mapping_iterator i = begin_0; i != end_0; ++i, ++count);
        BOOST_REQUIRE_EQUAL(count, track_size);
        count = 0;
        for(mapping_iterator i = begin_1; i != end_1; ++i, ++count);
        BOOST_REQUIRE_EQUAL(count, track_size);
        count = 0;
        for(mapping_iterator i = end_0; i != begin_0; --i, ++count);
        BOOST_REQUIRE_EQUAL(count, track_size);
        count = 0;
        for(mapping_iterator i = end_1; i != begin_1; --i, ++count);
        BOOST_REQUIRE_EQUAL(count, track_size);
        BOOST_REQUIRE_NO_THROW(fix.container.erase(eraser));
        BOOST_CHECK_EQUAL(fix.container.size(), --track_size);
      }
  }
}

#endif // SPATIAL_TEST_RELAXED_KDTREE_HPP
