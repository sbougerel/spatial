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
  pointset_fix<int2> fix(0);
  BOOST_CHECK_NO_THROW
    (except::check_dimension(fix.container.dimension(), 0));
  BOOST_CHECK_THROW
    (except::check_dimension(fix.container.dimension(), 2),
     invalid_dimension);
}

BOOST_AUTO_TEST_CASE( text_except_check_node )
{
  {
    details::Node<details::Kdtree_link<int, int> >* ptr = 0;
    BOOST_CHECK_THROW(except::check_node(ptr),
                      invalid_node);
  }
  {
    int2_node_fixture fix;
    BOOST_CHECK_NO_THROW(except::check_node(&fix.node_root));
    BOOST_CHECK_THROW(except::check_node(&fix.header),
                      invalid_node);
  }
}

BOOST_AUTO_TEST_CASE( text_except_check_iterator )
{
  {
    details::Node_iterator<details::Kdtree_link<int2, int2> > i(0);
    BOOST_CHECK_THROW(except::check_node_iterator(i.node),
                      invalid_iterator);
  }
  {
    int2_node_fixture fix;
    details::Node_iterator<details::Kdtree_link<int2, int2> >
      i(&fix.node_root);
    details::Node_iterator<details::Kdtree_link<int2, int2> >
      j(&fix.header);
    BOOST_CHECK_NO_THROW(except::check_node_iterator(i.node));
    BOOST_CHECK_THROW(except::check_node_iterator(j.node),
                      invalid_iterator);
    BOOST_CHECK_NO_THROW(except::check_iterator(i, i));
    BOOST_CHECK_THROW(except::check_iterator(i, j),
                      invalid_iterator);
  }
}

BOOST_AUTO_TEST_CASE( text_except_check_empty_tree )
{
  pointset_fix<int2> fix(0);
  BOOST_CHECK_THROW(except::check_empty_container(fix.container),
                    invalid_empty_container);
}

BOOST_AUTO_TEST_CASE( text_except_check_range )
{
  pointset_fix<int2> fix(0);
  BOOST_CHECK_NO_THROW(except::check_open_bounds
                       (fix.container, zeros, ones));
  BOOST_CHECK_NO_THROW(except::check_bounds
                       (fix.container, zeros, ones));
  BOOST_CHECK_NO_THROW(except::check_closed_bounds
                       (fix.container, zeros, zeros));
  BOOST_CHECK_THROW(except::check_open_bounds
                    (fix.container, zeros, zeros), invalid_bounds);
  BOOST_CHECK_THROW(except::check_bounds
                    (fix.container, zeros, zeros), invalid_bounds);
  BOOST_CHECK_THROW(except::check_closed_bounds
                    (fix.container, ones, zeros), invalid_bounds);
  // For boxes, point2d are used as 1D boxes, or intervals.
  int2 lh_box(0, 1);
  int2 hl_box(1, 0);
  BOOST_CHECK_NO_THROW
    (except::check_box(fix.container, lh_box, llhh_layout_tag()));
  BOOST_CHECK_THROW
    (except::check_box(fix.container, hl_box, llhh_layout_tag()),
     invalid_box);
  BOOST_CHECK_NO_THROW
    (except::check_box(fix.container, lh_box, lhlh_layout_tag()));
  BOOST_CHECK_THROW
    (except::check_box(fix.container, hl_box, lhlh_layout_tag()),
     invalid_box);
  BOOST_CHECK_NO_THROW
    (except::check_box(fix.container, hl_box, hhll_layout_tag()));
  BOOST_CHECK_THROW
    (except::check_box(fix.container, lh_box, hhll_layout_tag()),
     invalid_box);
  BOOST_CHECK_NO_THROW
    (except::check_box(fix.container, hl_box, hlhl_layout_tag()));
  BOOST_CHECK_THROW
    (except::check_box(fix.container, lh_box, hlhl_layout_tag()),
     invalid_box);
}

#endif // SPATIAL_TEST_EXCEPTIONS_HPP
