// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)


/**
 *  @file   spatial_test_output.hpp
 *  @brief
 *
 */
#include <iostream>

#ifndef SPATIAL_TEST_OUTPUT_HPP
#define SPATIAL_TEST_OUTPUT_HPP

inline std::ostream&
operator << (std::ostream& o, const int2& x)
{
  o << "{<int2:" << &x << "> items:["
    << x[0] << ", " << x[1] << "]}";
  return o;
}

inline std::ostream&
operator << (std::ostream& o, const quad& x)
{
  o << "{<quad:" << &x << "> items:[" << x.x << ", "
    << x.y << ", " << x.z << ", " << x.w << "]}";
  return o;
}

inline std::ostream&
operator << (std::ostream& o, const double6& x)
{
  o << "{<double6:" << &x << "> items:["
    << x[0] << ", " << x[1] << ", " << x[2] << ", "
    << x[3] << ", " << x[4] << ", " << x[5] << "]}";
  return o;
}

template <typename Key, typename Value>
inline std::ostream&
operator << (std::ostream& o, const details::Node
             <details::Kdtree_link<Key, Value> >& node)
{
  o << "{<node:" << &node << "> parent:" << node.parent
    << " left:" << node.left << " right:" << node.right
    << " key:" << details::key(&node) << "}";
  return o;
}

template <typename Key, typename Value>
inline std::ostream&
operator << (std::ostream& o, const details::Node
             <details::Relaxed_kdtree_link<Key, Value> >& node)
{
  o << "{<node:" << &node << "> parent:" << node.parent
    << " left:" << node.left << " right:" << node.right
    << " weight:" << details::link(&node)->weight
    << " key:" << details::key(&node) << "}";
  return o;
}

template <typename Rank, typename Key, typename Value, typename Compare,
          typename Alloc>
inline std::ostream&
operator << (std::ostream& o, const
             details::Kdtree<Rank, Key, Value, Compare, Alloc>& t)
{
  typedef details::Kdtree<Rank, Key, Value, Compare, Alloc>
    container_type;
  typedef typename container_type::mode_type mode_type;
  typedef typename mode_type::const_node_ptr node_ptr;
  node_ptr header = t.end().node;
  o << "{<relaxed_kdtree:" << &t << ">" << std::endl
    << " header:{<node:" << header << "> parent:" << header->parent
    << " left:" << header->left << " right:" << header->right
    << " }" << std::endl
    << " size:" << t.size()
    << " items:[" << std::endl;
  for(details::Preorder_node_iterator<mode_type>
        it(t.end().node->parent), end(t.end().node); it != end; ++it)
    {
      node_ptr node = it.node;
      o << "  (" << std::flush;
      while(!details::header(node->parent))
        {
          node_ptr parent = node->parent;
          if (parent->left == node) o << "l";
          else o << "r";
          node = parent;
        }
      o << ")" << *it.node << "," << std::endl;
    }
  o << "  ]}" << std::endl;
  return o;
}

template <typename Rank, typename Key, typename Value, typename Compare,
          typename Balance, typename Alloc>
inline std::ostream&
operator << (std::ostream& o, const details::
             Relaxed_kdtree<Rank, Key, Value, Compare, Balance, Alloc>& t)
{
  typedef details::Relaxed_kdtree<Rank, Key, Value, Compare, Balance, Alloc>
    container_type;
  typedef typename container_type::mode_type mode_type;
  typedef typename mode_type::const_node_ptr node_ptr;
  node_ptr header = t.end().node;
  o << "{<relaxed_kdtree:" << &t << ">" << std::endl
    << " header:{<node:" << header << "> parent:" << header->parent
    << " left:" << header->left << " right:" << header->right
    << " }" << std::endl
    << " size:" << t.size()
    << " items:[" << std::endl;
  for(details::Preorder_node_iterator<mode_type>
        it(t.end().node->parent), end(t.end().node); it != end; ++it)
    {
      node_ptr node = it.node;
      o << "  (" << std::flush;
      while(!details::header(node->parent))
        {
          node_ptr parent = node->parent;
          if (parent->left == node) o << "l";
          else o << "r";
          node = parent;
        }
      o << ")" << *it.node << "," << std::endl;
    }
  o << "  ]}" << std::endl;
  return o;
}

#endif // SPATIAL_TEST_OUTPUT_HPP
