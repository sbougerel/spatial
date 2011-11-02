// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_test_kdtree.hpp
 *  @brief  All tests for the elements defined in spatial_kdtree.hpp are
 *  located in this file.
 *  @see spatial_kdtree.hpp
 */

#ifndef SPATIAL_TEST_KDTREE_HPP
#define SPATIAL_TEST_KDTREE_HPP

BOOST_AUTO_TEST_CASE( test_empty_Kdtree_basic )
{
  // Up to now, we only knew it compiled
  Empty_2D_fixture fix;
  BOOST_CHECK(fix.kdtree.end().node->left == fix.kdtree.end().node);
  BOOST_CHECK(fix.kdtree.end().node->right == fix.kdtree.end().node);
  BOOST_CHECK(fix.kdtree.end().node->parent == fix.kdtree.end().node);
  BOOST_CHECK(fix.kdtree.begin().node == fix.kdtree.end().node); // leftmost
  BOOST_CHECK(fix.kdtree.cbegin().node == fix.kdtree.end().node);
  BOOST_CHECK(fix.kdtree.empty());
  BOOST_CHECK(fix.kdtree.begin() == fix.kdtree.end());
  BOOST_CHECK(fix.kdtree.cbegin() == fix.kdtree.end());
  BOOST_CHECK(fix.kdtree.cbegin() == fix.kdtree.end());
  BOOST_CHECK(fix.kdtree.cend() == fix.kdtree.end());
  BOOST_CHECK(fix.kdtree.rbegin() == fix.kdtree.rend());
  BOOST_CHECK(fix.kdtree.crbegin() == fix.kdtree.rend());
  BOOST_CHECK(fix.kdtree.crend() == fix.kdtree.rend());
}

BOOST_AUTO_TEST_CASE( test_empty_Kdtree_basic_clear )
{
  Empty_2D_fixture fix;
  BOOST_REQUIRE_NO_THROW(fix.kdtree.clear());
  BOOST_CHECK(fix.kdtree.cend().node->left == fix.kdtree.end().node);
  BOOST_CHECK(fix.kdtree.end().node->right == fix.kdtree.end().node);
  BOOST_CHECK(fix.kdtree.end().node->parent == fix.kdtree.end().node);
  BOOST_CHECK(fix.kdtree.begin().node->right == fix.kdtree.end().node); // leftmost
  BOOST_CHECK(fix.kdtree.empty());
}

BOOST_AUTO_TEST_CASE( test_empty_Kdtree_basic_swap )
{
  Empty_2D_fixture fix1;
  Empty_2D_fixture fix2;
  Empty_2D_fixture::kdtree_type::iterator iter1 = fix1.kdtree.end();
  Empty_2D_fixture::kdtree_type::iterator iter2 = fix2.kdtree.end();
  swap(fix1.kdtree, fix2.kdtree);
  BOOST_CHECK(fix1.kdtree.end() == iter1);
  BOOST_CHECK(fix2.kdtree.end() == iter2);
  BOOST_CHECK(fix1.kdtree.empty());
  BOOST_CHECK(fix2.kdtree.empty());
}

BOOST_AUTO_TEST_CASE( test_empty_Kdtree_basic_max_size )
{
  Empty_2D_fixture fix;
  std::allocator<Kdtree_node<point2d> > node_alloc;
  BOOST_CHECK(fix.kdtree.max_size() == node_alloc.max_size());
}

BOOST_AUTO_TEST_CASE( test_empty_Kdtree_basic_accessor )
{
  Empty_2D_fixture fix;
  std::allocator<point2d> alloc;
  Empty_2D_fixture::kdtree_type::rank_type rank;
  Empty_2D_fixture::kdtree_type::key_compare compare;
  BOOST_CHECK_EQUAL(fix.kdtree.dimension(), 2);
  BOOST_CHECK_NO_THROW(rank = fix.kdtree.rank());
  BOOST_CHECK_NO_THROW(compare = fix.kdtree.key_comp());
  BOOST_CHECK_NO_THROW(alloc = fix.kdtree.get_allocator());
}

BOOST_AUTO_TEST_CASE( test_Kdtree_basic_equal_range )
{
  Empty_Kdtree_2D_fixture fix;
  fix.kdtree.insert(zeros);
  fix.kdtree.insert(ones);
  fix.kdtree.insert(twos);
  fix.kdtree.insert(threes);
  fix.kdtree.insert(fours);
  // and more twos
  fix.kdtree.insert(twos);
  fix.kdtree.insert(twos);
  fix.kdtree.insert(twos);
  // There are 4 twos.
  typedef Empty_Kdtree_2D_fixture::kdtree_type::equal_iterator
    equal_iterator;
  std::pair<equal_iterator, equal_iterator>
    eq_pair = fix.kdtree.equal_range(twos);
  BOOST_REQUIRE(eq_pair.first != eq_pair.second);
  BOOST_CHECK(*eq_pair.first == twos);
  ++eq_pair.first;
  BOOST_REQUIRE(eq_pair.first != eq_pair.second);
  BOOST_CHECK(*eq_pair.first == twos);
  ++eq_pair.first;
  BOOST_REQUIRE(eq_pair.first != eq_pair.second);
  BOOST_CHECK(*eq_pair.first == twos);
  ++eq_pair.first;
  BOOST_REQUIRE(eq_pair.first != eq_pair.second);
  BOOST_CHECK(*eq_pair.first == twos);
  ++eq_pair.first;
  BOOST_CHECK(eq_pair.first == eq_pair.second);
  // There are no fives...
  eq_pair = fix.kdtree.equal_range(fives);
  BOOST_REQUIRE(eq_pair.first == eq_pair.second);
}

BOOST_AUTO_TEST_CASE( test_Kdtree_basic_const_equal_range )
{
  Empty_Kdtree_2D_fixture fix;
  fix.kdtree.insert(zeros);
  fix.kdtree.insert(ones);
  fix.kdtree.insert(twos);
  fix.kdtree.insert(threes);
  fix.kdtree.insert(fours);
  // and more twos
  fix.kdtree.insert(twos);
  fix.kdtree.insert(twos);
  fix.kdtree.insert(twos);
  // There are 4 twos.
  typedef Empty_Kdtree_2D_fixture::kdtree_type::const_equal_iterator
    equal_iterator;
  std::pair<equal_iterator, equal_iterator>
    eq_pair = fix.kdtree.equal_range(twos);
  BOOST_REQUIRE(eq_pair.first != eq_pair.second);
  BOOST_CHECK(*eq_pair.first == twos);
  ++eq_pair.first;
  BOOST_REQUIRE(eq_pair.first != eq_pair.second);
  BOOST_CHECK(*eq_pair.first == twos);
  ++eq_pair.first;
  BOOST_REQUIRE(eq_pair.first != eq_pair.second);
  BOOST_CHECK(*eq_pair.first == twos);
  ++eq_pair.first;
  BOOST_REQUIRE(eq_pair.first != eq_pair.second);
  BOOST_CHECK(*eq_pair.first == twos);
  ++eq_pair.first;
  BOOST_CHECK(eq_pair.first == eq_pair.second);
  // There are no fives...
  eq_pair = fix.kdtree.equal_range(fives);
  BOOST_REQUIRE(eq_pair.first == eq_pair.second);
}

BOOST_AUTO_TEST_CASE( test_empty_Kdtree )
{
  Empty_Kdtree_2D_fixture fix;
  BOOST_CHECK(fix.kdtree.empty());
  BOOST_CHECK(fix.kdtree.begin() == fix.kdtree.end());
  // The rest was tested above...
}

BOOST_AUTO_TEST_CASE( test_Kdtree_insert_100_iterate_forward )
{
  Empty_Kdtree_2D_fixture fix;
  std::tr1::array<point2d, 100> points;
  for(std::tr1::array<point2d, 100>::iterator i
        = points.begin(); i != points.end(); ++i)
    {
      (*i)[0] = rand() % 20; // to increase chances of collisions
      (*i)[1] = rand() % 20;
      BOOST_REQUIRE_NO_THROW(fix.kdtree.insert(*i));
      BOOST_TEST_CHECKPOINT("Adding element " << (i - points.begin()));
    }
  BOOST_CHECK(fix.kdtree.empty() == false);
  BOOST_REQUIRE_EQUAL(fix.kdtree.size(), 100);
  BOOST_CHECK_EQUAL(fix.kdtree.count(), 100);
  int count = 0;
  for(Empty_Kdtree_2D_fixture::kdtree_type::iterator i
        = fix.kdtree.begin(); i != fix.kdtree.end(); ++i)
    {
      std::tr1::array<point2d, 100>::iterator match;
      BOOST_REQUIRE((match = std::find(points.begin(), points.end(), *i))
                  != points.end());
      (*match)[0] = -1; // Prevent the same point from being found twice.
      (*match)[1] = -1;
      BOOST_TEST_CHECKPOINT("Finding element " << count);
      BOOST_REQUIRE_LE(++count, 100);
    }
  BOOST_CHECK_EQUAL(count, 100);
}

BOOST_AUTO_TEST_CASE( test_Kdtree_insert_100_iterate_backward )
{
  typedef Empty_Kdtree_2D_fixture::kdtree_type::iterator iterator;
  Empty_Kdtree_2D_fixture fix;
  std::tr1::array<point2d, 100> points;
  for(std::tr1::array<point2d, 100>::iterator i
        = points.begin(); i != points.end(); ++i)
    {
      (*i)[0] = rand() % 20;
      (*i)[1] = rand() % 20;
      iterator result; // result iterator must be useful too
      BOOST_REQUIRE_NO_THROW(result = fix.kdtree.insert(*i));
      BOOST_TEST_CHECKPOINT("Adding element " << (i - points.begin()));
      BOOST_CHECK_EQUAL((*i)[0], (*result)[0]);
      BOOST_CHECK_EQUAL((*i)[1], (*result)[1]);
    }
  BOOST_CHECK(fix.kdtree.empty() == false);
  BOOST_REQUIRE_EQUAL(fix.kdtree.size(), 100);
  BOOST_CHECK_EQUAL(fix.kdtree.count(), 100);
  int count = 0;
  for(Empty_Kdtree_2D_fixture::kdtree_type::reverse_iterator i
        = fix.kdtree.rbegin(); i != fix.kdtree.rend(); ++i)
    {
      std::tr1::array<point2d, 100>::iterator match;
      BOOST_REQUIRE((match = std::find(points.begin(), points.end(), *i))
                  != points.end());
      (*match)[0] = -1; // Prevent the same point from being found twice.
      (*match)[1] = -1;
      BOOST_TEST_CHECKPOINT("Finding element " << count);
      BOOST_REQUIRE_LE(++count, 100);
    }
  BOOST_CHECK_EQUAL(count, 100);
}

BOOST_AUTO_TEST_CASE( test_kdtree_erase_iter )
{
  // check that erase at edge preserve basic iterators
  {
    Twenty_kdtree_3D_fixture fix;
    typedef Twenty_kdtree_3D_fixture::kdtree_type
      ::iterator iter_type;
    std::size_t track_size = fix.kdtree.size();
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
    Twenty_kdtree_3D_fixture fix;
    typedef Twenty_kdtree_3D_fixture::kdtree_type
      ::iterator iter_type;
    typedef Twenty_kdtree_3D_fixture::kdtree_type
      ::reverse_iterator riter_type;
    std::size_t track_size = fix.kdtree.size();
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
    typedef Hundred_kdtree_2D_fixture::kdtree_type
      ::iterator iterator;
    typedef details::Const_Mapping_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_kdtree_2D_fixture::kdtree_type::node_type,
      bracket_less<point2d> > mapping_iterator;
    Hundred_kdtree_2D_fixture fix;
    std::size_t track_size = fix.kdtree.size();
    while (fix.kdtree.size() != 0)
      {
        iterator eraser = fix.kdtree.begin();
        std::advance(eraser, static_cast<std::size_t>(rand())
                     % fix.kdtree.size());
        mapping_iterator begin_0;
        BOOST_REQUIRE_NO_THROW(begin_0 = mapping_iterator::minimum
                               (fix.kdtree.rank(),
                                fix.kdtree.key_comp(),
                                0, 0,
                                static_cast<Kdtree_node<point2d>*>
                                (fix.kdtree.end().node->parent)));
        mapping_iterator end_0;
        BOOST_REQUIRE_NO_THROW(end_0 = mapping_iterator
                               (fix.kdtree.rank(),
                                fix.kdtree.key_comp(),
                                0, details::decr_dim
                                (fix.kdtree.rank(), 0),
                                static_cast<Kdtree_node<point2d>*>
                                (fix.kdtree.end().node)));
        mapping_iterator begin_1;
        BOOST_REQUIRE_NO_THROW(begin_1 = mapping_iterator::minimum
                               (fix.kdtree.rank(),
                                fix.kdtree.key_comp(),
                                1, 0,
                                static_cast<Kdtree_node<point2d>*>
                                (fix.kdtree.end().node->parent)));
        mapping_iterator end_1;
        BOOST_REQUIRE_NO_THROW(end_1 = mapping_iterator
                               (fix.kdtree.rank(),
                                fix.kdtree.key_comp(),
                                1, details::decr_dim
                                (fix.kdtree.rank(), 0),
                                static_cast<Kdtree_node<point2d>*>
                                (fix.kdtree.end().node)));
        std::size_t count = 0;
        for(mapping_iterator i = begin_0; i != end_0; ++i, ++count);
        BOOST_CHECK_EQUAL(count, track_size);
        if (count != track_size && count < 12) abort();
        count = 0;
        for(mapping_iterator i = begin_1; i != end_1; ++i, ++count);
        BOOST_CHECK_EQUAL(count, track_size);
        if (count != track_size && count < 12) abort();
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

BOOST_AUTO_TEST_CASE( test_kdtree_bulk_erase )
{
  Hundred_kdtree_2D_fixture fix;
  std::vector<point2d> store;
  store.reserve(100);
  store.insert(store.end(), fix.kdtree.begin(), fix.kdtree.end());
  BOOST_REQUIRE_NO_THROW(fix.kdtree.erase(store.begin(), store.end()));
  BOOST_CHECK(fix.kdtree.empty() == true);
}

BOOST_AUTO_TEST_CASE( test_kdtree_bulk_insert )
{
  // reuse test_kdtree_insert_100_iterate_forward
  Empty_Kdtree_2D_fixture fix;
  std::tr1::array<point2d, 100> points;
  for(std::tr1::array<point2d, 100>::iterator i
        = points.begin(); i != points.end(); ++i)
    {
      (*i)[0] = rand() % 20;
      (*i)[1] = rand() % 20;
    }
  BOOST_REQUIRE_NO_THROW(fix.kdtree.insert(points.begin(), points.end()));
  BOOST_CHECK(fix.kdtree.empty() == false);
  BOOST_REQUIRE_EQUAL(fix.kdtree.size(), 100);
  BOOST_CHECK_EQUAL(fix.kdtree.count(), 100);
  int count = 0;
  for(Empty_Kdtree_2D_fixture::kdtree_type::iterator i
        = fix.kdtree.begin(); i != fix.kdtree.end(); ++i)
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

BOOST_AUTO_TEST_CASE( test_kdtree_copy_empty )
{
  Empty_Kdtree_2D_fixture fix;
  Empty_Kdtree_2D_fixture::kdtree_type copy(fix.kdtree);
  BOOST_CHECK_EQUAL(fix.kdtree.size(), copy.size());
  BOOST_CHECK_EQUAL(fix.kdtree.dimension(), copy.dimension());
  BOOST_CHECK(copy.begin() == copy.end());
}

BOOST_AUTO_TEST_CASE( test_kdtree_copy )
{
  // Simple copy (no rebalancing) should result in identical tree structure, so
  // iterators should output the same sequence.
  Hundred_kdtree_2D_fixture fix;
  Hundred_kdtree_2D_fixture::kdtree_type copy(fix.kdtree);
  BOOST_CHECK_EQUAL(fix.kdtree.size(), copy.size());
  BOOST_CHECK_EQUAL(fix.kdtree.dimension(), copy.dimension());
  typedef Hundred_kdtree_2D_fixture::kdtree_type::iterator iterator;
  int count = 0;
  iterator i = fix.kdtree.begin(), j = copy.begin();
  for(; i != fix.kdtree.end() && j != copy.end(); ++i, ++j, ++count)
    {
      BOOST_CHECK_EQUAL((*i)[0], (*j)[0]);
      BOOST_CHECK_EQUAL((*i)[1], (*j)[1]);
    }
  BOOST_CHECK_EQUAL(count, fix.kdtree.size());
  BOOST_CHECK(j == copy.end());
}

BOOST_AUTO_TEST_CASE( test_kdtree_copy_rebalance )
{
  // Simple copy (rebalancing) should result in a tree that has the same nodes,
  // and mapping iteration in one dimension can prove it.
  Hundred_kdtree_2D_fixture fix;
  Hundred_kdtree_2D_fixture::kdtree_type copy(fix.kdtree, true);
  BOOST_CHECK_EQUAL(fix.kdtree.size(), copy.size());
  BOOST_CHECK_EQUAL(fix.kdtree.dimension(), copy.dimension());
  int count = 0;
  typedef details::mapping::iterator
    <Hundred_kdtree_2D_fixture::kdtree_type>::type iterator;
  iterator i = details::mapping::begin(fix.kdtree, 0),
    j = details::mapping::begin(copy, 0),
    i_end = details::mapping::end(fix.kdtree, 0),
    j_end = details::mapping::end(copy, 0);
  for(; i != i_end && j != j_end; ++i, ++j, ++count)
    {
      BOOST_CHECK_EQUAL((*i)[0], (*j)[0]);
    }
  BOOST_CHECK_EQUAL(count, fix.kdtree.size());
  BOOST_CHECK(i == i_end);
  BOOST_CHECK(j == j_end);
}

BOOST_AUTO_TEST_CASE( test_kdtree_assign_empty_trees )
{
  Empty_Kdtree_2D_fixture empty;
  Hundred_kdtree_2D_fixture full;
  full.kdtree = empty.kdtree;
  BOOST_CHECK_EQUAL(empty.kdtree.size(), full.kdtree.size());
  BOOST_CHECK_EQUAL(empty.kdtree.dimension(), full.kdtree.dimension());
  BOOST_CHECK(full.kdtree.begin() == full.kdtree.end());
}

BOOST_AUTO_TEST_CASE( test_kdtree_assignment )
{
  Hundred_kdtree_2D_fixture fix1;
  Hundred_kdtree_2D_fixture fix2;
  fix2.kdtree = fix1.kdtree;
  BOOST_CHECK_EQUAL(fix1.kdtree.size(), fix2.kdtree.size());
  BOOST_CHECK_EQUAL(fix1.kdtree.dimension(), fix2.kdtree.dimension());
  typedef Hundred_kdtree_2D_fixture::kdtree_type::iterator iterator;
  int count = 0;
  iterator i = fix1.kdtree.begin(), j = fix2.kdtree.begin();
  for(; i != fix1.kdtree.end() && j != fix2.kdtree.end(); ++i, ++j, ++count)
    {
      BOOST_CHECK_EQUAL((*i)[0], (*j)[0]);
      BOOST_CHECK_EQUAL((*i)[1], (*j)[1]);
    }
  BOOST_CHECK_EQUAL(count, fix1.kdtree.size());
  BOOST_CHECK(j == fix2.kdtree.end());
}

BOOST_AUTO_TEST_CASE( test_kdtree_swap )
{
  Hundred_kdtree_2D_fixture fix1;
  Hundred_kdtree_2D_fixture fix2;
  std::vector<point2d> point_fix1; point_fix1.reserve(100);
  std::vector<point2d> point_fix2; point_fix2.reserve(100);
  std::copy(fix1.kdtree.cbegin(), fix1.kdtree.cend(),
            std::back_inserter(point_fix1));
  std::copy(fix2.kdtree.cbegin(), fix2.kdtree.cend(),
            std::back_inserter(point_fix2));
  using std::swap; // it should still resolve into spatial::swap
  // swap once
  swap(fix1.kdtree, fix2.kdtree);
  typedef Hundred_kdtree_2D_fixture::kdtree_type::iterator iterator;
  std::size_t count = 0;
  for(iterator i = fix1.kdtree.begin();
      i != fix1.kdtree.end(); ++i, ++count)
    {
      BOOST_CHECK_EQUAL((*i)[0], point_fix2[count][0]);
      BOOST_CHECK_EQUAL((*i)[1], point_fix2[count][1]);
    }
  BOOST_CHECK_EQUAL(count, 100);
  count = 0;
  for(iterator i = fix2.kdtree.begin();
      i != fix2.kdtree.end(); ++i, ++count)
    {
      BOOST_CHECK_EQUAL((*i)[0], point_fix1[count][0]);
      BOOST_CHECK_EQUAL((*i)[1], point_fix1[count][1]);
    }
  BOOST_CHECK_EQUAL(count, 100);
  // swap twice, back to normal
  swap(fix1.kdtree, fix2.kdtree);
  typedef Hundred_kdtree_2D_fixture::kdtree_type::iterator iterator;
  count = 0;
  for(iterator i = fix1.kdtree.begin();
      i != fix1.kdtree.end(); ++i, ++count)
    {
      BOOST_CHECK_EQUAL((*i)[0], point_fix1[count][0]);
      BOOST_CHECK_EQUAL((*i)[1], point_fix1[count][1]);
    }
  BOOST_CHECK_EQUAL(count, 100);
  count = 0;
  for(iterator i = fix2.kdtree.begin();
      i != fix2.kdtree.end(); ++i, ++count)
    {
      BOOST_CHECK_EQUAL((*i)[0], point_fix2[count][0]);
      BOOST_CHECK_EQUAL((*i)[1], point_fix2[count][1]);
    }
  BOOST_CHECK_EQUAL(count, 100);
}

BOOST_AUTO_TEST_CASE( test_kdtree_swap_empty )
{
  Hundred_kdtree_2D_fixture fix1;
  Empty_Kdtree_2D_fixture fix2;
  std::vector<point2d> point_fix1; point_fix1.reserve(100);
  std::copy(fix1.kdtree.cbegin(), fix1.kdtree.cend(),
            std::back_inserter(point_fix1));
  using std::swap; // it should still resolve into spatial::swap
  // swap once
  swap(fix1.kdtree, fix2.kdtree);
  typedef Empty_Kdtree_2D_fixture::kdtree_type::iterator iterator;
  std::size_t count = 0;
  for(iterator i = fix2.kdtree.begin();
      i != fix2.kdtree.end(); ++i, ++count)
    {
      BOOST_CHECK_EQUAL((*i)[0], point_fix1[count][0]);
      BOOST_CHECK_EQUAL((*i)[1], point_fix1[count][1]);
    }
  BOOST_CHECK_EQUAL(count, 100);
  BOOST_CHECK(fix1.kdtree.begin() == fix1.kdtree.end());
  BOOST_CHECK_EQUAL(fix1.kdtree.size(), 0);
  // swap twice, back to normal
  swap(fix1.kdtree, fix2.kdtree);
  count = 0;
  for(iterator i = fix1.kdtree.begin();
      i != fix1.kdtree.end(); ++i, ++count)
    {
      BOOST_CHECK_EQUAL((*i)[0], point_fix1[count][0]);
      BOOST_CHECK_EQUAL((*i)[1], point_fix1[count][1]);
    }
  BOOST_CHECK_EQUAL(count, 100);
  BOOST_CHECK(fix2.kdtree.begin() == fix2.kdtree.end());
  BOOST_CHECK_EQUAL(fix2.kdtree.size(), 0);
}

BOOST_AUTO_TEST_CASE( test_kdtree_rebalance )
{
  typedef details::Const_Mapping_iterator
    <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
    Hundred_kdtree_2D_fixture::kdtree_type::key_type,
    Hundred_kdtree_2D_fixture::kdtree_type::value_type,
    Hundred_kdtree_2D_fixture::kdtree_type::node_type,
    bracket_less<point2d> > mapping_iterator;
  // The performance gain of rebalance cannot be mesured now (see performance
  // executable) but we can make sure that total ordering as remained.
  Hundred_kdtree_2D_fixture fix;
  size_type size = fix.kdtree.size();
  Hundred_kdtree_2D_fixture::kdtree_type copy(fix.kdtree);
  BOOST_REQUIRE_NO_THROW(copy.rebalance());
  mapping_iterator orig_begin_0;
  BOOST_REQUIRE_NO_THROW(orig_begin_0 = mapping_iterator::minimum
                         (fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                          static_cast<Kdtree_node<point2d>*>
                          (fix.kdtree.end().node->parent)));
  mapping_iterator orig_end_0;
  BOOST_REQUIRE_NO_THROW(orig_end_0 = mapping_iterator
                         (fix.kdtree.rank(), fix.kdtree.key_comp(), 0,
                          details::decr_dim(fix.kdtree.rank(), 0),
                          static_cast<Kdtree_node<point2d>*>
                          (fix.kdtree.end().node)));
  mapping_iterator orig_begin_1;
  BOOST_REQUIRE_NO_THROW(orig_begin_1 = mapping_iterator::minimum
                         (fix.kdtree.rank(), fix.kdtree.key_comp(), 1, 0,
                          static_cast<Kdtree_node<point2d>*>
                          (fix.kdtree.end().node->parent)));
  mapping_iterator orig_end_1;
  BOOST_REQUIRE_NO_THROW(orig_end_1 = mapping_iterator
                         (fix.kdtree.rank(), fix.kdtree.key_comp(), 1,
                          details::decr_dim(fix.kdtree.rank(), 0),
                          static_cast<Kdtree_node<point2d>*>
                          (fix.kdtree.end().node)));
  mapping_iterator copy_begin_0;
  BOOST_REQUIRE_NO_THROW(copy_begin_0 = mapping_iterator::minimum
                         (copy.rank(), copy.key_comp(),
                          0, 0, static_cast<Kdtree_node<point2d>*>
                          (copy.end().node->parent)));
  mapping_iterator copy_end_0;
  BOOST_REQUIRE_NO_THROW(copy_end_0 = mapping_iterator
                         (copy.rank(), copy.key_comp(), 0,
                          details::decr_dim(copy.rank(), 0),
                          static_cast<Kdtree_node<point2d>*>
                          (copy.end().node)));
  mapping_iterator copy_begin_1;
  BOOST_REQUIRE_NO_THROW(copy_begin_1 = mapping_iterator::minimum
                         (copy.rank(), copy.key_comp(), 1, 0,
                          static_cast<Kdtree_node<point2d>*>
                          (copy.end().node->parent)));
  mapping_iterator copy_end_1;
  BOOST_REQUIRE_NO_THROW(copy_end_1 = mapping_iterator
                         (copy.rank(), copy.key_comp(), 1,
                          details::decr_dim(copy.rank(), 0),
                          static_cast<Kdtree_node<point2d>*>
                          (copy.end().node)));
  int count = 0;
  mapping_iterator i = orig_begin_0, j = copy_begin_0;
  for(; i != orig_end_0 && j != copy_end_0; ++i, ++j, ++count);
  BOOST_CHECK_EQUAL(count, size);
  BOOST_CHECK(i == orig_end_0);
  BOOST_CHECK(j == copy_end_0);
  count = 0;
  i = orig_begin_1; j = copy_begin_1;
  for(; i != orig_end_1 && j != copy_end_1; ++i, ++j, ++count);
  BOOST_CHECK_EQUAL(count, size);
  BOOST_CHECK(i == orig_end_1);
  BOOST_CHECK(j == copy_end_1);
  count = 0;
  i = orig_end_0; j = copy_end_0;
  for(; i != orig_begin_0 && j != copy_begin_0; --i, --j, ++count);
  BOOST_CHECK_EQUAL(count, size);
  BOOST_CHECK(i == orig_begin_0);
  BOOST_CHECK(j == copy_begin_0);
  count = 0;
  i = orig_end_1; j = copy_end_1;
  for(; i != orig_begin_1 && j != copy_begin_1; --i, --j, ++count);
  BOOST_CHECK_EQUAL(count, size);
  BOOST_CHECK(i == orig_begin_1);
  BOOST_CHECK(j == copy_begin_1);
}

BOOST_AUTO_TEST_CASE( test_kdtree_copy_and_rebalance )
{
  typedef details::Const_Mapping_iterator
    <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
    Hundred_kdtree_2D_fixture::kdtree_type::key_type,
    Hundred_kdtree_2D_fixture::kdtree_type::value_type,
    Hundred_kdtree_2D_fixture::kdtree_type::node_type,
    bracket_less<point2d> > mapping_iterator;
  // The performance gain of rebalance cannot be mesured now (see performance
  // executable) but we can make sure that total ordering as remained.
  Hundred_kdtree_2D_fixture fix;
  size_type size = fix.kdtree.size();
  Hundred_kdtree_2D_fixture::kdtree_type copy(fix.kdtree, true);
  mapping_iterator orig_begin_0;
  BOOST_REQUIRE_NO_THROW(orig_begin_0 = mapping_iterator::minimum
                         (fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                          static_cast<Kdtree_node<point2d>*>
                          (fix.kdtree.end().node->parent)));
  mapping_iterator orig_end_0;
  BOOST_REQUIRE_NO_THROW(orig_end_0 = mapping_iterator
                         (fix.kdtree.rank(), fix.kdtree.key_comp(), 0,
                          details::decr_dim(fix.kdtree.rank(), 0),
                          static_cast<Kdtree_node<point2d>*>
                          (fix.kdtree.end().node)));
  mapping_iterator orig_begin_1;
  BOOST_REQUIRE_NO_THROW(orig_begin_1 = mapping_iterator::minimum
                         (fix.kdtree.rank(), fix.kdtree.key_comp(), 1, 0,
                          static_cast<Kdtree_node<point2d>*>
                          (fix.kdtree.end().node->parent)));
  mapping_iterator orig_end_1;
  BOOST_REQUIRE_NO_THROW(orig_end_1 = mapping_iterator
                         (fix.kdtree.rank(), fix.kdtree.key_comp(), 1,
                          details::decr_dim(fix.kdtree.rank(), 0),
                          static_cast<Kdtree_node<point2d>*>
                          (fix.kdtree.end().node)));
  mapping_iterator copy_begin_0;
  BOOST_REQUIRE_NO_THROW(copy_begin_0 = mapping_iterator::minimum
                         (copy.rank(), copy.key_comp(), 0, 0,
                          static_cast<Kdtree_node<point2d>*>
                          (copy.end().node->parent)));
  mapping_iterator copy_end_0;
  BOOST_REQUIRE_NO_THROW(copy_end_0 = mapping_iterator
                         (copy.rank(), copy.key_comp(), 0,
                          details::decr_dim(copy.rank(), 0),
                          static_cast<Kdtree_node<point2d>*>
                          (copy.end().node)));
  mapping_iterator copy_begin_1;
  BOOST_REQUIRE_NO_THROW(copy_begin_1 = mapping_iterator::minimum
                         (copy.rank(), copy.key_comp(), 1, 0,
                          static_cast<Kdtree_node<point2d>*>
                          (copy.end().node->parent)));
  mapping_iterator copy_end_1;
  BOOST_REQUIRE_NO_THROW(copy_end_1 = mapping_iterator
                         (copy.rank(), copy.key_comp(), 1,
                          details::decr_dim(copy.rank(), 0),
                          static_cast<Kdtree_node<point2d>*>
                          (copy.end().node)));
  int count = 0;
  mapping_iterator i = orig_begin_0, j = copy_begin_0;
  for(; i != orig_end_0 && j != copy_end_0; ++i, ++j, ++count);
  BOOST_CHECK_EQUAL(count, size);
  BOOST_CHECK(i == orig_end_0);
  BOOST_CHECK(j == copy_end_0);
  count = 0;
  i = orig_begin_1; j = copy_begin_1;
  for(; i != orig_end_1 && j != copy_end_1; ++i, ++j, ++count);
  BOOST_CHECK_EQUAL(count, size);
  BOOST_CHECK(i == orig_end_1);
  BOOST_CHECK(j == copy_end_1);
  count = 0;
  i = orig_end_0; j = copy_end_0;
  for(; i != orig_begin_0 && j != copy_begin_0; --i, --j, ++count);
  BOOST_CHECK_EQUAL(count, size);
  BOOST_CHECK(i == orig_begin_0);
  BOOST_CHECK(j == copy_begin_0);
  count = 0;
  i = orig_end_1; j = copy_end_1;
  for(; i != orig_begin_1 && j != copy_begin_1; --i, --j, ++count);
  BOOST_CHECK_EQUAL(count, size);
  BOOST_CHECK(i == orig_begin_1);
  BOOST_CHECK(j == copy_begin_1);
}

BOOST_AUTO_TEST_CASE( test_kdtree_rebalance_empty )
{
  Empty_Kdtree_2D_fixture fix;
  BOOST_REQUIRE_NO_THROW(fix.kdtree.rebalance());
  BOOST_CHECK(fix.kdtree.empty());
}

BOOST_AUTO_TEST_CASE( test_kdtree_find )
{
  {
    // Find on empty tree should return end.
    Empty_Kdtree_2D_fixture fix;
    point2d some_value = { {0, 0} };
    Empty_Kdtree_2D_fixture::kdtree_type::iterator it
      = fix.kdtree.find(some_value);
    BOOST_CHECK(it == fix.kdtree.end());
  }
  {
    // Find on empty const tree should return const end
    Empty_Kdtree_2D_fixture fix;
    point2d some_value = { {0, 0} };
    Empty_Kdtree_2D_fixture::kdtree_type::const_iterator it
      = const_cast<const Empty_Kdtree_2D_fixture*>
      (&fix)->kdtree.find(some_value);
    BOOST_CHECK(it == fix.kdtree.cend());
  }
  {
    // Find the one value of a tree should return beginning.
    Empty_Kdtree_2D_fixture fix;
    point2d some_value = { {0, 0} };
    fix.kdtree.insert(some_value);
    Empty_Kdtree_2D_fixture::kdtree_type::iterator it
      = fix.kdtree.find(some_value);
    BOOST_CHECK(it == fix.kdtree.begin());
  }
  {
    // Find on a const tree with one value should return const beginning.
    Empty_Kdtree_2D_fixture fix;
    point2d some_value = { {0, 0} };
    fix.kdtree.insert(some_value);
    Empty_Kdtree_2D_fixture::kdtree_type::const_iterator it
      = const_cast<const Empty_Kdtree_2D_fixture::kdtree_type*>
      (&fix.kdtree)->find(some_value);
    BOOST_CHECK(it == fix.kdtree.cbegin());
  }
  {
    // Find a value not in the tree should return end.
    Empty_Kdtree_2D_fixture fix;
    point2d some_value = { {0, 0} };
    point2d other_value = { {1, 1} };
    fix.kdtree.insert(some_value);
    Empty_Kdtree_2D_fixture::kdtree_type::iterator it
      = fix.kdtree.find(other_value);
    BOOST_CHECK(it == fix.kdtree.end());
  }
  {
    // Find a value not in the const tree should return const end.
    Empty_Kdtree_2D_fixture fix;
    point2d some_value = { {0, 0} };
    point2d other_value = { {1, 1} };
    fix.kdtree.insert(some_value);
    Empty_Kdtree_2D_fixture::kdtree_type::const_iterator it
      = const_cast<const Empty_Kdtree_2D_fixture::kdtree_type*>
      (&fix.kdtree)->find(other_value);
    BOOST_CHECK(it == fix.kdtree.end());
  }
  {
    // Find should find all points in the tree, even when const
    Hundred_kdtree_2D_fixture fix;
    {
      Hundred_kdtree_2D_fixture::kdtree_type::iterator iter
        = fix.kdtree.begin();
      Hundred_kdtree_2D_fixture::kdtree_type::iterator end
        = fix.kdtree.end();
      for(; iter != end; ++iter)
        {
          Hundred_kdtree_2D_fixture::kdtree_type::iterator it
            = fix.kdtree.find(*iter);
          BOOST_REQUIRE(it != end);
          BOOST_CHECK(*it == *iter);
        }
    }
    {
      Hundred_kdtree_2D_fixture::kdtree_type::const_iterator iter
        = fix.kdtree.cbegin();
      Hundred_kdtree_2D_fixture::kdtree_type::const_iterator end
        = fix.kdtree.cend();
      for(; iter != end; ++iter)
        {
          Hundred_kdtree_2D_fixture::kdtree_type::const_iterator it
            = const_cast<const Hundred_kdtree_2D_fixture::kdtree_type*>
            (&fix.kdtree)->find(*iter);
          BOOST_REQUIRE(it != end);
          BOOST_CHECK(*it == *iter);
        }
    }
    // But it should not find a point that is not in the tree!
    {
      point2d some_value = { { 30, 30 } };
      Hundred_kdtree_2D_fixture::kdtree_type::iterator iter
        = fix.kdtree.find(some_value);
      BOOST_CHECK(iter == fix.kdtree.end());
    }
    {
      point2d some_value = { { 30, 30 } };
      Hundred_kdtree_2D_fixture::kdtree_type::const_iterator iter
        = const_cast<const Hundred_kdtree_2D_fixture::kdtree_type*>
        (&fix.kdtree)->find(some_value);
      BOOST_CHECK(iter == fix.kdtree.end());
    }
    // Verify that the equal operator is indeed used in place of the rest
    {
      typedef Kdtree<details::Dynamic_rank, point2d, point2d,
                     bracket_less<point2d>, std::allocator<point2d>, false>
        kdtree_type;
      // 1D tree where we store 2d objects...
      kdtree_type tree(details::Dynamic_rank(1));
      point2d one   = { { 0, 1 } };
      point2d two   = { { 0, 2 } };
      point2d three = { { 0, 3 } };
      tree.insert(one);
      tree.insert(two);
      tree.insert(three);
      kdtree_type::iterator iter;
      kdtree_type::iterator end = tree.end();
      iter = tree.find(one);
      BOOST_REQUIRE(iter != end);
      BOOST_CHECK(one == *iter || two == *iter || three == *iter);
    }
  }
}

BOOST_AUTO_TEST_CASE ( test_kdtree_erase_key )
{
  {
    // Erase on empty tree should return 0.
    Empty_Kdtree_2D_fixture fix;
    point2d some_value = { {0, 0} };
    size_type count = fix.kdtree.erase(some_value);
    BOOST_CHECK_EQUAL(count, 0);
    BOOST_CHECK(fix.kdtree.end() == fix.kdtree.begin());
    BOOST_CHECK(fix.kdtree.empty());
  }
  {
    // Erase one value of a tree should return 1 and leave an empty tree.
    Empty_Kdtree_2D_fixture fix;
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
    Empty_Kdtree_2D_fixture fix;
    point2d some_value = { {0, 0} };
    point2d other_value = { {1, 1} };
    fix.kdtree.insert(some_value);
    Empty_Kdtree_2D_fixture::kdtree_type::iterator
      begin_before = fix.kdtree.begin();
    Empty_Kdtree_2D_fixture::kdtree_type::iterator
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
    typedef Kdtree<details::Dynamic_rank, point2d, point2d,
                   bracket_less<point2d>, std::allocator<point2d>, false>
      kdtree_type;
    // 1D tree where we store 2d objects...
    kdtree_type tree(details::Dynamic_rank(1));
    point2d one   = { { 1, 0 } };
    point2d two   = { { 2, 0 } };
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

#endif // SPATIAL_TEST_KDTREE_HPP
