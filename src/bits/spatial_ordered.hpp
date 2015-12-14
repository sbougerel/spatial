// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_ordered.hpp
 *  Contains the definition of \ordered_iterator.
 */

#ifndef SPATIAL_ORDERED_HPP
#define SPATIAL_ORDERED_HPP

#include "../traits.hpp"
#include "spatial_bidirectional.hpp"
#include "spatial_rank.hpp"
#include "spatial_except.hpp"
#include "spatial_import_tuple.hpp"

namespace spatial
{
  /**
   *  All elements returned by this iterator are ordered from the smallest to
   *  the largest value of their key's coordinate along a single dimension.
   *
   *  These iterators walk through all items in the container in order from the
   *  lowest to the highest value along a particular dimension. The key
   *  comparator of the container is used for comparision.
   */
  template<typename Ct>
  class ordered_iterator
    : public details::Bidirectional_iterator
  <typename container_traits<Ct>::mode_type,
   typename container_traits<Ct>::rank_type>
  {
  private:
    typedef details::Bidirectional_iterator
    <typename container_traits<Ct>::mode_type,
     typename container_traits<Ct>::rank_type> Base;

  public:
    using Base::node;
    using Base::node_dim;
    using Base::rank;

    //! Alias for the key_compare type used by the iterator.
    typedef typename container_traits<Ct>::key_compare key_compare;

    //! Uninitialized iterator.
    ordered_iterator() { }

    /**
     *  The standard way to build this iterator: specify a ordered dimension, an
     *  iterator on a container, and that container.
     *
     *  \param container   The container to iterate.
     *  \param iter        Use the value of \c iter as the start point for the
     *                     iteration.
     */
    ordered_iterator(Ct& container,
                     typename container_traits<Ct>::iterator iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        _cmp(container.key_comp())
    { }

    /**
     *  When the information of the dimension for the current node being
     *  pointed to by the iterator is known, this function saves some CPU
     *  cycle, by comparison to the other.
     *
     *  In order to iterate through nodes in the \kdtree built in the
     *  container, the algorithm must know at each node which dimension is
     *  used to partition the space. Some algorithms will provide this
     *  dimension, such as the function \ref spatial::details::modulo().
     *
     *  \attention Specifying the incorrect dimension value for the node will
     *  result in unknown behavior. It is recommended that you do not use this
     *  constructor if you are not sure about this dimension, and use the
     *  other constructors instead.
     *
     *  \param container The container to iterate.
     *  \param dim The dimension of the node pointed to by iterator.
     *  \param ptr Use the value of node as the start point for the
     *             iteration.
     */
    ordered_iterator(Ct& container, dimension_type dim,
                     typename container_traits<Ct>::mode_type::node_ptr ptr)
      : Base(container.rank(), ptr, dim), _cmp(container.key_comp())
    { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    ordered_iterator<Ct>& operator++()
    {
      spatial::import::tie(node, node_dim)
        = increment_ordered(node, node_dim, rank(), key_comp());
      return *this;
    }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    ordered_iterator<Ct> operator++(int)
    {
      ordered_iterator<Ct> x(*this);
      spatial::import::tie(node, node_dim)
        = increment_ordered(node, node_dim, rank(), key_comp());
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    ordered_iterator<Ct>& operator--()
    {
      spatial::import::tie(node, node_dim)
        = decrement_ordered(node, node_dim, rank(), key_comp());
      return *this;
    }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    ordered_iterator<Ct> operator--(int)
    {
      ordered_iterator<Ct> x(*this);
      spatial::import::tie(node, node_dim)
        = decrement_ordered(node, node_dim, rank(), key_comp());
      return x;
    }

    //! Return the key_comparator used by the iterator
    key_compare
    key_comp() const { return _cmp; }

  private:
    //! The related data for the iterator.
    key_compare _cmp;
  };

  /**
   *  All elements returned by this iterator are ordered from the smallest to
   *  the largest value of their key's coordinate along a single dimension,
   *  called the ordered dimension.
   *
   *  Object deferenced by this iterator are always constant.
   */
  template<typename Ct>
  class ordered_iterator<const Ct>
    : public details::Const_bidirectional_iterator
  <typename container_traits<Ct>::mode_type,
   typename container_traits<Ct>::rank_type>
  {
  private:
    typedef details::Const_bidirectional_iterator
    <typename container_traits<Ct>::mode_type,
     typename container_traits<Ct>::rank_type> Base;

  public:
    using Base::node;
    using Base::node_dim;
    using Base::rank;

    //! Alias for the key_compare type used by the iterator.
    typedef typename container_traits<Ct>::key_compare key_compare;

    //! Build an uninitialized iterator.
    ordered_iterator() { }

    /**
     *  The standard way to build this iterator: specify a ordered dimension,
     *  an iterator on a container, and that container.
     *
     *  \param container The container to iterate.
     *  \param iter Use the iterator \c iter as the start point for the
     *  iteration.
     */
    ordered_iterator(const Ct& container,
                     typename container_traits<Ct>::const_iterator iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        _cmp(container.key_comp())
    { }

    /**
     *  When the information of the dimension for the current node being
     *  pointed to by the iterator is known, this function saves some CPU
     *  cycle, by comparison to the other.
     *
     *  \param container The container to iterate.
     *  \param dim The dimension of the node pointed to by iterator.
     *  \param ptr Use \c node as the start point for the iteration.
     *
     *  In order to iterate through nodes in the \kdtree built in the
     *  container, the algorithm must know at each node which dimension is
     *  used to partition the space. Some algorithms will provide this
     *  dimension, such as the function \ref spatial::details::modulo().
     *
     *  \attention Specifying the incorrect dimension value for the node will
     *  result in unknown behavior. It is recommended that you do not use this
     *  constructor if you are not sure about this dimension, and use the
     *  other constructors instead.
     */
    ordered_iterator
    (const Ct& container, dimension_type dim,
     typename container_traits<Ct>::mode_type::const_node_ptr ptr)
      : Base(container.rank(), ptr, dim), _cmp(container.key_comp())
    { }

    //! Convertion of mutable iterator into a constant iterator is permitted.
    ordered_iterator(const ordered_iterator<Ct>& iter)
      : Base(iter.rank(), iter.node, iter.node_dim), _cmp(iter.key_comp())
    { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    ordered_iterator<const Ct>& operator++()
    {
      spatial::import::tie(node, node_dim)
        = increment_ordered(node, node_dim, rank(), key_comp());
      return *this;
    }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    ordered_iterator<const Ct> operator++(int)
    {
      ordered_iterator<const Ct> x(*this);
      spatial::import::tie(node, node_dim)
        = increment_ordered(node, node_dim, rank(), key_comp());
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    ordered_iterator<const Ct>& operator--()
    {
      spatial::import::tie(node, node_dim)
        = decrement_ordered(node, node_dim, rank(), key_comp());
      return *this;
    }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    ordered_iterator<const Ct> operator--(int)
    {
      ordered_iterator<const Ct> x(*this);
      spatial::import::tie(node, node_dim)
        = decrement_ordered(node, node_dim, rank(), key_comp());
      return x;
    }

    //! Return the key_comparator used by the iterator
    key_compare
    key_comp() const { return _cmp; }

  private:
    //! The related data for the iterator.
    key_compare _cmp;
  };

  /**
   *  Finds the past-the-end position in \c container for this constant
   *  iterator.
   *
   *  \tparam Container The type of container to iterate.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *  \return An iterator pointing to the past-the-end position in the
   *  container.
   *
   *  \consttime
   *
   *  \attention This iterator impose constness constraints on its \c value_type
   *  if the container's is a set and not a map. Iterators on sets prevent
   *  modification of the \c value_type because modifying the key may result in
   *  invalidation of the tree. If the container is a map, only the \c
   *  mapped_type can be modified (the \c second element).
   */
  template <typename Container>
  inline ordered_iterator<Container>
  ordered_end(Container& container)
  {
    return ordered_iterator<Container>
      // At header (dim = rank - 1)
      (container, container.dimension() - 1, container.end().node);
  }

  template <typename Container>
  inline ordered_iterator<const Container>
  ordered_cend(const Container& container)
  { return ordered_end(container); }
  ///@}

  /**
   *  Finds the value in \c container for which its key has the smallest
   *  coordinate over the dimension \c ordered_dim.
   *
   *  \tparam Container The type of container to iterate.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *
   *  \fractime
   *
   *  \attention This iterator impose constness constraints on its \c value_type
   *  if the container's is a set and not a map. Iterators on sets prevent
   *  modification of the \c value_type because modifying the key may result in
   *  invalidation of the tree. If the container is a map, only the \c
   *  mapped_type can be modified (the \c second element).
   */
  template <typename Container>
  inline ordered_iterator<Container>
  ordered_begin(Container& container)
  {
    if (container.empty()) return ordered_end(container);
    typename ordered_iterator<Container>::node_ptr node
      = container.end().node->parent;
    dimension_type dim;
    spatial::import::tie(node, dim)
      = first_ordered(node, 0, container.rank(),
                      container.key_comp());
    return ordered_iterator<Container>(container, dim, node);
  }

  template <typename Container>
  inline ordered_iterator<const Container>
  ordered_cbegin(const Container& container)
  { return ordered_begin(container); }
  ///@}

  namespace details
  {
    template <typename Cmp, typename Rank, typename Key>
    inline bool
    order_less(const Cmp& cmp, const Rank rank,
               const Key& a, const Key& b)
    {
      for (dimension_type d = 0; d < rank(); ++d)
        {
          if (cmp(d, a, b)) return true;
          if (cmp(d, b, a)) return false;
        }
      return false;
    }

    /**
     *  Move the iterator given in parameter to the minimum value along the
     *  iterator's ordered dimension but only in the sub-tree composed of the
     *  node pointed to by the iterator and its children.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter An iterator that points to the root node of the search.
     *  \return The iterator given in parameter is moved to the value with the
     *  smallest coordinate along \c iter's \c ordered_dim, and among the
     *  children of the node pointed to by \c iter.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  ordered_begin(). In any case, use it cautiously, as this function does
     *  not perform any sanity checks on the iterator given in parameter.
     *
     *  \fractime
     */
    template <typename NodePtr, typename Rank, typename KeyCompare>
    inline std::pair<NodePtr, dimension_type>
    first_ordered(NodePtr node, dimension_type dim, const Rank rank,
                  const KeyCompare& cmp)
    {
      SPATIAL_ASSERT_CHECK(!header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      NodePtr end = node->parent;
      while (node->left != 0)
        { node = node->left; ++dim; }
      NodePtr best = node;
      dimension_type best_dim = dim;
      // Similarly to mapping iterator, scans all nodes and prune whichever is
      // higher in the dimension 0 only. When the search is over, the lowest
      // in the total order must have been found (since all other dimensions are
      // explored with no pruning).
      for(;;)
        {
          if (node->right != 0
              && ((dim % rank()) > 0
                  || !cmp(0, const_key(best), const_key(node))))
            {
              node = node->right; ++dim;
              while (node->left != 0)
                { node = node->left; ++dim; }
            }
          else
            {
              NodePtr prev_node = node;
              node = node->parent; --dim;
              while (node != end && prev_node == node->right)
                {
                  prev_node = node;
                  node = node->parent; --dim;
                }
              if (node == end) break;
            }
          if (order_less(cmp, rank, const_key(node), const_key(best)))
            { best = node; best_dim = dim; }
        }
      SPATIAL_ASSERT_CHECK(best != 0);
      SPATIAL_ASSERT_CHECK(best != end);
      return std::make_pair(best, best_dim);
    }

    /**
     *  Move the iterator given in parameter to the maximum value along the
     *  iterator's ordered dimension but only in the sub-tree composed of the
     *  node pointed to by the iterator and its children.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter An iterator that points to the root node of the search.
     *  \return The iterator given in parameter is moved to the value with the
     *  largest coordinate along \c iter's \c ordered_dim, among the children of
     *  the node pointed to by \c iter.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  ordered_begin(). In any case, use it cautiously, as this function does
     *  not perform any sanity checks on the iterator given in parameter.
     *
     *  \fractime
     */
    template <typename NodePtr, typename Rank, typename KeyCompare>
    inline std::pair<NodePtr, dimension_type>
    last_ordered(NodePtr node, dimension_type dim, const Rank rank,
                 const KeyCompare& cmp)
    {
      SPATIAL_ASSERT_CHECK(!header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      NodePtr end = node->parent;
      while (node->right != 0)
        { node = node->right; ++dim; }
      NodePtr best = node;
      dimension_type best_dim = dim;
      for (;;)
        {
          if (node->left != 0
              && ((dim % rank()) > 0
                  || !cmp(0, const_key(node), const_key(best))))
            {
              node = node->left; ++dim;
              while (node->right != 0)
                { node = node->right; ++dim; }
            }
          else
            {
              NodePtr prev_node = node;
              node = node->parent; --dim;
              while (node != end
                     && prev_node == node->left)
                {
                  prev_node = node;
                  node = node->parent; --dim;
                }
              if (node == end) break;
            }
          if (order_less(cmp, rank, const_key(best), const_key(node)))
            { best = node; best_dim = dim; }
        }
      SPATIAL_ASSERT_CHECK(best != 0);
      SPATIAL_ASSERT_CHECK(best != end);
      return std::make_pair(best, best_dim);
    }

    /**
     *  Move the pointer given in parameter to the next element in the ordered
     *  iteration of values along the ordered dimension.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter The reference iterator that points to the current node.
     *  \return An iterator pointing to the value with the smallest coordinate
     *  along \c iter's \c ordered_dim, and among the children of the node
     *  pointed to by \c iter.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking to increment an
     *  \ordered_iterator via the overloaded \c operator++().
     *
     *  Since Container is based on a \kdtree and \kdtrees exhibit good locality
     *  of reference (for arranging values in space, not for values location in
     *  memory), the function will run with time complexity close to \Olog in
     *  practice.
     *
     *  \fractime
     */
    template <typename NodePtr, typename Rank, typename KeyCompare>
    inline std::pair<NodePtr, dimension_type>
    increment_ordered
    (NodePtr node, dimension_type dim, const Rank rank,
     const KeyCompare& cmp)
    {
      SPATIAL_ASSERT_CHECK(!header(node));
      NodePtr orig = node;
      dimension_type orig_dim = dim;
      NodePtr best = 0;
      dimension_type best_dim = 0;
      // Look forward to find an equal or greater next best
      // If an equal next best is found, then no need to look further
      for (;;)
        {
          if (node->right != 0
              && ((dim % rank()) > 0 || best == 0
                  || !cmp(0, const_key(best), const_key(node))))
            {
              node = node->right; ++dim;
              while (node->left != 0
                     && ((dim % rank()) > 0
                         || !cmp(0, const_key(node), const_key(orig))))
                { node = node->left; ++dim; }
            }
          else
            {
              NodePtr prev_node = node;
              node = node->parent; --dim;
              while (!header(node) && prev_node == node->right)
                {
                  prev_node = node;
                  node = node->parent; --dim;
                }
              if (header(node)) break;
            }
          if (order_less(cmp, rank, const_key(orig), const_key(node)))
            {
              if (best == 0
                  || order_less(cmp, rank, const_key(node), const_key(best)))
                { best = node; best_dim = dim; }
            }
          else if (!order_less(cmp, rank, const_key(node), const_key(orig)))
            { return std::make_pair(node, dim); }
        }
      SPATIAL_ASSERT_CHECK(header(node));
      // Maybe there is a better best looking backward...
      node = orig;
      dim = orig_dim;
      for (;;)
        {
          if (node->left != 0
              && (dim % rank() > 0
                  || !cmp(0, const_key(node), const_key(orig))))
            {
              node = node->left; ++dim;
              while (node->right != 0
                     && (dim % rank() > 0 || best == 0
                         || !cmp(0, const_key(best), const_key(node))))
                { node = node->right; ++dim; }
            }
          else
            {
              NodePtr prev_node = node;
              node = node->parent; --dim;
              while (!header(node) && prev_node == node->left)
                {
                  prev_node = node;
                  node = node->parent; --dim;
                }
              if (header(node)) break;
            }
          if (order_less(cmp, rank, const_key(orig), const_key(node))
              && (best == 0
                  || !order_less(cmp, rank, const_key(best), const_key(node))))
            { best = node; best_dim = dim; }
        }
      if (best != 0)
        { node = best; dim = best_dim; }
      SPATIAL_ASSERT_CHECK((best == 0 && header(node))
                           || (best != 0 && !header(node)));
      return std::make_pair(node, dim);
    }

    /**
     *  Move the pointer given in parameter to the previous element in the
     *  ordered iteration of values along the ordered dimension.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter The reference iterator that points to the current node.
     *  \return An iterator pointing to the value with the smallest coordinate
     *  along \c iter's \c ordered_dim, and among the children of the node
     *  pointed to by \c iter.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking to decrement an
     *  \ordered_iterator via the overloaded \c operator--().
     *
     *  Since Container is based on a \kdtree and \kdtrees exhibit good locality
     *  of reference (for arranging values in space, not for values location in
     *  memory), the function will run with time complexity close to \Olog in
     *  practice.
     *
     *  \fractime
     */
    template <typename NodePtr, typename Rank, typename KeyCompare>
    inline std::pair<NodePtr, dimension_type>
    decrement_ordered
    (NodePtr node, dimension_type dim, const Rank rank,
     const KeyCompare& cmp)
    {
      if (header(node))
        return last_ordered(node->parent, 0, rank, cmp);
      NodePtr orig = node;
      dimension_type orig_dim = dim;
      NodePtr best = 0;
      dimension_type best_dim = 0;
      // Look forward to find an equal or greater next best
      // If an equal next best is found, then no need to look further
      for (;;)
        {
          if (node->left != 0
              && ((dim % rank()) > 0 || best == 0
                  || !cmp(0, const_key(node), const_key(best))))
            {
              node = node->left; ++dim;
              while (node->right != 0
                     && ((dim % rank()) > 0
                         || !cmp(0, const_key(orig), const_key(node))))
                { node = node->right; ++dim; }
            }
          else
            {
              NodePtr prev_node = node;
              node = node->parent; --dim;
              while (!header(node) && prev_node == node->left)
                {
                  prev_node = node;
                  node = node->parent; --dim;
                }
              if (header(node)) break;
            }
          if (order_less(cmp, rank, const_key(node), const_key(orig)))
            {
              if (best == 0
                  || order_less(cmp, rank, const_key(best), const_key(node)))
                { best = node; best_dim = dim; }
            }
          else if (!order_less(cmp, rank, const_key(orig), const_key(node)))
            { return std::make_pair(node, dim); }
        }
      SPATIAL_ASSERT_CHECK(header(node));
      // Maybe there is a better best looking backward...
      node = orig;
      dim = orig_dim;
      for (;;)
        {
          if (node->right != 0
              && ((dim % rank()) > 0
                  || !cmp(0, const_key(orig), const_key(node))))
            {
              node = node->right; ++dim;
              while (node->left != 0
                     && ((dim % rank()) > 0 || best == 0
                         || !cmp(0, const_key(node), const_key(best))))
                { node = node->left; ++dim; }
            }
          else
            {
              NodePtr prev_node = node;
              node = node->parent; --dim;
              while (!header(node) && prev_node == node->right)
                {
                  prev_node = node;
                  node = node->parent; --dim;
                }
              if (header(node)) break;
            }
          if (order_less(cmp, rank, const_key(node), const_key(orig))
              && (best == 0
                  || !order_less(cmp, rank, const_key(node), const_key(best))))
            { best = node; best_dim = dim; }
        }
      if (best != 0)
        { node = best; dim = best_dim; }
      SPATIAL_ASSERT_CHECK((best == 0 && header(node))
                           || (best != 0 && !header(node)));
      return std::make_pair(node, dim);
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_ORDERED_HPP
