// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_region.hpp
 *  @brief  Contains the definition of the region iterators. These iterators
 *  walk through all items in the container that are contained within a region
 *  defined by a predicate. The predicate works on orthogonal ranges.
 */

#ifndef SPATIAL_REGION_HPP
#define SPATIAL_REGION_HPP

#include "spatial_bidirectional.hpp"
#include "spatial_traits.hpp"
#include "spatial_rank.hpp"
#include "spatial_except.hpp"

namespace spatial
{
  /**
   *  @brief  For all @e{x} in the set @e{S}, we define @e{lower} and @e{upper}
   *  such that orthogonal region iterates over all the @e{x} that satify: for
   *  all dimensions {0, ..., k-1 | loweri <= xi && higheri >= xi}. In other
   *  words, iterates over all the region of elements in @{S} that are within the
   *  interval {lower, upper} with respect to all dimensions.
   *
   *  @concept bounds is a model of RegionPredicate.
   */
  template <typename Key, typename Compare>
  class bounds
    : private Compare // empty member optimization
  {
  public:
    /**
     *  @brief  The default constructor leaves everything un-initialized
     */
    bounds()
      : Compare(), _lower(), _upper() { }

    /**
     *  @brief  Set the lower and upper boundary for the orthogonal region
     *  search.
     */
    bounds(const Compare& compare, const Key& lower, const Key& upper)
      : Compare(compare), _lower(lower), _upper(upper)
    { }

    /**
     *  @brief  The operator that tells wheather the point is in region or not.
     */
    relative_order
    operator()(dimension_type dim, dimension_type, const Key& key) const
    {
      return (Compare::operator()(dim, key, _lower)
              ? below
              : (Compare::operator()(dim, key, _upper)
                 ? matching
                 : above));
    }

  private:
    /**
     *  @brief  The lower bound for the orthogonal region iterator.
     */
    Key _lower;

    /**
     *  @brief  The upper bound for the orthogonal region iterator.
     */
    Key _upper;
  };

  /**
   *  @brief  Region bounds factory that takes in a @c container, a @c lower and
   *  @c upper key region, returning an @c open_bounds<Key, Compare> type.  @see
   *  equal_bounds<Key, Compare>
   *
   *  This factory also check that lower and upper satisfy region requirements
   *  over an regular interval for each dimension, e.g: @c
   *  container.key_comp()(d, lower, upper) must be true for all value of @c d
   *  within 0 and @c container.dimension() or container.key_comp()(d, upper,
   *  lower) must also be false (which mean the values are equal on that
   *  particular dimension).
   */
  template <typename Tp>
  bounds<typename container_traits<Tp>::key_type,
         typename container_traits<Tp>::key_compare>
  make_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& lower,
   const typename container_traits<Tp>::key_type& upper)
  {
    ::spatial::except::check_bounds(container, lower, upper);
    return bounds
      <typename container_traits<Tp>::key_type,
      typename container_traits<Tp>::key_compare>
      (container.key_comp(), lower, upper);
  }

  /**
   *  @brief  For all @c x in the set @c S, we define @c y such that the
   *  orthogonal region iterates over all the @e{x} that satify: for all
   *  dimensions {0, ..., k-1 | yi <= xi && yi >= xi}. In other words, iterates
   *  orthogonally over all the region of elements in @c S that are equal to y
   *  with respect to all dimensions.
   *
   *  @concept equal_bounds is a model of RegionPredicate.
   */
  template <typename Key, typename Compare>
  class equal_bounds
    : private Compare // empty member optimization
  {
  public:
    /**
     *  @brief  The default constructor leaves everything un-initialized
     */
    equal_bounds()
      : Compare(), _match() { }

    /**
     *  @brief  Set the key for the boundaries.
     */
    equal_bounds(const Compare &compare, const Key& match)
      : Compare(compare), _match(match)
    { }

    /**
     *  @brief  The operator that tells wheather the key is in region or not.
     */
    relative_order
    operator()(dimension_type dim, dimension_type, const Key& key) const
    {
      return (Compare::operator()(dim, key, _match)
              ? below
              : (Compare::operator()(dim, _match, key)
                 ? above
                 : matching));
    }

    /**
     *  @brief  The unique element that defines both lower and upper region for
     *  the equal region iterator.
     */
  private:
    Key _match;
  };

  /**
   *  @brief  Equal bound factory that takes in a @c container and a @c key,
   *  returning an @c equal_bounds<Key, Compare> type.
   *  @see equal_bounds<Key, Compare>
   */
  template <typename Tp>
  equal_bounds<typename container_traits<Tp>::key_type,
               typename container_traits<Tp>::key_compare>
  make_equal_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& key)
  {
    return equal_bounds
      <typename container_traits<Tp>::key_type,
      typename container_traits<Tp>::key_compare>
      (container.key_comp(), key);
  }

  /**
   *  This type provides both an iterator and a constant iterator to iterate
   *  through all elements of a tree that match an orthogonal region defined by
   *  a predicate. If no predicate is provided, the orthogonal region search
   *  default to a \ref bounds predicate, which matches all points
   *  contained within an orthogonal region of space formed by 2 points,
   *  inclusive of lower values, but exclusive of upper values.
   *
   *  \tparam Ct The container upon which these iterator relate to.
   *  \tparam Predicate A model of \ref RegionPredicate, defaults to \ref
   *  bounds
   *  \see region_query<>::iterator
   *  \see region_query<>::const_iterator
   */
  template <typename Ct, typename Predicate
            = bounds<typename container_traits<Ct>::key_type,
                     typename container_traits<Ct>::key_compare> >
  class region_iterator
    : public details::Bidirectional_iterator
      <typename container_traits<Ct>::mode_type,
       typename container_traits<Ct>::rank_type>
  {
  private:
    typedef typename details::Bidirectional_iterator
    <typename container_traits<Ct>::mode_type,
     typename container_traits<Ct>::rank_type> Base;

  public:
    //! Uninitialized iterator.
    region_iterator() { }

    /**
     *  Build a region iterator from a container's iterator type.
     *
     *  This constructor should be used in the general case where the
     *  dimension for the node pointed to by \c iter is not known. The
     *  dimension of the node will be recomputed from the given iterator by
     *  iterating through all parents until the header node has been
     *  reached. This iteration is bounded by \Olog in case the tree is
     *  perfectly balanced.
     *
     *  \param container The container being iterated.
     *  \param pred A model of the \ref RegionPredicate concept.
     *  \param iter An iterator on the type Ct.
     */
    region_iterator(Ct& container, const Predicate& pred,
                    typename container_traits<Ct>::iterator iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        _pred(pred) { }

    /**
     *  Build a region iterator from the node and current dimension of a
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
     *  \param pred A model of the \ref RegionPredicate concept.
     *  \param ptr An iterator on the type Ct.
     *  \param dim The node's dimension for the node pointed to by node.
     *  \param container The container being iterated.
     */
    region_iterator
    (Ct& container, const Predicate& pred, dimension_type dim,
     typename container_traits<Ct>::mode_type::node_ptr ptr)
      : Base(container.rank(), ptr, dim), _pred(pred) { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    region_iterator<Ct, Predicate>& operator++()
    { return increment_region(*this); }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    region_iterator<Ct, Predicate> operator++(int)
    {
      region_iterator<Ct, Predicate> x(*this);
      increment_region(*this);
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    region_iterator<Ct, Predicate>& operator--()
    { return decrement_region(*this); }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    region_iterator<Ct, Predicate> operator--(int)
    {
      region_iterator<Ct, Predicate> x(*this);
      decrement_region(*this);
      return x;
    }

    //! Return the key_comparator used by the iterator
    Predicate predicate() const { return _pred; }

  private:
    //! The related data for the iterator.
    Predicate _pred;
  };

  /**
   *  This type provides both an iterator and a constant iterator to iterate
   *  through all elements of a tree that match an orthogonal region defined by
   *  a predicate. If no predicate is provided, the orthogonal region search
   *  default to a \ref bounds predicate, which matches all points
   *  contained within an orthogonal region of space formed by 2 points,
   *  inclusive of lower values, but exclusive of upper values.
   *
   *  \tparam Ct The container upon which these iterator relate to.
   *  \tparam Predicate A model of \ref RegionPredicate, defaults to \ref
   *  bounds
   *  \see region_query<>::iterator
   *  \see region_query<>::const_iterator
   */
  template <typename Ct, typename Predicate>
  class region_iterator<const Ct, Predicate>
    : public details::Const_bidirectional_iterator
      <typename container_traits<Ct>::mode_type,
       typename container_traits<Ct>::rank_type>
  {
  private:
    typedef details::Const_bidirectional_iterator
    <typename container_traits<Ct>::mode_type,
     typename container_traits<Ct>::rank_type> Base;

  public:
    //! Uninitialized iterator.
    region_iterator() { }

    /**
     *  Build a region iterator from a container's iterator type.
     *
     *  This constructor should be used in the general case where the
     *  dimension for the node pointed to by \c iter is not known. The
     *  dimension of the node will be recomputed from the given iterator by
     *  iterating through all parents until the header node has been
     *  reached. This iteration is bounded by \Olog in case the tree is
     *  perfectly balanced.
     *
     *  \param container The container being iterated.
     *  \param predicate A model of the \ref RegionPredicate concept.
     *  \param iter An iterator on the type Ct.
     */
    region_iterator(const Ct& container, const Predicate& pred,
                    typename container_traits<Ct>::const_iterator iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        _pred(pred) { }

    /**
     *  Build a region iterator from the node and current dimension of a
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
     *  \param predicate A model of the \ref RegionPredicate concept.
     *  \param iter An iterator on the type Ct.
     */
    region_iterator
    (const Ct& container, const Predicate& pred, dimension_type dim,
     typename container_traits<Ct>::mode_type::const_node_ptr ptr)
      : Base(container.rank(), ptr, dim), _pred(pred) { }

    //! Convertion of an iterator into a const_iterator is permitted.
    region_iterator(const region_iterator<Ct, Predicate>& iter)
      : Base(iter.rank(), iter.node, iter.node_dim), _pred(iter.predicate()) { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    region_iterator<const Ct, Predicate>& operator++()
    { return increment_region(*this); }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    region_iterator<const Ct, Predicate> operator++(int)
    {
      region_iterator<const Ct, Predicate> x(*this);
      increment_region(*this);
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    region_iterator<const Ct, Predicate>& operator--()
    { return decrement_region(*this); }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    region_iterator<const Ct, Predicate> operator--(int)
    {
      region_iterator<const Ct, Predicate> x(*this);
      decrement_region(*this);
      return x;
    }

    //! Return the key_comparator used by the iterator
    Predicate predicate() const { return _pred; }

  private:
    //! The related data for the iterator.
    Predicate _pred;
  };

  namespace details
  {
    template <typename Ct, typename Predicate>
    region_iterator<Ct, Predicate>&
    increment_region(region_iterator<Ct, Predicate>& iter);

    template <typename Ct, typename Predicate>
    region_iterator<Ct, Predicate>&
    decrement_region(region_iterator<Ct, Predicate>& iter);

    /**
     *  From \c x, find the node with the minimum value in the region
     *  delimited by p. If multiple nodes are matching, return the first
     *  matching node in in-order transversal.
     *
     *  \param node_dim  The current dimension for @c node.
     *  \param node  The node from which to find the minimum.
     *  \param key_dimension  The number of dimensions of key.
     *  \param predicate  The predicate for the orthogonal region query.
     *  \return  An iterator pointing the minimum, or to the parent of @c node.
     *
     *  If \c node is a header node, the search will stop immediately.
     */
    template <typename Ct, typename Predicate>
    region_iterator<Ct, Predicate>&
    minimum_region(region_iterator<Ct, Predicate>& iter);

    /**
     *  From \c x, find the node with the maximum value in the region
     *  delimited by p. If multiple nodes are matching, return the last
     *  matching node in in-order transversal.
     *
     *  \param node_dim  The current dimension for @c node.
     *  \param node  The node from which to find the minimum.
     *  \param key_dimension  The number of dimensions of key.
     *  \param predicate  The predicate for the orthogonal region query.
     *  \return  An iterator pointing the maximum, or to the parent of @c node.
     *
     *  If \c node is a header node, the search will stop immediately.
     */
    template <typename Ct, typename Predicate>
    region_iterator<Ct, Predicate>&
    maximum_region(region_iterator<Ct, Predicate>& iter);

  } // namespace details

  template <typename Ct, typename Predicate>
  inline region_iterator<Ct, Predicate>
  region_end(Ct& container, const Predicate& pred)
  {
    return region_iterator<Ct, Predicate>
      (container, pred, container.dimension() - 1,
       container.end().node); // At header, dim = rank - 1
  }

  template <typename Ct, typename Predicate>
  inline region_iterator<const Ct, Predicate>
  region_end(const Ct& container, const Predicate& pred)
  {
    return region_iterator<const Ct, Predicate>
      (container, pred, container.dimension() - 1,
       container.end().node); // At header, dim = rank - 1
  }

  template <typename Ct, typename Predicate>
  inline region_iterator<const Ct, Predicate>
  region_cend(const Ct& container, const Predicate& pred)
  { return region_end(container, pred); }

  template <typename Ct, typename Predicate>
  inline region_iterator<Ct, Predicate>
  region_begin(Ct& container, const Predicate& pred)
  {
    if (container.empty()) return region_end(container, pred);
    region_iterator<Ct, Predicate>
      it(container, pred, 0, container.end().node->parent); // At root, dim = 0
    return details::minimum_region(it);
  }

  template <typename Ct, typename Predicate>
  inline region_iterator<const Ct, Predicate>
  region_begin(const Ct& container, const Predicate& pred)
  {
    if (container.empty()) return region_end(container, pred);
    region_iterator<const Ct, Predicate>
      it(container, pred, 0, container.end().node->parent); // At root, dim = 0
    return details::minimum_region(it);
  }

  template <typename Ct, typename Predicate>
  inline region_iterator<const Ct, Predicate>
  region_cbegin(const Ct& container, const Predicate& pred)
  { return region_begin(container, pred); }

  template<typename Ct>
  struct equal_iterator
    : region_iterator
  <Ct, equal_bounds<typename container_traits<Ct>::key_type,
                    typename container_traits<Ct>::key_compare> >
  {
    equal_iterator() { }
    equal_iterator
    (const region_iterator
     <Ct, equal_bounds<typename container_traits<Ct>::key_type,
     typename container_traits<Ct>::key_compare> >&
     other)
      : region_iterator
        <Ct, equal_bounds<typename container_traits<Ct>::key_type,
                          typename container_traits<Ct>::key_compare> >
        (other) { }
  };
  template<typename Ct>
  struct equal_iterator<const Ct>
    : region_iterator
  <const Ct, equal_bounds<typename container_traits<Ct>::key_type,
                          typename container_traits<Ct>::key_compare> >
  {
    equal_iterator() { }
    equal_iterator
    (const region_iterator
     <const Ct, equal_bounds<typename container_traits<Ct>::key_type,
     typename container_traits<Ct>::key_compare> >&
     other)
      : region_iterator
        <const Ct, equal_bounds<typename container_traits<Ct>::key_type,
                                typename container_traits<Ct>::key_compare> >
        (other) { }
    equal_iterator
    (const region_iterator
     <Ct, equal_bounds<typename container_traits<Ct>::key_type,
     typename container_traits<Ct>::key_compare> >&
     other)
      : region_iterator
        <const Ct, equal_bounds<typename container_traits<Ct>::key_type,
                                typename container_traits<Ct>::key_compare> >
        (other) { }
  };

  template <typename Ct>
  inline equal_iterator<Ct>
  equal_begin(Ct& container,
    const typename container_traits<Ct>::key_type& match)
  { return region_begin(container, make_equal_bounds(container, match)); }
  template <typename Ct>
  inline equal_iterator<const Ct>
  equal_begin(const Ct& container,
    const typename container_traits<Ct>::key_type& match)
  { return region_begin(container, make_equal_bounds(container, match)); }
  template <typename Ct>
  inline equal_iterator<const Ct>
  equal_cbegin(const Ct& container,
    const typename container_traits<Ct>::key_type& match)
  { return region_cbegin(container, make_equal_bounds(container, match)); }

  template <typename Ct>
  inline equal_iterator<Ct>
  equal_end(Ct& container,
            const typename container_traits<Ct>::key_type& match)
  { return region_end(container, make_equal_bounds(container, match)); }
  template <typename Ct>
  inline equal_iterator<const Ct>
  equal_end(const Ct& container,
            const typename container_traits<Ct>::key_type& match)
  { return region_end(container, make_equal_bounds(container, match)); }
  template <typename Ct>
  inline equal_iterator<const Ct>
  equal_cend(const Ct& container,
             const typename container_traits<Ct>::key_type& match)
  { return region_cend(container, make_equal_bounds(container, match)); }

  namespace details
  {
    template <typename Container, typename Predicate>
    inline region_iterator<Container, Predicate>&
    increment_region(region_iterator<Container, Predicate>& iter)
    {
      const typename container_traits<Container>::rank_type rank(iter.rank());
      const Predicate pred(iter.predicate());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      do
        {
          if (iter.node->right != 0
              && pred(iter.node_dim, rank(), const_key(iter.node)) != above)
            {
              iter.node = iter.node->right;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->left != 0
                     && pred(iter.node_dim, rank(),
                             const_key(iter.node)) != below)
                {
                  iter.node = iter.node->left;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
            }
          else
            {
              typename region_iterator<Container, Predicate>::node_ptr p
                = iter.node->parent;
              while (!header(p) && iter.node == p->right)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
            }
        }
      while (!header(iter.node)
             && match_all(rank, const_key(iter.node), pred) == false);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container, typename Predicate>
    inline region_iterator<Container, Predicate>&
    decrement_region(region_iterator<Container, Predicate>& iter)
    {
      const typename container_traits<Container>::rank_type rank(iter.rank());
      const Predicate pred(iter.predicate());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      if (header(iter.node))
        {
          iter.node = iter.node->parent;
          iter.node_dim = 0; // root is always compared on dimension 0
          return maximum_region(iter);
        }
      do
        {
          if (iter.node->left != 0
              && pred(iter.node_dim, rank(), const_key(iter.node)) != below)
            {
              iter.node = iter.node->left;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->right != 0
                     && pred(iter.node_dim, rank(),
                             const_key(iter.node)) != above)
                {
                  iter.node = iter.node->right;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
            }
          else
            {
              typename region_iterator<Container, Predicate>::node_ptr p
                = iter.node->parent;
              while (!header(p) && iter.node == p->left)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
            }
        }
      while (!header(iter.node)
             && match_all(rank, const_key(iter.node), pred) == false);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container, typename Predicate>
    inline region_iterator<Container, Predicate>&
    minimum_region(region_iterator<Container, Predicate>& iter)
    {
      const typename container_traits<Container>::rank_type rank(iter.rank());
      const Predicate pred(iter.predicate());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      typename region_iterator<Container, Predicate>::node_ptr end
        = iter.node->parent;
      // Quick positioning according to in-order transversal.
      while(iter.node->right != 0
            && pred(iter.node_dim, rank(), const_key(iter.node)) == below)
        {
          iter.node = iter.node->right;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      while(iter.node->left != 0
            && pred(iter.node_dim, rank(), const_key(iter.node)) != below)
        {
          iter.node = iter.node->left;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      // Start algorithm.
      do
        {
          if (match_all(rank, const_key(iter.node), pred) == true) { break; }
          if (iter.node->right != 0
              && pred(iter.node_dim, rank(),
                      const_key(iter.node)) != above)
            {
              iter.node = iter.node->right;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->left != 0
                     && pred(iter.node_dim, rank(),
                             const_key(iter.node)) != below)
                {
                  iter.node = iter.node->left;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
            }
          else
            {
              typename region_iterator<Container, Predicate>::node_ptr p
                = iter.node->parent;
              while (p != end && iter.node == p->right)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
            }
        }
      while (iter.node != end);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container, typename Predicate>
    inline region_iterator<Container, Predicate>&
    maximum_region(region_iterator<Container, Predicate>& iter)
    {
      const typename container_traits<Container>::rank_type rank(iter.rank());
      const Predicate pred(iter.predicate());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      typename region_iterator<Container, Predicate>::node_ptr end
        = iter.node->parent;
      // Quick positioning according to in-order transversal.
      while (iter.node->left != 0
             && pred(iter.node_dim, rank(), const_key(iter.node)) == above)
        {
          iter.node = iter.node->left;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      while (iter.node->right != 0
             && pred(iter.node_dim, rank(), const_key(iter.node)) != above)
        {
          iter.node = iter.node->right;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      // Start algorithm.
      do
        {
          if (match_all(rank, key(iter.node), pred) == true) { break; }
          if (iter.node->left != 0
              && pred(iter.node_dim, rank(), const_key(iter.node)) != below)
            {
              iter.node = iter.node->left;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->right != 0
                     && pred(iter.node_dim, rank(),
                             const_key(iter.node)) != above)
                {
                  iter.node = iter.node->right;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
            }
          else
            {
              typename region_iterator<Container, Predicate>::node_ptr p
                = iter.node->parent;
              while (p != end && iter.node == p->left)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
            }
        }
      while (iter.node != end);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_REGION_HPP
