// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_test_range.hpp
 *  @brief  All tests for the elements defined in spatial_range.hpp are
 *  located in this file.
 *  @see spatial_range.hpp
 */

#ifndef SPATIAL_TEST_RANGE_HPP
#define SPATIAL_TEST_RANGE_HPP

BOOST_AUTO_TEST_CASE( test_range_iterator_default_ctor )
{
  typedef details::Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    range_iterator_true_type;
  typedef details::Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    range_iterator_false_type;
  typedef details::Const_Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    const_range_iterator_true_type;
  typedef details::Const_Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    const_range_iterator_false_type;
  range_iterator_true_type i;
  const_range_iterator_true_type ci;
  range_iterator_false_type j;
  const_range_iterator_false_type cj;
}

BOOST_AUTO_TEST_CASE( test_range_iterator_value_ctor_equal )
{
  typedef details::Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    range_iterator_true_type;
  typedef details::Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    range_iterator_false_type;
  typedef details::Const_Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    const_range_iterator_true_type;
  typedef details::Const_Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    const_range_iterator_false_type;
  range_bounds<pair_type, pair_less> bounds;
  range_iterator_true_type i(details::Dynamic_rank(2), bounds, 1, 0);
  range_iterator_false_type j(details::Dynamic_rank(2), bounds, 1, 0);
  const_range_iterator_true_type ci
    (details::Dynamic_rank(2), bounds, 1, 0);
  const_range_iterator_false_type cj
    (details::Dynamic_rank(2), bounds, 1, 0);
  BOOST_CHECK_EQUAL(i.rank()(), 2);
  BOOST_CHECK(i.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(i.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(j.rank()(), 2);
  BOOST_CHECK(j.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(j.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(ci.rank()(), 2);
  BOOST_CHECK(ci.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(ci.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(cj.rank()(), 2);
  BOOST_CHECK(cj.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(cj.impl_.node_dim_(), 1);
}

BOOST_AUTO_TEST_CASE( test_range_iterator_copy_ctor )
{
  typedef details::Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    range_iterator_true_type;
  typedef details::Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    range_iterator_false_type;
  typedef details::Const_Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    const_range_iterator_true_type;
  typedef details::Const_Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    const_range_iterator_false_type;
  range_bounds<pair_type, pair_less> bounds;
  range_iterator_true_type k(details::Dynamic_rank(2), bounds, 1, 0);
  range_iterator_false_type l(details::Dynamic_rank(2), bounds, 1, 0);
  const_range_iterator_true_type ck
    (details::Dynamic_rank(2), bounds, 1, 0);
  const_range_iterator_false_type cl
    (details::Dynamic_rank(2), bounds, 1, 0);
  range_iterator_true_type copy_k(k);
  range_iterator_false_type copy_l(l);
  const_range_iterator_true_type copy_ck(ck);
  const_range_iterator_false_type copy_cl(cl);
  const_range_iterator_true_type const_copy_k(k);
  const_range_iterator_false_type const_copy_l(l);
  BOOST_CHECK_EQUAL(copy_k.rank()(), 2);
  BOOST_CHECK(copy_k.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(copy_k.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(copy_l.rank()(), 2);
  BOOST_CHECK(copy_l.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(copy_l.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(copy_ck.rank()(), 2);
  BOOST_CHECK(copy_ck.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(copy_ck.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(copy_cl.rank()(), 2);
  BOOST_CHECK(copy_cl.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(copy_cl.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(const_copy_k.rank()(), 2);
  BOOST_CHECK(const_copy_k.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(const_copy_k.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(const_copy_l.rank()(), 2);
  BOOST_CHECK(const_copy_l.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(const_copy_l.impl_.node_dim_(), 1);
}

BOOST_AUTO_TEST_CASE( test_range_iterator_assignment )
{
  typedef details::Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    range_iterator_true_type;
  typedef details::Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    range_iterator_false_type;
  typedef details::Const_Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    const_range_iterator_true_type;
  typedef details::Const_Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    const_range_iterator_false_type;
  range_bounds<pair_type, pair_less> bounds;
  range_iterator_true_type k(details::Dynamic_rank(2), bounds, 1, 0);
  range_iterator_false_type l(details::Dynamic_rank(2), bounds, 1, 0);
  const_range_iterator_true_type ck
    (details::Dynamic_rank(2), bounds, 1, 0);
  const_range_iterator_false_type cl
    (details::Dynamic_rank(2), bounds, 1, 0);
  range_iterator_true_type assign_k = k;
  range_iterator_false_type assign_l = l;
  const_range_iterator_true_type assign_ck = ck;
  const_range_iterator_false_type assign_cl = cl;
  const_range_iterator_true_type const_assign_k = k;
  const_range_iterator_false_type const_assign_l = l;
  BOOST_CHECK_EQUAL(assign_k.rank()(), 2);
  BOOST_CHECK(assign_k.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(assign_k.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(assign_l.rank()(), 2);
  BOOST_CHECK(assign_l.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(assign_l.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(assign_ck.rank()(), 2);
  BOOST_CHECK(assign_ck.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(assign_ck.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(assign_cl.rank()(), 2);
  BOOST_CHECK(assign_cl.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(assign_cl.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(const_assign_k.rank()(), 2);
  BOOST_CHECK(const_assign_k.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(const_assign_k.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(const_assign_l.rank()(), 2);
  BOOST_CHECK(const_assign_l.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(const_assign_l.impl_.node_dim_(), 1);
}

BOOST_AUTO_TEST_CASE( test_range_iterator_equal )
{
  typedef details::Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    range_iterator_true_type;
  typedef details::Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    range_iterator_false_type;
  typedef details::Const_Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    const_range_iterator_true_type;
  typedef details::Const_Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    const_range_iterator_false_type;
  range_bounds<pair_type, pair_less> bounds;
  range_iterator_true_type k(details::Dynamic_rank(2), bounds, 1, 0);
  range_iterator_false_type l(details::Dynamic_rank(2), bounds, 1, 0);
  const_range_iterator_true_type ck
    (details::Dynamic_rank(2), bounds, 1, 0);
  const_range_iterator_false_type cl
    (details::Dynamic_rank(2), bounds, 1, 0);
  BOOST_CHECK(k == k);
  BOOST_CHECK(k == l);
  BOOST_CHECK(k == ck);
  BOOST_CHECK(k == cl);
  BOOST_CHECK(l == cl);
  BOOST_CHECK(l == ck);
  BOOST_CHECK(cl == ck);
}

BOOST_AUTO_TEST_CASE( test_range_iterator_deference )
{
  typedef details::Range_iterator
    <details::Dynamic_rank, pair_type, pair_type, Kdtree_node<pair_type>,
     range_bounds<pair_type, pair_less> >
    range_iterator_true_type;
  Kdtree_node<pair_type> node;
  node.parent = 0;
  node.right = 0;
  node.left = 0;
  node.value = pair_type(1, 2);
  range_bounds<pair_type, pair_less> bounds;
  range_iterator_true_type i(details::Dynamic_rank(2), bounds, 0, &node);
  BOOST_CHECK_EQUAL((*i).first, 1);
  BOOST_CHECK_EQUAL((*i).second, 2);
  BOOST_CHECK_EQUAL(i->first, 1);
  BOOST_CHECK_EQUAL(i->second, 2);
}

BOOST_AUTO_TEST_CASE( test_range_iterator_minimum )
{
  {
    Hundred_kdtree_2D_fixture fix;
    typedef closed_range_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), whole_tree_box, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.begin().node);
  }
  {
    Hundred_kdtree_2D_fixture fix;
    typedef equal_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // reverse in-order iteration until we hit a different key
    Hundred_kdtree_2D_fixture::kdtree_type::iterator
      max = --fix.kdtree.end(), tmp = max, begin = fix.kdtree.begin();
    point2d value = *max;
    for (; tmp != begin; --tmp)
      { if (*tmp == value) { max = tmp; } }
    bounds_type equal(fix.kdtree.key_comp(), value);
    // In this case, the minimum of the interval must be equal to max.
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), equal, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == max.node);
  }
}

BOOST_AUTO_TEST_CASE( test_range_iterator_minimum_empty )
{
  {
    Hundred_kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // These bounds are too narrow to contain anything
    point2d mid = { {10, 10} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), mid, mid);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
  {
    // These bounds do not intersect with the tree bounding box
    Hundred_kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // These bounds do not intersect with the tree bounding box
    point2d low = { {20, 20} };
    point2d high = { {30, 30} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
  {
    // These bounds do not intersect with the tree bounding box
    Hundred_kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> range_iterator;
    // These bounds do not intersect with the tree bounding box
    point2d low = { {-10, -10} };
    point2d high = { {0, 0} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
  {
    Hundred_kdtree_2D_fixture fix;
    typedef closed_range_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // reverse in-order iteration until we hit a different key
    for (int shrink = 0; shrink != 20; ++shrink)
      {
        point2d low = { { shrink, shrink } };
        point2d high = { { 20, 20} };
        bounds_type shrinking_bounds(fix.kdtree.key_comp(), low, high);
        // In this case, the minimum of the interval must be equal to min.
        range_iterator it = range_iterator::minimum
          (fix.kdtree.rank(), shrinking_bounds,
           0, fix.kdtree.end().node->parent);
        Hundred_kdtree_2D_fixture::kdtree_type::iterator
          min = fix.kdtree.begin(), end = fix.kdtree.end();
        for (; min != end && !spatial::details::match_all
               (fix.kdtree.rank(), *min, shrinking_bounds); ++min);
        BOOST_CHECK(it.impl_.node_ == min.node);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_range_iterator_maximum )
{
  {
    Hundred_kdtree_2D_fixture fix;
    typedef closed_range_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), whole_tree_box, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == (--fix.kdtree.end()).node);
  }
  {
    Hundred_kdtree_2D_fixture fix;
    typedef equal_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // reverse in-order iteration until we hit a different key
    Hundred_kdtree_2D_fixture::kdtree_type::iterator
      min = fix.kdtree.begin(), tmp = min, end = fix.kdtree.end();
    point2d value = *min;
    for (; tmp != end; ++tmp) { if (*tmp == value) { min = tmp; } }
    bounds_type equal(fix.kdtree.key_comp(), value);
    // In this case, the minimum of the interval must be equal to max.
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), equal, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == min.node);
  }
  {
    Hundred_kdtree_2D_fixture fix;
    typedef closed_range_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // reverse in-order iteration until we hit a different key
    for (int shrink = 20; shrink != 0; --shrink)
      {
        point2d low = { { 0, 0} };
        point2d high = { { shrink, shrink } };
        bounds_type shrinking_bounds(fix.kdtree.key_comp(), low, high);
        // In this case, the minimum of the interval must be equal to max.
        range_iterator it = range_iterator::maximum
          (fix.kdtree.rank(), shrinking_bounds,
           0, fix.kdtree.end().node->parent);
        Hundred_kdtree_2D_fixture::kdtree_type::iterator
          max = fix.kdtree.end(), begin = fix.kdtree.begin();
        for (; max != begin && !spatial::details::match_all
               (fix.kdtree.rank(), *(--max), shrinking_bounds););
        if (spatial::details::match_all
            (fix.kdtree.rank(), *max, shrinking_bounds))
          { BOOST_CHECK(it.impl_.node_ == max.node); }
        else
          { BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node); }
      }
  }
}

BOOST_AUTO_TEST_CASE( test_range_iterator_maximum_empty )
{
  {
    Hundred_kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // These bounds are too narrow to contain anything
    point2d mid = { {10, 10} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), mid, mid);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
  {
    // These bounds do not intersect with the tree bounding box
    Hundred_kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // These bounds do not intersect with the tree bounding box
    point2d low = { {20, 20} };
    point2d high = { {30, 30} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
  {
    // These bounds do not intersect with the tree bounding box
    Hundred_kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // These bounds do not intersect with the tree bounding box
    point2d low = { {-10, -10} };
    point2d high = { {0, 0} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
}

BOOST_AUTO_TEST_CASE( test_range_iterator_pre_increment )
{
  {
    typedef closed_range_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    typedef Hundred_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    range_iterator range_min = range_iterator::minimum
      (fix.kdtree.rank(), whole_tree_box, 0,
       fix.kdtree.end().node->parent);
    const_iterator min; min.node = range_min.impl_.node_;
    const_iterator end(fix.kdtree.end());
    for(; min != end; ++min)
      {
        if (spatial::details::match_all
            (fix.kdtree.rank(), *min, whole_tree_box))
          {
            BOOST_CHECK(range_min.impl_.node_ == min.node);
            ++range_min;
          }
      }
    BOOST_CHECK(range_min.impl_.node_ == min.node); // at end!
  }
  {
    typedef open_range_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> range_iterator;
    typedef Hundred_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_kdtree_2D_fixture fix;
    // bounds encloses only a small part of the tree..
    point2d low = { {7, 8} };
    point2d high = { {11, 12} };
    bounds_type small_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    range_iterator range_min = range_iterator::minimum
      (fix.kdtree.rank(), small_box, 0,
       fix.kdtree.end().node->parent);
    const_iterator min; min.node = range_min.impl_.node_;
    const_iterator end(fix.kdtree.end());
    for(; min != end; ++min)
      {
        if (spatial::details::match_all
            (fix.kdtree.rank(), *min, small_box))
          {
            BOOST_CHECK(range_min.impl_.node_ == min.node);
            ++range_min;
          }
      }
    BOOST_CHECK(range_min.impl_.node_ == min.node); // at end!
  }
}

BOOST_AUTO_TEST_CASE( test_range_iterator_pre_decrement )
{
  {
    typedef closed_range_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> range_iterator;
    typedef Hundred_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    range_iterator range_max = range_iterator
      (fix.kdtree.rank(), whole_tree_box, 1,
       static_cast<Hundred_kdtree_2D_fixture::kdtree_type::node_type *>
       (fix.kdtree.end().node));
    const_iterator max; max.node = range_max.impl_.node_;
    const_iterator begin(fix.kdtree.begin());
    for(; max != begin; --max)
      {
        if (spatial::details::match_all
            (fix.kdtree.rank(), *max, whole_tree_box))
          {
            --range_max;
            BOOST_CHECK(range_max.impl_.node_ == max.node);
          }
      }
  }
  {
    typedef open_range_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    typedef Hundred_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_kdtree_2D_fixture fix;
    // bounds encloses only a small part of the tree..
    point2d low = { {7, 7} };
    point2d high = { {12, 12} };
    bounds_type small_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    range_iterator range_max = range_iterator
      (fix.kdtree.rank(), small_box, 1,
       static_cast<Hundred_kdtree_2D_fixture::kdtree_type::node_type *>
       (fix.kdtree.end().node));
    const_iterator max; max.node = range_max.impl_.node_;
    const_iterator begin(fix.kdtree.begin());
    for(; max != begin; --max)
      {
        if (spatial::details::match_all
            (fix.kdtree.rank(), *max, small_box))
          {
            --range_max;
            BOOST_CHECK(range_max.impl_.node_ == max.node);
          }
      }
  }
}

BOOST_AUTO_TEST_CASE( test_range_iterator_post_decrement )
{
    typedef closed_range_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    typedef Hundred_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    range_iterator test = range_iterator
      (fix.kdtree.rank(), whole_tree_box, 1,
       static_cast<Hundred_kdtree_2D_fixture::kdtree_type::node_type *>
       (fix.kdtree.end().node));
    range_iterator before = test;
    range_iterator after = test--;
    BOOST_CHECK(before == after);
    --after;
    BOOST_CHECK(after == test);
}

BOOST_AUTO_TEST_CASE( test_range_iterator_post_increment )
{
    typedef closed_range_bounds
      <Hundred_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> range_iterator;
    typedef Hundred_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    range_iterator test = range_iterator::minimum
      (fix.kdtree.rank(), whole_tree_box,
       0, fix.kdtree.end().node->parent);
    range_iterator before = test;
    range_iterator after = test++;
    BOOST_CHECK(before == after);
    ++after;
    BOOST_CHECK(after == test);
}

BOOST_AUTO_TEST_CASE( test_pointset_range )
{
  pointset<2, point2d> points;
  // Now add some points to pointset and iterate throught these points.
  points.insert(zeros);
  points.insert(ones);
  points.insert(ones);
  points.insert(twos);
  points.insert(twos);
  points.insert(fours);
  points.insert(fives);
  range_predicate_view<pointset<2, point2d>,
    range_bounds<typename spatial::container_traits<pointset<2, point2d> >::key_type,
                 typename spatial::container_traits<pointset<2, point2d> >::key_compare> >
    range1(points, make_range_bounds(points, ones, threes));
  BOOST_CHECK(range1.begin() != range1.end());
  BOOST_CHECK(range1.cbegin() != range1.cend());
  range_predicate_view<pointset<2, point2d>,
    range_bounds<typename spatial::container_traits<pointset<2, point2d> >::key_type,
                 typename spatial::container_traits<pointset<2, point2d> >::key_compare> >
    range2(points, make_range_bounds(points, threes, fours));
  BOOST_CHECK(range2.begin() == range2.end());
  BOOST_CHECK(range2.cbegin() == range2.cend());
  range_predicate_view<const pointset<2, point2d>,
    range_bounds<typename spatial::container_traits<pointset<2, point2d> >::key_type,
                 typename spatial::container_traits<pointset<2, point2d> >::key_compare> >
    range3(points, make_range_bounds(points, threes, fours));
  BOOST_CHECK(range3.begin() == range3.end());
  BOOST_CHECK(range3.cbegin() == range3.cend());
  range_predicate_view<const pointset<2, point2d>,
    range_bounds<typename spatial::container_traits<pointset<2, point2d> >::key_type,
                 typename spatial::container_traits<pointset<2, point2d> >::key_compare> >
    range4(points, make_range_bounds(points, ones, threes));
  BOOST_CHECK(range4.begin() != range4.end());
  BOOST_CHECK(range4.cbegin() != range4.cend());
}

BOOST_AUTO_TEST_CASE( test_pointset_other_range_view )
{
  pointset<2, point2d> points;
  range_view<pointset<2, point2d> > view1(points, zeros, ones);
  range_view<const pointset<2, point2d> > view2(points, zeros, ones);
  closed_range_view<pointset<2, point2d> > view3(points, zeros, ones);
  closed_range_view<const pointset<2, point2d> > view4(points, zeros, ones);
  open_range_view<pointset<2, point2d> > view5(points, zeros, ones);
  open_range_view<const pointset<2, point2d> > view6(points, zeros, ones);
}

BOOST_AUTO_TEST_CASE( test_relaxed_range_iterator_minimum )
{
  {
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef closed_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> range_iterator;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), whole_tree_box, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.begin().node);
  }
  {
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef equal_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> range_iterator;
    // reverse in-order iteration until we hit a different key
    Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::iterator
      max = --fix.kdtree.end(), tmp = max, begin = fix.kdtree.begin();
    point2d value = *max;
    for (; tmp != begin; --tmp)
      { if (*tmp == value) { max = tmp; } }
    bounds_type equal(fix.kdtree.key_comp(), value);
    // In this case, the minimum of the interval must be equal to max.
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), equal, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == max.node);
  }
}

BOOST_AUTO_TEST_CASE( test_relaxed_range_iterator_minimum_empty )
{
  {
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> range_iterator;
    // These bounds are too narrow to contain anything
    point2d mid = { {10, 10} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), mid, mid);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
  {
    // These bounds do not intersect with the tree bounding box
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // These bounds do not intersect with the tree bounding box
    point2d low = { {20, 20} };
    point2d high = { {30, 30} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
  {
    // These bounds do not intersect with the tree bounding box
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // These bounds do not intersect with the tree bounding box
    point2d low = { {-10, -10} };
    point2d high = { {0, 0} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
  {
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef closed_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // reverse in-order iteration until we hit a different key
    for (int shrink = 0; shrink != 20; ++shrink)
      {
        point2d low = { { shrink, shrink } };
        point2d high = { { 20, 20} };
        bounds_type shrinking_bounds(fix.kdtree.key_comp(), low, high);
        // In this case, the minimum of the interval must be equal to min.
        range_iterator it = range_iterator::minimum
          (fix.kdtree.rank(), shrinking_bounds,
           0, fix.kdtree.end().node->parent);
        Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::iterator
          min = fix.kdtree.begin(), end = fix.kdtree.end();
        for (; min != end && !spatial::details::match_all
               (fix.kdtree.rank(), *min, shrinking_bounds); ++min);
        BOOST_CHECK(it.impl_.node_ == min.node);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_relaxed_range_iterator_maximum )
{
  {
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef closed_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), whole_tree_box, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == (--fix.kdtree.end()).node);
  }
  {
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef equal_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // reverse in-order iteration until we hit a different key
    Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::iterator
      min = fix.kdtree.begin(), tmp = min, end = fix.kdtree.end();
    point2d value = *min;
    for (; tmp != end; ++tmp) { if (*tmp == value) { min = tmp; } }
    bounds_type equal(fix.kdtree.key_comp(), value);
    // In this case, the minimum of the interval must be equal to max.
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), equal, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == min.node);
  }
  {
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef closed_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // reverse in-order iteration until we hit a different key
    for (int shrink = 20; shrink != 0; --shrink)
      {
        point2d low = { { 0, 0} };
        point2d high = { { shrink, shrink } };
        bounds_type shrinking_bounds(fix.kdtree.key_comp(), low, high);
        // In this case, the minimum of the interval must be equal to max.
        range_iterator it = range_iterator::maximum
          (fix.kdtree.rank(), shrinking_bounds,
           0, fix.kdtree.end().node->parent);
        Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::iterator
          max = fix.kdtree.end(), begin = fix.kdtree.begin();
        for (; max != begin && !spatial::details::match_all
               (fix.kdtree.rank(), *(--max), shrinking_bounds););
        if (spatial::details::match_all
            (fix.kdtree.rank(), *max, shrinking_bounds))
          { BOOST_CHECK(it.impl_.node_ == max.node); }
        else
          { BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node); }
      }
  }
}

BOOST_AUTO_TEST_CASE( test_relaxed_range_iterator_maximum_empty )
{
  {
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // These bounds are too narrow to contain anything
    point2d mid = { {10, 10} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), mid, mid);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
  {
    // These bounds do not intersect with the tree bounding box
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // These bounds do not intersect with the tree bounding box
    point2d low = { {20, 20} };
    point2d high = { {30, 30} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
  {
    // These bounds do not intersect with the tree bounding box
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // These bounds do not intersect with the tree bounding box
    point2d low = { {-10, -10} };
    point2d high = { {0, 0} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
}

BOOST_AUTO_TEST_CASE( test_relaxed_range_iterator_pre_increment )
{
  {
    typedef closed_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    typedef Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Relaxed_kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    range_iterator range_min = range_iterator::minimum
      (fix.kdtree.rank(), whole_tree_box, 0,
       fix.kdtree.end().node->parent);
    const_iterator min; min.node = range_min.impl_.node_;
    const_iterator end(fix.kdtree.end());
    for(; min != end; ++min)
      {
        if (spatial::details::match_all
            (fix.kdtree.rank(), *min, whole_tree_box))
          {
            BOOST_CHECK(range_min.impl_.node_ == min.node);
            ++range_min;
          }
      }
    BOOST_CHECK(range_min.impl_.node_ == min.node); // at end!
  }
  {
    typedef open_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> range_iterator;
    typedef Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Relaxed_kdtree_2D_fixture fix;
    // bounds encloses only a small part of the tree..
    point2d low = { {7, 8} };
    point2d high = { {11, 12} };
    bounds_type small_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    range_iterator range_min = range_iterator::minimum
      (fix.kdtree.rank(), small_box, 0,
       fix.kdtree.end().node->parent);
    const_iterator min; min.node = range_min.impl_.node_;
    const_iterator end(fix.kdtree.end());
    for(; min != end; ++min)
      {
        if (spatial::details::match_all
            (fix.kdtree.rank(), *min, small_box))
          {
            BOOST_CHECK(range_min.impl_.node_ == min.node);
            ++range_min;
          }
      }
    BOOST_CHECK(range_min.impl_.node_ == min.node); // at end!
  }
}

BOOST_AUTO_TEST_CASE( test_relaxed_range_iterator_pre_decrement )
{
  {
    typedef closed_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> range_iterator;
    typedef Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Relaxed_kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    range_iterator range_max = range_iterator
      (fix.kdtree.rank(), whole_tree_box, 1,
       static_cast<Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type *>
       (fix.kdtree.end().node));
    const_iterator max; max.node = range_max.impl_.node_;
    const_iterator begin(fix.kdtree.begin());
    for(; max != begin; --max)
      {
        if (spatial::details::match_all
            (fix.kdtree.rank(), *max, whole_tree_box))
          {
            --range_max;
            BOOST_CHECK(range_max.impl_.node_ == max.node);
          }
      }
  }
  {
    typedef open_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> range_iterator;
    typedef Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Relaxed_kdtree_2D_fixture fix;
    // bounds encloses only a small part of the tree..
    point2d low = { {7, 7} };
    point2d high = { {12, 12} };
    bounds_type small_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    range_iterator range_max = range_iterator
      (fix.kdtree.rank(), small_box, 1,
       static_cast<Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type *>
       (fix.kdtree.end().node));
    const_iterator max; max.node = range_max.impl_.node_;
    const_iterator begin(fix.kdtree.begin());
    for(; max != begin; --max)
      {
        if (spatial::details::match_all
            (fix.kdtree.rank(), *max, small_box))
          {
            --range_max;
            BOOST_CHECK(range_max.impl_.node_ == max.node);
          }
      }
  }
}

BOOST_AUTO_TEST_CASE( test_relaxed_range_iterator_post_decrement )
{
    typedef closed_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    typedef Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Relaxed_kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    range_iterator test = range_iterator
      (fix.kdtree.rank(), whole_tree_box, 1,
       static_cast<Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type *>
       (fix.kdtree.end().node));
    range_iterator before = test;
    range_iterator after = test--;
    BOOST_CHECK(before == after);
    --after;
    BOOST_CHECK(after == test);
}

BOOST_AUTO_TEST_CASE( test_relaxed_range_iterator_post_increment )
{
    typedef closed_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> range_iterator;
    typedef Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Relaxed_kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    range_iterator test = range_iterator::minimum
      (fix.kdtree.rank(), whole_tree_box,
       0, fix.kdtree.end().node->parent);
    range_iterator before = test;
    range_iterator after = test++;
    BOOST_CHECK(before == after);
    ++after;
    BOOST_CHECK(after == test);
}

#endif // SPATIAL_TEST_RANGE_HPP
