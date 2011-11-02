// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_test_mapping.hpp
 *  @brief  All tests for the elements defined in spatial_mapping.hpp are
 *  located in this file.
 *  @see spatial_mapping.hpp
 */

#ifndef SPATIAL_TEST_MAPPING_HPP
#define SPATIAL_TEST_MAPPING_HPP

BOOST_AUTO_TEST_CASE( test_Mapping_iterator_equal )
{
  Pair_kdtree_fixture fix;
  {
    mapping_view<Pair_kdtree_fixture::kdtree_type>::const_iterator
      it1 = details::mapping::const_begin(fix.kdtree, 0);
    mapping_view<Pair_kdtree_fixture::kdtree_type>::const_iterator
      it2 = details::mapping::const_begin(fix.kdtree, 0);
    BOOST_CHECK(it1 == it2);
  }
  {
    details::Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type, Kdtree_node<pair_type>,
       pair_less> it1(fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                      static_cast<Kdtree_node<pair_type>*>
                      (fix.kdtree.begin().node));
    details::Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type, Kdtree_node<pair_type>,
       pair_less> it2(fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                      static_cast<Kdtree_node<pair_type>*>
                      (fix.kdtree.begin().node));
    BOOST_CHECK(it1 == it2);
  }
  {
    details::Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type, Kdtree_node<pair_type>,
       pair_less> it1(fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                      static_cast<Kdtree_node<pair_type>*>
                      (fix.kdtree.begin().node));
    details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type, Kdtree_node<pair_type>,
       pair_less> it2(fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                      static_cast<Kdtree_node<pair_type>*>
                      (fix.kdtree.begin().node));
    BOOST_CHECK(it1 == it2);
  }
  {
    details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type, Kdtree_node<pair_type>,
       pair_less> it1(fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                      static_cast<Kdtree_node<pair_type>*>
                      (fix.kdtree.begin().node));
    details::Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type, Kdtree_node<pair_type>,
       pair_less> it2(fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                      static_cast<Kdtree_node<pair_type>*>
                      (fix.kdtree.begin().node));
    BOOST_CHECK(it1 == it2);
  }
}

BOOST_AUTO_TEST_CASE( test_Mapping_iterator_deference )
{
  {
    Pair_kdtree_fixture fix;
    details::Mapping_iterator<details::Dynamic_rank, pair_type, pair_type,
                              Kdtree_node<pair_type>, pair_less> itr
      (fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    BOOST_CHECK_EQUAL((*itr).first, 1);
    BOOST_CHECK_EQUAL((*itr).second, 2);
    BOOST_CHECK_EQUAL(((*itr).first = 3), 3);
    BOOST_CHECK_EQUAL(((*itr).second = 4), 4);
    BOOST_CHECK_EQUAL((*itr).first, 3);
    BOOST_CHECK_EQUAL((*itr).second, 4);
    BOOST_CHECK_EQUAL((itr->first = 5), 5);
    BOOST_CHECK_EQUAL((itr->second = 6), 6);
    BOOST_CHECK_EQUAL(itr->first, 5);
    BOOST_CHECK_EQUAL(itr->second, 6);
  }
  {
    Pair_kdtree_fixture fix;
    details::Mapping_iterator<details::Dynamic_rank, pair_type, pair_type,
                              Kdtree_node<pair_type>, pair_less> itr
      (fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    BOOST_CHECK_EQUAL((*itr).first, 1);
    BOOST_CHECK_EQUAL((*itr).second, 2);
    BOOST_CHECK_EQUAL(itr->first, 1);
    BOOST_CHECK_EQUAL(itr->second, 2);
  }
}

BOOST_AUTO_TEST_CASE( test_Const_Mapping_iterator_deference )
{
  {
    Pair_kdtree_fixture fix;
    details::Const_Mapping_iterator<details::Dynamic_rank, pair_type, pair_type,
                                    Kdtree_node<pair_type>, pair_less>
      itr(fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
          static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    BOOST_CHECK_EQUAL((*itr).first, 1);
    BOOST_CHECK_EQUAL((*itr).second, 2);
    BOOST_CHECK_EQUAL(itr->first, 1);
    BOOST_CHECK_EQUAL(itr->second, 2);
  }
  {
    Pair_kdtree_fixture fix;
    details::Const_Mapping_iterator<details::Dynamic_rank, pair_type, pair_type,
                                    Kdtree_node<pair_type>, pair_less>
      itr(fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
          static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    BOOST_CHECK_EQUAL((*itr).first, 1);
    BOOST_CHECK_EQUAL((*itr).second, 2);
    BOOST_CHECK_EQUAL(itr->first, 1);
    BOOST_CHECK_EQUAL(itr->second, 2);
  }
}

BOOST_AUTO_TEST_CASE( test_Mapping_iterator_minimum )
{
  {
    dimension_type mapping_dim = 0;
    Hundred_kdtree_2D_fixture fix;
    typedef details::Mapping_iterator
      <details::Dynamic_rank, point2d, point2d,
       Kdtree_node<point2d>, bracket_less<point2d> > iter_type;
    iter_type iter;
    int count = 0;
    int min_value = (*fix.kdtree.begin())[mapping_dim];
    for(Hundred_kdtree_2D_fixture::kdtree_type::iterator i
          = fix.kdtree.begin(); i != fix.kdtree.end(); ++i)
      {
        int tmp = (*i)[mapping_dim];
        if (tmp < min_value) { min_value = tmp; }
        ++count;
      }
    BOOST_CHECK_EQUAL(count, 100);
    BOOST_REQUIRE_NO_THROW(iter = iter_type::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent)));
    BOOST_REQUIRE_EQUAL((*iter)[mapping_dim], min_value);
  }
  {
    dimension_type mapping_dim = 1;
    Hundred_kdtree_2D_fixture fix;
    typedef details::Mapping_iterator
      <details::Dynamic_rank, point2d, point2d,
       Kdtree_node<point2d>, bracket_less<point2d> > iter_type;
    iter_type iter;
    int count = 0;
    int min_value = (*fix.kdtree.begin())[mapping_dim];
    for(Hundred_kdtree_2D_fixture::kdtree_type::iterator i
          = fix.kdtree.begin(); i != fix.kdtree.end(); ++i)
      {
        int tmp = (*i)[mapping_dim];
        if (tmp < min_value) { min_value = tmp; }
        ++count;
      }
    BOOST_CHECK_EQUAL(count, 100);
    BOOST_REQUIRE_NO_THROW(iter = iter_type::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent)));
    BOOST_REQUIRE_EQUAL((*iter)[mapping_dim], min_value);
  }
  { // test at the limit: an unbalanced tree!
    dimension_type mapping_dim = 1;
    Unbalanced_right_kdtree_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<pair_type>*>
                            (fix.kdtree.end().node->parent)));
    BOOST_CHECK_EQUAL(iter->first, 1); // should be (1, 2);
    BOOST_CHECK_EQUAL(iter->second, 2); // should be (1, 2);
  }
  { // test at the limit: an unbalanced tree!
    dimension_type mapping_dim = 0;
    Unbalanced_left_kdtree_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<pair_type>*>
                            (fix.kdtree.end().node->parent)));
    BOOST_CHECK_EQUAL(iter->first, 1); // should be (1, 2);
    BOOST_CHECK_EQUAL(iter->second, 2); // should be (1, 2);
  }
}

BOOST_AUTO_TEST_CASE( test_Const_Mapping_iterator_maximum )
{
  {
    dimension_type mapping_dim = 0;
    Hundred_kdtree_2D_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Dynamic_rank, point2d, point2d,
       Kdtree_node<point2d>, bracket_less<point2d> > iter_type;
    iter_type iter;
    int count = 0;
    int max_value = (*fix.kdtree.begin())[mapping_dim];
    for(Hundred_kdtree_2D_fixture::kdtree_type::iterator i
          = fix.kdtree.begin(); i != fix.kdtree.end(); ++i)
      {
        int tmp = (*i)[mapping_dim];
        if (tmp > max_value) { max_value = tmp; }
        ++count;
      }
    BOOST_CHECK_EQUAL(count, 100);
    BOOST_REQUIRE_NO_THROW(iter = iter_type::maximum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent)));
    BOOST_REQUIRE_EQUAL((*iter)[mapping_dim], max_value);
  }
  {
    dimension_type mapping_dim = 1;
    Hundred_kdtree_2D_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Dynamic_rank, point2d, point2d,
       Kdtree_node<point2d>, bracket_less<point2d> > iter_type;
    iter_type iter;
    int count = 0;
    int max_value = (*fix.kdtree.begin())[mapping_dim];
    for(Hundred_kdtree_2D_fixture::kdtree_type::iterator i
          = fix.kdtree.begin(); i != fix.kdtree.end(); ++i)
      {
        int tmp = (*i)[mapping_dim];
        if (tmp > max_value) { max_value = tmp; }
        ++count;
      }
    BOOST_CHECK_EQUAL(count, 100);
    BOOST_REQUIRE_NO_THROW(iter = iter_type::maximum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent)));
    BOOST_REQUIRE_EQUAL((*iter)[mapping_dim], max_value);
  }
  { // test at the limit: an unbalanced tree!
    dimension_type mapping_dim = 1;
    Unbalanced_right_kdtree_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::maximum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<pair_type>*>
                            (fix.kdtree.end().node->parent)));
    BOOST_CHECK_EQUAL(iter->first, 7); // should be (1, 2);
    BOOST_CHECK_EQUAL(iter->second, 8); // should be (1, 2);
  }
  { // test at the limit: an unbalanced tree!
    dimension_type mapping_dim = 0;
    Unbalanced_left_kdtree_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::maximum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<pair_type>*>
                            (fix.kdtree.end().node->parent)));
    BOOST_CHECK_EQUAL(iter->first, 7); // should be (1, 2);
    BOOST_CHECK_EQUAL(iter->second, 8); // should be (1, 2);
  }
}

BOOST_AUTO_TEST_CASE( test_Mapping_iterator_increment )
{
  {
    dimension_type mapping_dim = 0;
    Hundred_kdtree_2D_fixture fix;
    typedef details::Mapping_iterator
      <details::Dynamic_rank, point2d, point2d,
       Kdtree_node<point2d>, bracket_less<point2d> > iter_type;
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, details::decr_dim
                            (fix.kdtree.rank(), 0),
                            static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node)));
    int count = 0;
    int tmp = (*iter)[mapping_dim];
    for (; iter != end; ++iter)
    {
      BOOST_CHECK_LE(tmp, (*iter)[mapping_dim]);
      tmp = (*iter)[mapping_dim];
      BOOST_REQUIRE_LE(++count, 100);
    }
    BOOST_CHECK_EQUAL(count, 100);
  }
  {
    dimension_type mapping_dim = 1;
    Hundred_kdtree_2D_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Dynamic_rank, point2d, point2d,
       Kdtree_node<point2d>, bracket_less<point2d> > iter_type;
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, details::decr_dim
                            (fix.kdtree.rank(), 0),
                            static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node)));
    int count = 0;
    int tmp = (*iter)[mapping_dim];
    for (; iter != end; ++iter)
    {
      BOOST_CHECK_LE(tmp, (*iter)[mapping_dim]);
      tmp = (*iter)[mapping_dim];
      BOOST_REQUIRE_LE(++count, 100);
    }
    BOOST_CHECK_EQUAL(count, 100);
  }
  { // test at the limit: an unbalanced tree
    dimension_type mapping_dim = 0;
    Unbalanced_left_kdtree_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<pair_type>*>
                            (fix.kdtree.end().node->parent)));
    BOOST_CHECK_EQUAL(iter->first, 1);
    BOOST_CHECK_EQUAL(iter->second, 2);
    ++iter;
    BOOST_CHECK_EQUAL(iter->first, 3);
    BOOST_CHECK_EQUAL(iter->second, 4);
    ++iter;
    BOOST_CHECK_EQUAL(iter->first, 5);
    BOOST_CHECK_EQUAL(iter->second, 6);
    ++iter;
    BOOST_CHECK_EQUAL(iter->first, 7);
    BOOST_CHECK_EQUAL(iter->second, 8);
    ++iter;
    BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
  }
  { // test at the limit: an unbalanced tree
    dimension_type mapping_dim = 1;
    Unbalanced_right_kdtree_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<pair_type>*>
                            (fix.kdtree.end().node->parent)));
    BOOST_CHECK_EQUAL(iter->first, 1);
    BOOST_CHECK_EQUAL(iter->second, 2);
    ++iter;
    BOOST_CHECK_EQUAL(iter->first, 3);
    BOOST_CHECK_EQUAL(iter->second, 4);
    ++iter;
    BOOST_CHECK_EQUAL(iter->first, 5);
    BOOST_CHECK_EQUAL(iter->second, 6);
    ++iter;
    BOOST_CHECK_EQUAL(iter->first, 7);
    BOOST_CHECK_EQUAL(iter->second, 8);
    ++iter;
    BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
  }
}

BOOST_AUTO_TEST_CASE( test_Mapping_iterator_pre_post_increment )
{
  {
    Pair_kdtree_fixture fix;
    details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> it
      (fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    details::Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> end
      (fix.kdtree.rank(), fix.kdtree.key_comp(), 0,
       details::decr_dim(fix.kdtree.rank(), 0),
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.end().node));
    BOOST_CHECK(++it == end);
  }
  {
    Pair_kdtree_fixture fix;
    details::Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> it
      (fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> end
      (fix.kdtree.rank(), fix.kdtree.key_comp(), 0,
       details::decr_dim(fix.kdtree.rank(), 0),
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.end().node));
    BOOST_CHECK(it++ != end);
    BOOST_CHECK(it == end);
  }
}

BOOST_AUTO_TEST_CASE( test_Const_Mapping_iterator_decrement )
{
  {
    dimension_type mapping_dim = 0;
    Hundred_kdtree_2D_fixture fix;
    typedef details::Mapping_iterator
      <details::Dynamic_rank, point2d, point2d,
       Kdtree_node<point2d>, bracket_less<point2d> > iter_type;
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, details::decr_dim
                            (fix.kdtree.rank(), 0),
                            static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node)));
    typedef std::reverse_iterator<iter_type> reverse_iter_type;
    reverse_iter_type rend(iter);
    reverse_iter_type riter(end);
    int count = 0;
    int tmp = (*riter)[mapping_dim];
    for (; riter != rend; ++riter)
    {
      BOOST_CHECK_GE(tmp, (*riter)[mapping_dim]);
      tmp = (*riter)[mapping_dim];
      BOOST_REQUIRE_LE(++count, 100);
    }
    BOOST_CHECK_EQUAL(count, 100);
  }
  {
    dimension_type mapping_dim = 1;
    Hundred_kdtree_2D_fixture fix;
    typedef details::Mapping_iterator
      <details::Dynamic_rank, point2d, point2d,
       Kdtree_node<point2d>, bracket_less<point2d> > iter_type;
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, details::decr_dim
                            (fix.kdtree.rank(), 0),
                            static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node)));
    typedef std::reverse_iterator<iter_type> reverse_iter_type;
    reverse_iter_type rend(iter);
    reverse_iter_type riter(end);
    int count = 0;
    int tmp = (*riter)[mapping_dim];
    for (; riter != rend; ++riter)
    {
      BOOST_CHECK_GE(tmp, (*riter)[mapping_dim]);
      tmp = (*riter)[mapping_dim];
      BOOST_REQUIRE_LE(++count, 100);
    }
    BOOST_CHECK_EQUAL(count, 100);
  }
  { // test at the limit: an unbalanced tree
    dimension_type mapping_dim = 0;
    Unbalanced_left_kdtree_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::maximum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<pair_type>*>
                            (fix.kdtree.end().node->parent)));
    BOOST_CHECK_EQUAL(iter->first, 7);
    BOOST_CHECK_EQUAL(iter->second, 8);
    --iter;
    BOOST_CHECK_EQUAL(iter->first, 5);
    BOOST_CHECK_EQUAL(iter->second, 6);
    --iter;
    BOOST_CHECK_EQUAL(iter->first, 3);
    BOOST_CHECK_EQUAL(iter->second, 4);
    --iter;
    BOOST_CHECK_EQUAL(iter->first, 1);
    BOOST_CHECK_EQUAL(iter->second, 2);
    --iter;
    BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
  }
  { // test at the limit: an unbalanced tree
    dimension_type mapping_dim = 1;
    Unbalanced_right_kdtree_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::maximum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<pair_type>*>
                            (fix.kdtree.end().node->parent)));
    BOOST_CHECK_EQUAL(iter->first, 7);
    BOOST_CHECK_EQUAL(iter->second, 8);
    --iter;
    BOOST_CHECK_EQUAL(iter->first, 5);
    BOOST_CHECK_EQUAL(iter->second, 6);
    --iter;
    BOOST_CHECK_EQUAL(iter->first, 3);
    BOOST_CHECK_EQUAL(iter->second, 4);
    --iter;
    BOOST_CHECK_EQUAL(iter->first, 1);
    BOOST_CHECK_EQUAL(iter->second, 2);
    --iter;
    BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
  }
}

BOOST_AUTO_TEST_CASE( test_Mapping_iterator_pre_post_decrement )
{
  {
    Pair_kdtree_fixture fix;
    details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> it
      (fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    details::Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> end
      (fix.kdtree.rank(), fix.kdtree.key_comp(), 0,
       details::decr_dim(fix.kdtree.rank(), 0),
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.end().node));
    BOOST_CHECK(--end == it);
  }
  {
    Pair_kdtree_fixture fix;
    details::Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> it
      (fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> end
      (fix.kdtree.rank(), fix.kdtree.key_comp(), 0,
       details::decr_dim(fix.kdtree.rank(), 0),
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.end().node));
    BOOST_CHECK(end-- != it);
    BOOST_CHECK(it == end);
  }
}

BOOST_AUTO_TEST_CASE( test_Mapping_iterator_lower_bound )
{
  // return the smallest element in set that is greater or equal to key
  // test with high density and oob values
  {
    dimension_type mapping_dim = 1;
    Hundred_kdtree_2D_fixture fix;
    typedef details::Mapping_iterator
      <details::Dynamic_rank, point2d, point2d,
       Kdtree_node<point2d>, bracket_less<point2d> > iter_type;
    point2d flag = { { 10, 10 } };
    point2d low_flag = { { -10, -10 } };
    point2d high_flag = { { 30, 30 } };
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::lower_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent), flag));
    iter_type low_iter;
    BOOST_REQUIRE_NO_THROW(low_iter = iter_type::lower_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent), low_flag));
    iter_type high_iter;
    BOOST_REQUIRE_NO_THROW(high_iter = iter_type::lower_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent), high_flag));
    iter_type begin;
    BOOST_REQUIRE_NO_THROW(begin = iter_type::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, details::decr_dim
                            (fix.kdtree.rank(), 0),
                            static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node)));
    if (iter != end) // chances that this is false are extremely low 1/(2^100)
      {
        BOOST_CHECK_GE((*iter)[mapping_dim], flag[mapping_dim]);
        if (iter != begin) // same as above
          {
            iter_type tmp = iter;
            BOOST_CHECK_LT((*(--tmp))[mapping_dim], (*iter)[mapping_dim]);
            BOOST_CHECK_LT((*tmp)[mapping_dim], flag[mapping_dim]);
          }
      }
    BOOST_CHECK(low_iter == begin);
    BOOST_CHECK(high_iter == end);
  }
  // test with high dispersion
  {
    dimension_type mapping_dim = 2;  // triple::z
    Twenty_kdtree_3D_fixture fix;
    typedef details::Mapping_iterator
      <details::Dynamic_rank, triple, triple,
       Kdtree_node<triple>, triple_less> iter_type;
    triple flag; flag.x = 0; flag.y = 0; flag.z = 0;
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::lower_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<triple>*>
                            (fix.kdtree.end().node->parent), flag));
    iter_type begin;
    BOOST_REQUIRE_NO_THROW(begin = iter_type::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<triple>*>
                            (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, details::decr_dim
                            (fix.kdtree.rank(), 0),
                            static_cast<Kdtree_node<triple>*>
                            (fix.kdtree.end().node)));
    if (iter != end) // chances that this is false are low 1/(2^20)
      {
        BOOST_CHECK_GE(iter->z, flag.z);
        if (iter != begin) // same as above
          {
            iter_type tmp = iter;
            BOOST_CHECK_LT((--tmp)->z, iter->z);
            BOOST_CHECK_LT(tmp->z, flag.z);
          }
      }
  }
}

BOOST_AUTO_TEST_CASE( test_Const_Mapping_iterator_upper_bound )
{
  // return the smallest element in set that is strictly greater than key
  // test with high density and oob values
  {
    typedef details::Const_Mapping_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_kdtree_2D_fixture::kdtree_type::node_type,
      bracket_less<point2d> > mapping_iterator;
    dimension_type mapping_dim = 1;
    Hundred_kdtree_2D_fixture fix;
    point2d flag = { { 10, 10 } };
    point2d low_flag = { { -10, -10 } };
    point2d high_flag = { { 30, 30 } };
    mapping_iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = mapping_iterator::upper_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent), flag));
    mapping_iterator low_iter;
    BOOST_REQUIRE_NO_THROW(low_iter = mapping_iterator::upper_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent), low_flag));
    mapping_iterator high_iter;
    BOOST_REQUIRE_NO_THROW(high_iter = mapping_iterator::upper_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent), high_flag));
    mapping_iterator begin;
    BOOST_REQUIRE_NO_THROW(begin = mapping_iterator::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent)));
    mapping_iterator end;
    BOOST_REQUIRE_NO_THROW(end = mapping_iterator
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, details::decr_dim
                            (fix.kdtree.rank(), 0),
                            static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node)));
    if (iter != end) // chances that this is false are extremely low 1/(2^100)
      {
        BOOST_CHECK_GT((*iter)[mapping_dim], flag[mapping_dim]);
        if (iter != begin) // same as above
          {
            mapping_iterator tmp = iter;
            BOOST_CHECK_LT((*(--tmp))[mapping_dim], (*iter)[mapping_dim]);
            BOOST_CHECK_LE((*tmp)[mapping_dim], flag[mapping_dim]);
          }
      }
    BOOST_CHECK(low_iter == begin);
    BOOST_CHECK(high_iter == end);
  }
  // test with high dispersion
  {
    typedef details::Const_Mapping_iterator
      <Twenty_kdtree_3D_fixture::kdtree_type::rank_type,
      Twenty_kdtree_3D_fixture::kdtree_type::key_type,
      Twenty_kdtree_3D_fixture::kdtree_type::value_type,
      Twenty_kdtree_3D_fixture::kdtree_type::node_type,
      triple_less> mapping_iterator;
    dimension_type mapping_dim = 2;  // triple::z
    Twenty_kdtree_3D_fixture fix;
    triple flag; flag.x = 0; flag.y = 0; flag.z = 0;
    mapping_iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = mapping_iterator::upper_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<triple>*>
                            (fix.kdtree.end().node->parent), flag));
    mapping_iterator begin;
    BOOST_REQUIRE_NO_THROW(begin = mapping_iterator::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<triple>*>
                            (fix.kdtree.end().node->parent)));
    mapping_iterator end;
    BOOST_REQUIRE_NO_THROW(end = mapping_iterator
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, details::decr_dim
                            (fix.kdtree.rank(), 0),
                            static_cast<Kdtree_node<triple>*>
                            (fix.kdtree.end().node)));
    if (iter != end) // chances that this is false are low 1/(2^20)
      {
        BOOST_CHECK_GT(iter->z, flag.z);
        if (iter != begin) // same as above
          {
            mapping_iterator tmp = iter;
            BOOST_CHECK_LT((--tmp)->z, iter->z);
            BOOST_CHECK_LE(tmp->z, flag.z);
          }
      }
  }
}

BOOST_AUTO_TEST_CASE( test_pointset_mapping_view )
{
  pointset<2, point2d> points;
  mapping_view<pointset<2, point2d> > set_zero(points, 0);
  BOOST_CHECK(set_zero.begin() == set_zero.end());
  // Now add some points to pointset and iterate throught these points.
  points.insert(ones);
  points.insert(ones);
  points.insert(twos);
  points.insert(zeros);
  points.insert(fives);
  points.insert(fives);
  BOOST_CHECK(set_zero.begin() != set_zero.end());
  BOOST_CHECK(set_zero.cbegin() != set_zero.cend());
  BOOST_CHECK(set_zero.rbegin() != set_zero.rend());
  BOOST_CHECK(set_zero.crbegin() != set_zero.crend());
  BOOST_CHECK(*set_zero.begin() == zeros);
  BOOST_CHECK(*set_zero.cbegin() == zeros);
  BOOST_CHECK(*--set_zero.end() == fives);
  BOOST_CHECK(*--set_zero.cend() == fives);
  BOOST_CHECK(*set_zero.rbegin() == fives);
  BOOST_CHECK(*set_zero.crbegin() == fives);
  BOOST_CHECK(*set_zero.lower_bound(ones) == ones);
  BOOST_CHECK(*set_zero.clower_bound(ones) == ones);
  BOOST_CHECK(*set_zero.upper_bound(ones) == twos);
  BOOST_CHECK(*set_zero.cupper_bound(ones) == twos);
}

BOOST_AUTO_TEST_CASE( test_pointset_const_mapping_view )
{
  pointset<2, point2d> points;
  mapping_view<const pointset<2, point2d> > set_zero(points, 0);
  BOOST_CHECK(set_zero.begin() == set_zero.end());
  // Now add some points to pointset and iterate throught these points.
  points.insert(ones);
  points.insert(ones);
  points.insert(twos);
  points.insert(zeros);
  points.insert(fives);
  points.insert(fives);
  BOOST_CHECK(set_zero.begin() != set_zero.end());
  BOOST_CHECK(set_zero.cbegin() != set_zero.cend());
  BOOST_CHECK(set_zero.rbegin() != set_zero.rend());
  BOOST_CHECK(set_zero.crbegin() != set_zero.crend());
  BOOST_CHECK(*set_zero.begin() == zeros);
  BOOST_CHECK(*set_zero.cbegin() == zeros);
  BOOST_CHECK(*--set_zero.end() == fives);
  BOOST_CHECK(*--set_zero.cend() == fives);
  BOOST_CHECK(*set_zero.rbegin() == fives);
  BOOST_CHECK(*set_zero.crbegin() == fives);
  BOOST_CHECK(*set_zero.lower_bound(ones) == ones);
  BOOST_CHECK(*set_zero.clower_bound(ones) == ones);
  BOOST_CHECK(*set_zero.upper_bound(ones) == twos);
  BOOST_CHECK(*set_zero.cupper_bound(ones) == twos);
}

BOOST_AUTO_TEST_CASE( test_Relaxed_mapping_increment )
{
  {
    // Test basic 7 node tree first.
    Seven_Relaxed_kdtree_node_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Static_rank<2>, point2d, point2d,
       Relaxed_kdtree_node<point2d>, bracket_less<point2d> > iterator_type;
    iterator_type it(details::Static_rank<2>(), bracket_less<point2d>(), 0, 0,
                     &fix.node_left_left);
    BOOST_CHECK(*it == zeros);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == ones);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == ones);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == twos);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    ++it;
    BOOST_CHECK(it.impl_.node_ == &fix.header);
  }
  {
    Grow_Relaxed_Kdtree_2D_fixture fix;
    typedef details::mapping::iterator
      <Grow_Relaxed_Kdtree_2D_fixture::kdtree_type>::type iterator_type;
    iterator_type end = details::mapping::end(fix.kdtree, 0);
    iterator_type begin = details::mapping::begin(fix.kdtree, 0);
    int old = (*begin)[0];
    int count = 0;
    for(; count != 20; ++begin, ++count)
      {
        BOOST_CHECK((*begin)[0] >= old);
        BOOST_CHECK(begin != end);
        if (begin == end) break;
        old = (*begin)[0];
      }
    BOOST_CHECK(begin == end);
    BOOST_CHECK_EQUAL(count, 20);
    end = details::mapping::end(fix.kdtree, 1);
    begin = details::mapping::begin(fix.kdtree, 1);
    old = (*begin)[1];
    for(count = 0; count != 20; ++begin, ++count)
      {
        BOOST_CHECK((*begin)[1] >= old);
        BOOST_CHECK(begin != end);
        if (begin == end) break;
        old = (*begin)[1];
      }
    BOOST_CHECK(begin == end);
    BOOST_CHECK_EQUAL(count, 20);
  }
  {
    Reduce_Relaxed_Kdtree_2D_fixture fix;
    typedef details::mapping::iterator
      <Reduce_Relaxed_Kdtree_2D_fixture::kdtree_type>::type iterator_type;
    iterator_type end = details::mapping::end(fix.kdtree, 0);
    iterator_type begin = details::mapping::begin(fix.kdtree, 0);
    int old = (*begin)[0];
    int count = 0;
    for(; count != 20; ++begin, ++count)
      {
        BOOST_CHECK((*begin)[0] >= old);
        BOOST_CHECK(begin != end);
        if (begin == end) break;
        old = (*begin)[0];
      }
    BOOST_CHECK(begin == end);
    BOOST_CHECK_EQUAL(count, 20);
    end = details::mapping::end(fix.kdtree, 1);
    begin = details::mapping::begin(fix.kdtree, 1);
    old = (*begin)[1];
    for(count = 0; count != 20; ++begin, ++count)
      {
        BOOST_CHECK((*begin)[1] >= old);
        BOOST_CHECK(begin != end);
        if (begin == end) break;
        old = (*begin)[1];
      }
    BOOST_CHECK(begin == end);
    BOOST_CHECK_EQUAL(count, 20);
  }
}

BOOST_AUTO_TEST_CASE( test_Relaxed_mapping_maxium )
{
  {
    // Test basic 7 node tree first.
    Seven_Relaxed_kdtree_node_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Static_rank<2>, point2d, point2d,
       Relaxed_kdtree_node<point2d>, bracket_less<point2d> > iterator_type;
    iterator_type it = iterator_type::maximum
      (details::Static_rank<2>(), bracket_less<point2d>(), 0, 0,
       static_cast<Node_base*>(&fix.node_root));
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == twos);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == ones);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == ones);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == zeros);
    --it;
    BOOST_REQUIRE(it.impl_.node_ == &fix.header);
  }
  // erase and check that max value found match returned
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
        std::advance(eraser,
                     static_cast<size_type>(rand()) % fix.kdtree.size());
        mapping_iterator max_0;
        BOOST_REQUIRE_NO_THROW(max_0 = mapping_iterator::maximum
                               (fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                                static_cast<Relaxed_kdtree_node<point5d>*>
                                (fix.kdtree.end().node->parent)));
        mapping_iterator max_1;
        BOOST_REQUIRE_NO_THROW(max_1 = mapping_iterator::maximum
                               (fix.kdtree.rank(), fix.kdtree.key_comp(), 1, 0,
                                static_cast<Relaxed_kdtree_node<point5d>*>
                                (fix.kdtree.end().node->parent)));
        mapping_iterator max_2;
        BOOST_REQUIRE_NO_THROW(max_2 = mapping_iterator::maximum
                               (fix.kdtree.rank(), fix.kdtree.key_comp(), 2, 0,
                                static_cast<Relaxed_kdtree_node<point5d>*>
                                (fix.kdtree.end().node->parent)));
        double found_max_0 = -11.0; // absolute lower
        Node_base* found_max_0_address = 0;
        double found_max_1 = -11.0;
        Node_base* found_max_1_address = 0;
        double found_max_2 = -11.0;
        Node_base* found_max_2_address = 0;
        for(iterator i = fix.kdtree.begin(); i != fix.kdtree.end(); ++i)
          {
            if ((*i)[0] > found_max_0)
              {
                found_max_0 = (*i)[0];
                found_max_0_address = i.node;
              }
            if ((*i)[0] == found_max_0 && found_max_0_address < i.node)
              {
                found_max_0_address = i.node;
              }
            if ((*i)[1] > found_max_1)
              {
                found_max_1 = (*i)[1];
                found_max_1_address = i.node;
              }
            if ((*i)[1] == found_max_1 && found_max_1_address < i.node)
              {
                found_max_1_address = i.node;
              }
            if ((*i)[2] > found_max_2)
              {
                found_max_2 = (*i)[2];
                found_max_2_address = i.node;
              }
            if ((*i)[2] == found_max_2 && found_max_2_address < i.node)
              {
                found_max_2_address = i.node;
              }
          }
        BOOST_CHECK_EQUAL((*max_0)[0], found_max_0);
        BOOST_CHECK_EQUAL(max_0.impl_.node_, found_max_0_address);
        BOOST_CHECK_EQUAL((*max_1)[1], found_max_1);
        BOOST_CHECK_EQUAL(max_1.impl_.node_, found_max_1_address);
        BOOST_CHECK_EQUAL((*max_2)[2], found_max_2);
        BOOST_CHECK_EQUAL(max_2.impl_.node_, found_max_2_address);
        BOOST_REQUIRE_NO_THROW(fix.kdtree.erase(eraser));
        BOOST_CHECK_EQUAL(fix.kdtree.size(), --track_size);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_Relaxed_mapping_minimum )
{
  {
    // Test basic 7 node tree first.
    Seven_Relaxed_kdtree_node_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Static_rank<2>, point2d, point2d,
       Relaxed_kdtree_node<point2d>, bracket_less<point2d> > iterator_type;
    iterator_type it = iterator_type::minimum
      (details::Static_rank<2>(), bracket_less<point2d>(), 0, 0,
       static_cast<Node_base*>(&fix.node_root));
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == zeros);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == ones);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == ones);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == twos);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    ++it;
    BOOST_CHECK(it.impl_.node_ == &fix.header);
  }
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
        std::advance(eraser,
                     static_cast<size_type>(rand()) % fix.kdtree.size());
        mapping_iterator min_0;
        BOOST_REQUIRE_NO_THROW(min_0 = mapping_iterator::minimum
                               (fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                                static_cast<Relaxed_kdtree_node<point5d>*>
                                (fix.kdtree.end().node->parent)));
        mapping_iterator min_1;
        BOOST_REQUIRE_NO_THROW(min_1 = mapping_iterator::minimum
                               (fix.kdtree.rank(), fix.kdtree.key_comp(), 1, 0,
                                static_cast<Relaxed_kdtree_node<point5d>*>
                                (fix.kdtree.end().node->parent)));
        mapping_iterator min_2;
        BOOST_REQUIRE_NO_THROW(min_2 = mapping_iterator::minimum
                               (fix.kdtree.rank(), fix.kdtree.key_comp(), 2, 0,
                                static_cast<Relaxed_kdtree_node<point5d>*>
                                (fix.kdtree.end().node->parent)));
        double found_min_0 = 11.0; // absolute upper
        Node_base* found_min_0_address = 0;
        double found_min_1 = 11.0;
        Node_base* found_min_1_address = 0;
        double found_min_2 = 11.0;
        Node_base* found_min_2_address = 0;
        for(iterator i = fix.kdtree.begin(); i != fix.kdtree.end(); ++i)
          {
            if ((*i)[0] < found_min_0)
              {
                found_min_0 = (*i)[0];
                found_min_0_address = i.node;
              }
            if ((*i)[0] == found_min_0 && found_min_0_address > i.node)
              {
                found_min_0_address = i.node;
              }
            if ((*i)[1] < found_min_1)
              {
                found_min_1 = (*i)[1];
                found_min_1_address = i.node;
              }
            if ((*i)[1] == found_min_1 && found_min_1_address > i.node)
              {
                found_min_1_address = i.node;
              }
            if ((*i)[2] < found_min_2)
              {
                found_min_2 = (*i)[2];
                found_min_2_address = i.node;
              }
            if ((*i)[2] == found_min_2 && found_min_2_address > i.node)
              {
                found_min_2_address = i.node;
              }
          }
        BOOST_CHECK_EQUAL((*min_0)[0], found_min_0);
        BOOST_CHECK_EQUAL(min_0.impl_.node_, found_min_0_address);
        BOOST_CHECK_EQUAL((*min_1)[1], found_min_1);
        BOOST_CHECK_EQUAL(min_1.impl_.node_, found_min_1_address);
        BOOST_CHECK_EQUAL((*min_2)[2], found_min_2);
        BOOST_CHECK_EQUAL(min_2.impl_.node_, found_min_2_address);
        BOOST_REQUIRE_NO_THROW(fix.kdtree.erase(eraser));
        BOOST_CHECK_EQUAL(fix.kdtree.size(), --track_size);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_Relaxed_mapping_decrement )
{
  {
    // Test basic 7 node tree first.
    Seven_Relaxed_kdtree_node_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Static_rank<2>, point2d, point2d,
       Relaxed_kdtree_node<point2d>, bracket_less<point2d> > iterator_type;
    iterator_type it(details::Static_rank<2>(), bracket_less<point2d>(), 0, 1,
                     static_cast<Relaxed_kdtree_node<point2d>*>(&fix.header));
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == twos);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == ones);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == ones);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == zeros);
    --it;
    BOOST_REQUIRE(it.impl_.node_ == &fix.header);
  }
  {
    Grow_Relaxed_Kdtree_2D_fixture fix;
    typedef details::mapping::iterator
      <Grow_Relaxed_Kdtree_2D_fixture::kdtree_type>::type iterator_type;
    iterator_type end = details::mapping::end(fix.kdtree, 0);
    iterator_type begin = details::mapping::begin(fix.kdtree, 0);
    iterator_type before_end = end; --before_end;
    int old = (*before_end)[0];
    int count = 0;
    for(; count != 20; --end, ++count)
      {
        BOOST_CHECK(old >= (*before_end)[0]);
        BOOST_CHECK(begin != end);
        if (begin == end) break;
        old = (*before_end)[0];
        if (before_end != begin) --before_end;
      }
    BOOST_CHECK(begin == end);
    BOOST_CHECK_EQUAL(count, 20);
    end = details::mapping::end(fix.kdtree, 1);
    begin = details::mapping::begin(fix.kdtree, 1);
    before_end = end; --before_end;
    old = (*before_end)[1];
    for(count = 0; count != 20; --end, ++count)
      {
        BOOST_CHECK((*before_end)[1] <= old);
        BOOST_CHECK(begin != end);
        if (begin == end) break;
        old = (*before_end)[1];
        if (before_end != begin) --before_end;
      }
    BOOST_CHECK(begin == end);
    BOOST_CHECK_EQUAL(count, 20);
  }
  {
    Reduce_Relaxed_Kdtree_2D_fixture fix;
    typedef details::mapping::iterator
      <Reduce_Relaxed_Kdtree_2D_fixture::kdtree_type>::type iterator_type;
    iterator_type end = details::mapping::end(fix.kdtree, 0);
    iterator_type begin = details::mapping::begin(fix.kdtree, 0);
    iterator_type before_end = end; --before_end;
    int old = (*before_end)[0];
    int count = 0;
    for(; count != 20; --end, ++count)
      {
        BOOST_CHECK(old >= (*before_end)[0]);
        BOOST_CHECK(begin != end);
        if (begin == end) break;
        old = (*before_end)[0];
        if (before_end != begin) --before_end;
      }
    BOOST_CHECK(begin == end);
    BOOST_CHECK_EQUAL(count, 20);
    end = details::mapping::end(fix.kdtree, 1);
    begin = details::mapping::begin(fix.kdtree, 1);
    before_end = end; --before_end;
    old = (*before_end)[1];
    for(count = 0; count != 20; --end, ++count)
      {
        BOOST_CHECK((*before_end)[1] <= old);
        BOOST_CHECK(begin != end);
        if (begin == end) break;
        old = (*before_end)[1];
        if (before_end != begin) --before_end;
      }
    BOOST_CHECK(begin == end);
    BOOST_CHECK_EQUAL(count, 20);
  }
}

BOOST_AUTO_TEST_CASE( test_Relaxed_Mapping_lower_bound )
{
  // return the smallest element in set that is greater or equal to key
  // test with high density and oob values
  {
    dimension_type mapping_dim = 1;
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef details::Mapping_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare>
        iter_type;
    point2d flag = { { 10, 10 } };
    point2d low_flag = { { -10, -10 } };
    point2d high_flag = { { 30, 30 } };
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::lower_bound
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, 0,
                            static_cast<Hundred_Relaxed_kdtree_2D_fixture
                            ::kdtree_type::node_type*>
                            (fix.kdtree.end().node->parent), flag));
    iter_type low_iter;
    BOOST_REQUIRE_NO_THROW(low_iter = iter_type::lower_bound
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, 0,
                            static_cast<Hundred_Relaxed_kdtree_2D_fixture
                            ::kdtree_type::node_type*>
                            (fix.kdtree.end().node->parent), low_flag));
    iter_type high_iter;
    BOOST_REQUIRE_NO_THROW(high_iter = iter_type::lower_bound
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, 0,
                            static_cast<Hundred_Relaxed_kdtree_2D_fixture
                            ::kdtree_type::node_type*>
                            (fix.kdtree.end().node->parent), high_flag));
    iter_type begin;
    BOOST_REQUIRE_NO_THROW(begin = iter_type::minimum
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, 0,
                            static_cast<Hundred_Relaxed_kdtree_2D_fixture
                            ::kdtree_type::node_type*>
                            (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, details::decr_dim
                            (fix.kdtree.rank(), 0),
                            static_cast<Hundred_Relaxed_kdtree_2D_fixture
                            ::kdtree_type::node_type*>
                            (fix.kdtree.end().node)));
    if (iter != end) // chances that this is false are extremely low 1/(2^100)
      {
        BOOST_CHECK_GE((*iter)[mapping_dim], flag[mapping_dim]);
        if (iter != begin) // same as above
          {
            iter_type tmp = iter;
            BOOST_CHECK_LT((*(--tmp))[mapping_dim], (*iter)[mapping_dim]);
            BOOST_CHECK_LT((*tmp)[mapping_dim], flag[mapping_dim]);
          }
      }
    BOOST_CHECK(low_iter == begin);
    BOOST_CHECK(high_iter == end);
  }
  // test with high dispersion
  {
    dimension_type mapping_dim = 2;  // triple::z
    Dispersed_Relaxed_kdtree_3D_fixture fix;
    typedef details::Mapping_iterator
      <Dispersed_Relaxed_kdtree_3D_fixture::kdtree_type::rank_type,
       Dispersed_Relaxed_kdtree_3D_fixture::kdtree_type::key_type,
       Dispersed_Relaxed_kdtree_3D_fixture::kdtree_type::value_type,
       Dispersed_Relaxed_kdtree_3D_fixture::kdtree_type::node_type,
       Dispersed_Relaxed_kdtree_3D_fixture::kdtree_type::key_compare>
      iter_type;
    triple flag; flag.x = 0; flag.y = 0; flag.z = 0;
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::lower_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0,
                            static_cast<Dispersed_Relaxed_kdtree_3D_fixture
                            ::kdtree_type::node_type*>
                            (fix.kdtree.end().node->parent), flag));
    iter_type begin;
    BOOST_REQUIRE_NO_THROW(begin = iter_type::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0,
                            static_cast<Dispersed_Relaxed_kdtree_3D_fixture
                            ::kdtree_type::node_type*>
                            (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, details::decr_dim
                            (fix.kdtree.rank(), 0),
                            static_cast<Dispersed_Relaxed_kdtree_3D_fixture
                            ::kdtree_type::node_type*>
                            (fix.kdtree.end().node)));
    if (iter != end) // chances that this is false are low 1/(2^20)
      {
        BOOST_CHECK_GE(iter->z, flag.z);
        if (iter != begin) // same as above
          {
            iter_type tmp = iter;
            BOOST_CHECK_LT((--tmp)->z, iter->z);
            BOOST_CHECK_LT(tmp->z, flag.z);
          }
      }
  }
}

BOOST_AUTO_TEST_CASE( test_Relaxed_Mapping_upper_bound )
{
  // return the smallest element in set that is strictly greater than key
  // test with high density and oob values
  {
    typedef details::Const_Mapping_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
      bracket_less<point2d> > mapping_iterator;
    dimension_type mapping_dim = 1;
    Hundred_Relaxed_kdtree_2D_fixture fix;
    point2d flag = { { 10, 10 } };
    point2d low_flag = { { -10, -10 } };
    point2d high_flag = { { 30, 30 } };
    mapping_iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = mapping_iterator::upper_bound
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Relaxed_kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent), flag));
    mapping_iterator low_iter;
    BOOST_REQUIRE_NO_THROW(low_iter = mapping_iterator::upper_bound
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Relaxed_kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent), low_flag));
    mapping_iterator high_iter;
    BOOST_REQUIRE_NO_THROW(high_iter = mapping_iterator::upper_bound
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Relaxed_kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent), high_flag));
    mapping_iterator begin;
    BOOST_REQUIRE_NO_THROW(begin = mapping_iterator::minimum
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Relaxed_kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent)));
    mapping_iterator end;
    BOOST_REQUIRE_NO_THROW(end = mapping_iterator
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, details::decr_dim
                            (fix.kdtree.rank(), 0),
                            static_cast<Relaxed_kdtree_node<point2d>*>
                            (fix.kdtree.end().node)));
    if (iter != end) // chances that this is false are extremely low 1/(2^100)
      {
        BOOST_CHECK_GT((*iter)[mapping_dim], flag[mapping_dim]);
        if (iter != begin) // same as above
          {
            mapping_iterator tmp = iter;
            BOOST_CHECK_LT((*(--tmp))[mapping_dim], (*iter)[mapping_dim]);
            BOOST_CHECK_LE((*tmp)[mapping_dim], flag[mapping_dim]);
          }
      }
    BOOST_CHECK(low_iter == begin);
    BOOST_CHECK(high_iter == end);
  }
  // test with high dispersion
  {
    typedef details::Const_Mapping_iterator
      <Dispersed_Relaxed_kdtree_3D_fixture::kdtree_type::rank_type,
      Dispersed_Relaxed_kdtree_3D_fixture::kdtree_type::key_type,
      Dispersed_Relaxed_kdtree_3D_fixture::kdtree_type::value_type,
      Dispersed_Relaxed_kdtree_3D_fixture::kdtree_type::node_type,
      triple_less> mapping_iterator;
    dimension_type mapping_dim = 2;  // triple::z
    Dispersed_Relaxed_kdtree_3D_fixture fix;
    triple flag; flag.x = 0; flag.y = 0; flag.z = 0;
    mapping_iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = mapping_iterator::upper_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Relaxed_kdtree_node<triple>*>
                            (fix.kdtree.end().node->parent), flag));
    mapping_iterator begin;
    BOOST_REQUIRE_NO_THROW(begin = mapping_iterator::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Relaxed_kdtree_node<triple>*>
                            (fix.kdtree.end().node->parent)));
    mapping_iterator end;
    BOOST_REQUIRE_NO_THROW(end = mapping_iterator
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, details::decr_dim
                            (fix.kdtree.rank(), 0),
                            static_cast<Relaxed_kdtree_node<triple>*>
                            (fix.kdtree.end().node)));
    if (iter != end) // chances that this is false are low 1/(2^20)
      {
        BOOST_CHECK_GT(iter->z, flag.z);
        if (iter != begin) // same as above
          {
            mapping_iterator tmp = iter;
            BOOST_CHECK_LT((--tmp)->z, iter->z);
            BOOST_CHECK_LE(tmp->z, flag.z);
          }
      }
  }
}

#endif // SPATIAL_TEST_MAPPING_HPP
