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

#include "spatial_bidirectional.hpp"
#include "../traits.hpp"
#include "spatial_rank.hpp"
#include "spatial_except.hpp"
#include "spatial_compress.hpp"
#include "spatial_preorder.hpp"

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
          if (node->right != 0
              && !key_comp(depth % rank(), key, const_key(node)))
            { node = node->right; ++depth; }
          else if (node->left != 0
                   && !key_comp(depth % rank(), const_key(node), key))
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
          if (node->right == prev_node && node->left != 0
              && !key_comp(depth % rank(), const_key(node), key))
            {
              node = node->left; ++depth;
              for (;;)
                {
                  if (node->right != 0
                      && !key_comp(depth % rank(), key, const_key(node)))
                    { node = node->right; ++depth; }
                  else if (node->left != 0
                           && !key_comp(depth % rank(), const_key(node), key))
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
          if (node->left != 0
              && !key_comp(depth % rank(), const_key(node), key))
            { node = node->left; ++depth; }
          else if (node->right != 0
                   && !key_comp(depth % rank(), key, const_key(node)))
            { node = node->right; ++depth; }
          else
            {
              NodePtr prev_node = node;
              node = node->parent; --depth;
              while (!header(node)
                     && (prev_node == node->right
                         || node->right == 0
                         || key_comp(depth % rank(), key, const_key(node))))
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
          if (node->right == prev_node && node->left != 0
              && !key_comp(depth % rank(), const_key(node), key))
            {
              node = node->left; ++depth;
              for (;;)
                {
                  if (node->right != 0
                      && !key_comp(depth % rank(), key, const_key(node)))
                    { node = node->right; ++depth; }
                  else if (node->left != 0
                           && !key_comp(depth % rank(), const_key(node), key))
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

  /**
   *  This type provides an iterator to iterate through all elements of a
   *  container that match a given key, passed as a parameter to the
   *  constructor. The given key is called the model.
   *
   *  \tparam Container The container upon which these iterator relate to.
   *  \headerfile equal_iterator.hpp
   */
  template <typename Container>
  class equal_iterator
    : public details::Bidirectional_iterator
      <typename container_traits<Container>::mode_type,
       typename container_traits<Container>::rank_type>
  {
  private:
    //! The preorder iterator without its criterion
    typedef typename details::Bidirectional_iterator
    <typename container_traits<Container>::mode_type,
     typename container_traits<Container>::rank_type> Base;

  public:
    using Base::node;
    using Base::node_dim;
    using Base::rank;

    //! The type used to store the model key to be looked up in the container.
    typedef typename container_traits<Container>::key_type key_type;

    //! The comparison functor used to compare keys.
    typedef typename container_traits<Container>::key_compare key_compare;

    //! \empty
    equal_iterator() { }

    /**
     *  Build an equal iterator from a container's iterator type.
     *
     *  This constructor should be used in the general case where the dimension
     *  for the node pointed to by \c iter is not known. The dimension of the
     *  node will be recomputed from the given iterator by iterating through all
     *  parents until the header node has been reached. This iteration is
     *  bounded by \Olog when the container is perfectly balanced.
     *
     *  \param container The container being iterated.
     *  \param value The key to look for.
     *  \param iter An iterator on the type Ct.
     */
    equal_iterator(Container& container, const key_type& value,
                   typename container_traits<Container>::iterator iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        _data(container.key_comp(), value) { }

    /**
     *  Build an equal iterator from the node and current dimension of a
     *  container's element.
     *
     *  This constructor should be used only when the dimension of the node
     *  pointed to by iter is known. If in doubt, use the other
     *  constructor. This constructor perform slightly faster than the other,
     *  since the dimension does not have to be calculated. Note however that
     *  the calculation of the dimension in the other iterator takes slightly
     *  longer than \Olog in general, and so it is not likely to affect the
     *  performance of your application in any major way.
     *
     *  \param container The container being iterated.
     *  \param value The key to look for.
     *  \param ptr An iterator on the type Ct.
     *  \param dim The node's dimension for the node pointed to by node.
     *  \param container The container being iterated.
     */
    equal_iterator
    (Container& container, const key_type& value, dimension_type dim,
     typename container_traits<Container>::mode_type::node_ptr ptr)
      : Base(container.rank(), ptr, dim), _data(container.key_comp(), value)
    { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    equal_iterator<Container>& operator++()
    {
      import::tie(node, node_dim)
        = increment_equal(node, node_dim, rank(), _data.base(), _data());
      return *this;
    }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    equal_iterator<Container> operator++(int)
    {
      equal_iterator<Container> x(*this);
      import::tie(node, node_dim)
        = increment_equal(node, node_dim, rank(), _data.base(), _data());
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    equal_iterator<Container>& operator--()
    {
      import::tie(node, node_dim)
        = decrement_equal(node, node_dim, rank(), _data.base(), _data());
      return *this;
    }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    equal_iterator<Container> operator--(int)
    {
      equal_iterator<Container> x(*this);
      import::tie(node, node_dim)
        = decrement_equal(node, node_dim, rank(), _data.base(), _data());
      return x;
    }

    //! Return the value of key used to find equal keys in the container.
    key_type value() const { return _data(); }

    //! Return the functor used to compare keys in this iterator.
    key_compare key_comp() const { return _data.base(); }

  private:
    //! The model key used to find equal keys in the container.
    details::Compress<key_compare, key_type> _data;
  };

  /**
   *  This type provides an iterator to iterate through all elements of a
   *  container that match a given key, passed as a parameter to the
   *  constructor. The given key is called the model.
   *
   *  The values returned by this iterator will not be mutable.
   *
   *  \tparam Ct The container upon which these iterator relate to.
   */
  template <typename Container>
  class equal_iterator<const Container>
    : public details::Const_bidirectional_iterator
      <typename container_traits<Container>::mode_type,
       typename container_traits<Container>::rank_type>
  {
  private:
    //! The preorder iterator without its criterion.
    typedef details::Const_bidirectional_iterator
    <typename container_traits<Container>::mode_type,
     typename container_traits<Container>::rank_type> Base;

  public:
    using Base::node;
    using Base::node_dim;
    using Base::rank;

    //! The type used to store the model key to be looked up in the container.
    typedef typename container_traits<Container>::key_type key_type;

    //! The comparison functor used to compare keys.
    typedef typename container_traits<Container>::key_compare key_compare;

    //! \empty
    equal_iterator() { }

    /**
     *  Build an equal iterator from a container's iterator type.
     *
     *  This constructor should be used in the general case where the dimension
     *  for the node pointed to by \c iter is not known. The dimension of the
     *  node will be recomputed from the given iterator by iterating through all
     *  parents until the header node has been reached. This iteration is
     *  bounded by \Olog when the container is perfectly balanced.
     *
     *  \param container The container being iterated.
     *  \param value The key to look for.
     *  \param iter An iterator from the container.
     */
    equal_iterator(const Container& container, const key_type& value,
                   typename container_traits<Container>::const_iterator iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        _data(container.key_comp(), value) { }

    /**
     *  Build an equal iterator from the node and current dimension of a
     *  container's element.
     *
     *  This constructor should be used only when the dimension of the node
     *  pointed to by iter is known. If in doubt, use the other
     *  constructor. This constructor perform slightly faster since the
     *  dimension does not have to be calculated.
     *
     *  \param container The container being iterated.
     *  \param value The key to look for.
     *  \param dim The dimension associated with \c ptr when checking the
     *  invariant in \c container.
     *  \param ptr A pointer to a node belonging to \c container.
     */
    equal_iterator
    (const Container& container, const key_type& value, dimension_type dim,
     typename container_traits<Container>::mode_type::const_node_ptr ptr)
      : Base(container.rank(), ptr, dim), _data(container.key_comp(), value)
    { }

    //! Convertion of an iterator into a const_iterator is permitted.
    equal_iterator(const equal_iterator<Container>& iter)
      : Base(iter.rank(), iter.node, iter.node_dim),
        _data(iter.key_comp(), iter.value()) { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    equal_iterator<const Container>& operator++()
    {
      import::tie(node, node_dim)
        = increment_equal(node, node_dim, rank(), _data.base(), _data());
      return *this;
    }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    equal_iterator<const Container> operator++(int)
    {
      equal_iterator<const Container> x(*this);
      import::tie(node, node_dim)
        = increment_equal(node, node_dim, rank(), _data.base(), _data());
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    equal_iterator<const Container>& operator--()
    {
      import::tie(node, node_dim)
        = decrement_equal(node, node_dim, rank(), _data.base(), _data());
      return *this;
    }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    equal_iterator<const Container> operator--(int)
    {
      equal_iterator<const Container> x(*this);
      import::tie(node, node_dim)
        = preorder_decrement(node, node_dim, rank(), _data.base(), _data());
      return x;
    }

    //! Returns the value used to find equivalent keys in the container.
    key_type value() const { return _data(); }

    //! Returns the functor used to compare keys in this iterator.
    key_compare key_comp() const { return _data.base(); }

  private:
    //! The model key used to find equal keys in the container.
    details::Compress<key_compare, key_type> _data;
  };

  template <typename Container>
  inline equal_iterator<Container>
  equal_end(Container& container,
            const typename equal_iterator<Container>::key_type& value)
  {
    return equal_iterator<Container>
      (container, value, container.dimension() - 1,
       container.end().node); // At header, dim = rank - 1
  }

  template <typename Container>
  inline equal_iterator<const Container>
  equal_cend(const Container& container,
             const typename equal_iterator<Container>::key_type& value)
  { return equal_end(container, value); }

  /**
   *  Find the first element in \c container that compares equally to \c value,
   *  using \c container's \c key_compare comparator.
   *
   *  \tparam Container The container type being iterated.
   *  \param container The container being iterated.
   *  \param value A value to find matches among other keys stored in the
   *  container.
   */
  ///@{
  template <typename Container>
  inline equal_iterator<Container>
  equal_begin(Container& container,
              const typename equal_iterator<Container>::key_type& value)
  {
    if (container.empty()) return equal_end(container, value);
    typename equal_iterator<Container>::node_ptr node
      = container.end().node->parent;
    dimension_type dim;
    import::tie(node, dim)
      = first_equal(node, 0, container.rank(),
                    container.key_comp(), value);
    return equal_iterator<Container>(container, value, dim, node);
  }

  template <typename Container>
  inline equal_iterator<const Container>
  equal_cbegin(const Container& container,
               const typename equal_iterator<Container>::key_type& value)
  { return equal_begin(container, value); }
  ///@}

} // namespace spatial

#endif // SPATIAL_EQUAL_HPP
