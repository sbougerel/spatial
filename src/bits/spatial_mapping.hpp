// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_mapping.hpp
 *  Contains the definition of the \mapping_iterator.
 */

#ifndef SPATIAL_MAPPING_HPP
#define SPATIAL_MAPPING_HPP

#include <utility> // provides ::std::pair<> and ::std::make_pair()
#include "../traits.hpp"
#include "spatial_bidirectional.hpp"
#include "spatial_rank.hpp"
#include "spatial_except.hpp"
#include "spatial_assign.hpp"

namespace spatial
{
  namespace details
  {
    /**
     *  Extra information needed by the iterator to perform its work. This
     *  information is copied to each iterator from a given container.
     *
     *  Although it may be possible to modify this information directly from
     *  it's members, it may be unwise to do so, as it could invalidate the
     *  iterator and cause the program to behave unexpectedly. If any of this
     *  information needs to be modified, it is probably recommended to create a
     *  new iterator altogether.
     *
     *  \tparam Ct The container to which these iterator relate to.
     */
    template <typename Container>
    struct Mapping : Container::key_compare
    {
      //! Build an uninitialized mapping data object.
      Mapping() { }

      /**
       *  Builds required mapping data from the given key comparison functor,
       *  dimension and mapping dimension.
       *
       *  \param c The container being iterated.
       *  \param m The mapping dimension used in the iteration.
       */
      Mapping
      (const typename Container::key_compare& c, dimension_type m)
        : Container::key_compare(c), mapping_dim(m)
      { }

      typename Container::key_compare key_comp() const
      { return static_cast<typename Container::key_compare>(*this); }

      /**
       *  The current dimension of iteration.
       *
       *  You can modify this key if you suddenly want the iterator to change
       *  dimension of iteration. However this field must always satisfy:
       *  \f[
       *  mapping_dim() < Rank()
       *  \f]
       *  Rank being the template rank provider for the iterator.
       *
       *  \attention If you modify this value directly, no safety check will be
       *  performed.
       */
      dimension_type mapping_dim;
    };
  }

  /**
   *  This iterator walks through all items in the container in order from the
   *  lowest to the highest value along a particular dimension. The \c key_comp
   *  comparator of the container is used for comparision.
   *
   *  In effect, that makes any container of the library behave as a \c std::set
   *  or \c std::map. Through this iterator, a spatial container with 3
   *  dimensions can provide the same features as 3 \c std::set(s) or \c
   *  std::map(s) containing the same elements and ordered on each of these
   *  dimensions. Beware that iteration through the tree is very efficient when
   *  the dimension \kdtree is very small by comparison to the number of
   *  objects, but pretty inefficient otherwise, by comparison to a \c std::set.
   */
  template<typename Ct>
  class mapping_iterator
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

    typedef typename container_traits<Ct>::key_compare key_compare;

    //! Uninitialized iterator.
    mapping_iterator() { }

    /**
     *  The standard way to build this iterator: specify a mapping dimension, an
     *  iterator on a container, and that container.
     *
     *  \param container   The container to iterate.
     *  \param mapping_dim The dimension used to order all nodes during the
     *                     iteration.
     *  \param iter        Use the value of \c iter as the start point for the
     *                     iteration.
     */
    mapping_iterator(Ct& container, dimension_type mapping_dim,
                     typename container_traits<Ct>::iterator iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        _data(container.key_comp(), mapping_dim)
    { except::check_dimension(container.dimension(), mapping_dim); }

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
     *  \param mapping_dim The dimension used to order all nodes during the
     *                     iteration.
     *  \param dim The dimension of the node pointed to by iterator.
     *  \param ptr Use the value of node as the start point for the
     *             iteration.
     */
    mapping_iterator(Ct& container, dimension_type mapping_dim,
                     dimension_type dim,
                     typename container_traits<Ct>::mode_type::node_ptr ptr)
      : Base(container.rank(), ptr, dim),
        _data(container.key_comp(), mapping_dim)
    { except::check_dimension(container.dimension(), mapping_dim); }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    mapping_iterator<Ct>& operator++()
    {
      details::assign(node, node_dim,
                      increment_mapping(node, node_dim, rank(),
                                        _data.mapping_dim, key_comp()));
      return *this;
    }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    mapping_iterator<Ct> operator++(int)
    {
      mapping_iterator<Ct> x(*this);
      details::assign(node, node_dim,
                      increment_mapping(node, node_dim, rank(),
                                        _data.mapping_dim, key_comp()));
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    mapping_iterator<Ct>& operator--()
    {
      details::assign(node, node_dim,
                      decrement_mapping(node, node_dim, rank(),
                                        _data.mapping_dim, key_comp()));
      return *this;
    }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    mapping_iterator<Ct> operator--(int)
    {
      mapping_iterator<Ct> x(*this);
      details::assign(node, node_dim,
                      decrement_mapping(node, node_dim, rank(),
                                        _data.mapping_dim, key_comp()));
      return x;
    }

    //! Return the key_comparator used by the iterator
    key_compare
    key_comp() const { return _data.key_comp(); }

    /**
     *  Accessor to the mapping dimension used by the iterator.
     *
     *  No check is performed on this accessor if a new mapping dimension is
     *  given. If you need to check that the mapping dimension given does not
     *  exceed the rank use the function \ref spatial::mapping_dimension()
     *  instead:
     *
     *  \code
     *  // Create a mapping iterator that works on dimension 0
     *  mapping_iterator<my_container> iter (begin_mapping(container, 0));
     *  // Reset the mapping iterator to work on dimension 2
     *  mapping_dimension(iter, 2);
     *  // This will throw if the container used has a rank lower than 3.
     *  \endcode
     */
    ///@{
    dimension_type
    mapping_dimension() const { return _data.mapping_dim; }
    dimension_type&
    mapping_dimension() { return _data.mapping_dim; }
    ///@}

  private:
    //! The related data for the iterator.
    details::Mapping<Ct> _data;
  };

  /**
   *  This iterator walks through all items in the container in order from the
   *  lowest to the highest value along a particular dimension. The \c key_comp
   *  comparator of the container is used for comparision.
   *
   *  In effect, that makes any container of the library behave as a \c std::set
   *  or \c std::map. Through this iterator, a spatial container with 3
   *  dimensions can provide the same features as 3 \c std::set(s) or \c
   *  std::map(s) containing the same elements and ordered on each of these
   *  dimensions. Beware that iteration through the tree is very efficient when
   *  the dimension \kdtree is very small by comparison to the number of
   *  objects, but pretty inefficient otherwise, by comparison to a \c std::set.
   *
   *  Object deferenced by this iterator are always constant.
   */
  template<typename Ct>
  class mapping_iterator<const Ct>
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
    mapping_iterator() { }

    /**
     *  The standard way to build this iterator: specify a mapping dimension,
     *  an iterator on a container, and that container.
     *
     *  \param container The container to iterate.
     *  \param mapping_dim The dimension used to order all nodes during the
     *  iteration.
     *  \param iter Use the value of \c iter as the start point for the
     *  iteration.
     */
    mapping_iterator(const Ct& container, dimension_type mapping_dim,
                     typename container_traits<Ct>::const_iterator iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        _data(container.key_comp(), mapping_dim)
    { except::check_dimension(container.dimension(), mapping_dim); }

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
     *
     *  \param mapping_dim The dimension used to order all nodes during the
     *                     iteration.
     *
     *  \param dim The dimension of the node pointed to by iterator.
     *
     *  \param ptr Use the value of \c node as the start point for the
     *             iteration.
     */
    mapping_iterator
    (const Ct& container, dimension_type mapping_dim, dimension_type dim,
     typename container_traits<Ct>::mode_type::const_node_ptr ptr)
      : Base(container.rank(), ptr, dim),
        _data(container.key_comp(), mapping_dim)
    { except::check_dimension(container.dimension(), mapping_dim); }

    //! Convertion of mutable iterator into a constant iterator is permitted.
    mapping_iterator(const mapping_iterator<Ct>& iter)
      : Base(iter.rank(), iter.node, iter.node_dim),
        _data(iter.key_comp(), iter.mapping_dimension()) { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    mapping_iterator<const Ct>& operator++()
    {
      details::assign(node, node_dim,
                      increment_mapping(node, node_dim, rank(),
                                        _data.mapping_dim, key_comp()));
      return *this;
    }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    mapping_iterator<const Ct> operator++(int)
    {
      mapping_iterator<const Ct> x(*this);
      details::assign(node, node_dim,
                      increment_mapping(node, node_dim, rank(),
                                        _data.mapping_dim, key_comp()));
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    mapping_iterator<const Ct>& operator--()
    {
      details::assign(node, node_dim,
                      decrement_mapping(node, node_dim, rank(),
                                        _data.mapping_dim, key_comp()));
      return *this;
    }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    mapping_iterator<const Ct> operator--(int)
    {
      mapping_iterator<const Ct> x(*this);
      details::assign(node, node_dim,
                      decrement_mapping(node, node_dim, rank(),
                                        _data.mapping_dim, key_comp()));
      return x;
    }

    //! Return the key_comparator used by the iterator
    key_compare
    key_comp() const { return _data.key_comp(); }

    /**
     *  Accessor to the mapping dimension used by the iterator.
     *
     *  No check is performed on this accessor if a new mapping dimension is
     *  given. If you need to check that the mapping dimension given does not
     *  exceed the rank use the function \ref spatial::mapping_dimension()
     *  instead:
     *
     *  \code
     *  // Create a mapping iterator that works on dimension 0
     *  mapping_iterator<my_container> iter (begin_mapping(container, 0));
     *  // Reset the mapping iterator to work on dimension 2
     *  mapping_dimension(iter, 2);
     *  // This will throw if the container used has a rank lower than 3.
     *  \endcode
     */
    ///@{
    dimension_type
    mapping_dimension() const { return _data.mapping_dim; }
    dimension_type&
    mapping_dimension() { return _data.mapping_dim; }
    ///@}

  private:
    //! The related data for the iterator.
    details::Mapping<Ct> _data;
  };

  /**
   *  Return the mapping dimension of the iterator.
   *  \param it the iterator where the mapping dimension is retreived.
   */
  template <typename Container>
  inline dimension_type
  mapping_dimension(const mapping_iterator<Container>& it)
  { return it.mapping_dimension(); }

  /**
   *  Sets the mapping dimension of the iterator.
   *  \param it The iterator where the mapping dimension is set.
   *  \param mapping_dim The new mapping dimension to use.
   */
  template <typename Container>
  inline void
  mapping_dimension(mapping_iterator<Container>& it,
                    dimension_type mapping_dim)
  {
    except::check_dimension(it.dimension(), mapping_dim);
    it.mapping_dimension() = mapping_dim;
  }

  /**
   *  Finds the past-the-end position in \c container for this constant
   *  iterator.
   *
   *  \attention This iterator impose constness constraints on its \c value_type
   *  if the container's is a set and not a map. Iterators on sets prevent
   *  modification of the \c value_type because modifying the key may result in
   *  invalidation of the tree. If the container is a map, only the \c
   *  mapped_type can be modified (the \c second element).
   *
   *  \tparam Container The type of container to iterate.
   *  \param mapping_dim The dimension that is the reference for the iteration:
   *  all iterated values will be ordered along this dimension, from smallest to
   *  largest.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *  \return An iterator pointing to the past-the-end position in the
   *  container.
   *
   *  \consttime
   */
  ///@{
  template <typename Container>
  inline mapping_iterator<Container>
  mapping_end(Container& container, dimension_type mapping_dim)
  {
    except::check_dimension(container.dimension(), mapping_dim);
    return mapping_iterator<Container>
      (container, mapping_dim, container.dimension() - 1,
       container.end().node); // At header (dim = rank - 1)
  }

  template <typename Container>
  inline mapping_iterator<const Container>
  mapping_cend(const Container& container, dimension_type mapping_dim)
  { return mapping_end(container, mapping_dim); }
  ///@}

  /**
   *  Finds the value in \c container for which its key has the smallest
   *  coordinate over the dimension \c mapping_dim.
   *
   *  \attention This iterator impose constness constraints on its \c value_type
   *  if the container's is a set and not a map. Iterators on sets prevent
   *  modification of the \c value_type because modifying the key may result in
   *  invalidation of the tree. If the container is a map, only the \c
   *  mapped_type can be modified (the \c second element).
   *
   *  \tparam Container The type of container to iterate.
   *  \param mapping_dim The dimension that is the reference for the iteration:
   *  all iterated values will be ordered along this dimension, from smallest to
   *  largest.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *
   *  \fractime
   */
  ///@{
  template <typename Container>
  inline mapping_iterator<Container>
  mapping_begin(Container& container, dimension_type mapping_dim)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    typename mapping_iterator<Container>::node_ptr node
      = container.end().node->parent;
    dimension_type dim;
    details::assign(node, dim,
                    minimum_mapping(node, 0,
                                    container.rank(), mapping_dim,
                                    container.key_comp()));
    return mapping_iterator<Container>(container, mapping_dim, dim, node);
  }

  template <typename Container>
  inline mapping_iterator<const Container>
  mapping_cbegin(const Container& container, dimension_type mapping_dim)
  { return mapping_begin(container, mapping_dim); }
  ///@}

  namespace details
  {
    template <typename KeyCompare, typename Key>
    inline bool
    left_compare_mapping
    (KeyCompare key_comp, dimension_type map, const Key& x, const Key& y,
     strict_invariant_tag)
    { return key_comp(map, x, y); }

    template <typename KeyCompare, typename Key>
    inline bool
    left_compare_mapping
    (KeyCompare key_comp, dimension_type map, const Key& x, const Key& y,
     relaxed_invariant_tag)
    { return !key_comp(map, y, x); }

    /**
     *  Move the iterator given in parameter to the minimum value along the
     *  iterator's mapping dimension but only in the sub-tree composed of the
     *  node pointed to by the iterator and its children.
     *
     *  The tree in the container will be iterated in in-order fashion. As soon
     *  as the minimum is found, the first minimum is retained.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  spatial::mapping_begin(). This function does not perform any sanity
     *  checks on the iterator given in parameter.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter An iterator that points to the root node of the search.
     *  \return The iterator given in parameter is moved to the value with the
     *  smallest coordinate along \c iter's \c mapping_dim, and among the
     *  children of the node pointed to by \c iter.
     *
     *  \fractime
     */
    template <typename NodePtr, typename Rank, typename KeyCompare>
    inline std::pair<NodePtr, dimension_type>
    minimum_mapping(NodePtr node, dimension_type dim, Rank rank,
                    dimension_type map, KeyCompare key_comp)
    {
      SPATIAL_ASSERT_CHECK(map < rank());
      SPATIAL_ASSERT_CHECK(dim < rank());
      SPATIAL_ASSERT_CHECK(!header(node));
      NodePtr end = node->parent;
      while (node->left != 0)
        { node = node->left; dim = incr_dim(rank, dim); }
      NodePtr best = node;
      dimension_type best_dim = dim;
      for (;;)
        {
          if (node->right != 0 && dim != map)
            {
              node = node->right; dim = incr_dim(rank, dim);
              while (node->left != 0)
                { node = node->left; dim = incr_dim(rank, dim); }
            }
          else
            {
              NodePtr prev_node = node;
              node = node->parent; dim = decr_dim(rank, dim);
              while (node != end
                     && prev_node == node->right)
                {
                  prev_node = node;
                  node = node->parent; dim = decr_dim(rank, dim);
                }
              if (node == end) break;
            }
          if (key_comp(map, const_key(node), const_key(best)))
            { best = node; best_dim = dim; }
        }
      SPATIAL_ASSERT_CHECK(best_dim < rank());
      SPATIAL_ASSERT_CHECK(best != 0);
      SPATIAL_ASSERT_CHECK(best != end);
      return std::make_pair(best, best_dim);
    }

    /**
     *  Move the pointer given in parameter to the next element in the ordered
     *  iteration of values along the mapping dimension.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. You should use the
     *  overload \c operator++ on the \mapping_iterator instead. This function
     *  does not perform any sanity checks on the iterator given in parameter.
     *
     *  Since Container is based on \kdtree and \kdtree exhibit good locality of
     *  reference (for arranging values in space, not for values location in
     *  memory), the function will run with time complexity close to \Onlognk in
     *  practice.
     *
     *  \fractime
     */
    template <typename NodePtr, typename Rank, typename KeyCompare>
    inline std::pair<NodePtr, dimension_type>
    increment_mapping
    (NodePtr node, dimension_type dim, Rank rank, dimension_type map,
     KeyCompare key_comp)
    {
      SPATIAL_ASSERT_CHECK(dim < rank());
      SPATIAL_ASSERT_CHECK(!header(node));
      NodePtr orig = node;
      NodePtr best = 0;
      dimension_type best_dim;
      // Try to look in "front" first for equal or greater next best
      for (;;)
        {
          if (node->right != 0
              && (dim != map || best == 0))
            {
              node = node->right; dim = incr_dim(rank, dim);
              while (node->left != 0
                     && (dim != map
                         || left_compare_mapping
                            (key_comp, map, const_key(orig), const_key(node),
                             invariant_category(node))))
                { node = node->left; dim = incr_dim(rank, dim); }
            }
          else
            {
              NodePtr prev_node = node;
              node = node->parent; dim = decr_dim(rank, dim);
              while (!header(node)
                     && prev_node == node->right)
                {
                  prev_node = node;
                  node = node->parent; dim = decr_dim(rank, dim);
                }
              if (header(node)) break;
            }
          if (key_comp(map, const_key(orig), const_key(node)))
            {
              if (best == 0 || key_comp(map, const_key(node), const_key(best)))
                { best = node; best_dim = dim; }
            }
          else if (!key_comp(map, const_key(node), const_key(orig)))
            {
              SPATIAL_ASSERT_CHECK(dim < rank());
              SPATIAL_ASSERT_CHECK(!header(node));
              return std::make_pair(node, dim);
            }
        }
      SPATIAL_ASSERT_CHECK(dim == rank() - 1);
      SPATIAL_ASSERT_CHECK(header(node));
      // Since we have not found an equal next best, start from beginning
      NodePtr front_best = best;
      node = node->parent; dim = 0;
      while (node->left != 0
             && (dim != map
                 || key_comp(map, const_key(orig), const_key(node))))
        { node = node->left; dim = incr_dim(rank, dim); }
      if (key_comp(map, const_key(orig), const_key(node))
          && (best == 0
              || key_comp(map, const_key(node), const_key(best))
              || (front_best == best
                  && !key_comp(map, const_key(best), const_key(node)))))
        { best = node; best_dim = dim; }
      for (;;)
        {
          if (node->right != 0
              && (dim != map || best == 0
                  || key_comp(map, const_key(node), const_key(best))))
            {
              node = node->right; dim = incr_dim(rank, dim);
              while (node->left != 0
                     && (dim != map
                         || key_comp(map, const_key(orig), const_key(node))))
                { node = node->left; dim = incr_dim(rank, dim); }
            }
          else
            {
              NodePtr prev_node = node;
              node = node->parent; dim = decr_dim(rank, dim);
              while (!header(node)
                     && prev_node == node->right)
                {
                  prev_node = node;
                  node = node->parent; dim = decr_dim(rank, dim);
                }
              if (header(node)) break;
            }
          if (key_comp(map, const_key(orig), const_key(node))
              && (best == 0
                  || key_comp(map, const_key(node), const_key(best))
                  || (front_best == best
                      && !key_comp(map, const_key(best), const_key(node)))))
            { best = node; best_dim = dim; }
        }
      if (best != 0)
        { node = best; dim = best_dim; }
      SPATIAL_ASSERT_CHECK(dim < rank());
      SPATIAL_ASSERT_CHECK((best == 0 && header(node))
                           || (best != 0 && !header(node)));
      return std::make_pair(node, dim);
    }

    /**
     *  Move the iterator given in parameter to the maximum value along the
     *  iterator's mapping dimension but only in the sub-tree composed of the
     *  node pointed to by the iterator and its children.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  spatial::mapping_end().  This function does not perform any sanity
     *  checks on the iterator given in parameter.
     *
     *  The maximum element in the dimenion \c map is found by looking through
     *  the tree in reversed pre-order fashion. That means we start from the
     *  deepest, right-most element in the tree, and iterate all the way to the
     *  root node. We never, however, visit a left sub-tree when the dimension
     *  of the current node is equal to \c map: it's impossible to find a
     *  greater element in the sub-tree in this case.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter An iterator that points to the root node of the search.
     *  \return The iterator given in parameter is moved to the value with the
     *  largest coordinate along \c iter's \c mapping_dim, among the children of
     *  the node pointed to by \c iter.
     *
     *  \fractime
     */
    template <typename NodePtr, typename Rank, typename KeyCompare>
    inline std::pair<NodePtr, dimension_type>
    maximum_mapping(NodePtr node, dimension_type dim, Rank rank,
                    dimension_type map, KeyCompare key_comp)
    {
      SPATIAL_ASSERT_CHECK(map < rank());
      SPATIAL_ASSERT_CHECK(dim < rank());
      SPATIAL_ASSERT_CHECK(!header(node));
      NodePtr end = node->parent;
      while (node->right != 0)
        { node = node->right; dim = incr_dim(rank, dim); }
      NodePtr best = node;
      dimension_type best_dim = dim;
      for (;;)
        {
          if (node->left != 0 && dim != map)
            {
              node = node->left; dim = incr_dim(rank, dim);
              while (node->right != 0)
                { node = node->right; dim = incr_dim(rank, dim); }
            }
          else
            {
              NodePtr prev_node = node;
              node = node->parent; dim = decr_dim(rank, dim);
              while (node != end
                     && prev_node == node->left)
                {
                  prev_node = node;
                  node = node->parent; dim = decr_dim(rank, dim);
                }
              if (node == end) break;
            }
          if (key_comp(map, const_key(best), const_key(node)))
            { best = node; best_dim = dim; }
        }
      SPATIAL_ASSERT_CHECK(best_dim < rank());
      SPATIAL_ASSERT_CHECK(best != 0);
      SPATIAL_ASSERT_CHECK(best != end);
      return std::make_pair(best, best_dim);
    }

    /**
     *  Move the pointer given in parameter to the previous element in the
     *  ordered iteration of values along the mapping dimension.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. You should use the
     *  overload \c operator-- on the \mapping_iterator instead. This function
     *  does not perform any sanity checks on the iterator given in parameter.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter The reference iterator that points to the current node.
     *  \return An iterator pointing to the value with the smallest coordinate
     *  along \c iter's \c mapping_dim, and among the children of the node
     *  pointed to by \c iter.
     *
     *  Since Container is based on \kdtree and \kdtree exhibit good locality of
     *  reference (for arranging values in space, not for values location in
     *  memory), the function will run with time complexity close to \Onlognk in
     *  practice.
     *
     *  \fractime
     */
    template <typename NodePtr, typename Rank, typename KeyCompare>
    inline std::pair<NodePtr, dimension_type>
    decrement_mapping
    (NodePtr node, dimension_type dim, Rank rank, dimension_type map,
     KeyCompare key_comp)
    {
      SPATIAL_ASSERT_CHECK(dim < rank());
      if (header(node))
        return maximum_mapping(node->parent, 0, rank, map, key_comp);
      NodePtr orig = node;
      NodePtr best = 0;
      dimension_type best_dim;
      // Try to look in "front" first for equal or greater next best
      for (;;)
        {
          if (node->left != 0
              && (dim != map || best == 0))
            {
              node = node->left; dim = incr_dim(rank, dim);
              while (node->right != 0
                     && (dim != map
                         || !key_comp(map, const_key(orig), const_key(node))))
                { node = node->right; dim = incr_dim(rank, dim); }
            }
          else
            {
              NodePtr prev_node = node;
              node = node->parent; dim = decr_dim(rank, dim);
              while (!header(node)
                     && prev_node == node->left)
                {
                  prev_node = node;
                  node = node->parent; dim = decr_dim(rank, dim);
                }
              if (header(node)) break;
            }
          if (key_comp(map, const_key(node), const_key(orig)))
            {
              if (best == 0 || key_comp(map, const_key(best), const_key(node)))
                { best = node; best_dim = dim; }
            }
          else if (!key_comp(map, const_key(orig), const_key(node)))
            {
              SPATIAL_ASSERT_CHECK(dim < rank());
              SPATIAL_ASSERT_CHECK(!header(node));
              return std::make_pair(node, dim);
            }
        }
      SPATIAL_ASSERT_CHECK(dim == rank() - 1);
      SPATIAL_ASSERT_CHECK(header(node));
      // Since we have not found an equal next best, start from beginning
      NodePtr front_best = best;
      node = node->parent; dim = 0;
      while (node->right != 0
             && (dim != map
                 || key_comp(map, const_key(node), const_key(orig))))
        { node = node->right; dim = incr_dim(rank, dim); }
      if (key_comp(map, const_key(node), const_key(orig))
          && (best == 0
              || key_comp(map, const_key(best), const_key(node))
              || (front_best == best
                  && !key_comp(map, const_key(node), const_key(best)))))
        { best = node; best_dim = dim; }
      for (;;)
        {
          if (node->left != 0
              && (dim != map || best == 0
                  || key_comp(map, const_key(best), const_key(node))))
            {
              node = node->left; dim = incr_dim(rank, dim);
              while (node->right != 0
                     && (dim != map
                         || key_comp(map, const_key(node), const_key(orig))))
                { node = node->right; dim = incr_dim(rank, dim); }
            }
          else
            {
              NodePtr prev_node = node;
              node = node->parent; dim = decr_dim(rank, dim);
              while (!header(node)
                     && prev_node == node->left)
                {
                  prev_node = node;
                  node = node->parent; dim = decr_dim(rank, dim);
                }
              if (header(node)) break;
            }
          if (key_comp(map, const_key(node), const_key(orig))
              && (best == 0
                  || key_comp(map, const_key(best), const_key(node))
                  || (front_best == best
                      && !key_comp(map, const_key(node), const_key(best)))))
            { best = node; best_dim = dim; }
        }
      if (best != 0)
        { node = best; dim = best_dim; }
      SPATIAL_ASSERT_CHECK(dim < rank());
      SPATIAL_ASSERT_CHECK((best == 0 && header(node))
                           || (best != 0 && !header(node)));
      return std::make_pair(node, dim);
    }
  } // namespace details
} // namespace spatial

#endif // SPATIAL_MAPPING_HPP
