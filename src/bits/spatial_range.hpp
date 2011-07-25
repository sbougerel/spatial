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
    except::check_open_range_bounds(container, lower, upper);
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
    except::check_range_bounds(container, lower, upper);
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
    except::check_closed_range_bounds(container, lower, upper);
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
    except::check_box_argument(container, target, tag);
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
    except::check_box_argument(container, target, tag);
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
     *  @brief  Iterates over keys contained within an orthogonal range with
     *  in-order tree transveral.
     *
     *  Uses @c Predicate to find all matching keys. @c Predicate must be a
     *  model of RangePredicate. See the RangePredicate concept for more
     *  information on writing a range predicates.
     */
    template <typename Rank, typename Key, typename Value, typename Node,
              typename Predicate, bool Constant, typename Derived>
    struct Range_iterator_base
    {
      typedef Value                           value_type;
      typedef typename condition
      <Constant, const Value&, Value&>::type  reference;
      typedef typename details::condition
      <Constant, const Value*, Value*>::type  pointer;
      typedef std::ptrdiff_t                  difference_type;
      typedef std::bidirectional_iterator_tag iterator_category;

    protected:
      typedef typename condition
      <Constant, Node_base::Const_Base_ptr,
       Node_base::Base_ptr>::type             Base_ptr;
      typedef typename condition
      <Constant, const Node*, Node*>::type    Link_type;
      typedef Linker<Key, Value, Node>        Link_;

    public:
      /**
       *  @brief  The implementation that holds all members of the iterator.
       *
       *  To keep the iterator memory foot-print as low as possible, this class
       *  goes the extra mile to provide empty member optimization on all template
       *  paramerters.
       */
      struct Range_iterator_impl : Rank
      {
        Range_iterator_impl() : node_dim_(), node_() { }

        Range_iterator_impl
        (const Rank& rank, const Predicate& predicate,
         dimension_type node_dim, Base_ptr node)
          : Rank(rank), node_dim_(predicate, node_dim), node_(node)
        { }

        /**
         *  @brief  The dimension for node.
         *
         *  Modifing this key can potientially invalidate the iterator. Do not
         *  modify this key unless you know what you're doing. See node.
         */
        Compress<Predicate, dimension_type> node_dim_;

        /**
         *  @brief  The pointer to the current node for the iterator.
         *
         *  Modify this key can invalidate the iterator. Do not modify this key
         *  unless you know what you're doing. The key for node must always point
         *  to end() or to a node that statisfies:
         *
         *     Predicate(key) == matching
         */
        Base_ptr node_;
      };

      const Rank&
      rank() const
      { return *static_cast<const Rank*>(&impl_); }

      const Predicate&
      predicate() const
      { return impl_.node_dim_.base(); }

    private:
      void increment();

      void decrement();

    protected:
      Range_iterator_base() : impl_() { }

      Range_iterator_base(const Range_iterator_impl& impl)
        : impl_(impl)
      { }

    public:
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
      static Derived
      minimum(const Rank& rank, const Predicate& predicate,
              dimension_type node_dim, Base_ptr node);

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
      static Derived
      maximum(const Rank& rank, const Predicate& predicate,
              dimension_type node_dim, Base_ptr node);

      reference
      operator*() const
      { return static_cast<Link_type>(impl_.node_)->value; }

      pointer
      operator->() const
      { return &static_cast<Link_type>(impl_.node_)->value; }

      /**
       *  @brief  Find the next matching node in in-order transversal or
       *  return end. If the iterator is already at end, behavior is
       *  undertermined.
       */
      Derived&
      operator++()
      {
        increment();
        return *static_cast<Derived*>(this);
      }

      /**
       *  @brief  Find the next matching node in in-order transversal or
       *  point at end. If the iterator is already at end, behavior is
       *  undertermined.
       */
      Derived
      operator++(int)
      {
        Derived tmp = *static_cast<Derived*>(this);
        increment();
        return tmp;
      }

      /**
       *  @brief  Find the previous matching node in in-order transversal or
       *  return end. If the iterator is already at end, behavior is
       *  undertermined.
       */
      Derived&
      operator--()
      {
        decrement();
        return *static_cast<Derived*>(this);
      }

      /**
       *  @brief  Find the previous matching node in in-order transversal or
       *  point at end. If the iterator is already at end, behavior is
       *  undertermined.
       */
      Derived
      operator--(int)
      {
        Derived tmp = *static_cast<Derived*>(this);
        decrement();
        return tmp;
      }

      Range_iterator_impl impl_;
    };

    template <typename Rank, typename Key, typename Value, typename Node,
              typename Predicate, bool Constant1, bool Constant2,
              typename Derived1, typename Derived2>
    inline bool
    operator==(const Range_iterator_base
               <Rank, Key, Value, Node, Predicate, Constant1, Derived1>& x,
               const Range_iterator_base
               <Rank, Key, Value, Node, Predicate, Constant2, Derived2>& y)
    { return x.impl_.node_ == y.impl_.node_; }

    template <typename Rank, typename Key, typename Value, typename Node,
              typename Predicate, bool Constant1, bool Constant2,
              typename Derived1, typename Derived2>
    inline bool
    operator!=(const Range_iterator_base
               <Rank, Key, Value, Node, Predicate, Constant1, Derived1>& x,
               const Range_iterator_base
               <Rank, Key, Value, Node, Predicate, Constant2, Derived2>& y)
    { return !(x == y); }

    /**
     *  @brief  A mutable iterator based on Range_iterator_base.
     */
    template <typename Rank, typename Key, typename Value, typename Node,
              typename Predicate>
    struct Range_iterator
      : public Range_iterator_base
    <Rank, Key, Value, Node, Predicate, false,
     Range_iterator<Rank, Key, Value, Node, Predicate> >
    {
    private:
      typedef Range_iterator_base
      <Rank, Key, Value, Node, Predicate, false,
       Range_iterator<Rank, Key, Value, Node, Predicate> > Base;

    public:
      explicit
      Range_iterator(const Rank& r, const Predicate& p,
                     dimension_type node_dim, typename Base::Link_type link)
        : Base(typename Base::Range_iterator_impl(r, p, node_dim, link))
      { }

      Range_iterator() { }

      //@{
      /**
       *  This iterator can be casted silently into a container iterator. You can
       *  therefore use this iterator as an argument to the erase function of
       *  the container, for example.
       *
       *  @warning When using this iterator as an argument to the erase function
       *  of the container, this iterator will get invalidated after erase.
       */
      operator Node_iterator<Value, Node>()
      {
        return Node_iterator<Value, Node>
          (static_cast<typename Base::Link_type>(Base::impl_.node_));
      }

      operator Const_Node_iterator<Value, Node>()
      {
        return Const_Node_iterator<Value, Node>
          (static_cast<typename Base::Link_type>(Base::impl_.node_));
      }
      //@}
    };

    /**
     *  @brief  A constant iterator based on Range_iterator_base.
     */
    template <typename Rank, typename Key, typename Value, typename Node,
              typename Predicate>
    struct Const_Range_iterator
      : public Range_iterator_base
    <Rank, Key, Value, Node, Predicate, true,
     Const_Range_iterator<Rank, Key, Value, Node, Predicate> >
    {
    private:
      typedef Range_iterator
      <Rank, Key, Value, Node, Predicate>    iterator;

      typedef Range_iterator_base
      <Rank, Key, Value, Node, Predicate, true,
       Const_Range_iterator<Rank, Key, Value, Node, Predicate> > Base;

    public:
      explicit
      Const_Range_iterator
      (const Rank& r, const Predicate& p, dimension_type node_dim,
       typename Base::Link_type link)
        : Base(typename Base::Range_iterator_impl(r, p, node_dim, link))
      { }

      Const_Range_iterator() { }

      Const_Range_iterator(const iterator& i)
        : Base(typename Base::Range_iterator_impl
               (i.rank(), i.predicate(), i.impl_.node_dim_(), i.impl_.node_))
      { }

      /**
       *  This iterator can be casted silently into a container iterator. You can
       *  therefore use this iterator as an argument to the erase function of
       *  the container, for example.
       *
       *  @warning When using this iterator as an argument to the erase function
       *  of the container, this iterator will get invalidated after erase.
       */
      operator Const_Node_iterator<Value, Node>()
      {
        return Const_Node_iterator<Value, Node>
          (static_cast<typename Base::Link_type>(Base::impl_.node_));
      }
    };

    /**
     * Define the equal_iterator type that is used in the base tree structures
     * to search through a set of object of equivalent coordinates.
     */
    template<typename Container>
    struct equal_iterator
    {
      typedef spatial::details::Range_iterator
      <typename container_traits<Container>::rank_type,
       typename container_traits<Container>::key_type,
       typename container_traits<Container>::value_type,
       typename container_traits<Container>::node_type,
       equal_bounds<typename container_traits<Container>::key_type,
                    typename container_traits<Container>::key_compare> >
      type;
    };

    /**
     * Define the const_equal_iterator type that is used in the base tree
     * structures to search through a set of object of equivalent coordinates.
     */
    template<typename Container>
    struct const_equal_iterator
    {
      typedef spatial::details::Const_Range_iterator
      <typename container_traits<Container>::rank_type,
       typename container_traits<Container>::key_type,
       typename container_traits<Container>::value_type,
       typename container_traits<Container>::node_type,
       equal_bounds<typename container_traits<Container>::key_type,
                    typename container_traits<Container>::key_compare> >
        type;
    };

    namespace range
    {

      template<typename Container, typename Predicate>
      struct iterator
      {
        typedef Range_iterator
        <typename container_traits<Container>::rank_type,
         typename container_traits<Container>::key_type,
         typename container_traits<Container>::value_type,
         typename container_traits<Container>::node_type,
         Predicate> type;
      };

      template<typename Container, typename Predicate>
      struct const_iterator
      {
        typedef Const_Range_iterator
        <typename container_traits<Container>::rank_type,
         typename container_traits<Container>::key_type,
         typename container_traits<Container>::value_type,
         typename container_traits<Container>::node_type,
         Predicate> type;
      };

      template <typename Container, typename Predicate>
      inline typename iterator<Container, Predicate>::type
      end(Container& container, const Predicate& predicate)
      {
        return typename iterator<Container, Predicate>::type
          (container.rank(), predicate, container.dimension() - 1,
           static_cast<typename container_traits<Container>::node_type*>
           (get_end(container)));
      }

      template <typename Container, typename Predicate>
      inline typename const_iterator<Container, Predicate>::type
      const_end(const Container& container, const Predicate& predicate)
      {
        return end(*const_cast<Container*>(&container), predicate);
      }

      template <typename Container, typename Predicate>
      inline typename iterator<Container, Predicate>::type
      begin(Container& container, const Predicate& predicate)
      {
        // Guarentees begin(n) == end(n) if tree is empty
        if (container.empty())
        { return end(container, predicate); }
      else
        {
          return iterator<Container, Predicate>::type::minimum
            (container.rank(), predicate, 0,
             container.end().node->parent);
        }
      }

      template <typename Container, typename Predicate>
      inline typename const_iterator<Container, Predicate>::type
      const_begin(const Container& container, const Predicate& predicate)
      {
        return begin(*const_cast<Container*>(&container), predicate);
      }

    } // namespace range
  } // namespace details

  //@{
  /**
   *  @brief  View of the container that uses a predicate to control the
   *  orthogonal range search and provides standard iterator to access the
   *  results of the search. The predicate must be a model of @ref
   *  RangePredicate.
   *
   *  The following example shows how to define a predicate for a type called
   *  point3d. The predicate shall satisfy the following conditions:
   *
   *  @f[
   *  -1 < x _0 < 1, -\infty < x _1 < \infty, -\infty < x _2 < 2
   *  @f]
   *
   *  Supposing that @c point3d overloads the bracket operator to access its
   *  coordinates, the following predicate can satisfy the requirement above:
   *
   *  @code
   *  struct predicate
   *  {
   *    spatial::relative_order
   *    operator()(dimension_type dim, const point3d& x) const
   *    {
   *      switch(dim)
   *        {
   *        // for dimension 0, only the interval ]-1, 1[ matches...
   *        case 0: return x[0] <= -1 ? spatial::below
   *                       : (x[0] >= 1 ? spatial::above : spatial::matching );
   *        // for dimension 1, it's always a match...
   *        case 1: return spatial::matching;
   *        // for dimension 2, matches unless it's equal or above 2...
   *        case 2: return x[2] < 2 ? spatial::matching : spatial::above;
   *        // else we must be out of range...
   *        default: throw std::out_of_range();
   *        }
   *    }
   *  };
   *  @endcode
   *
   *  More information on predicate are provided in the tutorial section and the
   *  description of the RangePredicate concept.
   */
  template <typename Container, typename Predicate>
  class range_predicate_view
  {
    typedef typename spatial::container_traits<Container>   traits_type;

  public:
    // Container traits
    typedef typename traits_type::key_type            key_type;
    typedef typename traits_type::mapped_type         mapped_type;
    typedef typename traits_type::value_type          value_type;
    typedef typename traits_type::pointer             pointer;
    typedef typename traits_type::const_pointer       const_pointer;
    typedef typename traits_type::reference           reference;
    typedef typename traits_type::const_reference     const_reference;
    typedef typename traits_type::node_type           node_type;
    typedef typename traits_type::size_type           size_type;
    typedef typename traits_type::difference_type     difference_type;
    typedef typename traits_type::allocator_type      allocator_type;
    typedef typename traits_type::key_compare         key_compare;
    typedef typename traits_type::value_compare       value_compare;
    typedef typename traits_type::rank_type           rank_type;

    // Iterator types
    typedef typename spatial::details::condition
    <traits_type::const_iterator_tag::value,
     typename details::range::const_iterator<Container, Predicate>::type,
     typename details::range::iterator<Container, Predicate>::type
     >::type                                         iterator;
    typedef typename details::range::const_iterator
    <Container, Predicate>::type                     const_iterator;

    iterator begin()
    { return details::range::begin(*container_, predicate_); }

    const_iterator begin() const
    { return details::range::const_begin(*container_, predicate_); }

    const_iterator cbegin() const
    { return details::range::const_begin(*container_, predicate_); }

    iterator end()
    { return details::range::end(*container_, predicate_); }

    const_iterator end() const
    { return details::range::const_end(*container_, predicate_); }

    const_iterator cend() const
    { return details::range::const_end(*container_, predicate_); }

    /**
     *  Container must be a model of RangeIterable container.
     *  @see RangeIterable
     */
    range_predicate_view(Container& container, const Predicate& predicate)
      : predicate_(predicate), container_(&container)
    { }

  private:
    const Predicate predicate_;
    Container* container_;
  };

  // specialization for constant containers.
  template <typename Container, typename Predicate>
  class range_predicate_view<const Container, Predicate>
  {
    typedef typename spatial::container_traits<Container>   traits_type;

  public:
    // Container traits
    typedef typename traits_type::key_type            key_type;
    typedef typename traits_type::mapped_type         mapped_type;
    typedef typename traits_type::value_type          value_type;
    typedef typename traits_type::pointer             pointer;
    typedef typename traits_type::const_pointer       const_pointer;
    typedef typename traits_type::reference           reference;
    typedef typename traits_type::const_reference     const_reference;
    typedef typename traits_type::node_type           node_type;
    typedef typename traits_type::size_type           size_type;
    typedef typename traits_type::difference_type     difference_type;
    typedef typename traits_type::allocator_type      allocator_type;
    typedef typename traits_type::key_compare         key_compare;
    typedef typename traits_type::value_compare       value_compare;
    typedef typename traits_type::rank_type           rank_type;

    // Iterator types
    typedef typename details::range::const_iterator
    <Container, Predicate>::type                     iterator;
    typedef iterator                                 const_iterator;

    const_iterator begin() const
    { return details::range::const_begin(*container_, predicate_); }

    const_iterator cbegin() const
    { return details::range::const_begin(*container_, predicate_); }

    const_iterator end() const
    { return details::range::const_end(*container_, predicate_); }

    const_iterator cend() const
    { return details::range::const_end(*container_, predicate_); }

    range_predicate_view(const Container& container, const Predicate& predicate)
      : predicate_(predicate), container_(&container)
    { }

  private:
    const Predicate predicate_;
    const Container* container_;
  };
  //@}

  //@{
  /**
   *  A quick helper to return a range expressed through a pair of iterators.
   *  Uses the iterator types from @ref range_predicate_view.
   *
   *  Container must be a model of @ref RangeIterable. Predicate must be a model
   *  of @ref RangePredicate.
   */
  template <typename Container, typename Predicate>
  inline
  std::pair<typename range_predicate_view<Container, Predicate>::iterator,
            typename range_predicate_view<Container, Predicate>::iterator>
  range_predicate(Container& container, const Predicate& predicate)
  {
    return std::make_pair(details::range::begin(container, predicate),
                          details::range::end(container, predicate));
  }

  template <typename Container, typename Predicate>
  inline
  std::pair<typename range_predicate_view<Container, Predicate>::const_iterator,
            typename range_predicate_view<Container, Predicate>::const_iterator>
  range_predicate(const Container& container, const Predicate& predicate)
  {
    return std::make_pair(details::range::const_begin(container, predicate),
                          details::range::const_end(container, predicate));
  }

  template <typename Container, typename Predicate>
  inline
  std::pair<typename range_predicate_view<Container, Predicate>::const_iterator,
            typename range_predicate_view<Container, Predicate>::const_iterator>
  const_range_predicate(const Container& container, const Predicate& predicate)
  {
    return std::make_pair(details::range::const_begin(container, predicate),
                          details::range::const_end(container, predicate));
  }
  //@}

  //@{
  /**
   *  @brief  View of the Container that give access to all points whose
   *  coordinates are within the orthogonal area defined by the 2 points @p
   *  lower and @p upper, exluding @p upper. Provides iterators to get all
   *  results of the orthogonal range search.
   *
   *  The following condition is statisfied by the point returned as the
   *  result of the iteration of the view. This condition is the
   *  multi-dimension equivalent of an half closed interval, commonly used
   *  when programing, and thus, it is the one you are most likely to be
   *  using.
   *
   *  @f[
   *  \forall _{i = 0 \to n} , low _i \leq x _i < high _i
   *  @f]
   *
   *  The above invarient also implies that @p lower and @p upper bounds must
   *  not overlap on any dimension. An exception of the type
   *  spatial::invalid_range_bounds will be thrown otherwise.
   */
  template <typename Container>
  struct range_view
    : range_predicate_view
  <Container,
   range_bounds<typename spatial::container_traits<Container>::key_type,
                typename spatial::container_traits<Container>::key_compare> >
  {
    range_view
    (Container& container,
     const typename spatial::container_traits<Container>::key_type& lower,
     const typename spatial::container_traits<Container>::key_type& upper)
      : range_predicate_view
        <Container,
         range_bounds<typename spatial::container_traits<Container>::key_type,
                      typename spatial::container_traits<Container>::key_compare> >
        (container, make_range_bounds(container, lower, upper))
    { }
  };

  // specialization for constant containers.
  template <typename Container>
  struct range_view<const Container>
    : range_predicate_view
  <const Container,
   range_bounds<typename spatial::container_traits<Container>::key_type,
                typename spatial::container_traits<Container>::key_compare> >
  {
    range_view
    (const Container& container,
     const typename spatial::container_traits<Container>::key_type& lower,
     const typename spatial::container_traits<Container>::key_type& upper)
      : range_predicate_view
        <const Container,
         range_bounds<typename spatial::container_traits<Container>::key_type,
                      typename spatial::container_traits<Container>::key_compare> >
        (container, make_range_bounds(container, lower, upper))
    { }
  };
  //@}

  //@{
  /**
   *  A quick helper to return a range expressed through a pair of iterators.
   *  Uses the iterator types from @ref range_view.
   *
   *  Container must be a model of @ref RangeIterable.
   */
  template <typename Container>
  inline
  std::pair<typename range_view<Container>::iterator,
            typename range_view<Container>::iterator>
  range(Container& container,
        const typename spatial::container_traits<Container>::key_type& lower,
        const typename spatial::container_traits<Container>::key_type& upper)
  {
    range_bounds<typename container_traits<Container>::key_type,
                 typename container_traits<Container>::key_compare>
    bounds = make_range_bounds(container, lower, upper);
    return std::make_pair(details::range::begin(container, bounds),
                          details::range::end(container, bounds));
  }

  template <typename Container>
  inline
  std::pair<typename range_view<Container>::const_iterator,
            typename range_view<Container>::const_iterator>
  range(const Container& container,
        const typename spatial::container_traits<Container>::key_type& lower,
        const typename spatial::container_traits<Container>::key_type& upper)
  {
    range_bounds<typename container_traits<Container>::key_type,
                 typename container_traits<Container>::key_compare>
    bounds = make_range_bounds(container, lower, upper);
    return std::make_pair(details::range::const_begin(container, bounds),
                          details::range::const_end(container, bounds));
  }

  template <typename Container>
  inline
  std::pair<typename range_view<Container>::const_iterator,
            typename range_view<Container>::const_iterator>
  const_range(const Container& container,
              const typename spatial::container_traits<Container>::key_type& lower,
              const typename spatial::container_traits<Container>::key_type& upper)
  {
    range_bounds<typename container_traits<Container>::key_type,
                 typename container_traits<Container>::key_compare>
    bounds = make_range_bounds(container, lower, upper);
    return std::make_pair(details::range::const_begin(container, bounds),
                          details::range::const_end(container, bounds));
  }
  //@}

  //@{
  /**
   *  @brief  View of the Container that give access to all points whose
   *  coordinates are within the orthogonal area defined by the 2 points @p
   *  lower and @p upper, included. Provides iterators to get all results of
   *  the orthogonal range search.
   *
   *  The following condition is statisfied by the points returned as the
   *  result of the iteration of the view. This condition is the
   *  multi-dimension equivalent of a closed interval.
   *
   *  @f[
   *  \forall_{i = 0 \to n} \left( low_i \le x_i \le high_i \right)
   *  @f]
   *
   *  The above invarient also implies that @p lower and @p upper bounds must
   *  not overlap on any dimension. An exception of the type
   *  spatial::invalid_closed_range_bounds will be thrown otherwise.
   */
  template <typename Container>
  struct closed_range_view
    : range_predicate_view
  <Container,
   closed_range_bounds
   <typename spatial::container_traits<Container>::key_type,
    typename spatial::container_traits<Container>::key_compare> >
  {
    closed_range_view
    (Container& container,
     const typename spatial::container_traits<Container>::key_type& lower,
     const typename spatial::container_traits<Container>::key_type& upper)
      : range_predicate_view
        <Container,
         closed_range_bounds
         <typename spatial::container_traits<Container>::key_type,
          typename spatial::container_traits<Container>::key_compare> >
        (container, make_closed_range_bounds(container, lower, upper))
    { }
  };

  // specialization for constant containers.
  template <typename Container>
  struct closed_range_view<const Container>
    : range_predicate_view
  <const Container,
   closed_range_bounds
   <typename spatial::container_traits<Container>::key_type,
    typename spatial::container_traits<Container>::key_compare> >
  {
    closed_range_view
    (const Container& container,
     const typename spatial::container_traits<Container>::key_type& lower,
     const typename spatial::container_traits<Container>::key_type& upper)
      : range_predicate_view
        <const Container,
         closed_range_bounds
         <typename spatial::container_traits<Container>::key_type,
          typename spatial::container_traits<Container>::key_compare> >
        (container, make_closed_range_bounds(container, lower, upper))
    { }
  };
  //@}

  //@{
  /**
   *  A quick helper to return a closed_range expressed through a pair of
   *  iterators. Uses the iterator types from @ref closed_range_view.
   *
   *  Container must be a model of @ref RangeIterable.
   */
  template <typename Container>
  inline
  std::pair<typename closed_range_view<Container>::iterator,
            typename closed_range_view<Container>::iterator>
  closed_range
  (Container& container,
   const typename spatial::container_traits<Container>::key_type& lower,
   const typename spatial::container_traits<Container>::key_type& upper)
  {
    closed_range_bounds<typename container_traits<Container>::key_type,
                        typename container_traits<Container>::key_compare>
    bounds = make_closed_range_bounds(container, lower, upper);
    return std::make_pair(details::range::begin(container, bounds),
                          details::range::end(container, bounds));
  }

  template <typename Container>
  inline
  std::pair<typename closed_range_view<Container>::const_iterator,
            typename closed_range_view<Container>::const_iterator>
  closed_range
  (const Container& container,
   const typename spatial::container_traits<Container>::key_type& lower,
   const typename spatial::container_traits<Container>::key_type& upper)
  {
    closed_range_bounds<typename container_traits<Container>::key_type,
                        typename container_traits<Container>::key_compare>
    bounds = make_closed_range_bounds(container, lower, upper);
    return std::make_pair(details::range::const_begin(container, bounds),
                          details::range::const_end(container, bounds));
  }

  template <typename Container>
  inline
  std::pair<typename closed_range_view<Container>::const_iterator,
            typename closed_range_view<Container>::const_iterator>
  const_closed_range
  (const Container& container,
   const typename spatial::container_traits<Container>::key_type& lower,
   const typename spatial::container_traits<Container>::key_type& upper)
  {
    closed_range_bounds<typename container_traits<Container>::key_type,
                        typename container_traits<Container>::key_compare>
    bounds = make_closed_range_bounds(container, lower, upper);
    return std::make_pair(details::range::const_begin(container, bounds),
                          details::range::const_end(container, bounds));
  }
  //@}

  //@{
  /**
   *  @brief  View of the Container that give access to all points whose
   *  coordinates are within the orthogonal area defined by the 2 points @p
   *  lower and @p upper, excluded. Provides iterators to get all results of
   *  the orthogonal range search.
   *
   *  The following condition is statisfied by the points returned as the
   *  result of the iteration of the view. This condition is the
   *  multi-dimension equivalent of a open interval.
   *
   *  @f[
   *  \forall _{i = 0 \to n} , low _i < x _i < high _i
   *  @f]
   *
   *  The above invarient also implies that @p lower and @p upper bounds must
   *  not overlap on any dimension. An exception of the type
   *  spatial::invalid_open_range_bounds will be thrown otherwise.
   */
  template <typename Container>
  struct open_range_view
    : range_predicate_view
  <Container,
   open_range_bounds
   <typename spatial::container_traits<Container>::key_type,
    typename spatial::container_traits<Container>::key_compare> >
  {
    open_range_view
    (Container& container,
     const typename spatial::container_traits<Container>::key_type& lower,
     const typename spatial::container_traits<Container>::key_type& upper)
      : range_predicate_view
        <Container,
         open_range_bounds
         <typename spatial::container_traits<Container>::key_type,
          typename spatial::container_traits<Container>::key_compare> >
        (container, make_open_range_bounds(container, lower, upper))
    { }
  };

  // specialization for constant containers.
  template <typename Container>
  struct open_range_view<const Container>
    : range_predicate_view
  <const Container,
   open_range_bounds
   <typename spatial::container_traits<Container>::key_type,
    typename spatial::container_traits<Container>::key_compare> >
  {
    open_range_view
    (const Container& container,
     const typename spatial::container_traits<Container>::key_type& lower,
     const typename spatial::container_traits<Container>::key_type& upper)
      : range_predicate_view
        <const Container,
         open_range_bounds
         <typename spatial::container_traits<Container>::key_type,
          typename spatial::container_traits<Container>::key_compare> >
        (container, make_open_range_bounds(container, lower, upper))
    { }
  };
  //@}

  //@{
  /**
   *  A quick helper to return an open_range expressed through a pair of
   *  iterators. Uses the iterator types from @ref open_range_view.
   *
   *  Container must be a model of @ref RangeIterable.
   */
  template <typename Container>
  inline
  std::pair<typename open_range_view<Container>::iterator,
            typename open_range_view<Container>::iterator>
  open_range
  (Container& container,
   const typename spatial::container_traits<Container>::key_type& lower,
   const typename spatial::container_traits<Container>::key_type& upper)
  {
    open_range_bounds<typename container_traits<Container>::key_type,
                      typename container_traits<Container>::key_compare>
    bounds = make_open_range_bounds(container, lower, upper);
    return std::make_pair(details::range::begin(container, bounds),
                          details::range::end(container, bounds));
  }

  template <typename Container>
  inline
  std::pair<typename open_range_view<Container>::const_iterator,
            typename open_range_view<Container>::const_iterator>
  open_range
  (const Container& container,
   const typename spatial::container_traits<Container>::key_type& lower,
   const typename spatial::container_traits<Container>::key_type& upper)
  {
    open_range_bounds<typename container_traits<Container>::key_type,
                      typename container_traits<Container>::key_compare>
    bounds = make_open_range_bounds(container, lower, upper);
    return std::make_pair(details::range::const_begin(container, bounds),
                          details::range::const_end(container, bounds));
  }

  template <typename Container>
  inline
  std::pair<typename open_range_view<Container>::const_iterator,
            typename open_range_view<Container>::const_iterator>
  const_open_range
  (const Container& container,
   const typename spatial::container_traits<Container>::key_type& lower,
   const typename spatial::container_traits<Container>::key_type& upper)
  {
    open_range_bounds<typename container_traits<Container>::key_type,
                      typename container_traits<Container>::key_compare>
    bounds = make_open_range_bounds(container, lower, upper);
    return std::make_pair(details::range::const_begin(container, bounds),
                          details::range::const_end(container, bounds));
  }
  //@}

  //@{
  /**
   *  @brief  View of the Container that give access to all boxes whose
   *  coordinates overlap with that of a target box. Provides iterators to get
   *  all results of the search.
   *
   *  This view uses the @ref overlap_bounds predicate to perform the search.
   */
  template <typename Container, typename Layout = llhh_layout_tag>
  struct overlap_view
    : range_predicate_view
  <Container,
   overlap_bounds
   <typename spatial::container_traits<Container>::key_type,
    typename spatial::container_traits<Container>::key_compare, Layout> >
  {
    overlap_view
    (Container& container,
     const typename spatial::container_traits<Container>::key_type& target)
      : range_predicate_view
        <Container,
         overlap_bounds
         <typename spatial::container_traits<Container>::key_type,
          typename spatial::container_traits<Container>::key_compare, Layout> >
        (container, make_overlap_bounds(container, target, Layout()))
    { }
  };

  // specialization for constant containers.
  template <typename Container, typename Layout>
  struct overlap_view<const Container, Layout>
    : range_predicate_view
  <const Container,
   overlap_bounds
   <typename spatial::container_traits<Container>::key_type,
    typename spatial::container_traits<Container>::key_compare, Layout> >
  {
    overlap_view
    (const Container& container,
     const typename spatial::container_traits<Container>::key_type& target)
      : range_predicate_view
        <const Container,
         overlap_bounds
         <typename spatial::container_traits<Container>::key_type,
          typename spatial::container_traits<Container>::key_compare, Layout> >
        (container, make_overlap_bounds(container, target, Layout()))
    { }
  };
  //@}

  //@{
  /**
   *  A quick helper to return an overlap expressed through a pair of
   *  iterators. Uses the iterator types from @ref overlap_view.
   *
   *  Container must be a model of @ref RangeIterable.
   */
  template <typename Container, typename Layout>
  inline
  std::pair<typename overlap_view<Container, Layout>::iterator,
            typename overlap_view<Container, Layout>::iterator>
  overlap
  (Container& container,
   const typename spatial::container_traits<Container>::key_type& target,
   Layout)
  {
    overlap_bounds<typename container_traits<Container>::key_type,
                   typename container_traits<Container>::key_compare, Layout>
    bounds = make_overlap_bounds(container, target, Layout());
    return std::make_pair(details::range::begin(container, bounds),
                          details::range::end(container, bounds));
  }

  template <typename Container>
  inline
  std::pair<typename overlap_view<Container, llhh_layout_tag>::iterator,
            typename overlap_view<Container, llhh_layout_tag>::iterator>
  overlap
  (Container& container,
   const typename spatial::container_traits<Container>::key_type& target)
  {
    overlap_bounds<typename container_traits<Container>::key_type,
                   typename container_traits<Container>::key_compare,
                   llhh_layout_tag>
      bounds = make_overlap_bounds(container, target, llhh_layout_tag());
    return std::make_pair(details::range::begin(container, bounds),
                          details::range::end(container, bounds));
  }

  template <typename Container, typename Layout>
  inline
  std::pair<typename overlap_view<Container, Layout>::const_iterator,
            typename overlap_view<Container, Layout>::const_iterator>
  overlap
  (const Container& container,
   const typename spatial::container_traits<Container>::key_type& target,
   Layout)
  {
    overlap_bounds<typename container_traits<Container>::key_type,
                   typename container_traits<Container>::key_compare, Layout>
    bounds = make_overlap_bounds(container, target, Layout());
    return std::make_pair(details::range::const_begin(container, bounds),
                          details::range::const_end(container, bounds));
  }

  template <typename Container>
  inline
  std::pair<typename overlap_view<Container, llhh_layout_tag>::iterator,
            typename overlap_view<Container, llhh_layout_tag>::iterator>
  overlap
  (const Container& container,
   const typename spatial::container_traits<Container>::key_type& target)
  {
    overlap_bounds<typename container_traits<Container>::key_type,
                   typename container_traits<Container>::key_compare,
                   llhh_layout_tag>
      bounds = make_overlap_bounds(container, target, llhh_layout_tag());
    return std::make_pair(details::range::begin(container, bounds),
                          details::range::end(container, bounds));
  }

  template <typename Container, typename Layout>
  inline
  std::pair<typename overlap_view<Container, Layout>::const_iterator,
            typename overlap_view<Container, Layout>::const_iterator>
  const_overlap
  (const Container& container,
   const typename spatial::container_traits<Container>::key_type& target,
   Layout)
  {
    overlap_bounds<typename container_traits<Container>::key_type,
                   typename container_traits<Container>::key_compare, Layout>
    bounds = make_overlap_bounds(container, target, Layout());
    return std::make_pair(details::range::const_begin(container, bounds),
                          details::range::const_end(container, bounds));
  }

  template <typename Container>
  inline
  std::pair<typename overlap_view<Container, llhh_layout_tag>::iterator,
            typename overlap_view<Container, llhh_layout_tag>::iterator>
  const_overlap
  (const Container& container,
   const typename spatial::container_traits<Container>::key_type& target)
  {
    overlap_bounds<typename container_traits<Container>::key_type,
                   typename container_traits<Container>::key_compare,
                   llhh_layout_tag>
      bounds = make_overlap_bounds(container, target, llhh_layout_tag());
    return std::make_pair(details::range::begin(container, bounds),
                          details::range::end(container, bounds));
  }
  //@}

  //@{
  /**
   *  @brief  View of the Container that give access to all boxes whose
   *  coordinates enclosed with that of a target box. Provides iterators to get
   *  all results of the search.
   *
   *  This view uses the @ref enclose_bounds predicate to perform the search.
   */
  template <typename Container, typename Layout = llhh_layout_tag>
  struct enclose_view
    : range_predicate_view
  <Container,
   enclose_bounds
   <typename spatial::container_traits<Container>::key_type,
    typename spatial::container_traits<Container>::key_compare, Layout> >
  {
    enclose_view
    (Container& container,
     const typename spatial::container_traits<Container>::key_type& target)
      : range_predicate_view
        <Container,
         enclose_bounds
         <typename spatial::container_traits<Container>::key_type,
          typename spatial::container_traits<Container>::key_compare, Layout> >
        (container, make_enclose_bounds(container, target, Layout()))
    { }
  };

  // specialization for constant containers.
  template <typename Container, typename Layout>
  struct enclose_view<const Container, Layout>
    : range_predicate_view
  <const Container,
   enclose_bounds
   <typename spatial::container_traits<Container>::key_type,
    typename spatial::container_traits<Container>::key_compare, Layout> >
  {
    enclose_view
    (const Container& container,
     const typename spatial::container_traits<Container>::key_type& target)
      : range_predicate_view
        <const Container,
         enclose_bounds
         <typename spatial::container_traits<Container>::key_type,
          typename spatial::container_traits<Container>::key_compare, Layout> >
        (container, make_enclose_bounds(container, target, Layout()))
    { }
  };
  //@}

  //@{
  /**
   *  A quick helper to return an enclosed expressed through a pair of
   *  iterators. Uses the iterator types from @ref enclose_view.
   *
   *  Container must be a model of @ref RangeIterable.
   */
  template <typename Container, typename Layout>
  inline
  std::pair<typename enclose_view<Container, Layout>::iterator,
            typename enclose_view<Container, Layout>::iterator>
  enclose
  (Container& container,
   const typename spatial::container_traits<Container>::key_type& target,
   Layout)
  {
    enclose_bounds<typename container_traits<Container>::key_type,
                   typename container_traits<Container>::key_compare, Layout>
    bounds = make_enclose_bounds(container, target, Layout());
    return std::make_pair(details::range::begin(container, bounds),
                          details::range::end(container, bounds));
  }

  template <typename Container>
  inline
  std::pair<typename enclose_view<Container, llhh_layout_tag>::iterator,
            typename enclose_view<Container, llhh_layout_tag>::iterator>
  enclose
  (Container& container,
   const typename spatial::container_traits<Container>::key_type& target)
  {
    enclose_bounds<typename container_traits<Container>::key_type,
                   typename container_traits<Container>::key_compare,
                   llhh_layout_tag>
    bounds = make_enclose_bounds(container, target, llhh_layout_tag());
    return std::make_pair(details::range::begin(container, bounds),
                          details::range::end(container, bounds));
  }

  template <typename Container, typename Layout>
  inline
  std::pair<typename enclose_view<Container, Layout>::const_iterator,
            typename enclose_view<Container, Layout>::const_iterator>
  enclose
  (const Container& container,
   const typename spatial::container_traits<Container>::key_type& target,
   Layout)
  {
    enclose_bounds<typename container_traits<Container>::key_type,
                   typename container_traits<Container>::key_compare, Layout>
    bounds = make_enclose_bounds(container, target, Layout());
    return std::make_pair(details::range::const_begin(container, bounds),
                          details::range::const_end(container, bounds));
  }

  template <typename Container>
  inline
  std::pair<typename enclose_view<Container, llhh_layout_tag>::const_iterator,
            typename enclose_view<Container, llhh_layout_tag>::const_iterator>
  enclose
  (const Container& container,
   const typename spatial::container_traits<Container>::key_type& target)
  {
    enclose_bounds<typename container_traits<Container>::key_type,
                   typename container_traits<Container>::key_compare,
                   llhh_layout_tag>
    bounds = make_enclose_bounds(container, target, llhh_layout_tag());
    return std::make_pair(details::range::const_begin(container, bounds),
                          details::range::const_end(container, bounds));
  }

  template <typename Container, typename Layout>
  inline
  std::pair<typename enclose_view<Container, Layout>::const_iterator,
            typename enclose_view<Container, Layout>::const_iterator>
  const_enclose
  (const Container& container,
   const typename spatial::container_traits<Container>::key_type& target,
   Layout)
  {
    enclose_bounds<typename container_traits<Container>::key_type,
                   typename container_traits<Container>::key_compare,
                   Layout>
    bounds = make_enclose_bounds(container, target, Layout());
    return std::make_pair(details::range::const_begin(container, bounds),
                          details::range::const_end(container, bounds));
  }

  template <typename Container>
  inline
  std::pair<typename enclose_view<Container, llhh_layout_tag>::const_iterator,
            typename enclose_view<Container, llhh_layout_tag>::const_iterator>
  const_enclose
  (const Container& container,
   const typename spatial::container_traits<Container>::key_type& target)
  {
    enclose_bounds<typename container_traits<Container>::key_type,
                   typename container_traits<Container>::key_compare,
                   llhh_layout_tag>
    bounds = make_enclose_bounds(container, target, llhh_layout_tag());
    return std::make_pair(details::range::const_begin(container, bounds),
                          details::range::const_end(container, bounds));
  }
  //@}

} // namespace spatial

#include "spatial_range.tpp"

#endif // SPATIAL_RANGE_HPP
