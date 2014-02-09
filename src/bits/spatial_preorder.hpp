// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_preorder.hpp
 *  Defines all operations needed to walk the tree in preorder fashion.
 */

#ifndef SPATIAL_PREORDER_HPP
#define SPATIAL_PREORDER_HPP

namespace spatial
{
  namespace details
  {
    template <typename NodePtr, typename Rank, typename Query>
    inline std::pair<NodePtr, dimension_type>
    preorder_minimum(NodePtr node, dimension_type dim, Rank rank,
                     const Query& query)
    {
      SPATIAL_ASSERT_CHECK(!header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(dim < rank());
      while (!stop_traversal(node, rank, query))
        {
          if (node->left != 0 && left_traversal(node, dim, rank, query))
            {
              node = node->left;
              dim = incr_dim(rank, dim);
            }
          else if (node->right != 0 && right_traversal(node, dim, rank, query))
            {
              node = node->right;
              dim = incr_dim(rank, dim);
            }
          else
            {
              NodePtr prev_node = node;
              node = node->parent;
              dim = decr_dim(rank, dim);
              while (!header(node)
                     && (prev_node == node->right
                         || node->right == 0
                         || !right_traversal(node, dim, rank, query)))
                {
                  prev_node = node;
                  node = node->parent;
                  dim = decr_dim(rank, dim);
                }
              if (!header(node))
                {
                  node = node->right;
                  dim = incr_dim(rank, dim);
                }
              else break;
            }
        }
      SPATIAL_ASSERT_CHECK(dim < rank());
      SPATIAL_ASSERT_CHECK(node != 0);
      return std::make_pair(node, dim);
    }
    /*
    template <typename NodePtr, typename Rank, typename Query>
    inline std::pair<NodePtr, dimension_type>
    preorder_best_minimum(NodePtr node, dimension_type dim, Rank rank,
                          const Query& query)
    {
      SPATIAL_ASSERT_CHECK(!header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(dim < rank());
      NodePtr end = node->parent;
      NodePtr best_node = node;
      dimension_type best_dim = dim;
      do
        {
          if (node->left != 0
              && left_traversal(best_node, node, dim, rank, query))
            {
              node = node->left;
              dim = incr_dim(rank, dim);
            }
          else if (node->right != 0
                   && right_traversal(best_node, node, dim, rank, query))
            {
              node = node->right;
              dim = incr_dim(rank, dim);
            }
          else
            {
              NodePtr prev_node = node;
              node = node->parent;
              dim = decr_dim(rank, dim);
              while (node != end
                     && (prev_node == node->right
                         || node->right == 0
                         || !right_traversal(best_node, node, dim,
                                             rank, query)))
                {
                  prev_node = node;
                  node = node->parent;
                  dim = decr_dim(rank, dim);
                }
              if (node != end)
                {
                  node = node->right;
                  dim = incr_dim(rank, dim);
                }
              else break;
            }
          if (less_traversal(best_node, node, rank, query)
              && best_node < node)
            {
              best_node = node;
              best_dim = dim;
            }
        }
      while(node != end);
      SPATIAL_ASSERT_CHECK(best_dim < rank());
      SPATIAL_ASSERT_CHECK(best_node != 0);
      return std::make_pair(best_node, best_dim);
    }
    */
    template <typename NodePtr, typename Rank, typename Query>
    inline std::pair<NodePtr, dimension_type>
    preorder_maximum(NodePtr node, dimension_type dim, Rank rank,
                     const Query& query)
    {
      SPATIAL_ASSERT_CHECK(!header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(dim < rank());
      for (;;)
        {
          if (node->right != 0 && right_traversal(node, dim, rank, query))
            {
              node = node->right;
              dim = incr_dim(rank, dim);
            }
          else if (node->left != 0 && left_traversal(node, dim, rank, query))
            {
              node = node->left;
              dim = incr_dim(rank, dim);
            }
          else break;
        }
      while (!stop_traversal(node, rank, query))
        {
          NodePtr copy_node = node;
          dimension_type copy_dim = dim;
          node = node->parent;
          dim = decr_dim(rank, dim);
          if (header(node)) break;
          if (node->right == copy_node
              && node->left != 0 && left_traversal(node, dim, rank, query))
            {
              node = node->left;
              dim = copy_dim;
              for (;;)
                {
                  if (node->right != 0
                      && right_traversal(node, dim, rank, query))
                    {
                      node = node->right;
                      dim = incr_dim(rank, dim);
                    }
                  else if (node->left != 0
                           && left_traversal(node, dim, rank, query))
                    {
                      node = node->left;
                      dim = incr_dim(rank, dim);
                    }
                  else break;
                }
            }
        }
      SPATIAL_ASSERT_CHECK(dim < rank());
      SPATIAL_ASSERT_CHECK(node != 0);
      return std::make_pair(node, dim);
    }

    template <typename NodePtr, typename Rank, typename Query>
    inline std::pair<NodePtr, dimension_type>
    preorder_increment(NodePtr node, dimension_type dim, Rank rank,
                       const Query& query)
    {
      SPATIAL_ASSERT_CHECK(!header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(dim < rank());
      do
        {
          if (node->left != 0 && left_traversal(node, dim, rank, query))
            {
              node = node->left;
              dim = incr_dim(rank, dim);
            }
          else if (node->right != 0 && right_traversal(node, dim, rank, query))
            {
              node = node->right;
              dim = incr_dim(rank, dim);
            }
          else
            {
              NodePtr prev_node = node;
              node = node->parent;
              dim = decr_dim(rank, dim);
              while (!header(node)
                     && (prev_node == node->right
                         || node->right == 0
                         || !right_traversal(node, dim, rank, query)))
                {
                  prev_node = node;
                  node = node->parent;
                  dim = decr_dim(rank, dim);
                }
              if (!header(node))
                {
                  node = node->right;
                  dim = incr_dim(rank, dim);
                }
              else break;
            }
        }
      while (!stop_traversal(node, rank, query));
      SPATIAL_ASSERT_CHECK(dim < rank());
      SPATIAL_ASSERT_CHECK(node != 0);
      return std::make_pair(node, dim);
    }

    template <typename NodePtr, typename Rank, typename Query>
    inline std::pair<NodePtr, dimension_type>
    preorder_decrement(NodePtr node, dimension_type dim, Rank rank,
                       const Query& query)
    {
      if (header(node))
        {
          SPATIAL_ASSERT_CHECK(dim = rank() - 1);
          node = node->parent;
          dim = incr_dim(rank, dim);
          return preorder_maximum(node, dim, rank, query);
        }
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(dim < rank());
      NodePtr copy_node = node;
      dimension_type copy_dim = dim;
      node = node->parent;
      dim = decr_dim(rank, dim);
      while (!header(node))
        {
          if (node->right == copy_node
              && node->left != 0
              && left_traversal(node, dim, rank, query))
            {
              node = node->left;
              dim = copy_dim;
              for (;;)
                {
                  if (node->right != 0
                      && right_traversal(node, dim, rank, query))
                    {
                      node = node->right;
                      dim = incr_dim(rank, dim);
                    }
                  else if (node->left != 0
                           && left_traversal(node, dim, rank, query))
                    {
                      node = node->left;
                      dim = incr_dim(rank, dim);
                    }
                  else break;
                }
            }
          if (stop_traversal(node, rank, query)) break;
          copy_node = node;
          copy_dim = dim;
          node = node->parent;
          dim = decr_dim(rank, dim);
        }
      SPATIAL_ASSERT_CHECK(dim < rank());
      SPATIAL_ASSERT_CHECK(node != 0);
      return std::make_pair(node, dim);
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_PREORDER_HPP
