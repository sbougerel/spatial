// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_equal.hpp
 *  Contains the definition of the equal iterators. These iterators
 *  walk through all items in the container that are equal to a key given in
 *  parameter of the iterator.
 */

#ifndef SPATIAL_EQUAL_HPP
#define SPATIAL_EQUAL_HPP

#include "spatial_import_tuple.hpp"
#include "spatial_assert.hpp"

namespace spatial
{
  namespace details
  {
    template <typename NodePtr, typename Rank, typename KeyCompare,
              typename Key>
    inline std::pair<NodePtr, dimension_type>
    first_equal(NodePtr node, dimension_type depth, const Rank rank,
                const KeyCompare& key_comp, const Key& key)
    {
      // Write in pre-order fashion
      NodePtr end = node->parent;
      dimension_type end_depth = depth - 1;
      for (;;)
        {
          dimension_type dim = depth % rank();
          // Test coordinates of node's key, retain results for dim
          bool walk_left = !key_comp(dim, const_key(node), key);
          bool walk_right = !key_comp(dim, key, const_key(node));
          if (walk_left && walk_right)
            {
              dimension_type test = 0;
              for (; test < dim && !(key_comp(test, key, const_key(node))
                                     || key_comp(test, const_key(node), key));
                   ++test);
              if (test == dim)
                {
                  test = dim + 1;
                  for (; test < rank()
                         && !(key_comp(test, key, const_key(node))
                              || key_comp(test, const_key(node), key));
                       ++test);
                  if (test == rank())
                    { return std::make_pair(node, dim); }
                }
            }
          // Walk the tree to find an equal target
          if (walk_right && node->right != 0)
            {
              if (walk_left && node->left != 0)
                {
                  // Go recursively in this case only, left first
                  NodePtr other;
                  dimension_type other_depth;
                  import::tie(other, other_depth)
                    = first_equal(node->left, depth + 1,
                                  rank, key_comp, key);
                  if (other != node)
                    { return std::make_pair(other, other_depth); }
                }
              node = node->right; ++depth;
            }
          else if (walk_left && node->left != 0)
            { node = node->left; ++depth; }
          else { return std::make_pair(end, end_depth); }
        }
    }

    template <typename NodePtr, typename Rank, typename KeyCompare,
              typename Key>
    inline std::pair<NodePtr, dimension_type>
    last_equal(NodePtr node, dimension_type depth, const Rank rank,
               const KeyCompare& key_comp, const Key& key)
    {
      SPATIAL_ASSERT_CHECK(!header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      for (;;)
        {
          if (!key_comp(depth % rank(), key, const_key(node))
              && node->right != 0)
            { node = node->right; ++depth; }
          else if (!key_comp(depth % rank(), const_key(node), key)
                   && node->left != 0)
            { node = node->left; ++depth; }
          else break;
        }
      for (;;)
        {
          dimension_type test = 0;
          for(; test < rank() && !(key_comp(test, key, const_key(node))
                                   || key_comp(test, const_key(node), key));
              ++test);
          if (test == rank())
            { return std::make_pair(node, depth); }
          NodePtr prev_node = node;
          node = node->parent; --depth;
          if (header(node))
            { return std::make_pair(node, depth); }
          if (node->right == prev_node
              && !key_comp(depth % rank(), const_key(node), key)
              && node->left != 0)
            {
              node = node->left; ++depth;
              for (;;)
                {
                  if (!key_comp(depth % rank(), key, const_key(node))
                      && node->right != 0)
                    { node = node->right; ++depth; }
                  else if (!key_comp(depth % rank(), const_key(node), key)
                           && node->left != 0)
                    { node = node->left; ++depth; }
                  else break;
                }
            }
        }
    }

    template <typename NodePtr, typename Rank, typename KeyCompare,
              typename Key>
    inline std::pair<NodePtr, dimension_type>
    increment_equal(NodePtr node, dimension_type depth, const Rank rank,
                    const KeyCompare& key_comp, const Key& key)
    {
      SPATIAL_ASSERT_CHECK(!header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      for (;;)
        {
          if (!key_comp(depth % rank(), const_key(node), key)
              && node->left != 0)
            { node = node->left; ++depth; }
          else if (!key_comp(depth % rank(), key, const_key(node))
                   && node->right != 0)
            { node = node->right; ++depth; }
          else
            {
              NodePtr prev_node = node;
              node = node->parent; --depth;
              while (!header(node)
                     && (prev_node == node->right
                         || key_comp(depth % rank(), key, const_key(node))
                         || node->right == 0))
                {
                  prev_node = node;
                  node = node->parent; --depth;
                }
              if (!header(node))
                { node = node->right; ++depth; }
              else { return std::make_pair(node, depth); }
            }
          dimension_type test = 0;
          for(; test < rank() && !(key_comp(test, key, const_key(node))
                                   || key_comp(test, const_key(node), key));
              ++test);
          if (test == rank())
            { return std::make_pair(node, depth); }
        }
    }

    template <typename NodePtr, typename Rank, typename KeyCompare,
              typename Key>
    inline std::pair<NodePtr, dimension_type>
    decrement_equal(NodePtr node, dimension_type depth, const Rank rank,
                    const KeyCompare& key_comp, const Key& key)
    {
      if (header(node))
        { return last_equal(node->parent, 0, rank, key_comp, key); }
      SPATIAL_ASSERT_CHECK(node != 0);
      NodePtr prev_node = node;
      node = node->parent; --depth;
      while (!header(node))
        {
          if (node->right == prev_node
              && !key_comp(depth % rank(), const_key(node), key)
              && node->left != 0)
            {
              node = node->left; ++depth;
              for (;;)
                {
                  if (!key_comp(depth % rank(), key, const_key(node))
                      && node->right != 0)
                    { node = node->right; ++depth; }
                  else if (!key_comp(depth % rank(), const_key(node), key)
                           && node->left != 0)
                    { node = node->left; ++depth; }
                  else break;
                }
            }
          dimension_type test = 0;
          for(; test < rank() && !(key_comp(test, key, const_key(node))
                                   || key_comp(test, const_key(node), key));
              ++test);
          if (test == rank()) break;
          prev_node = node;
          node = node->parent; --depth;
        }
      return std::make_pair(node, depth);
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_EQUAL_HPP
