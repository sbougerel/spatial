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
        ? (Compare::operator()(dim + (rank >> 1), target_, dim, key)
           ? above : matching)
        : (Compare::operator()(dim, key, dim - (rank >> 1), target_)
           ? below : matching);
    }

    relative_order overlap_bounds_impl
    (dimension_type dim, dimension_type rank, const Key& key, lhlh_layout_tag)
    const
    {
      return ((dim % 2) == 0)
        ? (Compare::operator()(dim + 1, target_, dim, key) ? above : matching)
        : (Compare::operator()(dim, key, dim - 1, target_) ? below : matching);
    }

    relative_order overlap_bounds_impl
    (dimension_type dim, dimension_type rank, const Key& key, hhll_layout_tag)
    const
    {
      return (dim < (rank >> 1))
        ? (Compare::operator()(dim, key, dim + (rank >> 1), target_)
           ? below : matching)
        : (Compare::operator()(dim - (rank >> 1), target_, dim, key)
           ? above : matching);
    }

    relative_order overlap_bounds_impl
    (dimension_type dim, dimension_type rank, const Key& key, hlhl_layout_tag)
    const
    {
      return ((dim % 2) == 0)
        ? (Compare::operator()(dim, key, dim + 1, target_) ? below : matching)
        : (Compare::operator()(dim - 1, target_, dim, key) ? above : matching);
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
    (dimension_type dim, dimension_type rank, const Key& key, lhlh_layout_tag)
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
    (dimension_type dim, dimension_type rank, const Key& key, hlhl_layout_tag)
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
    ::spatial::except::check_box(container, target, tag);
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

  namespace details
  {
    /**
     *  Extra information needed by the region iterator. This information is
     *  stored in the iterator is copied for each iterator.
     *
     *  Although it may be possible to modify this information directly from
     *  it's members, it may be unwise to do so, as it could invalidate the
     *  iterator and cause the program to behave unexpectedly. If any of this
     *  information needs to be modified, it is probably recommended to create a
     *  new iterator altogether.
     */
    template<typename Ct, typename Predicate>
    struct Region_data
    {
      //! Build an uninitialized region data object.
      Region_data() { }

      /**
       *  Builds required region data from a given container, dimension, and a
       *  predicate.
       *
       *  \param c The container being iterated.
       *  \param p The model of \ref RegionPredicate.
       */
      Region_data
      (const Ct&, const Predicate& p) : pred(p)
        { }

      /**
       *  The predicate that contains the definition of the region.
       */
      Predicate pred;
    };
  } // namespace details

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
  struct region
  {
    /**
     *  Iterates over values contained within the orthogonal region expressed
     *  in the \ref RegionPredicate. The elements returned by this iterator are
     *  not ordered, but are guarrented to fall within the given Predicate
     *  provided.
     *
     *  Uses \ref Predicate to find all matching values. \ref Predicate must
     *  be a model of \ref RegionPredicate. See the \ref RegionPredicate concept
     *  for more information on writing a region predicates.
     */
    struct iterator : details::Bidirectional_iterator
    <typename Ct::mode_type, typename Ct::rank_type, iterator>
    {
    private:
      typedef typename details::Bidirectional_iterator
      <typename Ct::mode_type, typename Ct::rank_type, iterator>
      Base;

      template<typename Iterator> struct Rebind
      {
        typedef typename ::spatial::details::Bidirectional_iterator
        <typename container_traits<Ct>::mode_type, typename Ct::rank_type, Iterator> type;
      };

    public:
      //! Uninitialized iterator.
      iterator() { }

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
      iterator(Ct& container, const Predicate& predicate,
               typename container_traits<Ct>::iterator iter)
        : Base(iter.node, modulo(iter.node, container.rank())),
          data(predicate, container) { }

      /**
       *  Build a region iterator from a container and another bidirectional
       *  iterator on this container.
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
       *  \param iter A bidirectional iterator on the type Ct.
       */
      template<typename Iterator>
      iterator(Ct& container, const Predicate& predicate,
               typename Rebind<Iterator>::type iter)
        : Base(iter.node, iter.node_dim), data(predicate, container) { }

      /**
       *  Build a region iterator from the node and curretn dimension of a
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
       *  \param predicate A model of the \ref RegionPredicate concept.
       *  \param node An iterator on the type Ct.
       *  \param node_dim The node's dimension for the node pointed to by node.
       *  \param container The container being iterated.
       */
      iterator(Ct& container, const Predicate& predicate,
               dimension_type node_dim,
               typename container_traits<Ct>::mode_type::node_ptr node)
        : Base(node, node_dim), data(container, predicate) { }

      //@{
      /**
       *  This iterator can be casted silently into a container iterator. You can
       *  therefore use this iterator as an argument to the erase function of
       *  the container, for example.
       *
       *  \warning When using this iterator as an argument to the erase function
       *  of the container, this iterator will get invalidated after erase.
       */
      operator typename container_traits<Ct>::iterator()
      { return container_traits<Ct>::iterator(Base::node); }

      operator typename container_traits<Ct>::const_iterator()
      { return container_traits<Ct>::const_iterator(Base::node); }
      //@}

      //! The related data for the iterator.
      ::spatial::details::Region_data<Ct, Predicate> data;
    };

    /**
     *  A pair of iterators that represents a region of elements to
     *  iterates. These elements are the results of the orthogonal region search.
     */
    typedef ::std::pair<iterator, iterator>             iterator_pair;
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
  struct region<const Ct, Predicate>
  {
    /**
     *  Iterates over values contained within the orthogonal region expressed
     *  in the \ref RegionPredicate. The elements returned by this iterator are
     *  not ordered, but are guarrented to fall within the given Predicate
     *  provided.
     *
     *  Uses \ref Predicate to find all matching values. \ref Predicate must
     *  be a model of \ref RegionPredicate. See the \ref RegionPredicate concept
     *  for more information on writing a region predicates.
     */
    struct iterator : ::spatial::details::Const_bidirectional_iterator
    <typename container_traits<Ct>::mode_type, typename Ct::rank_type, iterator>
    {
    private:
      typedef ::spatial::details::Const_bidirectional_iterator
      <typename container_traits<Ct>::mode_type, typename Ct::rank_type, iterator> Base;

      template<typename Iterator> struct Rebind
      {
        typedef typename ::spatial::details::Const_bidirectional_iterator
        <typename container_traits<Ct>::mode_type, typename Ct::rank_type, Iterator> type;
      };

    public:
      //! Uninitialized iterator.
      iterator() { }

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
      iterator(const Ct& container, const Predicate& predicate,
               typename container_traits<Ct>::const_iterator iter)
        : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
          data(predicate, container) { }

      /**
       *  Build a region iterator from a container and another bidirectional
       *  iterator on this container.
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
       *  \param iter A bidirectional iterator on the type Ct.
       */
      template<typename Iterator>
      iterator(const Ct& container, const Predicate& predicate,
               typename Rebind<Iterator>::type iter)
        : Base(container.rank(), iter.node, iter.node_dim), data(predicate, container) { }

      /**
       *  Build a region iterator from the node and curretn dimension of a
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
      iterator(const Ct& container, const Predicate& predicate,
               dimension_type node_dim,
               typename container_traits<Ct>::mode_type::node_ptr node)
        : Base(node, node_dim), data(container, predicate) { }

      //! Convertion of an iterator into a const_iterator is permitted.
      iterator(const typename region<Ct>::iterator& iter)
        : Base(iter.node, iter.node_dim), data(iter.data) { }

      /**
       *  This iterator can be casted silently into a container iterator. You can
       *  therefore use this iterator as an argument to the erase function of
       *  the container, for example.
       *
       *  \warning When using this iterator as an argument to the erase function
       *  of the container, this iterator will get invalidated after erase.
       */
      operator typename container_traits<Ct>::const_iterator()
      { return container_traits<Ct>::const_iterator(Base::node); }

      //! The related data for the iterator.
      ::spatial::details::Region_data<Ct, Predicate> data;
    };

    /**
     *  A pair of iterators that represents a region of elements to
     *  iterates. These elements are the results of the orthogonal region search.
     */
    typedef ::std::pair<iterator, iterator>             iterator_pair;
  };

  namespace details
  {
    template <typename Ct, typename Predicate>
    typename region<Ct, Predicate>::iterator&
    increment(typename region<Ct, Predicate>::iterator& iter);

    template <typename Ct, typename Predicate>
    typename region<Ct, Predicate>::iterator&
    decrement(typename region<Ct, Predicate>::iterator& iter);

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
    typename region<Ct, Predicate>::iterator&
    minimum(typename region<Ct, Predicate>::iterator& iter);

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
    typename region<Ct, Predicate>::iterator&
    maximum(typename region<Ct, Predicate>::iterator& iter);

  } // namespace details

  template <typename Ct, typename Predicate>
  inline typename region<Ct, Predicate>::iterator
  region_begin(Ct& container, const Predicate& pred)
  {
    if (container.empty()) return end_region(pred, container);
    typename region<Ct, Predicate>::iterator
      it(pred, 0, container.top(), container); // At root, dim = 0
    return ::spatial::details::minimum(it);
  }

  template <typename Ct, typename Predicate>
  inline typename region<const Ct, Predicate>::iterator
  region_begin(const Ct& container, const Predicate& pred)
  {
    if (container.empty()) return end_region(pred, container);
    typename region<const Ct, Predicate>::iterator
      it(pred, 0, container.top(), container); // At root, dim = 0
    return ::spatial::details::minimum(it);
  }

  template <typename Ct, typename Predicate>
  inline typename region<const Ct, Predicate>::const_iterator
  region_cbegin(const Ct& container, const Predicate& pred)
  { return region_begin(container, pred); }

  template <typename Ct, typename Predicate>
  inline typename region<Ct, Predicate>::iterator
  region_end(Ct& container, const Predicate& pred)
  {
    return region<Ct, Predicate>::iterator
      (container, pred, container.dimension() - 1,
       container.end()); // At header, dim = rank - 1
  }

  template <typename Ct, typename Predicate>
  inline typename region<const Ct, Predicate>::iterator
  region_end(const Ct& container, const Predicate& pred)
  {
    return region<Ct, Predicate>::iterator
      (container, pred, container.dimension() - 1,
       container.end()); // At header, dim = rank - 1
  }

  template <typename Ct, typename Predicate>
  inline typename region<const Ct, Predicate>::iterator
  region_cend(const Ct& container, const Predicate& pred)
  { return region_end(container, pred); }

  template <typename Ct, typename Predicate>
  inline typename region<Ct, Predicate>::iterator_pair
  region_range(Ct& container, const Predicate& pred)
  {
    return ::std::make_pair(region_begin(container, pred),
                            region_end(container, pred));
  }

  template <typename Ct, typename Predicate>
  inline typename region<const Ct, Predicate>::iterator_pair
  region_range(const Ct& container, const Predicate& pred)
  {
    return ::std::make_pair(region_begin(container, pred),
                            region_end(container, pred));
  }

  template <typename Ct, typename Predicate>
  inline typename region<const Ct, Predicate>::iterator_pair
  region_crange(const Ct& container, const Predicate& pred)
  {
    return ::std::make_pair(region_begin(container, pred),
                            region_end(container, pred));
  }

/*  MACRO CLASS GENERATORS START HERE
 *
 *  The following part of the file contains macros definition in order to
 *  easily create classes such as \ref equal<Ct>, \ref open_region<Ct> or
 *  \ref enclosed<Ct> that are designed to facilitate expressions of iterators
 *  for these ranges.
 */

# define SPATIAL_REGION_SIMPLE_DECL(Name, Predicate)                     \
  template<typename Ct>                                                  \
  class Name                                                             \
  {                                                                      \
    typedef region                                                       \
      <Ct, Predicate<typename container_traits<Ct>::key_type,            \
                     typename container_traits<Ct>::key_compare> >       \
      Region;                                                            \
  public:                                                                \
    typedef typename Region::iterator            iterator;               \
    typedef typename Region::iterator_pair       iterator_pair;          \
  };                                                                     \
  template<typename Ct>                                                  \
  class Name<const Ct>                                                   \
  {                                                                      \
    typedef region                                                       \
      <const Ct, Predicate<typename container_traits<Ct>::key_type,      \
                           typename container_traits<Ct>::key_compare> > \
      Region;                                                            \
  public:                                                                \
    typedef typename Region::iterator            iterator;               \
    typedef typename Region::iterator_pair       iterator_pair;          \
  }

# define SPATIAL_REGION_LAYOUT_DECL(Name, Predicate)                     \
  template<typename Ct, typename Layout = llhh_layout_tag>               \
  class Name                                                             \
  {                                                                      \
    typedef region                                                       \
      <Ct, Predicate<typename container_traits<Ct>::key_type,            \
                     typename container_traits<Ct>::key_compare,         \
                     Layout> > Region;                                   \
  public:                                                                \
    typedef typename Region::iterator            iterator;               \
    typedef typename Region::iterator_pair       iterator_pair;          \
  };                                                                     \
  template<typename Ct, typename Layout>                                 \
  class Name<const Ct, Layout>                                           \
  {                                                                      \
    typedef region                                                       \
      <const Ct, Predicate<typename container_traits<Ct>::key_type,      \
                           typename container_traits<Ct>::key_compare,   \
                           Layout> > Region;                             \
  public:                                                                \
    typedef typename Region::iterator            iterator;               \
    typedef typename Region::iterator_pair       iterator_pair;          \
  }

  SPATIAL_REGION_SIMPLE_DECL(equal_region, equal_bounds);
  SPATIAL_REGION_SIMPLE_DECL(open_region, open_bounds);
  SPATIAL_REGION_SIMPLE_DECL(closed_region, closed_bounds);
  SPATIAL_REGION_LAYOUT_DECL(overlap_region, overlap_bounds);
  SPATIAL_REGION_LAYOUT_DECL(enclosed_region, enclosed_bounds);

# undef SPATIAL_REGION_SIMPLE_DECL
# undef SPATIAL_REGION_LAYOUT_DECL

/*  MACRO FUNCTION GENERATORS START HERE
 *
 *  The following part of the file contains macros definition in order to
 *  easily create functions such as begin_closed_region, end_closed_region,
 *  begin_equal, const_begin_enclosed, etc, as well as preset region types
 *  such as closed_region, enclosed, etc, that only take a container into
 *  account and are very conveniant to use.
 */

# define SPATIAL_REGION_MAKE_UNARY(Iter, Type, Region, Bounds)               \
  template <typename Ct>                                                     \
  inline typename region<Ct,                                                 \
                         Bounds<typename container_traits<Ct>::key_type,     \
                                typename container_traits<Ct>::key_compare>  \
                         >::Iter                                             \
  Region##_##Type(Ct& container,                                             \
                 typename container_traits<Ct>::key_type match)              \
  { return region_##Type(container, make_##Bounds(container, match)); }
# define SPATIAL_REGION_MAKE_UNARY_CONST(Iter, Type, Region, Bounds)         \
  template <typename Ct>                                                     \
  inline typename region<const Ct,                                           \
                         Bounds<typename container_traits<Ct>::key_type,     \
                                typename container_traits<Ct>::key_compare>  \
                         >::Iter                                             \
  Region##_##Type(const Ct& container,                                       \
                  typename container_traits<Ct>::key_type match)             \
  { return region_##Type(container, make_##Bounds(container, match)); }

SPATIAL_REGION_MAKE_UNARY(iterator, begin, equal_region, equal_bounds)
SPATIAL_REGION_MAKE_UNARY_CONST(iterator, begin, equal_region, equal_bounds)
SPATIAL_REGION_MAKE_UNARY_CONST(iterator, cbegin, equal_region, equal_bounds)
SPATIAL_REGION_MAKE_UNARY(iterator, end, equal_region, equal_bounds)
SPATIAL_REGION_MAKE_UNARY_CONST(iterator, end, equal_region, equal_bounds)
SPATIAL_REGION_MAKE_UNARY_CONST(iterator, cend, equal_region, equal_bounds)
SPATIAL_REGION_MAKE_UNARY(iterator_pair, range, equal_region, equal_bounds)
SPATIAL_REGION_MAKE_UNARY_CONST(iterator_pair, range, equal_region, equal_bounds)
SPATIAL_REGION_MAKE_UNARY_CONST(iterator_pair, crange, equal_region, equal_bounds)

# undef SPATIAL_REGION_MAKE_UNARY
# undef SPATIAL_REGION_MAKE_UNARY_CONST

# define SPATIAL_REGION_MAKE_BINARY(Iter, Type, Region, Bounds)              \
  template <typename Ct>                                                     \
  inline typename region<Ct,                                                 \
                         Bounds<typename container_traits<Ct>::key_type,     \
                                typename container_traits<Ct>::key_compare>  \
                         >::Iter                                             \
  Region##_##Type(Ct& container,                                             \
                  typename container_traits<Ct>::key_type lower,             \
                  typename container_traits<Ct>::key_type upper)             \
  { return region_##Type(container, make_##Bounds(container, lower, upper)); }

# define SPATIAL_REGION_MAKE_BINARY_CONST(Iter, Type, Region, Bounds)        \
  template <typename Ct>                                                     \
  inline typename region<const Ct,                                           \
                         Bounds<typename container_traits<Ct>::key_type,     \
                                typename container_traits<Ct>::key_compare>  \
                         >::Iter                                             \
  Region##_##Type(const Ct& container,                                       \
                  typename container_traits<Ct>::key_type lower,             \
                  typename container_traits<Ct>::key_type upper)             \
  { return region_##Type(container, make_##Bounds(container, lower, upper)); }

# define SPATIAL_REGION_MAKE_BINARY_LAYOUT(Iter, Type, Region, Bounds)       \
  template <typename Ct, typename Layout>                                    \
  inline typename region<Ct,                                                 \
                         Bounds<typename container_traits<Ct>::key_type,     \
                                typename container_traits<Ct>::key_compare,  \
                                Layout> >::Iter                              \
  Region##_##Type(Ct& container,                                             \
                 typename container_traits<Ct>::key_type lower,              \
                 typename container_traits<Ct>::key_type upper,              \
                 Layout layout)                                              \
  {                                                                          \
    return region_##Type(container,                                          \
                         make_##Bounds(container, lower, upper, layout));    \
  }

# define SPATIAL_REGION_MAKE_BINARY_CLAYOUT(Iter, Type, Region, Bounds)      \
  template <typename Ct, typename Layout>                                    \
  inline typename region<const Ct,                                           \
                         Bounds<typename container_traits<Ct>::key_type,     \
                                typename container_traits<Ct>::key_compare,  \
                                Layout> >::Iter                              \
  Region##_##Type(const Ct& container,                                       \
                  typename container_traits<Ct>::key_type lower,             \
                  typename container_traits<Ct>::key_type upper,             \
                  Layout layout)                                             \
  { return region_##Type(container, make_##Bounds(container, lower, upper, layout)); }

# define SPATIAL_REGION_MAKE_BINARY_SET(region, bounds)                      \
SPATIAL_REGION_MAKE_BINARY(iterator, begin, region, bounds)                  \
SPATIAL_REGION_MAKE_BINARY_CONST(iterator, begin, region, bounds)            \
SPATIAL_REGION_MAKE_BINARY_CONST(iterator, cbegin, region, bounds)           \
SPATIAL_REGION_MAKE_BINARY(iterator, end, region, bounds)                    \
SPATIAL_REGION_MAKE_BINARY_CONST(iterator, end, region, bounds)              \
SPATIAL_REGION_MAKE_BINARY_CONST(iterator, cend, region, bounds)             \
SPATIAL_REGION_MAKE_BINARY(iterator_pair, range, region, bounds)             \
SPATIAL_REGION_MAKE_BINARY_CONST(iterator_pair, range, region, bounds)       \
SPATIAL_REGION_MAKE_BINARY_CONST(iterator_pair, crange, region, bounds)

# define SPATIAL_REGION_MAKE_LAYOUT_SET(region, bounds)                      \
SPATIAL_REGION_MAKE_BINARY(iterator, begin, region, bounds)                  \
SPATIAL_REGION_MAKE_BINARY_CONST(iterator, begin, region, bounds)            \
SPATIAL_REGION_MAKE_BINARY_CONST(iterator, cbegin, region, bounds)           \
SPATIAL_REGION_MAKE_BINARY_LAYOUT(iterator, begin, region, bounds)           \
SPATIAL_REGION_MAKE_BINARY_CLAYOUT(iterator, begin, region, bounds)          \
SPATIAL_REGION_MAKE_BINARY_CLAYOUT(iterator, cbegin, region, bounds)         \
SPATIAL_REGION_MAKE_BINARY(iterator, end, region, bounds)                    \
SPATIAL_REGION_MAKE_BINARY_CONST(iterator, end, region, bounds)              \
SPATIAL_REGION_MAKE_BINARY_CONST(iterator, cend, region, bounds)             \
SPATIAL_REGION_MAKE_BINARY_LAYOUT(iterator, end, region, bounds)             \
SPATIAL_REGION_MAKE_BINARY_CLAYOUT(iterator, end, region, bounds)            \
SPATIAL_REGION_MAKE_BINARY_CLAYOUT(iterator, cend, region, bounds)           \
SPATIAL_REGION_MAKE_BINARY(iterator_pair, range, region, bounds)             \
SPATIAL_REGION_MAKE_BINARY_CONST(iterator_pair, range, region, bounds)       \
SPATIAL_REGION_MAKE_BINARY_CONST(iterator_pair, crange, region, bounds)      \
SPATIAL_REGION_MAKE_BINARY_LAYOUT(iterator_pair, range, region, bounds)      \
SPATIAL_REGION_MAKE_BINARY_CLAYOUT(iterator_pair, range, region, bounds)     \
SPATIAL_REGION_MAKE_BINARY_CLAYOUT(iterator_pair, crange, region, bounds)

SPATIAL_REGION_MAKE_BINARY_SET(region, bounds)
SPATIAL_REGION_MAKE_BINARY_SET(open_region, open_bounds)
SPATIAL_REGION_MAKE_BINARY_SET(closed_region, closed_bounds)
SPATIAL_REGION_MAKE_LAYOUT_SET(enclosed_region, enclosed_bounds)
SPATIAL_REGION_MAKE_LAYOUT_SET(overlap_region, overlap_bounds)

# undef SPATIAL_REGION_MAKE_BINARY
# undef SPATIAL_REGION_MAKE_BINARY_CONST
# undef SPATIAL_REGION_MAKE_BINARY_SET
# undef SPATIAL_REGION_MAKE_BINARY_LAYOUT
# undef SPATIAL_REGION_MAKE_BINARY_CLAYOUT
# undef SPATIAL_REGION_MAKE_LAYOUT_SET

} // namespace spatial

#include "spatial_region.tpp"

#endif // SPATIAL_REGION_HPP
