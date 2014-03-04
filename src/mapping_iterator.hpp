// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   mapping_iterator.hpp
 *  Provides mapping iterator and all the functions around it.
 */

#ifndef SPATIAL_MAPPING_ITERATOR_HPP
#define SPATIAL_MAPPING_ITERATOR_HPP

#include <utility> // std::pair
#include "bits/spatial_mapping.hpp"

namespace spatial
{
  /**
   *  This structure defines a pair of mutable mapping iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \see mapping_iterator
   */
  template <typename Ct>
  struct mapping_iterator_pair
    : std::pair<mapping_iterator<Ct>, mapping_iterator<Ct> >
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef std::pair<mapping_iterator<Ct>, mapping_iterator<Ct> > Base;

    //! Empty constructor.
    mapping_iterator_pair() { }

    //! Regular constructor that builds a mapping_iterator_pair out of 2
    //! mapping_iterators.
    mapping_iterator_pair(const mapping_iterator<Ct>& a,
                          const mapping_iterator<Ct>& b) : Base(a, b) { }
  };

  /**
   *  This structure defines a pair of constant mapping iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \see mapping_iterator
   */
  template <typename Ct>
  struct mapping_iterator_pair<const Ct>
    : std::pair<mapping_iterator<const Ct>, mapping_iterator<const Ct> >
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef std::pair<mapping_iterator<const Ct>, mapping_iterator<const Ct> >
    Base;

    //! Empty constructor.
    mapping_iterator_pair() { }

    //! Regular constructor that builds a mapping_iterator_pair out of 2
    //! mapping_iterators.
    mapping_iterator_pair(const mapping_iterator<const Ct>& a,
                          const mapping_iterator<const Ct>& b) : Base(a, b)
    { }

    //! Convert a mutable mapping iterator pair into a const mapping iterator
    //!pair.
    mapping_iterator_pair(const mapping_iterator_pair<Ct>& p)
      : Base(p.first, p.second) { }
  };

  /**
   *  Returns a pair of iterator on the first and the last value in the range
   *  that can be iterated. This function is convenient to use with
   *  \c std::tie, and is equivalent to calling \ref mapping_begin() and \ref
   *  mapping_end() on both iterators.
   *
   *  To combine it with \c std::tie, use it this way:
   *  \code
   *  mapping<pointset<2, int> >::iterator it, end;
   *  std::tie(it, end) = mapping_range(0, my_points);
   *  for(; it != end; ++it)
   *  {
   *    // ...
   *  }
   *  \endcode
   *  Notice that an \c iterator type is declared, and not an \c iterator_pair
   *  type.
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
   *  \fractime
   */
  template <typename Container>
  inline mapping_iterator_pair<Container>
  mapping_range(Container& container, dimension_type mapping_dim)
  {
    return mapping_iterator_pair<Container>
          (mapping_begin(container, mapping_dim),
           mapping_end(container, mapping_dim));
  }

  ///@{
  /**
   *  Returns a pair of constant iterator on the first and the last value in the
   *  range that can be iterated. This function is convenient to use with
   *  \c std::tie, and is equivalent to calling \ref mapping_begin() and \ref
   *  mapping_end() on both iterators.
   *
   *  To combine it with \c std::tie, use it this way:
   *  \code
   *  mapping<pointset<2, int> >::iterator it, end;
   *  std::tie(it, end) = mapping_range(0, my_points);
   *  for(; it != end; ++it)
   *  {
   *    // ...
   *  }
   *  \endcode
   *  Notice that an \c iterator type is declared, and not an \c iterator_pair
   *  type.
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
   *  \fractime
   */
  template <typename Container>
  inline mapping_iterator_pair<const Container>
  mapping_range(const Container& container, dimension_type mapping_dim)
  {
    return mapping_iterator_pair<const Container>
      (mapping_begin(container, mapping_dim),
       mapping_end(container, mapping_dim));
  }

  template <typename Container>
  inline mapping_iterator_pair<const Container>
  mapping_crange(const Container& container, dimension_type mapping_dim)
  {
    return mapping_iterator_pair<const Container>
      (mapping_begin(container, mapping_dim),
       mapping_end(container, mapping_dim));
  }
  ///@}

  /**
   *  Finds the value with the smallest coordinate along the mapping dimension
   *  that is greater or equal to \c bound, and return an iterator pointing to
   *  this value.
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
   *  \param bound The lowest bound to the iterator position.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *  \return An iterator pointing to the value with the smallest coordinate
   *  greater or equal to \c bound along \c mapping_dim.
   *
   *  \fractime
   */
  ///@{
  template <typename Container>
  inline mapping_iterator<Container>
  mapping_lower_bound(Container& container, dimension_type mapping_dim,
                      const typename container_traits<Container>::key_type&
                      bound)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    dimension_type dim;
    typename mapping_iterator<Container>::node_ptr node;
    details::assign(dim, node,
                    lower_bound_mapping(container.end().node->parent, 0,
                                        container.rank(), mapping_dim,
                                        container.key_comp(), bound));
    return mapping_iterator<Container>(container, mapping_dim, dim, node);
  }

  template <typename Container>
  inline mapping_iterator<const Container>
  mapping_clower_bound
  (const Container& container, dimension_type mapping_dim,
   const typename container_traits<Container>::key_type& bound)
  { return mapping_lower_bound(container, mapping_dim, bound); }
  ///@}

  /**
   *  Finds the value with the largest coordinate along the mapping dimension
   *  that is stricly less than \c bound, and return an iterator pointing to
   *  this value.
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
   *  \param bound The lowest bound to the iterator position.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *  \return An iterator pointing to the value with the smallest coordinate
   *  greater or equal to \c bound along \c mapping_dim.
   *
   *  \fractime
   */
  ///@{
  template <typename Container>
  inline mapping_iterator<Container>
  mapping_upper_bound
  (Container& container, dimension_type mapping_dim,
   const typename container_traits<Container>::key_type& bound)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    dimension_type dim;
    typename mapping_iterator<Container>::node_ptr node;
    details::assign(dim, node,
                    upper_bound_mapping(container.end().node->parent, 0,
                                        container.rank(), mapping_dim,
                                        container.key_comp(), bound));
    return mapping_iterator<Container>(container, mapping_dim, dim, node);
  }

  template <typename Container>
  inline mapping_iterator<const Container>
  mapping_cupper_bound
  (const Container& container, dimension_type mapping_dim,
   const typename container_traits<Container>::key_type& bound)
  { return mapping_upper_bound(container, mapping_dim, bound); }
  ///@}

  namespace details
  {
    /**
     *  Move the iterator given in parameter to the value with the smallest
     *  coordinate greater or equal to \c bound along the mapping dimension of
     *  \c iter, but only in the sub-tree composed of the node pointed to by the
     *  iterator and its children. If no such value exists, then move the
     *  iterator to the parent of the value currently pointed to.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  mapping_begin(). In any case, use it cautiously, as this function does
     *  not perform any sanity checks on the iterator given in parameter.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter An iterator that points to the root node of the search.
     *  \param bound The lower bound to the iterator position.
     *  \return An iterator pointing to the value with the smallest coordinate
     *  greater or equal to \c bound along \c iter's \c mapping_dim, or to the
     *  parent of the value pointed to.
     *
     *  \fractime
     */
    ///@{
    template <typename NodePtr, typename Rank, typename KeyCompare,
              typename KeyType>
    inline std::pair<NodePtr, dimension_type>
    lower_bound_mapping(NodePtr node, dimension_type dim, Rank rank,
                        dimension_type map, KeyCompare key_comp,
                        const KeyType& bound, relaxed_invariant_tag)
    {
      SPATIAL_ASSERT_CHECK(map < rank());
      SPATIAL_ASSERT_CHECK(node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(node));
      while (node->left != 0
             && (dim != map // optimize for strict invariant
                 || !key_comp(map, const_key(node), const_key(bound))))
        {
          node = node->left;
          dim = incr_dim(rank, dim);
        }
      NodePtr best = 0;
      dimension_type best_dim;
      if (!key_comp(map, const_key(node), const_key(bound)))
        {
          best = node;
          best_dim = dim;
        }
      for (;;)
        {
          if (node->right != 0 && (dim != map || best == 0))
            {
              node = node->right;
              dim = incr_dim(rank, dim);
              while (node->left != 0
                     && (dim != map // optimize for strict invariant
                         || !key_comp(map, const_key(node), const_key(bound))))
                {
                  node = node->left;
                  dim = incr_dim(rank, dim);
                }
            }
          else
            {
              NodePtr prev_node = node;
              node = node->parent;
              dim = decr_dim(rank, dim);
              while (!header(node)
                     && prev_node == node->right)
                {
                  prev_node = node;
                  node = node->parent;
                  dim = decr_dim(rank, dim);
                }
              if (header(node)) break;
            }
          if (!key_comp(map, const_key(node), const_key(bound))
              && (best == 0 || key_comp(map, const_key(node), const_key(best))))
            {
              best = node;
              best_dim = dim;
            }
        }
      SPATIAL_ASSERT_CHECK(node_dim == rank() - 1);
      SPATIAL_ASSERT_CHECK(best != node);
      SPATIAL_ASSERT_CHECK(header(node));
      if (best == 0)
        {
          best = node;
          best_dim = node_dim;
        }
      return std::make_pair(best, best_dim);
    }

    template <typename NodePtr, typename Rank, typename KeyCompare,
              typename KeyType>
    inline std::pair<NodePtr, dimension_type>
    lower_bound_mapping(NodePtr node, dimension_type dim, Rank rank,
                        dimension_type map, KeyCompare key_comp,
                        const KeyType& bound, strict_invariant_tag)
    {
      SPATIAL_ASSERT_CHECK(map < rank());
      SPATIAL_ASSERT_CHECK(node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(node));
      while (node->left != 0
             && (dim != map
                 || key_comp(map, const_key(bound), const_key(node))))
        {
          node = node->left;
          dim = incr_dim(rank, dim);
        }
      NodePtr best = 0;
      dimension_type best_dim;
      if (!key_comp(map, const_key(node), const_key(bound)))
        {
          best = node;
          best_dim = dim;
        }
      for (;;)
        {
          if (node->right != 0 && (dim != map || best == 0))
            {
              node = node->right;
              dim = incr_dim(rank, dim);
              while (node->left != 0
                     && (dim != map
                         || key_comp(map, const_key(bound), const_key(node))))
                {
                  node = node->left;
                  dim = incr_dim(rank, dim);
                }
            }
          else
            {
              NodePtr prev_node = node;
              node = node->parent;
              dim = decr_dim(rank, dim);
              while (!header(node)
                     && prev_node == node->right)
                {
                  prev_node = node;
                  node = node->parent;
                  dim = decr_dim(rank, dim);
                }
              if (header(node)) break;
            }
          if (!key_comp(map, const_key(node), const_key(bound))
              && (best == 0 || key_comp(map, const_key(node), const_key(best))))
            {
              best = node;
              best_dim = dim;
            }
        }
      SPATIAL_ASSERT_CHECK(node_dim == rank() - 1);
      SPATIAL_ASSERT_CHECK(best != node);
      SPATIAL_ASSERT_CHECK(header(node));
      if (best == 0)
        {
          best = node;
          best_dim = node_dim;
        }
      return std::make_pair(best, best_dim);
    }
    ///@}

    /**
     *  Move the iterator given in parameter to the value with the largest
     *  coordinate strictly lower than \c bound along the mapping dimension of
     *  \c iter, but only in the sub-tree composed of the node pointed to by the
     *  iterator and its children. If no such value exists, then move the
     *  iterator to the parent of the value currently pointed to.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  mapping_begin(). In any case, use it cautiously, as this function does
     *  not perform any sanity checks on the iterator given in parameter.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter An iterator that points to the root node of the search.
     *  \param bound The upper bound to the iterator position.
     *  \return \c iter moved to the value with the largest coordinate strictly
     *  less than \c bound along \c iter's \c mapping_dim, or to the
     *  parent of the value pointed to.
     *
     *  \fractime
     */
    template <typename NodePtr, typename Rank, typename KeyCompare,
              typename KeyType>
    inline std::pair<NodePtr, dimension_type>
    upper_bound_mapping
    (NodePtr node, dimension_type dim, Rank rank, dimension_type map,
     KeyCompare key_comp, const KeyType& bound)
    {
      SPATIAL_ASSERT_CHECK(map < rank());
      SPATIAL_ASSERT_CHECK(node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(node));
      while (node->left != 0
             && (dim != map
                 || key_comp(map, const_key(bound), const_key(node))))
        {
          node = node->left;
          dim = incr_dim(rank, dim);
        }
      NodePtr best = 0;
      dimension_type best_dim;
      if (key_comp(map, const_key(bound), const_key(node)))
        {
          best = node;
          best_dim = dim;
        }
      for (;;)
        {
          if (node->right != 0 && (dim != map || best == 0))
            {
              node = node->right;
              dim = incr_dim(rank, dim);
              while (node->left != 0
                     && (dim != map
                         || key_comp(map, const_key(bound), const_key(node))))
                {
                  node = node->left;
                  dim = incr_dim(rank, dim);
                }
            }
          else
            {
              NodePtr prev_node = node;
              node = node->parent;
              dim = decr_dim(rank, dim);
              while (!header(node)
                     && prev_node == node->right)
                {
                  prev_node = node;
                  node = node->parent;
                  dim = decr_dim(rank, dim);
                }
              if (header(node)) break;
            }
          if (key_comp(map, const_key(bound), const_key(node))
              && (best == 0 || key_comp(map, const_key(node), const_key(best))))
            {
              best = node;
              best_dim = dim;
            }
        }
      SPATIAL_ASSERT_CHECK(node_dim == rank() - 1);
      SPATIAL_ASSERT_CHECK(best != node);
      SPATIAL_ASSERT_CHECK(header(node));
      if (best == 0)
        {
          best = node;
          best_dim = node_dim;
        }
      return std::make_pair(best, best_dim);
    }

  } // namespace details
}

#endif // SPATIAL_MAPPING_ITERATOR_HPP
