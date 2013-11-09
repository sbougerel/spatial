// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_test_kdtree.hpp
 *  All tests for the elements defined in spatial_kdtree.hpp are located in this
 *  file.
 *
 *  \see spatial_kdtree.hpp
 */

#ifndef SPATIAL_TEST_KDTREE_HPP
#define SPATIAL_TEST_KDTREE_HPP

BOOST_AUTO_TEST_CASE( test_empty_kdtree_basic )
{
  idle_pointset_fix<int2> fix;
  BOOST_CHECK(fix.container.end().node->left == fix.container.end().node);
  BOOST_CHECK(fix.container.end().node->right == fix.container.end().node);
  BOOST_CHECK(fix.container.end().node->parent == fix.container.end().node);
  BOOST_CHECK(fix.container.begin().node == fix.container.end().node); // leftmost
  BOOST_CHECK(fix.container.cbegin().node == fix.container.end().node);
  BOOST_CHECK(fix.container.empty());
  BOOST_CHECK(fix.container.begin() == fix.container.end());
  BOOST_CHECK(fix.container.cbegin() == fix.container.end());
  BOOST_CHECK(fix.container.cbegin() == fix.container.end());
  BOOST_CHECK(fix.container.cend() == fix.container.end());
  BOOST_CHECK(fix.container.rbegin() == fix.container.rend());
  BOOST_CHECK(fix.container.crbegin() == fix.container.rend());
  BOOST_CHECK(fix.container.crend() == fix.container.rend());
}

BOOST_AUTO_TEST_CASE( test_empty_kdtree_basic_clear )
{
  idle_pointset_fix<int2> fix;
  BOOST_REQUIRE_NO_THROW(fix.container.clear());
  BOOST_CHECK(fix.container.cend().node->left == fix.container.end().node);
  BOOST_CHECK(fix.container.end().node->right == fix.container.end().node);
  BOOST_CHECK(fix.container.end().node->parent == fix.container.end().node);
  BOOST_CHECK(fix.container.begin().node->right == fix.container.end().node); // leftmost
  BOOST_CHECK(fix.container.empty());
}

BOOST_AUTO_TEST_CASE( test_empty_kdtree_basic_swap )
{
  runtime_idle_pointset_fix<int2> fix1;
  runtime_idle_pointset_fix<int2> fix2;
  runtime_idle_pointset_fix<int2>::container_type::iterator iter1 = fix1.container.end();
  runtime_idle_pointset_fix<int2>::container_type::iterator iter2 = fix2.container.end();
  swap(fix1.container, fix2.container);
  BOOST_CHECK(fix1.container.end() == iter1);
  BOOST_CHECK(fix2.container.end() == iter2);
  BOOST_CHECK(fix1.container.empty());
  BOOST_CHECK(fix2.container.empty());
}

BOOST_AUTO_TEST_CASE( test_empty_kdtree_basic_max_size )
{
  idle_pointset_fix<double6> fix;
  std::allocator<details::Kdtree_link<double6, double6> > node_alloc;
  BOOST_CHECK(fix.container.max_size() == node_alloc.max_size());
}

BOOST_AUTO_TEST_CASE( test_empty_kdtree_basic_accessor )
{
  idle_pointset_fix<double6> fix;
  idle_pointset_fix<double6>::container_type::allocator_type alloc;
  idle_pointset_fix<double6>::container_type::rank_type rank;
  idle_pointset_fix<double6>::container_type::key_compare compare;
  BOOST_CHECK_EQUAL(fix.container.dimension(), 6);
  BOOST_CHECK_NO_THROW(rank = fix.container.rank());
  BOOST_CHECK_NO_THROW(compare = fix.container.key_comp());
  BOOST_CHECK_NO_THROW(alloc = fix.container.get_allocator());
}

BOOST_AUTO_TEST_CASE( test_empty_kdtree )
{
  idle_point_multimap_fix<int2, std::string> fix;
  BOOST_CHECK(fix.container.empty());
  BOOST_CHECK(fix.container.begin() == fix.container.end());
  // The rest was tested above...
}

BOOST_AUTO_TEST_CASE( test_kdtree_insert_iterate_forward )
{
  idle_pointset_fix<int2> fix;
  std::tr1::array<int2, 100> points;
  for(std::tr1::array<int2, 100>::iterator i
        = points.begin(); i != points.end(); ++i)
    {
      (*i)[0] = std::rand() % 20; // to increase chances of collisions
      (*i)[1] = std::rand() % 20;
      BOOST_REQUIRE_NO_THROW(fix.container.insert(*i));
      BOOST_TEST_CHECKPOINT("Adding element " << (i - points.begin()));
    }
  BOOST_CHECK(fix.container.empty() == false);
  BOOST_REQUIRE_EQUAL(fix.container.size(), 100);
  BOOST_CHECK_EQUAL(fix.container.count(), 100);
  int count = 0;
  for(idle_pointset_fix<int2>::container_type::iterator i
        = fix.container.begin(); i != fix.container.end(); ++i)
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

BOOST_AUTO_TEST_CASE( test_Kdtree_insert_100_iterate_backward )
{
  idle_pointset_fix<int2> fix;
  std::tr1::array<int2, 100> points;
  for(std::tr1::array<int2, 100>::iterator i
        = points.begin(); i != points.end(); ++i)
    {
      (*i)[0] = std::rand() % 20; // to increase chances of collisions
      (*i)[1] = std::rand() % 20;
      BOOST_REQUIRE_NO_THROW(fix.container.insert(*i));
      BOOST_TEST_CHECKPOINT("Adding element " << (i - points.begin()));
    }
  BOOST_CHECK(fix.container.empty() == false);
  BOOST_REQUIRE_EQUAL(fix.container.size(), 100);
  BOOST_CHECK_EQUAL(fix.container.count(), 100);
  int count = 0;
  for(idle_pointset_fix<int2>::container_type::reverse_iterator i
        = fix.container.rbegin(); i != fix.container.rend(); ++i)
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

BOOST_AUTO_TEST_CASE( test_kdtree_erase_iter )
{
  // check that erase at edge preserve basic iterators
  {
    idle_pointset_fix<quad> fix(30, randomize(-10, 10));
    typedef idle_pointset_fix<quad>::container_type
      ::iterator iter_type;
    std::size_t track_size = fix.container.size();
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
    idle_pointset_fix<quad> fix(30, randomize(-10, 10));
    typedef idle_pointset_fix<quad>::container_type
      ::iterator iter_type;
    typedef idle_pointset_fix<quad>::container_type
      ::reverse_iterator riter_type;
    std::size_t track_size = fix.container.size();
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
    typedef idle_pointset_fix<int2>::container_type
      ::iterator iterator;
    typedef mapping_iterator
      <const idle_pointset_fix<int2>::container_type> mapping_iterator;
    idle_pointset_fix<int2> fix(100, randomize(-10, 10));
    std::size_t track_size = fix.container.size();
    while (fix.container.size() != 0)
      {
        iterator eraser = fix.container.begin();
        std::advance(eraser, static_cast<std::size_t>(rand())
                     % fix.container.size());
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
        BOOST_REQUIRE_NO_THROW(fix.container.erase(eraser));
        BOOST_CHECK_EQUAL(fix.container.size(), --track_size);
      }
  }
}

// BOOST_AUTO_TEST_CASE( test_kdtree_bulk_insert )
// {
//   // reuse test_kdtree_insert_100_iterate_forward
//   Empty_Kdtree_2D_fixture fix;
//   std::tr1::array<point2d, 100> points;
//   for(std::tr1::array<point2d, 100>::iterator i
//         = points.begin(); i != points.end(); ++i)
//     {
//       (*i)[0] = std::rand() % 20;
//       (*i)[1] = std::rand() % 20;
//     }
//   BOOST_REQUIRE_NO_THROW(fix.container.insert(points.begin(), points.end()));
//   BOOST_CHECK(fix.container.empty() == false);
//   BOOST_REQUIRE_EQUAL(fix.container.size(), 100);
//   BOOST_CHECK_EQUAL(fix.container.count(), 100);
//   int count = 0;
//   for(Empty_Kdtree_2D_fixture::container_type::iterator i
//         = fix.container.begin(); i != fix.container.end(); ++i)
//     {
//       std::tr1::array<point2d, 100>::iterator match;
//       BOOST_REQUIRE((match = std::find(points.begin(), points.end(), *i))
//                   != points.end());
//       (*match)[0] = -1; // Prevent the same point from being found twice.
//       (*match)[1] = -1;
//       BOOST_REQUIRE_LE(++count, 100);
//     }
//   BOOST_CHECK_EQUAL(count, 100);
// }

// BOOST_AUTO_TEST_CASE( test_kdtree_copy_empty )
// {
//   Empty_Kdtree_2D_fixture fix;
//   Empty_Kdtree_2D_fixture::container_type copy(fix.container);
//   BOOST_CHECK_EQUAL(fix.container.size(), copy.size());
//   BOOST_CHECK_EQUAL(fix.container.dimension(), copy.dimension());
//   BOOST_CHECK(copy.begin() == copy.end());
// }

// BOOST_AUTO_TEST_CASE( test_kdtree_copy )
// {
//   // Simple copy (no rebalancing) should result in identical tree structure, so
//   // iterators should output the same sequence.
//   Hundred_kdtree_2D_fixture fix;
//   Hundred_kdtree_2D_fixture::container_type copy(fix.container);
//   BOOST_CHECK_EQUAL(fix.container.size(), copy.size());
//   BOOST_CHECK_EQUAL(fix.container.dimension(), copy.dimension());
//   typedef Hundred_kdtree_2D_fixture::container_type::iterator iterator;
//   int count = 0;
//   iterator i = fix.container.begin(), j = copy.begin();
//   for(; i != fix.container.end() && j != copy.end(); ++i, ++j, ++count)
//     {
//       BOOST_CHECK_EQUAL((*i)[0], (*j)[0]);
//       BOOST_CHECK_EQUAL((*i)[1], (*j)[1]);
//     }
//   BOOST_CHECK_EQUAL(count, fix.container.size());
//   BOOST_CHECK(j == copy.end());
// }

// BOOST_AUTO_TEST_CASE( test_kdtree_copy_rebalance )
// {
//   // Simple copy (rebalancing) should result in a tree that has the same nodes,
//   // and mapping iteration in one dimension can prove it.
//   Hundred_kdtree_2D_fixture fix;
//   Hundred_kdtree_2D_fixture::container_type copy(fix.container, true);
//   BOOST_CHECK_EQUAL(fix.container.size(), copy.size());
//   BOOST_CHECK_EQUAL(fix.container.dimension(), copy.dimension());
//   int count = 0;
//   typedef details::mapping::iterator
//     <Hundred_kdtree_2D_fixture::container_type>::type iterator;
//   iterator i = details::mapping::begin(fix.container, 0),
//     j = details::mapping::begin(copy, 0),
//     i_end = details::mapping::end(fix.container, 0),
//     j_end = details::mapping::end(copy, 0);
//   for(; i != i_end && j != j_end; ++i, ++j, ++count)
//     {
//       BOOST_CHECK_EQUAL((*i)[0], (*j)[0]);
//     }
//   BOOST_CHECK_EQUAL(count, fix.container.size());
//   BOOST_CHECK(i == i_end);
//   BOOST_CHECK(j == j_end);
// }

// BOOST_AUTO_TEST_CASE( test_kdtree_assign_empty_trees )
// {
//   Empty_Kdtree_2D_fixture empty;
//   Hundred_kdtree_2D_fixture full;
//   full.kdtree = empty.kdtree;
//   BOOST_CHECK_EQUAL(empty.kdtree.size(), full.kdtree.size());
//   BOOST_CHECK_EQUAL(empty.kdtree.dimension(), full.kdtree.dimension());
//   BOOST_CHECK(full.kdtree.begin() == full.kdtree.end());
// }

// BOOST_AUTO_TEST_CASE( test_kdtree_assignment )
// {
//   Hundred_kdtree_2D_fixture fix1;
//   Hundred_kdtree_2D_fixture fix2;
//   fix2.kdtree = fix1.kdtree;
//   BOOST_CHECK_EQUAL(fix1.kdtree.size(), fix2.kdtree.size());
//   BOOST_CHECK_EQUAL(fix1.kdtree.dimension(), fix2.kdtree.dimension());
//   typedef Hundred_kdtree_2D_fixture::container_type::iterator iterator;
//   int count = 0;
//   iterator i = fix1.kdtree.begin(), j = fix2.kdtree.begin();
//   for(; i != fix1.kdtree.end() && j != fix2.kdtree.end(); ++i, ++j, ++count)
//     {
//       BOOST_CHECK_EQUAL((*i)[0], (*j)[0]);
//       BOOST_CHECK_EQUAL((*i)[1], (*j)[1]);
//     }
//   BOOST_CHECK_EQUAL(count, fix1.kdtree.size());
//   BOOST_CHECK(j == fix2.kdtree.end());
// }

// BOOST_AUTO_TEST_CASE( test_kdtree_swap )
// {
//   Hundred_kdtree_2D_fixture fix1;
//   Hundred_kdtree_2D_fixture fix2;
//   std::vector<point2d> point_fix1; point_fix1.reserve(100);
//   std::vector<point2d> point_fix2; point_fix2.reserve(100);
//   std::copy(fix1.kdtree.cbegin(), fix1.kdtree.cend(),
//             std::back_inserter(point_fix1));
//   std::copy(fix2.kdtree.cbegin(), fix2.kdtree.cend(),
//             std::back_inserter(point_fix2));
//   using std::swap; // it should still resolve into spatial::swap
//   // swap once
//   swap(fix1.kdtree, fix2.kdtree);
//   typedef Hundred_kdtree_2D_fixture::container_type::iterator iterator;
//   std::size_t count = 0;
//   for(iterator i = fix1.kdtree.begin();
//       i != fix1.kdtree.end(); ++i, ++count)
//     {
//       BOOST_CHECK_EQUAL((*i)[0], point_fix2[count][0]);
//       BOOST_CHECK_EQUAL((*i)[1], point_fix2[count][1]);
//     }
//   BOOST_CHECK_EQUAL(count, 100);
//   count = 0;
//   for(iterator i = fix2.kdtree.begin();
//       i != fix2.kdtree.end(); ++i, ++count)
//     {
//       BOOST_CHECK_EQUAL((*i)[0], point_fix1[count][0]);
//       BOOST_CHECK_EQUAL((*i)[1], point_fix1[count][1]);
//     }
//   BOOST_CHECK_EQUAL(count, 100);
//   // swap twice, back to normal
//   swap(fix1.kdtree, fix2.kdtree);
//   typedef Hundred_kdtree_2D_fixture::container_type::iterator iterator;
//   count = 0;
//   for(iterator i = fix1.kdtree.begin();
//       i != fix1.kdtree.end(); ++i, ++count)
//     {
//       BOOST_CHECK_EQUAL((*i)[0], point_fix1[count][0]);
//       BOOST_CHECK_EQUAL((*i)[1], point_fix1[count][1]);
//     }
//   BOOST_CHECK_EQUAL(count, 100);
//   count = 0;
//   for(iterator i = fix2.kdtree.begin();
//       i != fix2.kdtree.end(); ++i, ++count)
//     {
//       BOOST_CHECK_EQUAL((*i)[0], point_fix2[count][0]);
//       BOOST_CHECK_EQUAL((*i)[1], point_fix2[count][1]);
//     }
//   BOOST_CHECK_EQUAL(count, 100);
// }

// BOOST_AUTO_TEST_CASE( test_kdtree_swap_empty )
// {
//   Hundred_kdtree_2D_fixture fix1;
//   Empty_Kdtree_2D_fixture fix2;
//   std::vector<point2d> point_fix1; point_fix1.reserve(100);
//   std::copy(fix1.kdtree.cbegin(), fix1.kdtree.cend(),
//             std::back_inserter(point_fix1));
//   using std::swap; // it should still resolve into spatial::swap
//   // swap once
//   swap(fix1.kdtree, fix2.kdtree);
//   typedef Empty_Kdtree_2D_fixture::container_type::iterator iterator;
//   std::size_t count = 0;
//   for(iterator i = fix2.kdtree.begin();
//       i != fix2.kdtree.end(); ++i, ++count)
//     {
//       BOOST_CHECK_EQUAL((*i)[0], point_fix1[count][0]);
//       BOOST_CHECK_EQUAL((*i)[1], point_fix1[count][1]);
//     }
//   BOOST_CHECK_EQUAL(count, 100);
//   BOOST_CHECK(fix1.kdtree.begin() == fix1.kdtree.end());
//   BOOST_CHECK_EQUAL(fix1.kdtree.size(), 0);
//   // swap twice, back to normal
//   swap(fix1.kdtree, fix2.kdtree);
//   count = 0;
//   for(iterator i = fix1.kdtree.begin();
//       i != fix1.kdtree.end(); ++i, ++count)
//     {
//       BOOST_CHECK_EQUAL((*i)[0], point_fix1[count][0]);
//       BOOST_CHECK_EQUAL((*i)[1], point_fix1[count][1]);
//     }
//   BOOST_CHECK_EQUAL(count, 100);
//   BOOST_CHECK(fix2.kdtree.begin() == fix2.kdtree.end());
//   BOOST_CHECK_EQUAL(fix2.kdtree.size(), 0);
// }

// BOOST_AUTO_TEST_CASE( test_kdtree_rebalance )
// {
//   typedef details::Const_Mapping_iterator
//     <Hundred_kdtree_2D_fixture::container_type::rank_type,
//     Hundred_kdtree_2D_fixture::container_type::key_type,
//     Hundred_kdtree_2D_fixture::container_type::value_type,
//     Hundred_kdtree_2D_fixture::container_type::node_type,
//     bracket_less<point2d> > mapping_iterator;
//   // The performance gain of rebalance cannot be mesured now (see performance
//   // executable) but we can make sure that total ordering as remained.
//   Hundred_kdtree_2D_fixture fix;
//   size_type size = fix.container.size();
//   Hundred_kdtree_2D_fixture::container_type copy(fix.container);
//   BOOST_REQUIRE_NO_THROW(copy.rebalance());
//   mapping_iterator orig_begin_0;
//   BOOST_REQUIRE_NO_THROW(orig_begin_0 = mapping_iterator::minimum
//                          (fix.container.rank(), fix.container.key_comp(), 0, 0,
//                           static_cast<Kdtree_node<point2d>*>
//                           (fix.container.end().node->parent)));
//   mapping_iterator orig_end_0;
//   BOOST_REQUIRE_NO_THROW(orig_end_0 = mapping_iterator
//                          (fix.container.rank(), fix.container.key_comp(), 0,
//                           details::decr_dim(fix.container.rank(), 0),
//                           static_cast<Kdtree_node<point2d>*>
//                           (fix.container.end().node)));
//   mapping_iterator orig_begin_1;
//   BOOST_REQUIRE_NO_THROW(orig_begin_1 = mapping_iterator::minimum
//                          (fix.container.rank(), fix.container.key_comp(), 1, 0,
//                           static_cast<Kdtree_node<point2d>*>
//                           (fix.container.end().node->parent)));
//   mapping_iterator orig_end_1;
//   BOOST_REQUIRE_NO_THROW(orig_end_1 = mapping_iterator
//                          (fix.container.rank(), fix.container.key_comp(), 1,
//                           details::decr_dim(fix.container.rank(), 0),
//                           static_cast<Kdtree_node<point2d>*>
//                           (fix.container.end().node)));
//   mapping_iterator copy_begin_0;
//   BOOST_REQUIRE_NO_THROW(copy_begin_0 = mapping_iterator::minimum
//                          (copy.rank(), copy.key_comp(),
//                           0, 0, static_cast<Kdtree_node<point2d>*>
//                           (copy.end().node->parent)));
//   mapping_iterator copy_end_0;
//   BOOST_REQUIRE_NO_THROW(copy_end_0 = mapping_iterator
//                          (copy.rank(), copy.key_comp(), 0,
//                           details::decr_dim(copy.rank(), 0),
//                           static_cast<Kdtree_node<point2d>*>
//                           (copy.end().node)));
//   mapping_iterator copy_begin_1;
//   BOOST_REQUIRE_NO_THROW(copy_begin_1 = mapping_iterator::minimum
//                          (copy.rank(), copy.key_comp(), 1, 0,
//                           static_cast<Kdtree_node<point2d>*>
//                           (copy.end().node->parent)));
//   mapping_iterator copy_end_1;
//   BOOST_REQUIRE_NO_THROW(copy_end_1 = mapping_iterator
//                          (copy.rank(), copy.key_comp(), 1,
//                           details::decr_dim(copy.rank(), 0),
//                           static_cast<Kdtree_node<point2d>*>
//                           (copy.end().node)));
//   int count = 0;
//   mapping_iterator i = orig_begin_0, j = copy_begin_0;
//   for(; i != orig_end_0 && j != copy_end_0; ++i, ++j, ++count);
//   BOOST_CHECK_EQUAL(count, size);
//   BOOST_CHECK(i == orig_end_0);
//   BOOST_CHECK(j == copy_end_0);
//   count = 0;
//   i = orig_begin_1; j = copy_begin_1;
//   for(; i != orig_end_1 && j != copy_end_1; ++i, ++j, ++count);
//   BOOST_CHECK_EQUAL(count, size);
//   BOOST_CHECK(i == orig_end_1);
//   BOOST_CHECK(j == copy_end_1);
//   count = 0;
//   i = orig_end_0; j = copy_end_0;
//   for(; i != orig_begin_0 && j != copy_begin_0; --i, --j, ++count);
//   BOOST_CHECK_EQUAL(count, size);
//   BOOST_CHECK(i == orig_begin_0);
//   BOOST_CHECK(j == copy_begin_0);
//   count = 0;
//   i = orig_end_1; j = copy_end_1;
//   for(; i != orig_begin_1 && j != copy_begin_1; --i, --j, ++count);
//   BOOST_CHECK_EQUAL(count, size);
//   BOOST_CHECK(i == orig_begin_1);
//   BOOST_CHECK(j == copy_begin_1);
// }

// BOOST_AUTO_TEST_CASE( test_kdtree_copy_and_rebalance )
// {
//   typedef details::Const_Mapping_iterator
//     <Hundred_kdtree_2D_fixture::container_type::rank_type,
//     Hundred_kdtree_2D_fixture::container_type::key_type,
//     Hundred_kdtree_2D_fixture::container_type::value_type,
//     Hundred_kdtree_2D_fixture::container_type::node_type,
//     bracket_less<point2d> > mapping_iterator;
//   // The performance gain of rebalance cannot be mesured now (see performance
//   // executable) but we can make sure that total ordering as remained.
//   Hundred_kdtree_2D_fixture fix;
//   size_type size = fix.container.size();
//   Hundred_kdtree_2D_fixture::container_type copy(fix.container, true);
//   mapping_iterator orig_begin_0;
//   BOOST_REQUIRE_NO_THROW(orig_begin_0 = mapping_iterator::minimum
//                          (fix.container.rank(), fix.container.key_comp(), 0, 0,
//                           static_cast<Kdtree_node<point2d>*>
//                           (fix.container.end().node->parent)));
//   mapping_iterator orig_end_0;
//   BOOST_REQUIRE_NO_THROW(orig_end_0 = mapping_iterator
//                          (fix.container.rank(), fix.container.key_comp(), 0,
//                           details::decr_dim(fix.container.rank(), 0),
//                           static_cast<Kdtree_node<point2d>*>
//                           (fix.container.end().node)));
//   mapping_iterator orig_begin_1;
//   BOOST_REQUIRE_NO_THROW(orig_begin_1 = mapping_iterator::minimum
//                          (fix.container.rank(), fix.container.key_comp(), 1, 0,
//                           static_cast<Kdtree_node<point2d>*>
//                           (fix.container.end().node->parent)));
//   mapping_iterator orig_end_1;
//   BOOST_REQUIRE_NO_THROW(orig_end_1 = mapping_iterator
//                          (fix.container.rank(), fix.container.key_comp(), 1,
//                           details::decr_dim(fix.container.rank(), 0),
//                           static_cast<Kdtree_node<point2d>*>
//                           (fix.container.end().node)));
//   mapping_iterator copy_begin_0;
//   BOOST_REQUIRE_NO_THROW(copy_begin_0 = mapping_iterator::minimum
//                          (copy.rank(), copy.key_comp(), 0, 0,
//                           static_cast<Kdtree_node<point2d>*>
//                           (copy.end().node->parent)));
//   mapping_iterator copy_end_0;
//   BOOST_REQUIRE_NO_THROW(copy_end_0 = mapping_iterator
//                          (copy.rank(), copy.key_comp(), 0,
//                           details::decr_dim(copy.rank(), 0),
//                           static_cast<Kdtree_node<point2d>*>
//                           (copy.end().node)));
//   mapping_iterator copy_begin_1;
//   BOOST_REQUIRE_NO_THROW(copy_begin_1 = mapping_iterator::minimum
//                          (copy.rank(), copy.key_comp(), 1, 0,
//                           static_cast<Kdtree_node<point2d>*>
//                           (copy.end().node->parent)));
//   mapping_iterator copy_end_1;
//   BOOST_REQUIRE_NO_THROW(copy_end_1 = mapping_iterator
//                          (copy.rank(), copy.key_comp(), 1,
//                           details::decr_dim(copy.rank(), 0),
//                           static_cast<Kdtree_node<point2d>*>
//                           (copy.end().node)));
//   int count = 0;
//   mapping_iterator i = orig_begin_0, j = copy_begin_0;
//   for(; i != orig_end_0 && j != copy_end_0; ++i, ++j, ++count);
//   BOOST_CHECK_EQUAL(count, size);
//   BOOST_CHECK(i == orig_end_0);
//   BOOST_CHECK(j == copy_end_0);
//   count = 0;
//   i = orig_begin_1; j = copy_begin_1;
//   for(; i != orig_end_1 && j != copy_end_1; ++i, ++j, ++count);
//   BOOST_CHECK_EQUAL(count, size);
//   BOOST_CHECK(i == orig_end_1);
//   BOOST_CHECK(j == copy_end_1);
//   count = 0;
//   i = orig_end_0; j = copy_end_0;
//   for(; i != orig_begin_0 && j != copy_begin_0; --i, --j, ++count);
//   BOOST_CHECK_EQUAL(count, size);
//   BOOST_CHECK(i == orig_begin_0);
//   BOOST_CHECK(j == copy_begin_0);
//   count = 0;
//   i = orig_end_1; j = copy_end_1;
//   for(; i != orig_begin_1 && j != copy_begin_1; --i, --j, ++count);
//   BOOST_CHECK_EQUAL(count, size);
//   BOOST_CHECK(i == orig_begin_1);
//   BOOST_CHECK(j == copy_begin_1);
// }

// BOOST_AUTO_TEST_CASE( test_kdtree_rebalance_empty )
// {
//   Empty_Kdtree_2D_fixture fix;
//   BOOST_REQUIRE_NO_THROW(fix.container.rebalance());
//   BOOST_CHECK(fix.container.empty());
// }

// BOOST_AUTO_TEST_CASE( test_kdtree_find )
// {
//   {
//     // Find on empty tree should return end.
//     Empty_Kdtree_2D_fixture fix;
//     point2d some_value = { {0, 0} };
//     Empty_Kdtree_2D_fixture::container_type::iterator it
//       = fix.container.find(some_value);
//     BOOST_CHECK(it == fix.container.end());
//   }
//   {
//     // Find on empty const tree should return const end
//     Empty_Kdtree_2D_fixture fix;
//     point2d some_value = { {0, 0} };
//     Empty_Kdtree_2D_fixture::container_type::const_iterator it
//       = const_cast<const Empty_Kdtree_2D_fixture*>
//       (&fix)->kdtree.find(some_value);
//     BOOST_CHECK(it == fix.container.cend());
//   }
//   {
//     // Find the one value of a tree should return beginning.
//     Empty_Kdtree_2D_fixture fix;
//     point2d some_value = { {0, 0} };
//     fix.container.insert(some_value);
//     Empty_Kdtree_2D_fixture::container_type::iterator it
//       = fix.container.find(some_value);
//     BOOST_CHECK(it == fix.container.begin());
//   }
//   {
//     // Find on a const tree with one value should return const beginning.
//     Empty_Kdtree_2D_fixture fix;
//     point2d some_value = { {0, 0} };
//     fix.container.insert(some_value);
//     Empty_Kdtree_2D_fixture::container_type::const_iterator it
//       = const_cast<const Empty_Kdtree_2D_fixture::container_type*>
//       (&fix.container)->find(some_value);
//     BOOST_CHECK(it == fix.container.cbegin());
//   }
//   {
//     // Find a value not in the tree should return end.
//     Empty_Kdtree_2D_fixture fix;
//     point2d some_value = { {0, 0} };
//     point2d other_value = { {1, 1} };
//     fix.container.insert(some_value);
//     Empty_Kdtree_2D_fixture::container_type::iterator it
//       = fix.container.find(other_value);
//     BOOST_CHECK(it == fix.container.end());
//   }
//   {
//     // Find a value not in the const tree should return const end.
//     Empty_Kdtree_2D_fixture fix;
//     point2d some_value = { {0, 0} };
//     point2d other_value = { {1, 1} };
//     fix.container.insert(some_value);
//     Empty_Kdtree_2D_fixture::container_type::const_iterator it
//       = const_cast<const Empty_Kdtree_2D_fixture::container_type*>
//       (&fix.container)->find(other_value);
//     BOOST_CHECK(it == fix.container.end());
//   }
//   {
//     // Find should find all points in the tree, even when const
//     Hundred_kdtree_2D_fixture fix;
//     {
//       Hundred_kdtree_2D_fixture::container_type::iterator iter
//         = fix.container.begin();
//       Hundred_kdtree_2D_fixture::container_type::iterator end
//         = fix.container.end();
//       for(; iter != end; ++iter)
//         {
//           Hundred_kdtree_2D_fixture::container_type::iterator it
//             = fix.container.find(*iter);
//           BOOST_REQUIRE(it != end);
//           BOOST_CHECK(*it == *iter);
//         }
//     }
//     {
//       Hundred_kdtree_2D_fixture::container_type::const_iterator iter
//         = fix.container.cbegin();
//       Hundred_kdtree_2D_fixture::container_type::const_iterator end
//         = fix.container.cend();
//       for(; iter != end; ++iter)
//         {
//           Hundred_kdtree_2D_fixture::container_type::const_iterator it
//             = const_cast<const Hundred_kdtree_2D_fixture::container_type*>
//             (&fix.container)->find(*iter);
//           BOOST_REQUIRE(it != end);
//           BOOST_CHECK(*it == *iter);
//         }
//     }
//     // But it should not find a point that is not in the tree!
//     {
//       point2d some_value = { { 30, 30 } };
//       Hundred_kdtree_2D_fixture::container_type::iterator iter
//         = fix.container.find(some_value);
//       BOOST_CHECK(iter == fix.container.end());
//     }
//     {
//       point2d some_value = { { 30, 30 } };
//       Hundred_kdtree_2D_fixture::container_type::const_iterator iter
//         = const_cast<const Hundred_kdtree_2D_fixture::container_type*>
//         (&fix.container)->find(some_value);
//       BOOST_CHECK(iter == fix.container.end());
//     }
//     // Verify that the equal operator is indeed used in place of the rest
//     {
//       typedef Kdtree<details::Dynamic_rank, point2d, point2d,
//                      bracket_less<point2d>, std::allocator<point2d>, false>
//         kdtree_type;
//       // 1D tree where we store 2d objects...
//       kdtree_type tree(details::Dynamic_rank(1));
//       point2d one   = { { 0, 1 } };
//       point2d two   = { { 0, 2 } };
//       point2d three = { { 0, 3 } };
//       tree.insert(one);
//       tree.insert(two);
//       tree.insert(three);
//       kdtree_type::iterator iter;
//       kdtree_type::iterator end = tree.end();
//       iter = tree.find(one);
//       BOOST_REQUIRE(iter != end);
//       BOOST_CHECK(one == *iter || two == *iter || three == *iter);
//     }
//   }
// }

// BOOST_AUTO_TEST_CASE ( test_kdtree_erase_key )
// {
//   {
//     // Erase on empty tree should return 0.
//     Empty_Kdtree_2D_fixture fix;
//     point2d some_value = { {0, 0} };
//     size_type count = fix.container.erase(some_value);
//     BOOST_CHECK_EQUAL(count, 0);
//     BOOST_CHECK(fix.container.end() == fix.container.begin());
//     BOOST_CHECK(fix.container.empty());
//   }
//   {
//     // Erase one value of a tree should return 1 and leave an empty tree.
//     Empty_Kdtree_2D_fixture fix;
//     point2d some_value = { {0, 0} };
//     fix.container.insert(some_value);
//     BOOST_CHECK(!fix.container.empty());
//     size_type count = fix.container.erase(some_value);
//     BOOST_CHECK_EQUAL(count, 1);
//     BOOST_CHECK(fix.container.end() == fix.container.begin());
//     BOOST_CHECK(fix.container.empty());
//   }
//   {
//     // Erase a value not in the tree should leave the tree untouched.
//     Empty_Kdtree_2D_fixture fix;
//     point2d some_value = { {0, 0} };
//     point2d other_value = { {1, 1} };
//     fix.container.insert(some_value);
//     Empty_Kdtree_2D_fixture::container_type::iterator
//       begin_before = fix.container.begin();
//     Empty_Kdtree_2D_fixture::container_type::iterator
//       end_before = fix.container.end();
//     size_type count  = fix.container.erase(other_value);
//     BOOST_CHECK_EQUAL(count, 0);
//     BOOST_CHECK(!fix.container.empty());
//     BOOST_CHECK(fix.container.end() != fix.container.begin());
//     BOOST_CHECK(fix.container.end() == end_before);
//     BOOST_CHECK(fix.container.begin() == begin_before);
//   }
//   {
//     // Should be able to erase multiple values
//     typedef Kdtree<details::Dynamic_rank, point2d, point2d,
//                    bracket_less<point2d>, std::allocator<point2d>, false>
//       kdtree_type;
//     // 1D tree where we store 2d objects...
//     kdtree_type tree(details::Dynamic_rank(1));
//     point2d one   = { { 1, 0 } };
//     point2d two   = { { 2, 0 } };
//     point2d four  = { { 4, 0 } };
//     tree.insert(one);
//     tree.insert(two);
//     tree.insert(two);
//     tree.insert(four);
//     // ... And equal should take the second dimension into account.
//     BOOST_REQUIRE_EQUAL(tree.count(), 4);
//     size_type count = tree.erase(two);
//     BOOST_CHECK_EQUAL(count, 2);
//     BOOST_REQUIRE_EQUAL(tree.count(), 2);
//     BOOST_CHECK(tree.find(one) != tree.end());
//     BOOST_CHECK(tree.find(four) != tree.end());
//     BOOST_CHECK(tree.find(two) == tree.end());
//     kdtree_type::iterator iter = tree.begin();
//     BOOST_REQUIRE(iter != tree.end());
//     BOOST_REQUIRE_NO_THROW(++iter);
//     BOOST_REQUIRE(iter != tree.end());
//     BOOST_REQUIRE_NO_THROW(++iter);
//     BOOST_REQUIRE(iter == tree.end());
//   }
// }

#endif // SPATIAL_TEST_KDTREE_HPP
