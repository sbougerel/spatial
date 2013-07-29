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
  BOOST_CHECK_EQUAL(test(rank, 3, 0), true);
  // reblance even if no left.
  BOOST_CHECK_EQUAL(test(rank, 0, 3), true);
  // should fail cause we are right under loose balancing conditions
  BOOST_CHECK_EQUAL(test(rank, 4, 2), false);
  // should fail cause we are right under loose balancing conditions
  BOOST_CHECK_EQUAL(test(rank, 6, 3), false);
  // should pass cause we are above loose balancing conditions
  BOOST_CHECK_EQUAL(test(rank, 2, 6), true);
  // should pass cause we are above loose balancing conditions
  BOOST_CHECK_EQUAL(test(rank, 6, 2), true);
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

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_ctor )
{
  typedef details::Relaxed_kdtree
    <details::Dynamic_rank, point2d, point2d, bracket_less<point2d>,
     tight_balancing, std::allocator<point2d>, true> kdtree_type;
  kdtree_type instance_one;
  kdtree_type instance_two(details::Dynamic_rank(2));
  kdtree_type instance_three(details::Dynamic_rank(2), bracket_less<point2d>());
  kdtree_type instance_four(details::Dynamic_rank(2), bracket_less<point2d>(),
                            tight_balancing());
  kdtree_type instance_five(details::Dynamic_rank(2), bracket_less<point2d>(),
                            tight_balancing(), std::allocator<point2d>());
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

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_insert_tight )
{
  typedef details::Relaxed_kdtree
    <details::Static_rank<2>, point2d, point2d, bracket_less<point2d>,
     tight_balancing, std::allocator<point2d>, false> kdtree_type;
  typedef details::Relaxed_kdtree
    <details::Static_rank<2>, point2d, point2d, bracket_less<point2d>,
     tight_balancing, std::allocator<point2d>, true> const_kdtree_type;
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

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_insert_loose )
{
  typedef details::Relaxed_kdtree
    <details::Static_rank<2>, point2d, point2d, bracket_less<point2d>,
     loose_balancing, std::allocator<point2d>, false> kdtree_type;
  typedef details::Relaxed_kdtree
    <details::Static_rank<2>, point2d, point2d, bracket_less<point2d>,
     loose_balancing, std::allocator<point2d>, true> const_kdtree_type;
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

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_insert_growing )
{
  Grow_Relaxed_Kdtree_2D_fixture fix;
  // because the tree is ordered on all dimensions and nodes are all sequential
  // values, iteration should be ordered as well.
  int i = 0;
  for (Grow_Relaxed_Kdtree_2D_fixture::kdtree_type::iterator it
         = fix.kdtree.begin(); it != fix.kdtree.end(); ++it, ++i)
    {
      BOOST_CHECK_EQUAL((*it)[0], i);
      BOOST_CHECK_EQUAL((*it)[1], i);
    }
}

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_insert_reduce )
{
  Reduce_Relaxed_Kdtree_2D_fixture fix;
  // because the tree is ordered on all dimensions and nodes are all sequential
  // values, iteration should be ordered as well.
  int i = 1; // Values inserted in [1:20], not [0:19]
  for (Reduce_Relaxed_Kdtree_2D_fixture::kdtree_type::iterator it
         = fix.kdtree.begin(); it != fix.kdtree.end(); ++it, ++i)
    {
      BOOST_CHECK_EQUAL((*it)[0], i);
      BOOST_CHECK_EQUAL((*it)[1], i);
    }
}

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_insert_lots )
{
  {
    // build repeatitively a lot of them, should not fail.
    // This tree has multiple similar node, by construction
    for (int i=0; i<100; ++i)
      {
        Twenty_Relaxed_kdtree_2D_fixture fix;
        int count = 0;
        for (Twenty_Relaxed_kdtree_2D_fixture::kdtree_type
               ::iterator it = fix.kdtree.begin(); it != fix.kdtree.end(); ++it)
          {
            BOOST_CHECK(std::find(fix.mem.begin(), fix.mem.end(), *it)
                        != fix.mem.end());
            ++count;
          }
        BOOST_CHECK_EQUAL(count, 20);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_copy )
{
  // Copy an 3d kdtree and iterate over it. Both iterators should yield same
  // results and exhibit same properties.
  Twenty_Relaxed_kdtree_3D_fixture fix;
  Twenty_Relaxed_kdtree_3D_fixture::kdtree_type copy(fix.kdtree);
  BOOST_CHECK_EQUAL(fix.kdtree.size(), copy.size());
  typedef Twenty_Relaxed_kdtree_3D_fixture::kdtree_type::iterator iterator_type;
  iterator_type orig_begin = fix.kdtree.begin();
  iterator_type orig_end = fix.kdtree.end();
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

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_assignment )
{
  // Copy an 3d kdtree and iterate over it. Both iterators should yield same
  // results and exhibit same properties.
  Twenty_Relaxed_kdtree_3D_fixture fix;
  Twenty_Relaxed_kdtree_3D_fixture fix_other;
  fix_other.kdtree = fix.kdtree;
  typedef Twenty_Relaxed_kdtree_3D_fixture::kdtree_type::iterator iterator_type;
  iterator_type orig_begin = fix.kdtree.begin();
  iterator_type orig_end = fix.kdtree.end();
  iterator_type copy_begin = fix_other.kdtree.begin();
  iterator_type copy_end = fix_other.kdtree.end();
  for (; orig_begin != orig_end && copy_begin != copy_end;
       ++copy_begin, ++orig_begin)
    {
      BOOST_CHECK(*orig_begin == *copy_begin);
      if (orig_begin.node->left == 0) BOOST_CHECK(copy_begin.node->left == 0);
      if (orig_begin.node->right == 0) BOOST_CHECK(copy_begin.node->right == 0);
    }
}

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_bulk_insert )
{
  // reuse test_kdtree_insert_100_iterate_forward
  typedef details::Relaxed_kdtree
    <details::Static_rank<2>, point2d, point2d, bracket_less<point2d>,
     loose_balancing, std::allocator<point2d>, false> kdtree_type;
  kdtree_type kdtree;
  std::tr1::array<point2d, 100> points;
  for(std::tr1::array<point2d, 100>::iterator i
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
      std::tr1::array<point2d, 100>::iterator match;
      BOOST_REQUIRE((match = std::find(points.begin(), points.end(), *i))
                  != points.end());
      (*match)[0] = -1; // Prevent the same point from being found twice.
      (*match)[1] = -1;
      BOOST_REQUIRE_LE(++count, 100);
    }
  BOOST_CHECK_EQUAL(count, 100);
}

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_erase_key )
{
  {
    // Erase on empty tree should return 0.
    Empty_Relaxed_kdtree_2D_fixture fix;
    point2d some_value = { {0, 0} };
    size_type count = fix.kdtree.erase(some_value);
    BOOST_CHECK_EQUAL(count, 0);
    BOOST_CHECK(fix.kdtree.end() == fix.kdtree.begin());
    BOOST_CHECK(fix.kdtree.empty());
  }
  {
    // Erase one value of a tree should return 1 and leave an empty tree.
    Empty_Relaxed_kdtree_2D_fixture fix;
    point2d some_value = { {0, 0} };
    fix.kdtree.insert(some_value);
    BOOST_CHECK(!fix.kdtree.empty());
    size_type count = fix.kdtree.erase(some_value);
    BOOST_CHECK_EQUAL(count, 1);
    BOOST_CHECK(fix.kdtree.end() == fix.kdtree.begin());
    BOOST_CHECK(fix.kdtree.empty());
  }
  {
    // Erase a value not in the tree should leave the tree untouched.
    Empty_Relaxed_kdtree_2D_fixture fix;
    point2d some_value = { {0, 0} };
    point2d other_value = { {1, 1} };
    fix.kdtree.insert(some_value);
    Empty_Relaxed_kdtree_2D_fixture::kdtree_type::iterator
      begin_before = fix.kdtree.begin();
    Empty_Relaxed_kdtree_2D_fixture::kdtree_type::iterator
      end_before = fix.kdtree.end();
    size_type count  = fix.kdtree.erase(other_value);
    BOOST_CHECK_EQUAL(count, 0);
    BOOST_CHECK(!fix.kdtree.empty());
    BOOST_CHECK(fix.kdtree.end() != fix.kdtree.begin());
    BOOST_CHECK(fix.kdtree.end() == end_before);
    BOOST_CHECK(fix.kdtree.begin() == begin_before);
  }
  {
    // Should be able to erase multiple values
    typedef details::Relaxed_kdtree
      <details::Dynamic_rank, point2d, point2d, bracket_less<point2d>,
       tight_balancing, std::allocator<point2d>, false>
      kdtree_type;
    // 1D tree where we store 2d objects...
    kdtree_type tree(details::Dynamic_rank(1));
    point2d one   = { { 1, 0 } };
    point2d two   = { { 2, 2 } };
    point2d four  = { { 4, 0 } };
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

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_erase_iterator )
{
  // check that erase at edge preserve basic iterators
  {
    Twenty_Relaxed_kdtree_3D_fixture fix;
    typedef Twenty_Relaxed_kdtree_3D_fixture::kdtree_type
      ::iterator iter_type;
    size_type track_size = fix.kdtree.size();
    while (fix.kdtree.size() != 0)
      {
        iter_type iter = fix.kdtree.begin();
        BOOST_REQUIRE_NO_THROW(fix.kdtree.erase(iter));
        BOOST_CHECK_EQUAL(fix.kdtree.size(), --track_size);
        BOOST_CHECK(iter != fix.kdtree.begin());
        int count = 0;
        for(iter_type i = fix.kdtree.begin();
            i != fix.kdtree.end(); ++i, ++count);
        BOOST_CHECK_EQUAL(count, track_size);
      }
  }
  {
    Twenty_Relaxed_kdtree_3D_fixture fix;
    typedef Twenty_Relaxed_kdtree_3D_fixture::kdtree_type
      ::iterator iter_type;
    typedef Twenty_Relaxed_kdtree_3D_fixture::kdtree_type
      ::reverse_iterator riter_type;
    size_type track_size = fix.kdtree.size();
    while (fix.kdtree.size() != 0)
      {
        iter_type iter = --fix.kdtree.end();
        BOOST_REQUIRE_NO_THROW(fix.kdtree.erase(iter));
        BOOST_CHECK_EQUAL(fix.kdtree.size(), --track_size);
        BOOST_CHECK(iter != (--fix.kdtree.end()));
        int count = 0;
        for(riter_type i = fix.kdtree.rbegin();
            i != fix.kdtree.rend(); ++i, ++count);
        BOOST_CHECK_EQUAL(count, track_size);
      }
  }
  // erase all and check that total ordering is preserved.
  {
    typedef Hundred_Relaxed_kdtree_5D_fixture::kdtree_type
      ::iterator iterator;
    typedef details::Const_Mapping_iterator
      <Hundred_Relaxed_kdtree_5D_fixture::kdtree_type::rank_type,
      Hundred_Relaxed_kdtree_5D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_5D_fixture::kdtree_type::value_type,
      Hundred_Relaxed_kdtree_5D_fixture::kdtree_type::node_type,
      bracket_less<point5d> > mapping_iterator;
    Hundred_Relaxed_kdtree_5D_fixture fix;
    size_type track_size = fix.kdtree.size();
    while (fix.kdtree.size() != 0)
      {
        iterator eraser = fix.kdtree.begin();
        std::advance(eraser, static_cast<size_type>(rand()) % fix.kdtree.size());
        mapping_iterator begin_0;
        BOOST_REQUIRE_NO_THROW(begin_0 = mapping_iterator::minimum
                               (fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                                static_cast<Relaxed_kdtree_node<point5d>*>
                                (fix.kdtree.end().node->parent)));
        mapping_iterator end_0;
        BOOST_REQUIRE_NO_THROW(end_0 = mapping_iterator
                               (fix.kdtree.rank(), fix.kdtree.key_comp(), 0,
                                details::decr_dim(fix.kdtree.rank(), 0),
                                static_cast<Relaxed_kdtree_node<point5d>*>
                                (fix.kdtree.end().node)));
        mapping_iterator begin_1;
        BOOST_REQUIRE_NO_THROW(begin_1 = mapping_iterator::minimum
                               (fix.kdtree.rank(), fix.kdtree.key_comp(), 1, 0,
                                static_cast<Relaxed_kdtree_node<point5d>*>
                                (fix.kdtree.end().node->parent)));
        mapping_iterator end_1;
        BOOST_REQUIRE_NO_THROW(end_1 = mapping_iterator
                               (fix.kdtree.rank(), fix.kdtree.key_comp(), 1,
                                details::decr_dim(fix.kdtree.rank(), 0),
                                static_cast<Relaxed_kdtree_node<point5d>*>
                                (fix.kdtree.end().node)));
        size_type count = 0;
        for(mapping_iterator i = begin_0; i != end_0; ++i, ++count);
        BOOST_CHECK_EQUAL(count, track_size);
        if (count != track_size && count < 23) abort(); // for inspection of tree
        count = 0;
        for(mapping_iterator i = begin_1; i != end_1; ++i, ++count);
        BOOST_CHECK_EQUAL(count, track_size);
        if (count != track_size && count < 23) abort(); // for inspection of tree
        count = 0;
        for(mapping_iterator i = end_0; i != begin_0; --i, ++count);
        BOOST_CHECK_EQUAL(count, track_size);
        count = 0;
        for(mapping_iterator i = end_1; i != begin_1; --i, ++count);
        BOOST_CHECK_EQUAL(count, track_size);
        BOOST_REQUIRE_NO_THROW(fix.kdtree.erase(eraser));
        BOOST_CHECK_EQUAL(fix.kdtree.size(), --track_size);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_erase_bulk )
{
  Hundred_Relaxed_kdtree_5D_fixture fix;
  std::vector<point5d> store;
  store.reserve(100);
  store.insert(store.end(), fix.kdtree.begin(), fix.kdtree.end());
  BOOST_REQUIRE_NO_THROW(fix.kdtree.erase(store.begin(), store.end()));
  BOOST_CHECK(fix.kdtree.empty() == true);
}

#endif // SPATIAL_TEST_RELAXED_KDTREE_HPP
