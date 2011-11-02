// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_test_exceptions.hpp
 *  @brief  All tests for the elements defined in spatial_exceptions.hpp are
 *  located in this file.
 *  @see spatial_exceptions.hpp
 */

#ifndef SPATIAL_TEST_EXCEPTIONS_HPP
#define SPATIAL_TEST_EXCEPTIONS_HPP


BOOST_AUTO_TEST_CASE( text_except_check_dimension )
{
  Empty_2D_fixture fix;
  BOOST_CHECK_NO_THROW(except::check_dimension_argument(fix.kdtree.dimension(), 0));
  BOOST_CHECK_THROW(except::check_dimension_argument(fix.kdtree.dimension(), 2),
                    invalid_dimension_argument);
}

BOOST_AUTO_TEST_CASE( text_except_check_node )
{
  {
    Node_base::Base_ptr ptr = 0;
    BOOST_CHECK_THROW(except::check_node_argument(ptr),
                      invalid_node_argument);
  }
  {
    Five_Node_basic_fixture fix;
    BOOST_CHECK_NO_THROW(except::check_node_argument(&fix.node_root));
    BOOST_CHECK_THROW(except::check_node_argument(&fix.header),
                      invalid_node_argument);
  }
}

BOOST_AUTO_TEST_CASE( text_except_check_iterator )
{
  {
    details::Node_iterator<point2d, Kdtree_node<point2d> > i(0);
    BOOST_CHECK_THROW(except::check_node_iterator_argument(i.node),
                      invalid_iterator_argument);
  }
  {
    Five_Kdtree_node_fixture fix;
    details::Node_iterator<point2d, Kdtree_node<point2d> >
      i(&fix.node_root);
    details::Node_iterator<point2d, Kdtree_node<point2d> >
      j(static_cast<Kdtree_node<point2d>*>
        (static_cast<Node_base*>(&fix.header)));
    BOOST_CHECK_NO_THROW(except::check_node_iterator_argument(i.node));
    BOOST_CHECK_THROW(except::check_node_iterator_argument(j.node),
                      invalid_iterator_argument);
    BOOST_CHECK_NO_THROW(except::check_iterator_argument(i, i));
    BOOST_CHECK_THROW(except::check_iterator_argument(i, j),
                      invalid_iterator_argument);
  }
}

BOOST_AUTO_TEST_CASE( text_except_check_empty_tree )
{
  Empty_2D_fixture fix;
  BOOST_CHECK_THROW(except::check_empty_container_argument(fix.kdtree),
                    invalid_empty_container_argument);
}

BOOST_AUTO_TEST_CASE( text_except_check_range )
{
  Empty_2D_fixture fix;
  BOOST_CHECK_NO_THROW(except::check_open_range_bounds(fix.kdtree, zeros, ones));
  BOOST_CHECK_NO_THROW(except::check_range_bounds(fix.kdtree, zeros, ones));
  BOOST_CHECK_NO_THROW(except::check_closed_range_bounds(fix.kdtree, zeros, zeros));
  BOOST_CHECK_THROW(except::check_open_range_bounds(fix.kdtree, zeros, zeros),
                    invalid_range_bounds);
  BOOST_CHECK_THROW(except::check_range_bounds(fix.kdtree, zeros, zeros),
                    invalid_range_bounds);
  BOOST_CHECK_THROW(except::check_closed_range_bounds(fix.kdtree, ones, zeros),
                    invalid_range_bounds);
  // For boxes, point2d are used as 1D boxes, or intervals.
  point2d lh_box = { { 0, 1 } };
  point2d hl_box = { { 1, 0 } };
  BOOST_CHECK_NO_THROW
    (except::check_box_argument(fix.kdtree, lh_box, llhh_layout_tag()));
  BOOST_CHECK_THROW
    (except::check_box_argument(fix.kdtree, hl_box, llhh_layout_tag()),
     invalid_box_argument);
  BOOST_CHECK_NO_THROW
    (except::check_box_argument(fix.kdtree, lh_box, lhlh_layout_tag()));
  BOOST_CHECK_THROW
    (except::check_box_argument(fix.kdtree, hl_box, lhlh_layout_tag()),
     invalid_box_argument);
  BOOST_CHECK_NO_THROW
    (except::check_box_argument(fix.kdtree, hl_box, hhll_layout_tag()));
  BOOST_CHECK_THROW
    (except::check_box_argument(fix.kdtree, lh_box, hhll_layout_tag()),
     invalid_box_argument);
  BOOST_CHECK_NO_THROW
    (except::check_box_argument(fix.kdtree, hl_box, hlhl_layout_tag()));
  BOOST_CHECK_THROW
    (except::check_box_argument(fix.kdtree, lh_box, hlhl_layout_tag()),
     invalid_box_argument);
}

#endif // SPATIAL_TEST_EXCEPTIONS_HPP
