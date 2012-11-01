// -*- C++ -*-

/**
 *  @file   spatial_region.hpp
 *  @brief
 *
 *  Change Log:
 *
 *  - 2009-10-19 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_REGION_HPP
#define SPATIAL_REGION_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#include <utility> // provides ::std::pair<> and ::std::make_pair()

namespace spatial
{

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
  struct equal_bounds
    : private Compare // empty member optimization
  {
    /**
     *  @brief  The default constructor leaves everything un-initialized
     */
    equal_bounds()
      : Compare(), match_() { }

    /**
     *  @brief  Set the key for the boundaries.
     */
    equal_bounds(const Compare &compare, const Key& match)
      : Compare(compare), match_(match)
    { }

    /**
     *  @brief  The operator that tells wheather the key is in region or not.
     */
    relative_order
    operator()(dimension_type dim, dimension_type, const Key& key) const
    {
      return (Compare::operator()(dim, key, match_)
              ? below
              : (Compare::operator()(dim, match_, key)
                 ? above
                 : matching));
    }

    /**
     *  @brief  The unique element that defines both lower and upper region for
     *  the equal region iterator.
     */
  private:
    Key match_;
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
   *  @brief  For all @e{x} in the set @e{S}, we define @e{lower} and @e{upper}
   *  such that orthogonal region iterates over all the @e{x} that satify: for
   *  all dimensions {0, ..., k-1 | loweri <= xi && higheri >= xi}. In other
   *  words, iterates over all the region of elements in @{S} that are within
   *  the interval {lower, upper} with respect to all dimensions.
   *
   *  @concept open_bounds is a model of RegionPredicate.
   */
  template <typename Key, typename Compare>
  struct open_bounds
    : private Compare // empty member optimization
  {
    /**
     *  @brief  The default constructor leaves everything un-initialized
     */
    open_bounds()
      : Compare(), lower_(), upper_() { }

    /**
     *  @brief  Set the lower and upper boundary for the orthogonal range
     *  search in the region.
     *  @see make_open_bounds
     *
     *  The constructor does not check the lower and upper satisfy the following
     *  requierment: @c compare(d, lower, upper) must return true for all @c d
     *  for the keys @c lower and @c upper.
     */
    open_bounds(const Compare& compare, const Key& lower,
                      const Key& upper)
      : Compare(compare), lower_(lower), upper_(upper)
    { }

    /**
     *  @brief  The operator that tells wheather the point is in region or not.
     */
    relative_order
    operator()(dimension_type dim, dimension_type, const Key& key) const
    {
      return (!Compare::operator()(dim, lower_, key)
              ? below
              : (Compare::operator()(dim, key, upper_)
                 ? matching
                 : above));
    }

  private:
    /**
     *  @brief  The lower bound for the orthogonal region iterator.
     */
    Key lower_;

    /**
     *  @brief  The upper bound for the orthogonal region iterator.
     */
    Key upper_;
  };

  /**
   *  @brief  Open bound factory that takes in a @c container, a @c lower and @c
   *  upper key region, returning an <tt>open_bounds<Key, Compare></tt> type.
   *  @see equal_bounds<Key, Compare>
   *
   *  This factory also checks that lower and upper satisfy region requirements
   *  over an open interval for each dimension, e.g: <tt>compare.key_comp()(d,
   *  lower, upper)</tt> must be true for all value of @c d within 0 and @c
   *  container.dimension().
   */
  template <typename Tp>
  open_bounds<typename container_traits<Tp>::key_type,
                    typename container_traits<Tp>::key_compare>
  make_open_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& lower,
   const typename container_traits<Tp>::key_type& upper)
  {
    ::spatial::except::check_open_bounds(container, lower, upper);
    return open_bounds
      <typename container_traits<Tp>::key_type,
      typename container_traits<Tp>::key_compare>
      (container.key_comp(), lower, upper);
  }

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
  struct bounds
    : private Compare // empty member optimization
  {
    /**
     *  @brief  The default constructor leaves everything un-initialized
     */
    bounds()
      : Compare(), lower_(), upper_() { }

    /**
     *  @brief  Set the lower and upper boundary for the orthogonal region
     *  search.
     */
    bounds(const Compare& compare, const Key& lower, const Key& upper)
      : Compare(compare), lower_(lower), upper_(upper)
    { }

    /**
     *  @brief  The operator that tells wheather the point is in region or not.
     */
    relative_order
    operator()(dimension_type dim, dimension_type, const Key& key) const
    {
      return (Compare::operator()(dim, key, lower_)
              ? below
              : (Compare::operator()(dim, key, upper_)
                 ? matching
                 : above));
    }

  private:
    /**
     *  @brief  The lower bound for the orthogonal region iterator.
     */
    Key lower_;

    /**
     *  @brief  The upper bound for the orthogonal region iterator.
     */
    Key upper_;
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
   *  @brief  For all @e{x} in the set @e{S}, we define @e{lower} and @e{upper}
   *  such that orthogonal region iterates over all the @e{x} that satify: for
   *  all dimensions {0, ..., k-1 | loweri <= xi && higheri >= xi}. In other
   *  words, iterates over all the region of elements in @{S} that are within the
   *  interval {lower, upper} with respect to all dimensions.
   *
   *  @concept closed_bounds is a model of RegionPredicate.
   */
  template <typename Key, typename Compare>
  struct closed_bounds
    : private Compare // empty member optimization
  {
    /**
     *  @brief  The default constructor leaves everything un-initialized
     */
    closed_bounds() : Compare(), lower_(), upper_() { }

    /**
     *  @brief  Set the lower and upper boundary for the orthogonal region
     *  search.
     *
     *  @throws a std::out_of_region if an element of lower is strictly greater
     *  than the corresponding element of upper with respect to their dimension.
     */
    closed_bounds(const Compare& compare, const Key& lower,
                        const Key& upper)
      : Compare(compare), lower_(lower), upper_(upper)
    { }

    /**
     *  @brief  The operator that tells wheather the point is in region or not.
     */
    relative_order
    operator()(dimension_type dim, dimension_type, const Key& key) const
    {
      return (Compare::operator()(dim, key, lower_)
              ? below
              : (Compare::operator()(dim, upper_, key)
                 ? above
                 : matching));
    }

  private:
    /**
     *  @brief  The lower bound for the orthogonal region iterator.
     */
    Key lower_;

    /**
     *  @brief  The upper bound for the orthogonal region iterator.
     */
    Key upper_;
  };

  /**
   *  @brief  Closed bounds factory that takes in a @c container, a @c lower and
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
  closed_bounds<typename container_traits<Tp>::key_type,
                typename container_traits<Tp>::key_compare>
  make_closed_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& lower,
   const typename container_traits<Tp>::key_type& upper)
  {
    ::spatial::except::check_closed_bounds(container, lower, upper);
    return closed_bounds
      <typename container_traits<Tp>::key_type,
      typename container_traits<Tp>::key_compare>
      (container.key_comp(), lower, upper);
  }

  /**
   *  This region predicate matches keys that are overlap with a target
   *  box. The keys must represent boxes, not points.
   *
   *  @concept overlap_bounds is a model of RegionPredicate.
   *
   *  The Compare functor is expected to be a model of \s RegularComparison.
   *
   *  In order to interpret the box coordinates appropriately, overlap_bounds
   *  expects a Layout template argument. Layout is one of:
   *  @ul @ref llhh_layout_tag,
   *  @l @ref lhlh_layout_tag,
   *  @l @ref hhll_layout_tag,
   *  @l @ref hlhl_layout_tag.
   *  @lu
   *  Each layout provides information on how to interpret the coordinates
   *  returned for each dimension of the boxes values.
   *
   *  For a given target box @f$P_{(x, y)}@f$, this region predicate matches any
   *  box @f$B_{(x, y)}@f$ in a space of rank @f$r@f$ such as:
   *
   *  @f[
   *  \sum_{i=1}^{r} \left( B_{x_i} \le P_{x_i} \le B_{y_i}
   *                        \; or \; B_{x_i} \le P_{y_i} \le B_{y_i} \right)
   *  @f]
   */
  template <typename Key, typename Compare,
            typename Layout = llhh_layout_tag>
  struct overlap_bounds
    : private Compare
  {
    /**
     *  @brief  The default constructor leaves everything un-initialized
     */
    overlap_bounds() : Compare(), target_() { }

    /**
     *  @brief  Set the target box and the comparator to the appropriate value.
     */
    overlap_bounds(const Compare& compare, const Key& target)
      : Compare(compare), target_(target)
    { }

    /**
     *  @brief  The operator that tells wheather the point is in region or not.
     */
    relative_order
    operator()(dimension_type dim, dimension_type rank, const Key& key) const
    {
      return overlap_bounds_impl(dim, rank, key, Layout());
    }

  private:
    /**
     *  @brief  The box value that will be used for overlaping comparison.
     */
    Key target_;

    relative_order overlap_bounds_impl
    (dimension_type dim, dimension_type rank, const Key& key, llhh_layout_tag)
    const
    {
      return (dim < (rank >> 1))
        ? (Compare::operator()(dim, key, dim + (rank >> 1), target_)
           ? matching : above)
        : (Compare::operator()(dim - (rank >> 1), target_, dim, key)
           ? matching : below);
    }

    relative_order overlap_bounds_impl
    (dimension_type dim, dimension_type, const Key& key, lhlh_layout_tag)
    const
    {
      return ((dim % 2) == 0)
        ? (Compare::operator()(dim, key, dim + 1, target_) ? matching : above)
        : (Compare::operator()(dim - 1, target_, dim, key) ? matching : below);
    }

    relative_order overlap_bounds_impl
    (dimension_type dim, dimension_type rank, const Key& key, hhll_layout_tag)
    const
    {
      return (dim < (rank >> 1))
        ? (Compare::operator()(dim + (rank >> 1), target_, dim, key)
           ? matching : below)
        : (Compare::operator()(dim, key, dim - (rank >> 1), target_)
           ? matching : above);
    }

    relative_order overlap_bounds_impl
    (dimension_type dim, dimension_type, const Key& key, hlhl_layout_tag)
    const
    {
      return ((dim % 2) == 0)
        ? (Compare::operator()(dim + 1, target_, dim, key) ? matching : below)
        : (Compare::operator()(dim, key, dim - 1, target_) ? matching : above);
    }
  };

  //@{
  /**
   *  @brief  Overlap bounds factory that takes in a @c container, a @c key and
   *  returns an @ref overlap_bounds type.
   *
   *  This factory also checks that box @c key is valid, meaning: all its lower
   *  coordinates are indeed lower or equal to its higher coordinates.
   */
  template <typename Tp, typename Layout>
  overlap_bounds<typename container_traits<Tp>::key_type,
                 typename container_traits<Tp>::key_compare,
                 Layout>
  make_overlap_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& target,
   Layout tag)
  {
    ::spatial::except::check_box(container, target, tag);
    return overlap_bounds
      <typename container_traits<Tp>::key_type,
      typename container_traits<Tp>::key_compare, Layout>
      (container.key_comp(), target);
  }

  template <typename Tp>
  overlap_bounds<typename container_traits<Tp>::key_type,
                 typename container_traits<Tp>::key_compare>
  make_overlap_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& target)
  { return make_overlap_bounds(container, target, llhh_layout_tag()); }
  //@}

  /**
   *  This region predicate matches keys that are enclosed or equal to a target
   *  box. The keys must represent boxes, not points.
   *
   *  @concept enclose_bounds is a model of RegionPredicate.
   *
   *  The Compare functor is expected to be a model of \s RegularComparison.
   *
   *  In order to interpret the box coordinates appropriately, overlap_bounds
   *  expects a Layout template argument. Layout is one of:
   *  @ul @ref llhh_layout_tag,
   *  @l @ref lhlh_layout_tag,
   *  @l @ref hhll_layout_tag,
   *  @l @ref hlhl_layout_tag.
   *  @lu
   *  Each layout provides information on how to interpret the coordinates
   *  returned for each dimension of the boxes values.
   *
   *  For a given target box @f$P_{(x, y)}@f$, this region predicate matches any
   *  box @f$B_{(x, y)}@f$ in a space of rank @f$r@f$ such as:
   *
   *  @f[
   *  \sum_{i=1}^{r} \left( P_{x_i} \le B_{x_i} \; and \;
   *                        B_{y_i} \le P_{y_i} \right)
   *  @f]
   */
  template <typename Key, typename Compare,
            typename Layout = llhh_layout_tag>
  struct enclosed_bounds
    : private Compare
  {
    /**
     *  @brief  The default constructor leaves everything un-initialized
     */
    enclosed_bounds() : Compare(), target_() { }

    /**
     *  @brief  Set the target box and the comparator to the appropriate value.
     */
    enclosed_bounds(const Compare& compare, const Key& target)
      : Compare(compare), target_(target)
    { }

    /**
     *  @brief  The operator that tells wheather the point is in region or not.
     */
    relative_order
    operator()(dimension_type dim, dimension_type rank, const Key& key) const
    {
      return enclose_bounds_impl(dim, rank, key, Layout());
    }

  private:
    /**
     *  @brief  The box value that will be used for the enclosing comparison.
     */
    Key target_;

    relative_order enclose_bounds_impl
    (dimension_type dim, dimension_type rank, const Key& key, llhh_layout_tag)
    const
    {
      return (dim < (rank >> 1))
        ? (Compare::operator()(dim , key, target_)
           ? below : (Compare::operator()(dim + (rank >> 1), target_, dim, key)
                      ? above : matching))
        : (Compare::operator()(dim, key, dim - (rank >> 1), target_)
           ? below : (Compare::operator()(dim, target_, key)
                      ? above : matching));
    }

    relative_order enclose_bounds_impl
    (dimension_type dim, dimension_type, const Key& key, lhlh_layout_tag)
    const
    {
      return ((dim % 2) == 0)
        ? (Compare::operator()(dim , key, target_)
           ? below : (Compare::operator()(dim + 1, target_, dim, key)
                      ? above : matching))
        : (Compare::operator()(dim, key, dim - 1, target_)
           ? below : (Compare::operator()(dim, target_, key)
                      ? above : matching));
    }

    relative_order enclose_bounds_impl
    (dimension_type dim, dimension_type rank, const Key& key, hhll_layout_tag)
    const
    {
      return (dim < (rank >> 1))
        ? (Compare::operator()(dim , target_, key)
           ? above : (Compare::operator()(dim, key, dim + (rank >> 1), target_)
                      ? below : matching))
        : (Compare::operator()(dim - (rank >> 1), target_, dim, key)
           ? above : (Compare::operator()(dim, key, target_)
                      ? below : matching));
    }

    relative_order enclose_bounds_impl
    (dimension_type dim, dimension_type, const Key& key, hlhl_layout_tag)
    const
    {
      return ((dim % 2) == 0)
        ? (Compare::operator()(dim , target_, key)
           ? above : (Compare::operator()(dim, key, dim + 1, target_)
                      ? below : matching))
        : (Compare::operator()(dim - 1, target_, dim, key)
           ? above : (Compare::operator()(dim, key, target_)
                      ? below : matching));
    }
  };

  //@{
  /**
   *  @brief  Overlap bounds factory that takes in a @c container, a @c key and
   *  returns an @ref enclose_bounds type.
   *
   *  This factory also checks that box @c key is valid, meaning: all its lower
   *  coordinates are indeed lower or equal to its higher coordinates.
   */
  template <typename Tp, typename Layout>
  enclosed_bounds<typename container_traits<Tp>::key_type,
                 typename container_traits<Tp>::key_compare,
                 Layout>
  make_enclosed_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& target,
   Layout tag)
  {
    except::check_box(container, target, tag);
    return enclosed_bounds
      <typename container_traits<Tp>::key_type,
       typename container_traits<Tp>::key_compare, Layout>
      (container.key_comp(), target);
  }

  template <typename Tp>
  enclosed_bounds<typename container_traits<Tp>::key_type,
                 typename container_traits<Tp>::key_compare>
  make_enclosed_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& target)
  { return make_enclosed_bounds(container, target, llhh_layout_tag()); }
  //@}

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
  struct region_iterator
    : details::Bidirectional_iterator
      <typename container_traits<Ct>::mode_type,
       typename container_traits<Ct>::rank_type,
       region_iterator<Ct, Predicate> >
  {
  private:
    typedef typename details::Bidirectional_iterator
    <typename container_traits<Ct>::mode_type,
     typename container_traits<Ct>::rank_type, region_iterator> Base;

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
        pred_(pred) { }

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
    region_iterator(Ct& container, const Predicate& pred,
                    dimension_type dim,
                    typename container_traits<Ct>::mode_type::node_ptr ptr)
      : Base(container.rank(), ptr, dim), pred_(pred) { }

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
    const Predicate& predicate() const { return pred_; }

  private:
    //! The related data for the iterator.
    Predicate pred_;
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
  struct region_iterator<const Ct, Predicate>
    : details::Const_bidirectional_iterator
      <typename container_traits<Ct>::mode_type,
       typename container_traits<Ct>::rank_type,
       region_iterator<const Ct, Predicate> >
  {
  private:
    typedef details::Const_bidirectional_iterator
    <typename container_traits<Ct>::mode_type,
     typename container_traits<Ct>::rank_type, region_iterator> Base;

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
        pred_(pred) { }

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
    region_iterator(const Ct& container, const Predicate& pred,
                    dimension_type dim,
                    typename container_traits<Ct>::mode_type::const_node_ptr
                    ptr)
      : Base(container.rank(), ptr, dim), pred_(pred) { }

    //! Convertion of an iterator into a const_iterator is permitted.
    region_iterator(const region_iterator<Ct, Predicate>& iter)
      : Base(iter.rank(), iter.node, iter.node_dim), pred_(iter.predicate()) { }

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
    const Predicate& predicate() const { return pred_; }

  private:
    //! The related data for the iterator.
    Predicate pred_;
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
     *  @brief  From @c x, find the node with the minimum value in the region
     *  delimited by p. If multiple nodes are matching, return the first
     *  matching node in in-order transversal.
     *
     *  @param node_dim  The current dimension for @c node.
     *  @param node  The node from which to find the minimum.
     *  @param key_dimension  The number of dimensions of key.
     *  @param predicate  The predicate for the orthogonal region query.
     *  @return  An iterator pointing the minimum, or to the parent of @c node.
     *
     *  If @c node is a header node, the search will stop immediately.
     */
    template <typename Ct, typename Predicate>
    region_iterator<Ct, Predicate>&
    minimum_region(region_iterator<Ct, Predicate>& iter);

    /**
     *  @brief  From @c x, find the node with the maximum value in the region
     *  delimited by p. If multiple nodes are matching, return the last
     *  matching node in in-order transversal.
     *
     *  @param node_dim  The current dimension for @c node.
     *  @param node  The node from which to find the minimum.
     *  @param key_dimension  The number of dimensions of key.
     *  @param predicate  The predicate for the orthogonal region query.
     *  @return  An iterator pointing the maximum, or to the parent of @c node.
     *
     *  If @c node is a header node, the search will stop immediately.
     */
    template <typename Ct, typename Predicate>
    region_iterator<Ct, Predicate>&
    maximum_region(region_iterator<Ct, Predicate>& iter);

  } // namespace details

  /**
   *  This structure defines a pair of mutable region iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \see region_iterator
   */
  template <typename Ct, typename Predicate
            = bounds<typename container_traits<Ct>::key_type,
                     typename container_traits<Ct>::key_compare> >
  struct region_iterator_pair
    : std::pair<region_iterator<Ct, Predicate>,
                region_iterator<Ct, Predicate> >
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef std::pair<region_iterator<Ct, Predicate>,
                              region_iterator<Ct, Predicate> > Base;

    //! Empty constructor.
    region_iterator_pair() { }

    //! Regular constructor that builds a region_iterator_pair out of 2
    //! region_iterators.
    region_iterator_pair(const region_iterator<Ct, Predicate>& a,
                         const region_iterator<Ct, Predicate>& b)
          : Base(a, b) { }
  };

  /**
   *  This structure defines a pair of constant region iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \see region_iterator
   */
  template <typename Ct, typename Predicate>
  struct region_iterator_pair<const Ct, Predicate>
    : std::pair<region_iterator<const Ct, Predicate>,
                region_iterator<const Ct, Predicate> >
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef std::pair<region_iterator<const Ct, Predicate>,
                      region_iterator<const Ct, Predicate> > Base;

    //! Empty constructor.
    region_iterator_pair() { }

    //! Regular constructor that builds a region_iterator_pair out of 2
    //! region_iterators.
    region_iterator_pair(const region_iterator<const Ct, Predicate>& a,
                         const region_iterator<const Ct, Predicate>& b)
      : Base(a, b) { }

    //! Convert a mutable region iterator pair into a const region iterator
    //! pair.
    region_iterator_pair(const region_iterator_pair<Ct, Predicate>& p)
      : Base(p.first, p.second) { }
  };

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

  template <typename Ct, typename Predicate>
  inline region_iterator_pair<Ct, Predicate>
  region_range(Ct& container, const Predicate& pred)
  {
    return region_iterator_pair<Ct, Predicate>
      (region_begin(container, pred), region_end(container, pred));
  }

  template <typename Ct, typename Predicate>
  inline region_iterator_pair<const Ct, Predicate>
  region_range(const Ct& container, const Predicate& pred)
  {
    return region_iterator_pair<const Ct, Predicate>
      (region_begin(container, pred), region_end(container, pred));
  }

  template <typename Ct, typename Predicate>
  inline region_iterator_pair<const Ct, Predicate>
  region_crange(const Ct& container, const Predicate& pred)
  {
    return region_iterator_pair<const Ct, Predicate>
      (region_begin(container, pred), region_end(container, pred));
  }

/* MACRO FOR GENERATION OF FACTORIES FOR ALL TYPES OF REGION ITERATORS
 *
 * The follwing sets of macros are used to rapidly define all factories for
 * every type of region iterators as well as the type variations themselves.
 */

# define SPATIAL_REGION_DEFINE(Name, Bounds)                            \
  template<typename Ct>                                                 \
  struct Name##_iterator                                                \
    : region_iterator                                                   \
       <Ct, Bounds<typename container_traits<Ct>::key_type,             \
                   typename container_traits<Ct>::key_compare> >        \
  {                                                                     \
    Name##_iterator() { }                                               \
    Name##_iterator                                                     \
      (const region_iterator                                            \
       <Ct, Bounds<typename container_traits<Ct>::key_type,             \
                   typename container_traits<Ct>::key_compare> >&       \
       other)                                                           \
      : region_iterator                                                 \
      <Ct, Bounds<typename container_traits<Ct>::key_type,              \
                  typename container_traits<Ct>::key_compare> >         \
      (other) { }                                                       \
  };                                                                    \
  template<typename Ct>                                                 \
  struct Name##_iterator<const Ct>                                      \
    : region_iterator                                                   \
       <const Ct, Bounds<typename container_traits<Ct>::key_type,       \
                         typename container_traits<Ct>::key_compare> >  \
  {                                                                     \
    Name##_iterator() { }                                               \
    Name##_iterator                                                     \
      (const region_iterator                                            \
       <const Ct, Bounds<typename container_traits<Ct>::key_type,       \
                         typename container_traits<Ct>::key_compare> >& \
       other)                                                           \
      : region_iterator                                                 \
      <const Ct, Bounds<typename container_traits<Ct>::key_type,        \
                        typename container_traits<Ct>::key_compare> >   \
      (other) { }                                                       \
    Name##_iterator                                                     \
      (const region_iterator                                            \
       <Ct, Bounds<typename container_traits<Ct>::key_type,             \
                   typename container_traits<Ct>::key_compare> >&       \
       other)                                                           \
      : region_iterator                                                 \
      <const Ct, Bounds<typename container_traits<Ct>::key_type,        \
                        typename container_traits<Ct>::key_compare> >   \
      (other) { }                                                       \
  };                                                                    \
  template<typename Ct>                                                 \
  struct Name##_iterator_pair                                           \
    : region_iterator_pair                                              \
      <Ct, Bounds<typename container_traits<Ct>::key_type,              \
                  typename container_traits<Ct>::key_compare> >         \
  {                                                                     \
    Name##_iterator_pair() { }                                          \
    Name##_iterator_pair                                                \
      (const region_iterator                                            \
       <Ct, Bounds<typename container_traits<Ct>::key_type,             \
                   typename container_traits<Ct>::key_compare> >& a,    \
       const region_iterator                                            \
       <Ct, Bounds<typename container_traits<Ct>::key_type,             \
                   typename container_traits<Ct>::key_compare> >& b)    \
      : region_iterator_pair                                            \
      <Ct, Bounds<typename container_traits<Ct>::key_type,              \
                  typename container_traits<Ct>::key_compare> >         \
      (a, b) { }                                                        \
  };                                                                    \
  template<typename Ct>                                                 \
  struct Name##_iterator_pair<const Ct>                                 \
    : region_iterator_pair                                              \
    <const Ct, Bounds<typename container_traits<Ct>::key_type,          \
                      typename container_traits<Ct>::key_compare> >     \
  {                                                                     \
    Name##_iterator_pair() { }                                          \
    Name##_iterator_pair                                                \
      (const region_iterator                                            \
       <const Ct, Bounds<typename container_traits<Ct>::key_type,       \
                         typename container_traits<Ct>::key_compare> >& a, \
       const region_iterator                                            \
       <const Ct, Bounds<typename container_traits<Ct>::key_type,       \
                         typename container_traits<Ct>::key_compare> >& b) \
      : region_iterator_pair                                            \
      <const Ct, Bounds<typename container_traits<Ct>::key_type,        \
                        typename container_traits<Ct>::key_compare> >   \
      (a, b) { }                                                        \
    Name##_iterator_pair(const Name##_iterator_pair<Ct>& other)         \
      : region_iterator_pair                                            \
      <const Ct, Bounds<typename container_traits<Ct>::key_type,        \
                        typename container_traits<Ct>::key_compare> >   \
      (other) { }                                                       \
  }

# define SPATIAL_REGION_DEFINE_L(Name, Bounds)                          \
  template<typename Ct, typename Layout = llhh_layout_tag>              \
  struct Name##_iterator                                                \
    : region_iterator                                                   \
       <Ct, Bounds<typename container_traits<Ct>::key_type,             \
                   typename container_traits<Ct>::key_compare,          \
                   Layout> >                                            \
  {                                                                     \
    Name##_iterator() { }                                               \
    Name##_iterator                                                     \
      (const region_iterator                                            \
       <Ct, Bounds<typename container_traits<Ct>::key_type,             \
                   typename container_traits<Ct>::key_compare,          \
                   Layout> >& other)                                    \
      : region_iterator                                                 \
      <Ct, Bounds<typename container_traits<Ct>::key_type,              \
                  typename container_traits<Ct>::key_compare,           \
                  Layout> >(other) { }                                  \
  };                                                                    \
  template<typename Ct, typename Layout>                                \
  struct Name##_iterator<const Ct, Layout>                              \
    : region_iterator                                                   \
      <const Ct, Bounds<typename container_traits<Ct>::key_type,        \
                        typename container_traits<Ct>::key_compare,     \
                        Layout> >                                       \
  {                                                                     \
    Name##_iterator() { }                                               \
    Name##_iterator                                                     \
      (const region_iterator                                            \
       <const Ct, Bounds<typename container_traits<Ct>::key_type,       \
                         typename container_traits<Ct>::key_compare,    \
                         Layout> >& other)                              \
      : region_iterator                                                 \
      <const Ct, Bounds<typename container_traits<Ct>::key_type,        \
                        typename container_traits<Ct>::key_compare,     \
                        Layout> >                                       \
      (other) { }                                                       \
    Name##_iterator                                                     \
      (const region_iterator                                            \
       <Ct, Bounds<typename container_traits<Ct>::key_type,             \
                   typename container_traits<Ct>::key_compare,          \
                   Layout> >& other)                                    \
      : region_iterator                                                 \
      <const Ct, Bounds<typename container_traits<Ct>::key_type,        \
                        typename container_traits<Ct>::key_compare,     \
                        Layout> >                                       \
      (other) { }                                                       \
  };                                                                    \
  template<typename Ct, typename Layout = llhh_layout_tag>              \
  struct Name##_iterator_pair                                           \
    : region_iterator_pair                                              \
      <Ct, Bounds<typename container_traits<Ct>::key_type,              \
                  typename container_traits<Ct>::key_compare,           \
                  Layout> >                                             \
  {                                                                     \
    Name##_iterator_pair() { }                                          \
    Name##_iterator_pair                                                \
      (const region_iterator                                            \
       <Ct, Bounds<typename container_traits<Ct>::key_type,             \
                   typename container_traits<Ct>::key_compare,          \
                   Layout> >& a,                                        \
       const region_iterator                                            \
       <Ct, Bounds<typename container_traits<Ct>::key_type,             \
                   typename container_traits<Ct>::key_compare,          \
                   Layout> >& b)                                        \
      : region_iterator_pair                                            \
      <Ct, Bounds<typename container_traits<Ct>::key_type,              \
                  typename container_traits<Ct>::key_compare,           \
                  Layout> >                                             \
      (a, b) { }                                                        \
  };                                                                    \
  template<typename Ct, typename Layout>                                \
  struct Name##_iterator_pair<const Ct, Layout>                         \
    : region_iterator_pair                                              \
    <const Ct, Bounds<typename container_traits<Ct>::key_type,          \
                      typename container_traits<Ct>::key_compare,       \
                      Layout> >                                         \
  {                                                                     \
    Name##_iterator_pair() { }                                          \
    Name##_iterator_pair                                                \
      (const region_iterator                                            \
       <const Ct, Bounds<typename container_traits<Ct>::key_type,       \
                         typename container_traits<Ct>::key_compare,    \
                         Layout> >& a,                                  \
       const region_iterator                                            \
       <const Ct, Bounds<typename container_traits<Ct>::key_type,       \
                         typename container_traits<Ct>::key_compare,    \
                         Layout> >& b)                                  \
      : region_iterator_pair                                            \
      <const Ct, Bounds<typename container_traits<Ct>::key_type,        \
                        typename container_traits<Ct>::key_compare,     \
                        Layout> >                                       \
      (a, b) { }                                                        \
    Name##_iterator_pair(const Name##_iterator_pair<Ct>& other)         \
      : region_iterator_pair                                            \
      <const Ct, Bounds<typename container_traits<Ct>::key_type,        \
                        typename container_traits<Ct>::key_compare,     \
                        Layout> >                                       \
      (other) { }                                                       \
  }

# define SPATIAL_REGION_MATCH_BIT(Region, Type, Bounds)                 \
  template <typename Ct>                                                \
  inline Region##_iterator<Ct>                                          \
  Region##_##Type(Ct& container,                                        \
                  const typename container_traits<Ct>::key_type& match) \
  { return region_##Type(container, make_##Bounds(container, match)); } \
  template <typename Ct>                                                \
  inline Region##_iterator<const Ct>                                    \
  Region##_##Type(const Ct& container,                                  \
                  const typename container_traits<Ct>::key_type& match) \
  { return region_##Type(container, make_##Bounds(container, match)); } \
  template <typename Ct>                                                \
  inline Region##_iterator<const Ct>                                    \
  Region##_c##Type(const Ct& container,                                 \
                   const typename container_traits<Ct>::key_type& match) \
  { return region_c##Type(container, make_##Bounds(container, match)); }

# define SPATIAL_REGION_MATCH(Region, Bounds)                           \
  SPATIAL_REGION_MATCH_BIT(Region, begin, Bounds)                       \
  SPATIAL_REGION_MATCH_BIT(Region, end, Bounds)                         \
  template <typename Ct>                                                \
  inline Region##_iterator_pair<Ct>                                     \
  Region##_range(Ct& container,                                         \
                 const typename container_traits<Ct>::key_type& match)  \
  { return region_range(container, make_##Bounds(container, match)); }  \
  template <typename Ct>                                                \
  inline Region##_iterator_pair<const Ct>                               \
  Region##_range(const Ct& container,                                   \
                 const typename container_traits<Ct>::key_type& match)  \
  { return region_range(container, make_##Bounds(container, match)); }  \
  template <typename Ct>                                                \
  inline Region##_iterator_pair<const Ct>                               \
  Region##_crange(const Ct& container,                                  \
                  const typename container_traits<Ct>::key_type& match) \
  { return region_crange(container, make_##Bounds(container, match)); }

# define SPATIAL_REGION_INTERVAL_BIT(Region, Type, Bounds)              \
  template <typename Ct>                                                \
  inline Region##_iterator<Ct>                                          \
  Region##_##Type(Ct& container,                                        \
                  const typename container_traits<Ct>::key_type& lower, \
                  const typename container_traits<Ct>::key_type& upper) \
  { return region_##Type(container, make_##Bounds(container, lower, upper)); }\
  template <typename Ct>                                                \
  inline Region##_iterator<const Ct>                                    \
  Region##_##Type(const Ct& container,                                  \
                  const typename container_traits<Ct>::key_type& lower, \
                  const typename container_traits<Ct>::key_type& upper) \
  { return region_##Type(container, make_##Bounds(container, lower, upper)); }\
  template <typename Ct>                                                \
  inline Region##_iterator<const Ct>                                    \
  Region##_c##Type(const Ct& container,                                 \
                   const typename container_traits<Ct>::key_type& lower, \
                   const typename container_traits<Ct>::key_type& upper) \
  { return region_c##Type(container, make_##Bounds(container, lower, upper)); }

# define SPATIAL_REGION_INTERVAL(Region, Bounds)                        \
  SPATIAL_REGION_INTERVAL_BIT(Region, begin, Bounds)                    \
  SPATIAL_REGION_INTERVAL_BIT(Region, end, Bounds)                      \
  template <typename Ct>                                                \
  inline Region##_iterator_pair<Ct>                                     \
  Region##_range(Ct& container,                                         \
                 const typename container_traits<Ct>::key_type& lower,  \
                 const typename container_traits<Ct>::key_type& upper)  \
  { return region_range(container, make_##Bounds(container, lower, upper)); } \
  template <typename Ct>                                                \
  inline Region##_iterator_pair<const Ct>                               \
  Region##_range(const Ct& container,                                   \
                 const typename container_traits<Ct>::key_type& lower,  \
                 const typename container_traits<Ct>::key_type& upper)  \
  { return region_range(container, make_##Bounds(container, lower, upper)); } \
  template <typename Ct>                                                \
  inline Region##_iterator_pair<const Ct>                               \
  Region##_crange(const Ct& container,                                  \
                  const typename container_traits<Ct>::key_type& lower, \
                  const typename container_traits<Ct>::key_type& upper) \
  { return region_crange(container, make_##Bounds(container, lower, upper)); }

# define SPATIAL_REGION_INTERVAL_L_BIT(Region, Type, Bounds)            \
  template <typename Ct>                                                \
  inline Region##_iterator<Ct>                                          \
  Region##_##Type(Ct& container,                                        \
                  const typename container_traits<Ct>::key_type& target) \
  {                                                                     \
    return region_##Type                                                \
      (container, make_##Bounds(container, target, llhh_layout_tag())); \
  }                                                                     \
  template <typename Ct, typename Layout>                               \
  inline Region##_iterator<Ct, Layout>                                  \
  Region##_##Type(Ct& container,                                        \
                  const typename container_traits<Ct>::key_type& target, \
                  const Layout& layout)                                 \
  {                                                                     \
    return region_##Type                                                \
      (container, make_##Bounds(container, target, layout));            \
  }                                                                     \
  template <typename Ct>                                                \
  inline Region##_iterator<const Ct>                                    \
  Region##_##Type(const Ct& container,                                  \
                  const typename container_traits<Ct>::key_type& target) \
  {                                                                     \
    return region_##Type                                                \
      (container, make_##Bounds(container, target, llhh_layout_tag())); \
  }                                                                     \
  template <typename Ct, typename Layout>                               \
  inline Region##_iterator<const Ct, Layout>                            \
  Region##_##Type(const Ct& container,                                  \
                  const typename container_traits<Ct>::key_type& target, \
                  const Layout& layout)                                 \
  {                                                                     \
    return region_##Type                                                \
      (container, make_##Bounds(container, target, layout));            \
  }                                                                     \
  template <typename Ct>                                                \
  inline Region##_iterator<const Ct>                                    \
  Region##_c##Type(const Ct& container,                                 \
                   const typename container_traits<Ct>::key_type& target) \
  {                                                                     \
    return region_c##Type                                               \
      (container, make_##Bounds(container, target, llhh_layout_tag())); \
  }                                                                     \
  template <typename Ct, typename Layout>                               \
  inline Region##_iterator<const Ct, Layout>                            \
  Region##_c##Type(const Ct& container,                                 \
                   const typename container_traits<Ct>::key_type& target, \
                   const Layout& layout)                                \
  {                                                                     \
    return region_c##Type                                               \
      (container, make_##Bounds(container, target, layout));            \
  }

# define SPATIAL_REGION_INTERVAL_L(Region, Bounds)                      \
  SPATIAL_REGION_INTERVAL_L_BIT(Region, begin, Bounds)                  \
  SPATIAL_REGION_INTERVAL_L_BIT(Region, end, Bounds)                    \
  template <typename Ct>                                                \
  inline Region##_iterator_pair<Ct>                                     \
  Region##_range(Ct& container,                                         \
                 const typename container_traits<Ct>::key_type& target) \
  {                                                                     \
    return region_range                                                 \
      (container, make_##Bounds(container, target, llhh_layout_tag())); \
  }                                                                     \
  template <typename Ct, typename Layout>                               \
  inline Region##_iterator_pair<Ct, Layout>                             \
  Region##_range(Ct& container,                                         \
                 const typename container_traits<Ct>::key_type& target, \
                 const Layout& layout)                                  \
  {                                                                     \
    return region_range                                                 \
      (container, make_##Bounds(container, target, layout));            \
  }                                                                     \
  template <typename Ct>                                                \
  inline Region##_iterator_pair<const Ct>                               \
  Region##_range(const Ct& container,                                   \
                 const typename container_traits<Ct>::key_type& target) \
  {                                                                     \
    return region_range                                                 \
    (container, make_##Bounds(container, target, llhh_layout_tag()));   \
  }                                                                     \
  template <typename Ct, typename Layout>                               \
  inline Region##_iterator_pair<const Ct, Layout>                       \
  Region##_range(const Ct& container,                                   \
                 const typename container_traits<Ct>::key_type& target, \
                 const Layout& layout)                                  \
  {                                                                     \
    return region_range                                                 \
      (container, make_##Bounds(container, target, layout));            \
  }                                                                     \
  template <typename Ct>                                                \
  inline Region##_iterator_pair<const Ct>                               \
  Region##_crange(const Ct& container,                                  \
                  const typename container_traits<Ct>::key_type& target)\
  {                                                                     \
    return region_crange                                                \
      (container, make_##Bounds(container, target, llhh_layout_tag())); \
  }                                                                     \
  template <typename Ct, typename Layout>                               \
  inline Region##_iterator_pair<const Ct, Layout>                       \
  Region##_crange(const Ct& container,                                  \
                  const typename container_traits<Ct>::key_type& target,\
                  const Layout& layout)                                 \
  {                                                                     \
    return region_crange                                                \
      (container, make_##Bounds(container, target, layout));            \
  }

  SPATIAL_REGION_DEFINE(equal, equal_bounds);
  SPATIAL_REGION_DEFINE(open_region, open_bounds);
  SPATIAL_REGION_DEFINE(closed_region, closed_bounds);
  SPATIAL_REGION_DEFINE_L(enclosed, enclosed_bounds);
  SPATIAL_REGION_DEFINE_L(overlap, overlap_bounds);

  // The ending ';' are not available for the following macros
  SPATIAL_REGION_MATCH(equal, equal_bounds)
  SPATIAL_REGION_INTERVAL(region, bounds)
  SPATIAL_REGION_INTERVAL(open_region, open_bounds)
  SPATIAL_REGION_INTERVAL(closed_region, closed_bounds)
  SPATIAL_REGION_INTERVAL_L(enclosed, enclosed_bounds)
  SPATIAL_REGION_INTERVAL_L(overlap, overlap_bounds)

# undef SPATIAL_REGION_DEFINE
# undef SPATIAL_REGION_DEFINE_L
# undef SPATIAL_REGION_MATCH_BIT
# undef SPATIAL_REGION_MATCH
# undef SPATIAL_REGION_INTERVAL_BIT
# undef SPATIAL_REGION_INTERVAL
# undef SPATIAL_REGION_INTERVAL_L_BIT
# undef SPATIAL_REGION_INTERVAL_L

  /**
   *  This constant is used to declare a type layout tag, which is required by
   *  the overlap and enclosed region iterators. Creation of these iterators is
   *  facilitated by these objects.
   *
   *  \warning This constant is unique for each translation unit, and therefore
   *  its address will differ for each translation unit.
   *  @{
   */
  const llhh_layout_tag llhh_layout = llhh_layout_tag();
  const lhlh_layout_tag lhlh_layout = lhlh_layout_tag();
  const hhll_layout_tag hhll_layout = hhll_layout_tag();
  const hlhl_layout_tag hlhl_layout = hlhl_layout_tag();
  //@}

} // namespace spatial

#include "spatial_region.tpp"

#endif // SPATIAL_REGION_HPP
