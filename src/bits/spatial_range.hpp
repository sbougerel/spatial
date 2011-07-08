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
    operator()(dimension_type dim, const Key& key) const
    {
      const Compare& compare = *static_cast<const Compare*>(this);
      return (compare(dim, key, match_)
              ? below
              : (compare(dim, match_, key)
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
               typename container_traits<Tp>::compare_type>
  make_equal_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& key)
  {
    return equal_bounds
      <typename container_traits<Tp>::key_type,
      typename container_traits<Tp>::compare_type>
      (container.compare(), key);
  }

  /**
   *  @brief  For all @e{x} in the set @e{S}, we define @e{lower} and @e{upper}
   *  such that orthogonal ranges iterates over all the @e{x} that satify: for
   *  all dimensions {0, ..., k-1 | loweri <= xi && higheri >= xi}. In other
   *  words, iterates over all the range of elements in @{S} that are within the
   *  interval {lower, upper} with respect to all dimensions.
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
    operator()(dimension_type dim, const Key& key) const
    {
      const Compare& compare = *static_cast<const Compare*>(this);
      return (!compare(dim, lower_, key)
              ? below
              : (compare(dim, key, upper_)
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
   *  upper key range, returning an @c open_bounds<Key, Compare> type.  @see
   *  equal_bounds<Key, Compare>
   *
   *  This factory also checks that lower and upper satisfy range requirements
   *  over an open interval for each dimension, e.g:
   *  @c compare.compare()(d, lower, upper) must be true for all value of @c d
   *  within 0 and @c tree.dimension().
   */
  template <typename Tp>
  open_range_bounds<typename container_traits<Tp>::key_type,
                    typename container_traits<Tp>::compare_type>
  make_open_range_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& lower,
   const typename container_traits<Tp>::key_type& upper)
  {
    except::check_open_range_bounds(container, lower, upper);
    return open_range_bounds
      <typename container_traits<Tp>::key_type,
      typename container_traits<Tp>::compare_type>
      (container.compare(), lower, upper);
  }

  /**
   *  @brief  For all @e{x} in the set @e{S}, we define @e{lower} and @e{upper}
   *  such that orthogonal ranges iterates over all the @e{x} that satify: for
   *  all dimensions {0, ..., k-1 | loweri <= xi && higheri >= xi}. In other
   *  words, iterates over all the range of elements in @{S} that are within the
   *  interval {lower, upper} with respect to all dimensions.
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
    operator()(dimension_type dim, const Key& key) const
    {
      const Compare& compare = *static_cast<const Compare*>(this);
      return (compare(dim, key, lower_)
              ? below
              : (compare(dim, key, upper_)
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
   *  container.compare()(d, lower, upper) must be true for all value of @c d
   *  within 0 and @c container.dimension() or container.compare()(d, upper,
   *  lower) must also be false (which mean the values are equal on that
   *  particular dimension).
   */
  template <typename Tp>
  range_bounds<typename container_traits<Tp>::key_type,
               typename container_traits<Tp>::compare_type>
  make_range_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& lower,
   const typename container_traits<Tp>::key_type& upper)
  {
    except::check_range_bounds(container, lower, upper);
    return range_bounds
      <typename container_traits<Tp>::key_type,
      typename container_traits<Tp>::compare_type>
      (container.compare(), lower, upper);
  }

  /**
   *  @brief  For all @e{x} in the set @e{S}, we define @e{lower} and @e{upper}
   *  such that orthogonal ranges iterates over all the @e{x} that satify: for
   *  all dimensions {0, ..., k-1 | loweri <= xi && higheri >= xi}. In other
   *  words, iterates over all the range of elements in @{S} that are within the
   *  interval {lower, upper} with respect to all dimensions.
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
    operator()(dimension_type dim, const Key& key) const
    {
      const Compare& compare = *static_cast<const Compare*>(this);
      return (compare(dim, key, lower_)
              ? below
              : (compare(dim, upper_, key)
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
   *  container.compare()(d, lower, upper) must be true for all value of @c d
   *  within 0 and @c container.dimension() or container.compare()(d, upper,
   *  lower) must also be false (which mean the values are equal on that
   *  particular dimension).
   */
  template <typename Tp>
  closed_range_bounds<typename container_traits<Tp>::key_type,
                      typename container_traits<Tp>::compare_type>
  make_closed_range_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& lower,
   const typename container_traits<Tp>::key_type& upper)
  {
    except::check_closed_range_bounds(container, lower, upper);
    return closed_range_bounds
      <typename container_traits<Tp>::key_type,
      typename container_traits<Tp>::compare_type>
      (container.compare(), lower, upper);
  }

  /**
   *  Represents a coordinate layout for the box. llhh stands for low, low,
   *  high, high. This layout means that the lower coordinates of the box are
   *  expressed first, and the higher coordinates are expressed later.
   *
   *  For a box of dimension 2, the llhh layout means that for each box,
   *  delmited by 2 points x and y, the coordinate are ordered as follow:
   *
   *      B = { x0, x1, y0, y1 }
   *
   */
  struct llhh_layout_tag { };

  /**
   *  Represents a coordinate layout for the box. lhlh stands for low, high,
   *  low, high. This layout means that the lower coordinates of the box are
   *  expressed first, and the higher coordinates are expressed later.
   *
   *  For a box of dimension 2, the lhlh layout means that for each box,
   *  delmited by 2 points x and y, the coordinate are ordered as follow:
   *
   *      B = { x0, y0, x1, y1 }
   *
   */
  struct lhlh_layout_tag { };

  /**
   *  Overlap bounds match boxes that overlaps. These bounds are only
   *  used for boxes, not points.
   */
  template <typename Key, typename Compare, typename Layout = llhh_layout_tag>
  struct overlap_bounds
    : private Compare
  {
    typedef Layout layout_category;

    /**
     *  @brief  The default constructor leaves everything un-initialized
     */
    overlap_bounds() : Compare(), target_() { }

    /**
     *  @brief  Set the target box and the comparator to the appropriate value.
     *
     *  @throws a std::out_of_range if an element of lower is strictly greater
     *  than the corresponding element of upper with respect to their dimension.
     */
    overlap_bounds(const Compare& compare, const Key& target)
      : Compare(compare), target_(target)
    { }

    /**
     *  @brief  The operator that tells wheather the point is in range or not.
     */
    relative_order
    operator()(dimension_type rank, dimension_type dim, const Key& key) const
    {
      return overlap_bounds_impl(rank, dim, key, Layout());
    }

  private:
    /**
     *  @brief  The lower bound for the orthogonal range iterator.
     */
    Key target_;

    /**
     *  Perform overlap tests for coordinates with llhh layout.
     */
    relative_order overlap_bounds_impl
    (dimension_type rank, dimension_type dim, const Key& key, llhh_layout_tag)
    {
      return (dim < (rank >> 1))
	? (Compare::operator()(dim + (rank >> 1), target_, dim, key)
	   ? above : matching)
	: (Compare::operator()(dim, key, dim - (rank >> 1), target_)
	   ? below : matching);
    }

    /**
     *  Perform overlap tests for coordinates with lhlh layout.
     */
    relative_order overlap_bounds_impl
    (dimension_type, dimension_type dim, const Key& key, lhlh_layout_tag)
    {
      return ((dim % 2) == 0)
	? (Compare::operator()(dim + 1, target_, dim, key) ? above : matching)
	: (Compare::operator()(dim, key, dim - 1, target_) ? below : matching);
    }
  };

  template <typename Key, typename Compare, typename Layout = llhh_layout_tag>
  struct enclose_bounds
  {
  };

  namespace details
  {

    /**
     *  @brief  Iterates over keys contained within an orthogonal range with
     *  in-order tree transveral.
     *
     *  Uses @c Predicate to tell whether a key is within the range or not. It
     *  iterates over keys that satisfy the following condition:
     *
     *    Predicate()(key) == tribool::match;
     *
     *  The complexity of the algorithm is at most O(m.log(n)) with m the number
     *  of keys within the range and n the number of keys in the tree.
     */
    template <typename Rank, typename Key, typename Node,
              typename Predicate, bool Constant, typename Derived>
    struct Range_iterator_base
    {
      typedef Key                             value_type;
      typedef typename details::condition
      <Constant, const Key&, Key&>::type      reference;
      typedef typename details::condition
      <Constant, const Key*, Key*>::type      pointer;
      typedef std::ptrdiff_t                  difference_type;
      typedef std::forward_iterator_tag       iterator_category;

    protected:
      typedef typename details::condition
      <Constant, details::Node_base::Const_Base_ptr,
       details::Node_base::Base_ptr>::type    Base_ptr;
      typedef typename details::condition
      <Constant, const Node*, Node*>::type    Link_type;
      typedef typename details::node_traits<Node>
      ::invariant_category                    invariant_category;

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
      { return static_cast<Link_type>(impl_.node_)->key_field; }

      pointer
      operator->() const
      { return &static_cast<Link_type>(impl_.node_)->key_field; }

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

    template <typename Rank, typename Key, typename Node,
              typename Predicate, bool Constant1, bool Constant2,
              typename Derived1, typename Derived2>
    inline bool
    operator==(const Range_iterator_base
               <Rank, Key, Node, Predicate, Constant1, Derived1>& x,
               const Range_iterator_base
               <Rank, Key, Node, Predicate, Constant2, Derived2>& y)
    { return x.impl_.node_ == y.impl_.node_; }

    template <typename Rank, typename Key, typename Node,
              typename Predicate, bool Constant1, bool Constant2,
              typename Derived1, typename Derived2>
    inline bool
    operator!=(const Range_iterator_base
               <Rank, Key, Node, Predicate, Constant1, Derived1>& x,
               const Range_iterator_base
               <Rank, Key, Node, Predicate, Constant2, Derived2>& y)
    { return !(x == y); }

    /**
     *  @brief  A mutable iterator based on Range_iterator_base.
     */
    template <typename Rank, typename Key, typename Node,
              typename Predicate>
    struct Range_iterator
      : public Range_iterator_base
    <Rank, Key, Node, Predicate, false,
     Range_iterator<Rank, Key, Node, Predicate> >
    {
    private:
      typedef Range_iterator_base
      <Rank, Key, Node, Predicate, false, Range_iterator
       <Rank, Key, Node, Predicate> > Base;

    public:
      explicit
      Range_iterator(const Rank& r, const Predicate& p,
                     dimension_type node_dim, typename Base::Link_type link)
        : Base(typename Base::Range_iterator_impl(r, p, node_dim, link))
      { }

      Range_iterator() { }
    };

    /**
     *  @brief  A constant iterator based on Range_iterator_base.
     */
    template <typename Rank, typename Key, typename Node,
              typename Predicate>
    struct Const_Range_iterator
      : public Range_iterator_base
    <Rank, Key, Node, Predicate, true,
     Const_Range_iterator<Rank, Key, Node, Predicate> >
    {
    private:
      typedef Range_iterator
      <Rank, Key, Node, Predicate>    iterator;

      typedef Range_iterator_base
      <Rank, Key, Node, Predicate, true, Const_Range_iterator
       <Rank, Key, Node, Predicate> > Base;

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
    };

    namespace range
    {

      template<typename Container, typename Predicate>
      struct iterator
      {
        typedef Range_iterator
        <typename container_traits<Container>::rank_type,
         typename container_traits<Container>::key_type,
         typename container_traits<Container>::node_type,
         Predicate> type;
      };

      template<typename Container, typename Predicate>
      struct const_iterator
      {
        typedef Const_Range_iterator
        <typename container_traits<Container>::rank_type,
         typename container_traits<Container>::key_type,
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

  /**
   *  @brief  Enforces the definition of the equal_iterable tag into a type.
   */
  template <typename Type>
  struct range_iterable_traits
    : spatial::details::identity<typename Type::range_iterable> { };

  //@{
  /**
   *  @brief  View of the Container that uses a predicate to control the
   *  orthogonal range search and provides standard iterator to access the
   *  results of the search.
   *
   *  The definition of the predicate must match the following prototype:
   *  @code
   *  struct my_predicate
   *  {
   *    spatial::relative_order
   *    operator()(spatial::dimension_type dim, const Key& key) const
   *    {
   *      // ...
   *    }
   *  };
   *  @endcode
   *
   *  With the type @p Key matching the type of objects inserted in the
   *  container. The type spatial::relative_order is an enumeration of 3
   *  values: above, matching and below that represent the position of the @p
   *  key with regard to the interval being considered on the dimension @p
   *  dim.
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
   *    // for dimension 0, only the interval ]-1, 1[ matches...
   *        case 0: return x[0] <= -1 ? spatial::below
   *                       : (x[0] >= 1 ? spatial::above : spatial::matching );
   *    // for dimension 1, it's always a match...
   *        case 1: return spatial::matching;
   *    // for dimension 2, matches unless it's equal or above 2...
   *        case 2: return x[2] < 2 ? spatial::matching : spatial::above;
   *    // else we must be out of range...
   *        default: throw std::out_of_range();
   *        }
   *    }
   *  };
   *  @endcode
   *
   *  As you can see, the enumeration of the type spatial::relative_order are
   *  used to represent the position of the parameter @p x with regard to the
   *  interval studied on the dimension @p dim.
   */
  template <typename Container, typename Predicate>
  class range_predicate_view
  {
    typedef typename spatial::container_traits<Container>   traits_type;

  public:
    typedef typename traits_type::key_type           key_type;
    typedef typename traits_type::pointer            pointer;
    typedef typename traits_type::const_pointer      const_pointer;
    typedef typename traits_type::reference          reference;
    typedef typename traits_type::const_reference    const_reference;
    typedef typename traits_type::node_type          node_type;
    typedef typename traits_type::size_type          size_type;
    typedef typename traits_type::difference_type    difference_type;
    typedef typename traits_type::allocator_type     allocator_type;
    typedef typename traits_type::compare_type       compare_type;
    typedef typename traits_type::rank_type          rank_type;

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

    range_predicate_view(typename range_iterable_traits<Container>
                         ::type& iterable, const Predicate& predicate)
      : predicate_(predicate), container_(&iterable)
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
    typedef typename traits_type::key_type           key_type;
    typedef typename traits_type::const_pointer      pointer;
    typedef typename traits_type::const_pointer      const_pointer;
    typedef typename traits_type::const_reference    reference;
    typedef typename traits_type::const_reference    const_reference;
    typedef typename traits_type::size_type          size_type;
    typedef typename traits_type::difference_type    difference_type;
    typedef typename traits_type::allocator_type     allocator_type;
    typedef typename traits_type::compare_type       compare_type;
    typedef typename traits_type::rank_type          rank_type;

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

    range_predicate_view(const typename range_iterable_traits<Container>
                         ::type& iterable, const Predicate& predicate)
      : predicate_(predicate), container_(&iterable)
    { }

  private:
    const Predicate predicate_;
    const Container* container_;
  };
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
                typename spatial::container_traits<Container>::compare_type> >
  {
    range_view
    (typename range_iterable_traits<Container>::type& iterable,
     const typename spatial::container_traits<Container>::key_type& lower,
     const typename spatial::container_traits<Container>::key_type& upper)
      : range_predicate_view
        <Container,
         range_bounds<typename spatial::container_traits<Container>::key_type,
                      typename spatial::container_traits<Container>::compare_type> >
        (iterable, make_range_bounds(iterable, lower, upper))
    { }
  };

  // specialization for constant containers.
  template <typename Container>
  struct range_view<const Container>
    : range_predicate_view
  <const Container,
   range_bounds<typename spatial::container_traits<Container>::key_type,
                typename spatial::container_traits<Container>::compare_type> >
  {
    range_view
    (const typename range_iterable_traits<Container>::type& iterable,
     const typename spatial::container_traits<Container>::key_type& lower,
     const typename spatial::container_traits<Container>::key_type& upper)
      : range_predicate_view
        <const Container,
         range_bounds<typename spatial::container_traits<Container>::key_type,
                      typename spatial::container_traits<Container>::compare_type> >
        (iterable, make_range_bounds(iterable, lower, upper))
    { }
  };
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
   *  \forall _{i = 0 \to n} , low _i \leq x _i \leq high _i
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
    typename spatial::container_traits<Container>::compare_type> >
  {
    closed_range_view
    (typename range_iterable_traits<Container>::type& iterable,
     const typename spatial::container_traits<Container>::key_type& lower,
     const typename spatial::container_traits<Container>::key_type& upper)
      : range_predicate_view
        <Container,
         closed_range_bounds
         <typename spatial::container_traits<Container>::key_type,
          typename spatial::container_traits<Container>::compare_type> >
        (iterable, make_closed_range_bounds(iterable, lower, upper))
    { }
  };

  // specialization for constant containers.
  template <typename Container>
  struct closed_range_view<const Container>
    : range_predicate_view
  <const Container,
   closed_range_bounds
   <typename spatial::container_traits<Container>::key_type,
    typename spatial::container_traits<Container>::compare_type> >
  {
    closed_range_view
    (const typename range_iterable_traits<Container>::type& iterable,
     const typename spatial::container_traits<Container>::key_type& lower,
     const typename spatial::container_traits<Container>::key_type& upper)
      : range_predicate_view
        <const Container,
         closed_range_bounds
         <typename spatial::container_traits<Container>::key_type,
          typename spatial::container_traits<Container>::compare_type> >
        (iterable, make_closed_range_bounds(iterable, lower, upper))
    { }
  };
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
    typename spatial::container_traits<Container>::compare_type> >
  {
    open_range_view
    (typename range_iterable_traits<Container>::type& iterable,
     const typename spatial::container_traits<Container>::key_type& lower,
     const typename spatial::container_traits<Container>::key_type& upper)
      : range_predicate_view
        <Container,
         open_range_bounds
         <typename spatial::container_traits<Container>::key_type,
          typename spatial::container_traits<Container>::compare_type> >
        (iterable, make_open_range_bounds(iterable, lower, upper))
    { }
  };

  // specialization for constant containers.
  template <typename Container>
  struct open_range_view<const Container>
    : range_predicate_view
  <const Container,
   open_range_bounds
   <typename spatial::container_traits<Container>::key_type,
    typename spatial::container_traits<Container>::compare_type> >
  {
    open_range_view
    (const typename range_iterable_traits<Container>::type& iterable,
     const typename spatial::container_traits<Container>::key_type& lower,
     const typename spatial::container_traits<Container>::key_type& upper)
      : range_predicate_view
        <const Container,
         open_range_bounds
         <typename spatial::container_traits<Container>::key_type,
          typename spatial::container_traits<Container>::compare_type> >
        (iterable, make_open_range_bounds(iterable, lower, upper))
    { }
  };
  //@}

} // namespace spatial

#endif // SPATIAL_RANGE_HPP
