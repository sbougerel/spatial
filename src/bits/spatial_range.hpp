// -*- C++ -*-

/**
 *  @file   spatial_range.hpp
 *  @brief
 *
 *  Change Log:
 *
 *  - 2009-10-19 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_RANGE_HPP
#define SPATIAL_RANGE_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#include <utility> // provides ::std::pair<> and ::std::make_pair()

namespace spatial
{

  /**
   *  @brief  For all @c x in the set @c S, we define @c y such that
   *  orthogonal ranges iterates over all the @e{x} that satify: for all
   *  dimensions {0, ..., k-1 | yi <= xi && yi >= xi}. In other words, iterates
   *  orthogonally over all the range of elements in @c S that are equal to y
   *  with respect to all dimensions.
   *
   *  @concept equal_bounds is a model of RangePredicate.
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
     *  @brief  The operator that tells wheather the key is in range or not.
     */
    relative_order
    operator()(dimension_type dim, const Key& key, dimension_type) const
    {
      return (Compare::operator()(dim, key, match_)
              ? below
              : (Compare::operator()(dim, match_, key)
                 ? above
                 : matching));
    }

    /**
     *  @brief  The unique element that defines both lower and upper range for
     *  the equal range iterator.
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
   *  such that orthogonal ranges iterates over all the @e{x} that satify: for
   *  all dimensions {0, ..., k-1 | loweri <= xi && higheri >= xi}. In other
   *  words, iterates over all the range of elements in @{S} that are within the
   *  interval {lower, upper} with respect to all dimensions.
   *
   *  @concept open_range_bounds is a model of RangePredicate.
   */
  template <typename Key, typename Compare>
  struct open_range_bounds
    : private Compare // empty member optimization
  {
    /**
     *  @brief  The default constructor leaves everything un-initialized
     */
    open_range_bounds()
      : Compare(), lower_(), upper_() { }

    /**
     *  @brief  Set the lower and upper boundary for the orthogonal range
     *  search.
     *  @see make_open_bounds
     *
     *  The constructor does not check the lower and upper satisfy the following
     *  requierment: @c compare(d, lower, upper) must return true for all @c d
     *  for the keys @c lower and @c upper.
     */
    open_range_bounds(const Compare& compare, const Key& lower,
                      const Key& upper)
      : Compare(compare), lower_(lower), upper_(upper)
    { }

    /**
     *  @brief  The operator that tells wheather the point is in range or not.
     */
    relative_order
    operator()(dimension_type dim, const Key& key, dimension_type) const
    {
      return (!Compare::operator()(dim, lower_, key)
              ? below
              : (Compare::operator()(dim, key, upper_)
                 ? matching
                 : above));
    }

  private:
    /**
     *  @brief  The lower bound for the orthogonal range iterator.
     */
    Key lower_;

    /**
     *  @brief  The upper bound for the orthogonal range iterator.
     */
    Key upper_;
  };

  /**
   *  @brief  Open bound factory that takes in a @c container, a @c lower and @c
   *  upper key range, returning an <tt>open_bounds<Key, Compare></tt> type.
   *  @see equal_bounds<Key, Compare>
   *
   *  This factory also checks that lower and upper satisfy range requirements
   *  over an open interval for each dimension, e.g: <tt>compare.key_comp()(d,
   *  lower, upper)</tt> must be true for all value of @c d within 0 and @c
   *  container.dimension().
   */
  template <typename Tp>
  open_range_bounds<typename container_traits<Tp>::key_type,
                    typename container_traits<Tp>::key_compare>
  make_open_range_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& lower,
   const typename container_traits<Tp>::key_type& upper)
  {
    ::spatial::except::check_open_range_bounds(container, lower, upper);
    return open_range_bounds
      <typename container_traits<Tp>::key_type,
      typename container_traits<Tp>::key_compare>
      (container.key_comp(), lower, upper);
  }

  /**
   *  @brief  For all @e{x} in the set @e{S}, we define @e{lower} and @e{upper}
   *  such that orthogonal ranges iterates over all the @e{x} that satify: for
   *  all dimensions {0, ..., k-1 | loweri <= xi && higheri >= xi}. In other
   *  words, iterates over all the range of elements in @{S} that are within the
   *  interval {lower, upper} with respect to all dimensions.
   *
   *  @concept range_bounds is a model of RangePredicate.
   */
  template <typename Key, typename Compare>
  struct range_bounds
    : private Compare // empty member optimization
  {
    /**
     *  @brief  The default constructor leaves everything un-initialized
     */
    range_bounds()
      : Compare(), lower_(), upper_() { }

    /**
     *  @brief  Set the lower and upper boundary for the orthogonal range
     *  search.
     */
    range_bounds(const Compare& compare, const Key& lower, const Key& upper)
      : Compare(compare), lower_(lower), upper_(upper)
    { }

    /**
     *  @brief  The operator that tells wheather the point is in range or not.
     */
    relative_order
    operator()(dimension_type dim, const Key& key, dimension_type) const
    {
      return (Compare::operator()(dim, key, lower_)
              ? below
              : (Compare::operator()(dim, key, upper_)
                 ? matching
                 : above));
    }

  private:
    /**
     *  @brief  The lower bound for the orthogonal range iterator.
     */
    Key lower_;

    /**
     *  @brief  The upper bound for the orthogonal range iterator.
     */
    Key upper_;
  };

  /**
   *  @brief  Range bounds factory that takes in a @c container, a @c lower and
   *  @c upper key range, returning an @c open_bounds<Key, Compare> type.  @see
   *  equal_bounds<Key, Compare>
   *
   *  This factory also check that lower and upper satisfy range requirements
   *  over an regular interval for each dimension, e.g: @c
   *  container.key_comp()(d, lower, upper) must be true for all value of @c d
   *  within 0 and @c container.dimension() or container.key_comp()(d, upper,
   *  lower) must also be false (which mean the values are equal on that
   *  particular dimension).
   */
  template <typename Tp>
  range_bounds<typename container_traits<Tp>::key_type,
               typename container_traits<Tp>::key_compare>
  make_range_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& lower,
   const typename container_traits<Tp>::key_type& upper)
  {
    ::spatial::except::check_range_bounds(container, lower, upper);
    return range_bounds
      <typename container_traits<Tp>::key_type,
      typename container_traits<Tp>::key_compare>
      (container.key_comp(), lower, upper);
  }

  /**
   *  @brief  For all @e{x} in the set @e{S}, we define @e{lower} and @e{upper}
   *  such that orthogonal ranges iterates over all the @e{x} that satify: for
   *  all dimensions {0, ..., k-1 | loweri <= xi && higheri >= xi}. In other
   *  words, iterates over all the range of elements in @{S} that are within the
   *  interval {lower, upper} with respect to all dimensions.
   *
   *  @concept closed_range_bounds is a model of RangePredicate.
   */
  template <typename Key, typename Compare>
  struct closed_range_bounds
    : private Compare // empty member optimization
  {
    /**
     *  @brief  The default constructor leaves everything un-initialized
     */
    closed_range_bounds() : Compare(), lower_(), upper_() { }

    /**
     *  @brief  Set the lower and upper boundary for the orthogonal range
     *  search.
     *
     *  @throws a std::out_of_range if an element of lower is strictly greater
     *  than the corresponding element of upper with respect to their dimension.
     */
    closed_range_bounds(const Compare& compare, const Key& lower,
                        const Key& upper)
      : Compare(compare), lower_(lower), upper_(upper)
    { }

    /**
     *  @brief  The operator that tells wheather the point is in range or not.
     */
    relative_order
    operator()(dimension_type dim, const Key& key, dimension_type) const
    {
      return (Compare::operator()(dim, key, lower_)
              ? below
              : (Compare::operator()(dim, upper_, key)
                 ? above
                 : matching));
    }

  private:
    /**
     *  @brief  The lower bound for the orthogonal range iterator.
     */
    Key lower_;

    /**
     *  @brief  The upper bound for the orthogonal range iterator.
     */
    Key upper_;
  };

  /**
   *  @brief  Closed bounds factory that takes in a @c container, a @c lower and
   *  @c upper key range, returning an @c open_bounds<Key, Compare> type.  @see
   *  equal_bounds<Key, Compare>
   *
   *  This factory also check that lower and upper satisfy range requirements
   *  over an regular interval for each dimension, e.g: @c
   *  container.key_comp()(d, lower, upper) must be true for all value of @c d
   *  within 0 and @c container.dimension() or container.key_comp()(d, upper,
   *  lower) must also be false (which mean the values are equal on that
   *  particular dimension).
   */
  template <typename Tp>
  closed_range_bounds<typename container_traits<Tp>::key_type,
                      typename container_traits<Tp>::key_compare>
  make_closed_range_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& lower,
   const typename container_traits<Tp>::key_type& upper)
  {
    ::spatial::except::check_closed_range_bounds(container, lower, upper);
    return closed_range_bounds
      <typename container_traits<Tp>::key_type,
      typename container_traits<Tp>::key_compare>
      (container.key_comp(), lower, upper);
  }

  /**
   *  This range predicate matches keys that are overlap with a target
   *  box. The keys must represent boxes, not points.
   *
   *  @concept overlap_bounds is a model of RangePredicate.
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
   *  For a given target box @f$P_{(x, y)}@f$, this range predicate matches any
   *  box @f$B_{(x, y)}@f$ in a space of rank @f$r@f$ such as:
   *
   *  @f[
   *  \sum_{i=1}^{r} \left( B_{x_i} \le P_{x_i} \le B_{y_i}
   *                        \; or \; B_{x_i} \le P_{y_i} \le B_{y_i} \right)
   *  @f]
   */
  template <typename Rank, typename Key, typename Compare,
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
     *  @brief  The operator that tells wheather the point is in range or not.
     */
    relative_order
    operator()(dimension_type dim, const Key& key, dimension_type rank) const
    {
      return overlap_bounds_impl(rank, dim, key, Layout());
    }

  private:
    /**
     *  @brief  The box value that will be used for overlaping comparison.
     */
    Key target_;

    relative_order overlap_bounds_impl
    (dimension_type rank, dimension_type dim, const Key& key, llhh_layout_tag)
    const
    {
      return (dim < (rank >> 1))
        ? (Compare::operator()(dim + (rank >> 1), target_, dim, key)
           ? above : matching)
        : (Compare::operator()(dim, key, dim - (rank >> 1), target_)
           ? below : matching);
    }

    relative_order overlap_bounds_impl
    (dimension_type rank, dimension_type dim, const Key& key, lhlh_layout_tag)
    const
    {
      return ((dim % 2) == 0)
        ? (Compare::operator()(dim + 1, target_, dim, key) ? above : matching)
        : (Compare::operator()(dim, key, dim - 1, target_) ? below : matching);
    }

    relative_order overlap_bounds_impl
    (dimension_type rank, dimension_type dim, const Key& key, hhll_layout_tag)
    const
    {
      return (dim < (rank >> 1))
        ? (Compare::operator()(dim, key, dim + (rank >> 1), target_)
           ? below : matching)
        : (Compare::operator()(dim - (rank >> 1), target_, dim, key)
           ? above : matching);
    }

    relative_order overlap_bounds_impl
    (dimension_type rank, dimension_type dim, const Key& key, hlhl_layout_tag)
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
                 typename container_traits<Tp>::key_compare,
                 llhh_layout_tag>
  make_overlap_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& target)
  { return make_overlap_bounds(container, target, llhh_layout_tag()); }
  //@}

  /**
   *  This range predicate matches keys that are enclosed or equal to a target
   *  box. The keys must represent boxes, not points.
   *
   *  @concept enclose_bounds is a model of RangePredicate.
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
   *  For a given target box @f$P_{(x, y)}@f$, this range predicate matches any
   *  box @f$B_{(x, y)}@f$ in a space of rank @f$r@f$ such as:
   *
   *  @f[
   *  \sum_{i=1}^{r} \left( P_{x_i} \le B_{x_i} \; and \;
   *                        B_{y_i} \le P_{y_i} \right)
   *  @f]
   */
  template <typename Rank, typename Key, typename Compare,
            typename Layout = llhh_layout_tag>
  struct enclose_bounds
    : private Compare
  {
    /**
     *  @brief  The default constructor leaves everything un-initialized
     */
    enclose_bounds() : Compare(), target_() { }

    /**
     *  @brief  Set the target box and the comparator to the appropriate value.
     */
    enclose_bounds(const Compare& compare, const Key& target)
      : Compare(compare), target_(target)
    { }

    /**
     *  @brief  The operator that tells wheather the point is in range or not.
     */
    relative_order
    operator()(dimension_type dim, const Key& key, dimension_type rank) const
    {
      return enclose_bounds_impl(rank, dim, key, Layout());
    }

  private:
    /**
     *  @brief  The box value that will be used for the enclosing comparison.
     */
    Key target_;

    relative_order enclose_bounds_impl
    (dimension_type rank, dimension_type dim, const Key& key, llhh_layout_tag)
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
    (dimension_type rank, dimension_type dim, const Key& key, lhlh_layout_tag)
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
    (dimension_type rank, dimension_type dim, const Key& key, hhll_layout_tag)
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
    (dimension_type rank, dimension_type dim, const Key& key, hlhl_layout_tag)
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
  enclose_bounds<typename container_traits<Tp>::key_type,
                 typename container_traits<Tp>::key_compare,
                 Layout>
  make_enclose_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& target,
   Layout tag)
  {
    ::spatial::except::check_box(container, target, tag);
    return enclose_bounds
      <typename container_traits<Tp>::key_type,
      typename container_traits<Tp>::key_compare, Layout>
      (container.key_comp(), target);
  }

  template <typename Tp>
  enclose_bounds<typename container_traits<Tp>::key_type,
                 typename container_traits<Tp>::key_compare,
                 llhh_layout_tag>
  make_enclose_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& target)
  { return make_enclose_bounds(container, target, llhh_layout_tag()); }
  //@}

  namespace details
  {
    /**
     *  Extra information needed by the range iterator. This information is
     *  stored in the iterator is copied for each iterator.
     *
     *  Although it may be possible to modify this information directly from
     *  it's members, it may be unwise to do so, as it could invalidate the
     *  iterator and cause the program to behave unexpectedly. If any of this
     *  information needs to be modified, it is probably recommended to create a
     *  new iterator altogether.
     */
    typename <typename Container, typename Predicate>
    struct Range_data : ::spatial::container_trait<Container>::rank_type
    {
      //! Build an uninitialized range data object.
      Range_data() : node_dim_(), node_() { }

      /**
       *  Builds required range data from a given container, dimension, and a
       *  predicate.
       *
       *  \param p The model of \ref RangePredicate.
       *  \param d The node dimension for the node pointed to by the iterator.
       *  \param c The container being iterated.
       */
      Range_data
      (const Predicate& p, dimension_type d, const Container& c)
        : ::spatial::container_trait<Container>::rank_type(c.rank()),
          node_dim(p, d)
        { }

      /**
       *  The dimension for node.
       *
       *  \Warning Modifing this key can potientially invalidate the
       *  iterator. Do not modify this key unless you know what you're
       *  doing.
       */
      Compress<Predicate, dimension_type> node_dim;
    };
  } // namespace details

  /**
   *  The define_range type provides an iterator and a constant iterator to
   *  iterate through all element of a tree that match a particular user defined
   *  range through a predicate.
   *
   *  \tparam Container The container upon which these iterator relate to.
   *  \tparam Predicate A model of \ref RangePredicate.
   *  \see range_query<>::iterator
   *  \see range_query<>::const_iterator
   */
  template <typename Container, typename Predicate>
  struct specify_range
  {
    /**
     *  Iterates over values contained within the orthogonal range expressed
     *  in the \ref RangePredicate. The elements returned by this iterator are
     *  not ordered, but are guarrented to fall within the given Predicate
     *  provided.
     *
     *  Uses \ref Predicate to find all matching values. \ref Predicate must
     *  be a model of \ref RangePredicate. See the \ref RangePredicate concept
     *  for more information on writing a range predicates.
     */
    struct iterator : ::spatial::details::bidirectional_iterator
    <typename Container::mode_type, iterator,
     ::std::tr1::is_same<typename Container::key_type,
                         typename Container::value_type>::value>
    {
    private:
      typedef typename ::spatial::details::bidirectional_iterator
      <typename Container::mode_type, iterator,
       ::std::tr1::is_same<typename Container::key_type,
                           typename Container::value_type>::value> Base;

    public:
      //! Uninitialized iterator.
      iterator() { }

      /**
       *  Build an iterator from another iterator of the same container type.
       *
       *  This constructor should be used in the general case where the
       *  dimension for the node pointed to by \c iter is not known. The
       *  dimension of the node will be recomputed from the given iterator by
       *  iterating through all parents until the header node has been
       *  reached. This iteration is bounded by \Olog in case the tree is
       *  perfectly balanced.
       *
       *  \param predicate A model of the \ref RangePredicate concept.
       *  \param iter An iterator on the type Container.
       *  \param container The container being iterated.
       */
      iterator(const Predicate& predicate, typename Container::iterator iter,
               Container& container)
        : Base(iter.node),
          data(predicate, modulo(iter.node, container.rank()), container) { }

      /**
       *  Build an iterator from another iterator of the same container type.
       *
       *  This constructor should be used only when the dimension of the node
       *  pointed to by iter is known. If in doubt, use the other
       *  constructor. This constructor perform slightly faster than the other,
       *  since the dimension does not have to be calculated. Note however that
       *  the calculation of the dimension in the other iterator takes slightly
       *  longer than \Olog in general, and so it is not likely to affect the
       *  performance of your application in any major way.
       *
       *  \param predicate A model of the \ref RangePredicate concept.
       *  \param iter An iterator on the type Container.
       *  \param container The container being iterated.
       */
      iterator(const Predicate& predicate, dimension_type node_dim,
               typename Container::iterator iter, Container& container)
        : Base(iter.node), data(predicate, node_dim, container) { }

      //@{
      /**
       *  This iterator can be casted silently into a container iterator. You can
       *  therefore use this iterator as an argument to the erase function of
       *  the container, for example.
       *
       *  \warning When using this iterator as an argument to the erase function
       *  of the container, this iterator will get invalidated after erase.
       */
      operator typename Container::iterator()
      { return Container::iterator(link(Base::node)); }

      operator typename Container::const_iterator()
      { return Container::const_iterator(link(Base::node)); }
      //@}

      //! The related data for the iterator.
      ::spatial::details::Range_data<Container, Predicate> data;
    };

    /**
     *  Iterates over values contained within the orthogonal range expressed
     *  in the \ref RangePredicate. The elements returned by this iterator are
     *  not ordered, but are guarrented to fall within the given Predicate
     *  provided.
     *
     *  Uses \ref Predicate to find all matching values. \ref Predicate must
     *  be a model of \ref RangePredicate. See the \ref RangePredicate concept
     *  for more information on writing a range predicates.
     */
    struct const_iterator : ::spatial::details::bidirectional_iterator
    <typename Container::mode_type, const_iterator,
     ::std::tr1::is_same<typename Container::key_type, true> >
    {
    private:
      typedef typename ::spatial::details::bidirectional_iterator
      <typename Container::mode_type, const_iterator,
       ::std::tr1::is_same<typename Container::key_type, true> > Base;

    public:
      //! Uninitialized iterator.
      const_iterator() { }

      /**
       *  Build an iterator from another iterator of the same container type.
       *
       *  This constructor should be used in the general case where the
       *  dimension for the node pointed to by \c iter is not known. The
       *  dimension of the node will be recomputed from the given iterator by
       *  iterating through all parents until the header node has been
       *  reached. This iteration is bounded by \Olog in case the tree is
       *  perfectly balanced.
       *
       *  \param predicate A model of the \ref RangePredicate concept.
       *  \param iter An iterator on the type Container.
       *  \param container The container being iterated.
       */
      const_iterator(const Predicate& predicate,
                     typename Container::const_iterator iter,
                     Container& container)
        : Base(iter.node),
          data(predicate, modulo(iter.node, container.rank()), container) { }

      /**
       *  Build an iterator from another iterator of the same container type.
       *
       *  This constructor should be used only when the dimension of the node
       *  pointed to by iter is known. If in doubt, use the other
       *  constructor. This constructor perform slightly faster than the other,
       *  since the dimension does not have to be calculated. Note however that
       *  the calculation of the dimension in the other iterator takes slightly
       *  longer than \Olog in general, and so it is not likely to affect the
       *  performance of your application in any major way.
       *
       *  \param predicate A model of the \ref RangePredicate concept.
       *  \param iter An iterator on the type Container.
       *  \param container The container being iterated.
       */
      const_iterator(const Predicate& predicate, dimension_type node_dim,
                     typename Container::const_iterator iter,
                     Container& container)
        : Base(iter.node), data(predicate, node_dim, container) { }

      //! Convertion of an iterator into a const_iterator is permitted.
      const_iterator(const iterator& iter)
        : Base(iter.node), data(iter.data) { }

      /**
       *  This iterator can be casted silently into a container iterator. You can
       *  therefore use this iterator as an argument to the erase function of
       *  the container, for example.
       *
       *  \warning When using this iterator as an argument to the erase function
       *  of the container, this iterator will get invalidated after erase.
       */
      operator typename Container::const_iterator()
      { return Container::const_iterator(link(Base::node)); }

      //! The related data for the iterator.
      ::spatial::details::Range_data<Container, Predicate> data;
    };

    /**
     *  A pair of iterators that represents a range of elements to
     *  iterates. These elements are the results of the orthogonal range search.
     */
    typedef ::std::pair<iterator, iterator>             iterator_pair;

    /**
     *  A pair of constant iterators that represents a range of elements to
     *  iterates. These elements are the results of the orthogonal range search.
     */
    typedef ::std::pair<const_iterator, const_iterator> const_iterator_pair;
  };

# define SPATIAL_RANGE_SIMPLE_DECL(Name, Predicate)                     \
  template<typename Ct>                                                 \
  class Name                                                            \
  {                                                                     \
    typedef typename specify_range                                       \
      <Ct, Predicate<typename container_traits<Ct>::key_type,           \
                     typename container_traits<Ct>::key_compare> Range; \
  public:                                                               \
    typedef typename Range::iterator            iterator;               \
    typedef typename Range::const_iterator      const_iterator;         \
    typedef typename Range::iterator_pair       iterator_pair;          \
    typedef typename Range::const_iterator_pair const_iterator_pair;    \
  }

# define SPATIAL_RANGE_LAYOUT_DECL(Name, Predicate)                     \
  template<typename Ct, typename Layout = llhh_layout_tag>              \
  class Name                                                            \
  {                                                                     \
    typedef typename specify_range                                       \
      <Ct, Predicate<typename container_traits<Ct>::rank_type,          \
                     typename container_traits<Ct>::key_type,           \
                     typename container_traits<Ct>::key_compare,        \
                     Layout> Range;                                     \
  public:                                                               \
    typedef typename Range::iterator            iterator;               \
    typedef typename Range::const_iterator      const_iterator;         \
    typedef typename Range::iterator_pair       iterator_pair;          \
    typedef typename Range::const_iterator_pair const_iterator_pair;    \
  }

  SPATIAL_RANGE_SIMPLE_DECL(equal_range, equal_bounds);
  SPATIAL_RANGE_SIMPLE_DECL(open_range, open_range_bounds);
  SPATIAL_RANGE_SIMPLE_DECL(closed_range, closed_range_bounds);
  SPATIAL_RANGE_SIMPLE_DECL(range, range_bounds);
  SPATIAL_RANGE_LAYOUT_DECL(overlap_range, overlap_bounds);
  SPATIAL_RANGE_LAYOUT_DECL(enclose_range, enclose_bounds);

# undef SPATIAL_RANGE_SIMPLE_DECL
# undef SPATIAL_RANGE_LAYOUT_DECL

  namespace details
  {
    template <typename Container, typename Predicate>
    typename specify_range<Container, Predicate>::iterator&
    increment(typename specify_range<Container, Predicate>::iterator& iter);

    template <typename Container, typename Predicate>
    typename specify_range<Container, Predicate>::iterator&
    decrement(typename specify_range<Container, Predicate>::iterator& iter);

    /**
     *  @brief  From @c x, find the node with the minimum value in the range
     *  delimited by p. If multiple nodes are matching, return the first
     *  matching node in in-order transversal.
     *
     *  @param node_dim  The current dimension for @c node.
     *  @param node  The node from which to find the minimum.
     *  @param key_dimension  The number of dimensions of key.
     *  @param predicate  The predicate for the orthogonal range query.
     *  @return  An iterator pointing the minimum, or to the parent of @c node.
     *
     *  If @c node is a header node, the search will stop immediately.
     */
    template <typename Container, typename Predicate>
    typename specify_range<Container, Predicate>::iterator&
    minimum(typename specify_range<Container, Predicate>::iterator& iter);

    /**
     *  @brief  From @c x, find the node with the maximum value in the range
     *  delimited by p. If multiple nodes are matching, return the last
     *  matching node in in-order transversal.
     *
     *  @param node_dim  The current dimension for @c node.
     *  @param node  The node from which to find the minimum.
     *  @param key_dimension  The number of dimensions of key.
     *  @param predicate  The predicate for the orthogonal range query.
     *  @return  An iterator pointing the maximum, or to the parent of @c node.
     *
     *  If @c node is a header node, the search will stop immediately.
     */
    template <typename Container, typename Predicate>
    typename specify_range<Container, Predicate>::iterator&
    maximum(typename specify_range<Container, Predicate>::iterator& iter);

  } // namespace details

  template <typename Container, typename Predicate>
  inline typename specify_range<Container, Predicate>::iterator
  begin_range(const Predicate& pred, Container& container)
  {
    if (container.empty()) return end_range(pred, container);
    typename specify_range<Container, Predicate>::iterator
      it(pred, 0, container.top(), container); // At root, dim = 0
    return ::spatial::details::minimum(it);
  }

  template <typename Container, typename Predicate>
  inline typename specify_range<Container, Predicate>::const_iterator
  begin_range(const Predicate& pred, const Container& container)
  { return begin_range(pred, *const_cast<Container*>(&container)); }

  template <typename Container, typename Predicate>
  inline typename specify_range<Container, Predicate>::const_iterator
  const_begin_range(const Predicate& pred, const Container& container)
  { return begin_range(pred, *const_cast<Container*>(&container)); }

  template <typename Container, typename Predicate>
  inline typename specify_range<Container, Predicate>::iterator
  end_range(const Predicate& pred, Container& container)
  {
    return specify_range<Container, Predicate>::iterator
      (pred, container.dimension() - 1, container.end(),
       container); // At header, dim = rank - 1
  }

  template <typename Container, typename Predicate>
  inline typename specify_range<Container, Predicate>::const_iterator
  end_range(const Predicate& pred, const Container& container)
  { return end_range(pred, *const_cast<Container*>(&container)); }

  template <typename Container, typename Predicate>
  inline typename specify_range<Container, Predicate>::const_iterator
  const_end_range(const Predicate& pred, const Container& container)
  { return end_range(pred, *const_cast<Container*>(&container)); }

  template <typename Container, typename Predicate>
  inline typename specify_range<Container, Predicate>::iterator_pair
  make_range(const Predicate& pred, Container& container)
  {
    return ::std::make_pair(begin_range(pred, container),
                            end_range(pred, container));
  }

  template <typename Container, typename Predicate>
  inline typename specify_range<Container, Predicate>::const_iterator_pair
  make_range(const Predicate& pred, const Container& container)
  {
    return ::std::make_pair(begin_range(pred, container),
                            end_range(pred, container));
  }

  template <typename Container, typename Predicate>
  inline typename specify_range<Container, Predicate>::const_iterator_pair
  make_range(const Predicate& pred, const Container& container)
  {
    return ::std::make_pair(begin_range(pred, container),
                            end_range(pred, container));
  }

} // namespace spatial

#include "spatial_range.tpp"

#endif // SPATIAL_RANGE_HPP
