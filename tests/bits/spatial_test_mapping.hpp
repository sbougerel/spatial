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


BOOST_AUTO_TEST_CASE_TEMPLATE
( test_mapping_equal, Tp, every_quad )
{
  Tp fix(0);
  {
    typename mapping_iterator<typename Tp::container_type>::type
      it1(fix.container, 0, 0, 0), it2(fix.container, 0, 0, 0);
    BOOST_CHECK(it1 == it2);
    BOOST_CHECK(!(it1 != it2));
    BOOST_CHECK(it2 == it1);
    BOOST_CHECK(!(it2 != it1));
  }
  {
    typename mapping_iterator<const typename Tp::container_type>::type
      it1(fix.container, 1, 0, 0), it2(fix.container, 1, 0, 0);
    BOOST_CHECK(it1 == it2);
    BOOST_CHECK(it2 == it1);
    BOOST_CHECK(!(it1 != it2));
    BOOST_CHECK(!(it2 != it1));
  }
  {
    typename mapping_iterator<typename Tp::container_type>::type
      it1(fix.container, 1, 0, 0);
    typename mapping_iterator<const typename Tp::container_type>::type
      it2(fix.container, 1, 0, 0);
    BOOST_CHECK(it1 == it2);
    BOOST_CHECK(it2 == it1);
    BOOST_CHECK(!(it1 != it2));
    BOOST_CHECK(!(it2 != it1));
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_mapping_dereference, Tp, quad_maps )
{
  Tp fix(1, same());
  typename mapping_iterator<typename Tp::container_type>::type
    it = mapping_begin(fix.container, 0);
  (*it).second = "some string";
  BOOST_CHECK((*it).first == quad(1, 1, 1, 1));
  it->second = "some other string";
  BOOST_CHECK(it->first == quad(1, 1, 1, 1));
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_mapping_minimum, Tp, int2_sets )
{
  {
    Tp fix(100, random());
    // Prove that you can find the max value with any number of nodes
    for (int num_elems = 100; num_elems > 1; --num_elems)
      {
        int count = 0;
        int min_value_0 = (*fix.record.begin())[0];
        int min_value_1 = (*fix.record.begin())[1];
        for(typename Tp::record_type::iterator
              i = fix.record.begin(); i != fix.record.end(); ++i)
          {
            int tmp = (*i)[0];
            if (tmp < min_value_0) { min_value_0 = tmp; }
            tmp = (*i)[1];
            if (tmp < min_value_1) { min_value_1 = tmp; }
            ++count;
          }
        BOOST_CHECK_EQUAL(count, num_elems);
        typename mapping_iterator<typename Tp::container_type>::type iter;
        dimension_type mapping_dim = 0;
        iter = mapping_begin(fix.container, mapping_dim);
        BOOST_REQUIRE_EQUAL((*iter)[mapping_dim], min_value_0);
        mapping_dim = 1;
        iter = mapping_begin(fix.container, mapping_dim);
        BOOST_REQUIRE_EQUAL((*iter)[mapping_dim], min_value_1);
        fix.container.erase(iter);
      }
  }
  { // test at the limit: a tree with 1 element
    Tp fix(1, same());
    typename mapping_iterator<typename Tp::container_type>::type iter;
    iter = mapping_begin(fix.container, 0);
    BOOST_CHECK_EQUAL((*iter)[0], 1); // should be (1, 1);
    BOOST_CHECK_EQUAL((*iter)[1], 1);
    iter = mapping_begin(fix.container, 1);
    BOOST_CHECK_EQUAL((*iter)[0], 1); // should be (1, 1);
    BOOST_CHECK_EQUAL((*iter)[1], 1);
  }
  { // test at the limit: an unbalanced tree (i.e. insertions in order)!
    Tp fix(100, decrease());
    typename mapping_iterator<typename Tp::container_type>::type iter;
    dimension_type mapping_dim = 0;
    iter = mapping_begin(fix.container, mapping_dim);
    BOOST_CHECK_EQUAL((*iter)[0], 1); // should be (1, 1);
    BOOST_CHECK_EQUAL((*iter)[1], 1);
  }
  { // test at the limit: an unbalanced tree (i.e insertions in order)!
    Tp fix(100, increase());
    typename mapping_iterator<typename Tp::container_type>::type iter;
    dimension_type mapping_dim = 1;
    iter = mapping_begin(fix.container, mapping_dim);
    BOOST_CHECK_EQUAL((*iter)[0], 0); // should be (0, 0);
    BOOST_CHECK_EQUAL((*iter)[1], 0);
  }
}

/*
BOOST_AUTO_TEST_CASE_TEMPLATE
( test_mapping_maximum, Tp, int2_sets )
{
  {
    Tp fix(100, random());
    // Prove that you can find the max value with any number of nodes
    for (int num_elems = 100; num_elems > 1; --num_elems)
      {
        int count = 0;
        int min_value_0 = (*fix.record.begin())[0];
        int min_value_1 = (*fix.record.begin())[1];
        for(typename Tp::record_type::iterator
              i = fix.record.begin(); i != fix.record.end(); ++i)
          {
            int tmp = (*i)[0];
            if (tmp < min_value_0) { min_value_0 = tmp; }
            tmp = (*i)[1];
            if (tmp < min_value_1) { min_value_1 = tmp; }
            ++count;
          }
        BOOST_CHECK_EQUAL(count, num_elems);
        typename mapping_iterator<typename Tp::container_type>::type iter;
        iter = mapping_end(fix.container, 0);
        --iter; // When at the end, this call the 'maximum' function
        BOOST_REQUIRE_EQUAL((*iter)[0], min_value_0);
        iter = mapping_end(fix.container, 1);
        --iter;
        BOOST_REQUIRE_EQUAL((*iter)[1], min_value_1);
        fix.container.erase(iter);
      }
  }
  { // test at the limit: an unbalanced tree!
    Tp fix(100, decrease());
    typename mapping_iterator<typename Tp::container_type>::type iter;
    iter = mapping_end(fix.container, 0);
    --iter;
    BOOST_CHECK_EQUAL((*iter)[0], 100); // should be (100, 100);
    BOOST_CHECK_EQUAL((*iter)[1], 100);
  }
  { // test at the limit: an unbalanced tree!
    Tp fix(100, increase());
    typename mapping_iterator<typename Tp::container_type>::type iter;
    iter = mapping_end(fix.container, 1);
    --iter;
    BOOST_CHECK_EQUAL((*iter)[0], 99); // should be (99, 99);
    BOOST_CHECK_EQUAL((*iter)[1], 99);
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_mapping_increment, Tp, double6_sets )
{
  { // test the invarient of the increment
    Tp fix(100, random());
    for (dimension_type mapping_dim = 0; mapping_dim < 6;
         ++mapping_dim)
      {
        typename mapping_iterator<typename Tp::container_type>::type
          iter = mapping_begin(fix.container, mapping_dim),
          end = mapping_end(fix.container, mapping_dim);
        int count = 0;
        double tmp = (*iter)[mapping_dim];
        for (; iter != end; ++iter)
          {
            BOOST_CHECK_LE(tmp, (*iter)[mapping_dim]);
            tmp = (*iter)[mapping_dim];
            BOOST_REQUIRE_LE(++count, 100);
          }
        BOOST_CHECK_EQUAL(count, 100);
      }
  }
  { // test at the limit: a tree with 1 element
    Tp fix(1, same());
    for (dimension_type mapping_dim = 0; mapping_dim < 6;
         ++mapping_dim)
      {
        typename mapping_iterator<typename Tp::container_type>::type
          pre = mapping_begin(fix.container, mapping_dim),
          post = mapping_begin(fix.container, mapping_dim),
          end = mapping_end(fix.container, mapping_dim);
        BOOST_CHECK(pre != end);
        BOOST_CHECK(post++ != end);
        BOOST_CHECK(++pre == end);
        BOOST_CHECK(post == end);
      }
  }
  { // test at the limit: an unbalanced tree
    Tp fix(100, increase());
    for (dimension_type mapping_dim = 0; mapping_dim < 6;
         ++mapping_dim)
      {
        typename mapping_iterator<typename Tp::container_type>::type
          iter = mapping_begin(fix.container, mapping_dim),
          end = mapping_end(fix.container, mapping_dim);
        int count = 0;
        double tmp = (*iter)[mapping_dim];
        for (; iter != end; ++iter)
          {
            BOOST_CHECK_LE(tmp, (*iter)[mapping_dim]);
            tmp = (*iter)[mapping_dim];
            BOOST_REQUIRE_LE(++count, 100);
          }
        BOOST_CHECK_EQUAL(count, 100);
      }
  }
  { // test at the limit: an unbalanced tree
    Tp fix(100, decrease());
    for (dimension_type mapping_dim = 0; mapping_dim < 6;
         ++mapping_dim)
      {
        typename mapping_iterator<typename Tp::container_type>::type
          iter = mapping_begin(fix.container, mapping_dim),
          end = mapping_end(fix.container, mapping_dim);
        int count = 0;
        double tmp = (*iter)[mapping_dim];
        for (; iter != end; ++iter)
          {
            BOOST_CHECK_LE(tmp, (*iter)[mapping_dim]);
            tmp = (*iter)[mapping_dim];
            BOOST_REQUIRE_LE(++count, 100);
          }
        BOOST_CHECK_EQUAL(count, 100);
      }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE
( test_mapping_decrement, Tp, double6_maps )
{
  { // test the invarient of the increment
    Tp fix(100, random());
    for (dimension_type mapping_dim = 0; mapping_dim < 6;
         ++mapping_dim)
      {
        typename mapping_iterator<typename Tp::container_type>::type
          iter = mapping_begin(fix.container, mapping_dim),
          end = mapping_end(fix.container, mapping_dim);
        int count = 0;
        double tmp = (*iter)[mapping_dim];
        for (; iter != end; ++iter)
          {
            BOOST_CHECK_LE(tmp, (*iter)[mapping_dim]);
            tmp = (*iter)[mapping_dim];
            BOOST_REQUIRE_LE(++count, 100);
          }
        BOOST_CHECK_EQUAL(count, 100);
      }
  }
  {
    dimension_type mapping_dim = 0;
    Hundred_kdtree_2D_fixture fix;
    typedef details::Mapping_iterator
      <details::Dynamic_rank, int2, int2,
       Kdtree_node<int2>, bracket_less<int2> > iter_type;
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<int2>*>
                            (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, details::decr_dim
                            (fix.kdtree.rank(), 0),
                            static_cast<Kdtree_node<int2>*>
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
      <details::Dynamic_rank, int2, int2,
       Kdtree_node<int2>, bracket_less<int2> > iter_type;
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<int2>*>
                            (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, details::decr_dim
                            (fix.kdtree.rank(), 0),
                            static_cast<Kdtree_node<int2>*>
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

BOOST_AUTO_TEST_CASE( test_Mapping_iterator_lower_bound )
{
  // return the smallest element in set that is greater or equal to key
  // test with high density and oob values
  {
    dimension_type mapping_dim = 1;
    Hundred_kdtree_2D_fixture fix;
    typedef details::Mapping_iterator
      <details::Dynamic_rank, int2, int2,
       Kdtree_node<int2>, bracket_less<int2> > iter_type;
    int2 flag = { { 10, 10 } };
    int2 low_flag = { { -10, -10 } };
    int2 high_flag = { { 30, 30 } };
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::lower_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<int2>*>
                            (fix.kdtree.end().node->parent), flag));
    iter_type low_iter;
    BOOST_REQUIRE_NO_THROW(low_iter = iter_type::lower_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<int2>*>
                            (fix.kdtree.end().node->parent), low_flag));
    iter_type high_iter;
    BOOST_REQUIRE_NO_THROW(high_iter = iter_type::lower_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<int2>*>
                            (fix.kdtree.end().node->parent), high_flag));
    iter_type begin;
    BOOST_REQUIRE_NO_THROW(begin = iter_type::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<int2>*>
                            (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, details::decr_dim
                            (fix.kdtree.rank(), 0),
                            static_cast<Kdtree_node<int2>*>
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
      bracket_less<int2> > mapping_iterator;
    dimension_type mapping_dim = 1;
    Hundred_kdtree_2D_fixture fix;
    int2 flag = { { 10, 10 } };
    int2 low_flag = { { -10, -10 } };
    int2 high_flag = { { 30, 30 } };
    mapping_iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = mapping_iterator::upper_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<int2>*>
                            (fix.kdtree.end().node->parent), flag));
    mapping_iterator low_iter;
    BOOST_REQUIRE_NO_THROW(low_iter = mapping_iterator::upper_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<int2>*>
                            (fix.kdtree.end().node->parent), low_flag));
    mapping_iterator high_iter;
    BOOST_REQUIRE_NO_THROW(high_iter = mapping_iterator::upper_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<int2>*>
                            (fix.kdtree.end().node->parent), high_flag));
    mapping_iterator begin;
    BOOST_REQUIRE_NO_THROW(begin = mapping_iterator::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<int2>*>
                            (fix.kdtree.end().node->parent)));
    mapping_iterator end;
    BOOST_REQUIRE_NO_THROW(end = mapping_iterator
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, details::decr_dim
                            (fix.kdtree.rank(), 0),
                            static_cast<Kdtree_node<int2>*>
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

BOOST_AUTO_TEST_CASE( test_Relaxed_mapping_maxium )
{
  {
    // Test basic 7 node tree first.
    Seven_Relaxed_kdtree_node_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Static_rank<2>, int2, int2,
       Relaxed_kdtree_node<int2>, bracket_less<int2> > iterator_type;
    iterator_type it = iterator_type::maximum
      (details::Static_rank<2>(), bracket_less<int2>(), 0, 0,
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
      <details::Static_rank<2>, int2, int2,
       Relaxed_kdtree_node<int2>, bracket_less<int2> > iterator_type;
    iterator_type it = iterator_type::minimum
      (details::Static_rank<2>(), bracket_less<int2>(), 0, 0,
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
    int2 flag = { { 10, 10 } };
    int2 low_flag = { { -10, -10 } };
    int2 high_flag = { { 30, 30 } };
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
      bracket_less<int2> > mapping_iterator;
    dimension_type mapping_dim = 1;
    Hundred_Relaxed_kdtree_2D_fixture fix;
    int2 flag = { { 10, 10 } };
    int2 low_flag = { { -10, -10 } };
    int2 high_flag = { { 30, 30 } };
    mapping_iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = mapping_iterator::upper_bound
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Relaxed_kdtree_node<int2>*>
                            (fix.kdtree.end().node->parent), flag));
    mapping_iterator low_iter;
    BOOST_REQUIRE_NO_THROW(low_iter = mapping_iterator::upper_bound
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Relaxed_kdtree_node<int2>*>
                            (fix.kdtree.end().node->parent), low_flag));
    mapping_iterator high_iter;
    BOOST_REQUIRE_NO_THROW(high_iter = mapping_iterator::upper_bound
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Relaxed_kdtree_node<int2>*>
                            (fix.kdtree.end().node->parent), high_flag));
    mapping_iterator begin;
    BOOST_REQUIRE_NO_THROW(begin = mapping_iterator::minimum
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Relaxed_kdtree_node<int2>*>
                            (fix.kdtree.end().node->parent)));
    mapping_iterator end;
    BOOST_REQUIRE_NO_THROW(end = mapping_iterator
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, details::decr_dim
                            (fix.kdtree.rank(), 0),
                            static_cast<Relaxed_kdtree_node<int2>*>
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
*/
#endif // SPATIAL_TEST_MAPPING_HPP
