// -*- C++ -*-

/**
 *  @file   spatial_neighbor.hpp
 *  @brief
 *
 *  Change log:
 *
 *  - 03-05-2010 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 */

#ifndef SPATIAL_NEIGHBOR_HPP
#define SPATIAL_NEIGHBOR_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#include "spatial_geometry.hpp"

namespace spatial
{
  namespace details
  {

    /**
     *  @brief A heavy iterator implementation meant to find nearest and
     *  furthest neighbors in a k-D tree where distances are applied in an
     *  arbitrary geometry space.
     *
     *  The Geometry type is a complex type that must comply to the following
     *  interface:
     *
     *  @code
     *  struct Geometry
     *  {
     *    typedef my_distance_type distance_type;                     // [1]
     *
     *    distance_type
     *    distance_to_key(dimension_type rank,
     *                    const Key& origin, const Key& key) const;   // [2]&[4]
     *
     *    distance_type
     *    distance_to_plane(dimension_type rank, dimension_type dim,
     *                      const Key& origin, const Key& key) const; // [3]&[4]
     *  };
     *  @code
     *
     *  The details of the Geometry type are as follow:
     *
     *  - [1] is the definition of the type used for the computation of the
     *  distance, generally defined as double.
     *
     *  - [2] is the general understanding of a distance: the amount of space
     *  that separates two points 'origin' and 'key' in the current frame of
     *  reference of dimension 'rank'.
     *
     *  - [3] represents the shortest possible distance between a point named
     *  'origin' and the plane that is orthogonal to the axis along the
     *  dimension 'dim' and that contains the point 'key'. While this may seem
     *  difficult to understand, in euclidian space, this operation is
     *  equivalent to the difference of the coordinate of 'origin' and 'key'
     *  along the dimension 'dim'.
     *
     *  - [4] for any 2 points 'origin' and 'key', [3] must always return a
     *  result that is lower or equal to [2], regardless of the dimension being
     *  considered. If rule [4] is not enforced in the geometry, the iterator
     *  will not work properly and skip some nodes.
     *
     *  Geometry are generally not defined by the user of the library given
     *  their complexity. Rather, the user of the library uses views or
     *  ready-made model of Geometry such as @c view::euclidian_double and @c
     *  view::manhattan. If another Geometry needs to be defined, the
     *  documentation should provide extensive help to design custom geometry
     *  types.
     */
    template <typename Rank, typename Key, typename Value, typename Node,
              typename Compare, typename Geometry, bool Constant,
              typename Derived>
    struct Neighbor_iterator_base
    {
      typedef Geometry                          geometry_type;
      typedef typename Geometry::distance_type  distance_type;
      typedef Value                             value_type;
      typedef typename details::condition
      <Constant, const Value&, Value&>::type    reference;
      typedef typename details::condition
      <Constant, const Value*, Value*>::type    pointer;
      typedef std::ptrdiff_t                    difference_type;
      typedef std::bidirectional_iterator_tag   iterator_category;

    protected:
      typedef typename details::condition
      <Constant, details::Node_base::Const_Base_ptr,
       details::Node_base::Base_ptr>::type      Base_ptr;
      typedef typename details::condition
      <Constant, const Node*, Node*>::type      Link_type;
      typedef Linker<Key, Value, Node>          Link_;

    public:
      /**
       *  @brief  The implementation that holds all members of the iterator.
       *
       *  To keep the iterator memory foot-print as low as possible, this class
       *  goes the extra mile to provide empty member optimization on all template
       *  paramerters.
       */
      struct Neighbor_iterator_impl : Rank
      {
        Neighbor_iterator_impl() { }

        Neighbor_iterator_impl
        (const Rank& rank, const Compare& cmp, const Geometry& geometry,
         const Key& target, dimension_type node_dim, Base_ptr node,
         const distance_type& distance = distance_type())
          : Rank(rank), target_(cmp, target), distance_(geometry, distance),
            node_dim_(node_dim), node_(node)
        { }

        /**
         *  @brief  The target point from which all distances are calculated
         *
         *  Modifying this key can potentially invalidate the iterator. Do not
         *  modify this key unless you know what you're doing. Use the read-only
         *  accessor origin() instead
         */
        details::Compress<Compare, Key> target_;

        /**
         *  @brief  The distance of the node from the origin.
         *
         *  Modifying this key can potentially invalidate the iterator. Do not
         *  modify this key unless you know what you're doing. Use the read-only
         *  accessor distance() instead.
         */
        details::Compress<Geometry, distance_type> distance_;

        /**
         *  @brief  The dimension for node.
         *
         *  Modifing this key can potientially invalidate the iterator. Do not
         *  modify this key unless you know what you're doing. See node.
         */
        dimension_type node_dim_;

        /**
         *  @brief  The pointer to the current node.
         *
         *  Modifying this key can potentially invalidate the iterator. Do not
         *  modify this key unless you know what you're doing. This iterator must
         *  always point to a node in the tree or to the end.
         */
        Base_ptr node_;
      };

      const Rank&
      rank() const
      { return *static_cast<const Rank*>(&impl_); }

      const Compare&
      compare() const
      { return impl_.target_.base(); }

      const Geometry&
      geometry() const
      { return impl_.distance_.base(); }

      const Key&
      target() const
      { return impl_.target_(); }

      const distance_type&
      distance() const
      { return impl_.distance_(); }

    private:
      void increment();

      void decrement();

    protected:
      Neighbor_iterator_base() : impl_() { }

      Neighbor_iterator_base(const Neighbor_iterator_impl& impl)
        : impl_(impl)
      { }

    public:
      /**
       *  @brief  Find the node closest to @p origin in the sub-tree of @p node.
       *  @param target The point of origin of the search.
       *  @param node_dim  The current dimension for the node x.
       *  @param node  The node from which to find the minimum.
       *  @param rank  The number of dimensions of the current space.
       *  @param cmp   The comparison function for the key.
       *  @param geometry The geometry being applied to the current space.
       *  @return  The iterator pointing the minimum, or to the parent of x.
       *
       *  @p node must be a tree node and not the header node or @c null.
       */
      static Derived
      minimum(const Rank& rank, const Compare& cmp, const Geometry& geometry,
              const Key& target, dimension_type node_dim, Base_ptr node);

      /**
       *  @brief  Find the node furthest to @p origin in the sub-tree of @p node.
       *  @param target The point of origin of the search.
       *  @param node_dim  The current dimension for the node x.
       *  @param node  The node from which to find the minimum.
       *  @param rank  The number of dimensions of the current space.
       *  @param cmp   The comparison function for the key.
       *  @param geometry The geometry being applied to the current space.
       *  @return  The iterator pointing the maximum, or to the parent of x.
       *
       *  @p node must be a tree node and not the header node or @c null.
       */
      static Derived
      maximum(const Rank& rank, const Compare& cmp, const Geometry& geometry,
              const Key& target, dimension_type node_dim, Base_ptr node);

      /**
       *  @brief  Find the node closest to @p origin with a distance greater or
       *  equal to @p lower_limit in the sub-tree of @p node,
       *  @param target The point of origin of the search.
       *  @param node_dim  The current dimension for the node x.
       *  @param node  The node from which to find the minimum.
       *  @param rank  The number of dimensions of the current space.
       *  @param cmp   The comparison function for the key.
       *  @param geometry The geometry being applied to the current space.
       *  @param lower_limit The minimum distance from @origin.
       *  @return  The iterator pointing the lower_bound, or to the parent of x.
       *
       *  @p node must be a tree node and not the header node or @c null.
       */
      static Derived
      lower_bound(const Rank& rank, const Compare& cmp, const Geometry& geometry,
                  const Key& target, distance_type lower_limit,
                  dimension_type node_dim, Base_ptr node);

      /**
       *  @brief  Find the node furthest from @p origin with a distance strictly
       *  less than @p upper_limit in the sub-tree of @p node,
       *  @param target The point of origin of the search.
       *  @param node_dim  The current dimension for the node x.
       *  @param node  The node from which to find the minimum.
       *  @param rank  The number of dimensions of the current space.
       *  @param cmp   The comparison function for the key.
       *  @param geometry The geometry being applied to the current space.
       *  @param upper_limit The maximum distance from @origin.
       *  @return  The iterator pointing the upper_bound, or to the parent of x.
       *
       *  @p node must be a tree node and not the header node or @c null.
       */
      static Derived
      upper_bound(const Rank& rank, const Compare& cmp, const Geometry& geometry,
                  const Key& target, distance_type upper_limit,
                  dimension_type node_dim, Base_ptr node);

      reference
      operator*() const
      { return static_cast<Link_type>(impl_.node_)->value; }

      pointer
      operator->() const
      { return &static_cast<Link_type>(impl_.node_)->value; }

      Derived&
      operator++()
      {
        increment();
        return *static_cast<Derived*>(this);
      }

      Derived
      operator++(int)
      {
        Derived tmp = *static_cast<Derived*>(this);
        increment();
        return tmp;
      }

      Derived&
      operator--()
      {
        decrement();
        return *static_cast<Derived*>(this);
      }

      Derived
      operator--(int)
      {
        Derived tmp = *static_cast<Derived*>(this);
        decrement();
        return tmp;
      }

      Neighbor_iterator_impl impl_;
    };

    template <typename Rank, typename Key, typename Value, typename Node,
              typename Compare, typename Geometry, bool Constant1,
              bool Constant2, typename Derived1, typename Derived2>
    inline bool
    operator==(const Neighbor_iterator_base<Rank, Key, Value, Node, Compare,
                                            Geometry, Constant1, Derived1>& x,
               const Neighbor_iterator_base<Rank, Key, Value, Node, Compare,
                                            Geometry, Constant2, Derived2>& y)
    { return x.impl_.node_ == y.impl_.node_; }

    template <typename Rank, typename Key, typename Value, typename Node,
              typename Compare, typename Geometry, bool Constant1,
              bool Constant2, typename Derived1, typename Derived2>
    inline bool
    operator!=(const Neighbor_iterator_base<Rank, Key, Value, Node, Compare,
                                            Geometry, Constant1, Derived1>& x,
               const Neighbor_iterator_base<Rank, Key, Value, Node, Compare,
                                            Geometry, Constant2, Derived2>& y)
    { return !(x == y); }

    /**
     *  @brief  A mutable iterator based on Neighbor_iterator_base.
     */
    template <typename Rank, typename Key, typename Value, typename Node,
              typename Compare, typename Geometry>
    struct Neighbor_iterator
      : Neighbor_iterator_base
    <Rank, Key, Value, Node, Compare, Geometry, false,
     Neighbor_iterator<Rank, Key, Value, Node, Compare, Geometry> >
    {
    private:
      typedef Neighbor_iterator_base
      <Rank, Key, Value, Node, Compare, Geometry, false,
       Neighbor_iterator
       <Rank, Key, Value, Node, Compare, Geometry> >   Base;
      typedef typename Base::Link_type                 Link_type;
      typedef typename Base::distance_type             distance_type;
      typedef typename Base::Neighbor_iterator_impl    impl_type;

    public:
      Neighbor_iterator(const Rank& r, const Compare& c, const Geometry& g,
                        const Key& k, dimension_type node_dim, Link_type link,
                        const distance_type& d = distance_type())
        : Base(impl_type(r, c, g, k, node_dim, link, d))
      { }

      Neighbor_iterator() { }

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
     *  @brief  A constant iterator based on Neighbor_iterator_base.
     */
    template <typename Rank, typename Key, typename Value, typename Node,
              typename Compare, typename Geometry>
    struct Const_Neighbor_iterator
      : Neighbor_iterator_base
    <Rank, Key, Value, Node, Compare, Geometry, true,
     Const_Neighbor_iterator<Rank, Key, Value, Node, Compare, Geometry> >
    {
    private:
      typedef Neighbor_iterator
      <Rank, Key, Value, Node, Compare, Geometry>       iterator;
      typedef Neighbor_iterator_base
      <Rank, Key, Value, Node, Compare, Geometry, true,
       Const_Neighbor_iterator
       <Rank, Key, Value, Node, Compare, Geometry> >  Base;
      typedef typename Base::Link_type                Link_type;
      typedef typename Base::distance_type            distance_type;
      typedef typename Base::Neighbor_iterator_impl   impl_type;

    public:
      Const_Neighbor_iterator
      (const Rank& r, const Compare& c, const Geometry& g, const Key& k,
       dimension_type node_dim, Link_type link,
       const distance_type& distance = distance_type())
        : Base(impl_type(r, c, g, k, node_dim, link, distance))
      { }

      Const_Neighbor_iterator() { }

      Const_Neighbor_iterator(const iterator& i)
        : Base(impl_type
               (i.rank(), i.compare(), i.geometry(),
                i.target(), i.distance(), i.impl.node_dim, i.impl.node))
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

    namespace neighbor
    {

      template<typename Container, typename Geometry>
      struct iterator
      {
        typedef spatial::details::Neighbor_iterator
        <typename container_traits<Container>::rank_type,
         typename container_traits<Container>::key_type,
         typename container_traits<Container>::value_type,
         typename container_traits<Container>::node_type,
         typename container_traits<Container>::compare_type,
         Geometry>
        type;
      };

      template<typename Container, typename Geometry>
      struct const_iterator
      {
        typedef spatial::details::Const_Neighbor_iterator
        <typename container_traits<Container>::rank_type,
         typename container_traits<Container>::key_type,
         typename container_traits<Container>::value_type,
         typename container_traits<Container>::node_type,
         typename container_traits<Container>::compare_type,
         Geometry>
        type;
      };

      template <typename Container, typename Geometry>
      inline typename iterator<Container, Geometry>::type
      end
      (Container& container, const Geometry& geometry,
       const typename container_traits<Container>::key_type& target)
      {
        return typename iterator<Container, Geometry>::type
          (container.rank(), container.compare(), geometry, target,
           container.dimension() - 1,
           static_cast<typename container_traits<Container>::node_type*>
           (get_end(container)));
      }

      template <typename Container, typename Geometry>
      inline typename const_iterator<Container, Geometry>::type
      const_end
      (const Container& container, const Geometry& geometry,
       const typename container_traits<Container>::key_type& target)
      {
        return end(*const_cast<Container*>(&container),
                   geometry, target);
      }

      template <typename Container, typename Geometry>
      inline typename iterator<Container, Geometry>::type
      begin
      (Container& container, const Geometry& geometry,
       const typename container_traits<Container>::key_type& target)
      {
        // Guarentees begin(n) == end(n) if tree is empty
        if (container.empty())
          { return end(container, geometry, target); }
        else
          {
            return iterator<Container, Geometry>::type::minimum
              (container.rank(), container.compare(), geometry, target,
               0, container.end().node->parent);
          }
      }

      template <typename Container, typename Geometry>
      inline typename const_iterator<Container, Geometry>::type
      const_begin
      (const Container& container, const Geometry& geometry,
       const typename container_traits<Container>::key_type& target)
      {
        return begin(*const_cast<Container*>(&container),
                     geometry, target);
      }

      template <typename Container, typename Geometry>
      inline typename iterator<Container, Geometry>::type
      lower_bound
      (Container& container, const Geometry& geometry,
       const typename container_traits<Container>::key_type& target,
       const typename geometry_traits<Geometry>::distance_type& lower)
      {
        if (container.empty())
          { return end(container, geometry, target); }
        else
          {
            return iterator<Container, Geometry>::type::lower_bound
              (container.rank(), container.compare(), geometry, target,
               lower, 0, container.end().node->parent);
          }
      }

      template <typename Container, typename Geometry>
      inline typename const_iterator<Container, Geometry>::type
      const_lower_bound
      (const Container& container, const Geometry& geometry,
       const typename container_traits<Container>::key_type& target,
       const typename geometry_traits<Geometry>::distance_type& lower)
      {
        return lower_bound(*const_cast<Container*>(&container),
                           geometry, target, lower);
      }

      template <typename Container, typename Geometry>
      inline typename iterator<Container, Geometry>::type
      upper_bound
      (Container& container, const Geometry& geometry,
       const typename container_traits<Container>::key_type& target,
       const typename geometry_traits<Geometry>::distance_type& upper)
      {
        if (container.empty())
          { return end(container, geometry, target); }
        else
          {
            return iterator<Container, Geometry>::type::upper_bound
              (container.rank(), container.compare(), geometry, target,
               upper, 0, container.end().node->parent);
          }
      }

      template <typename Container, typename Geometry>
      inline typename const_iterator<Container, Geometry>::type
      const_upper_bound
      (const Container& container, const Geometry& geometry,
       const typename container_traits<Container>::key_type& target,
       const typename geometry_traits<Geometry>::distance_type& upper)
      {
        return upper_bound(*const_cast<Container*>(&container),
                           geometry, target, upper);
      }

    } // namespace neighbor
  } // namespace details

  //@{
  /**
   *  @brief  View of the Kdtree that provides standard iterator accessors for
   *  kdtree types that inherit from @c Neighbor_iterable<Kdtree>.
   */
  template <typename Container, typename Geometry>
  class neighbor_view
  {
    typedef typename spatial::container_traits<Container>  traits_type;

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

    typedef typename Geometry::distance_type          distance_type;

    // Iterator types
    typedef typename spatial::details::condition
    <traits_type::const_iterator_tag::value,
     typename details::neighbor::const_iterator<Container, Geometry>::type,
     typename details::neighbor::iterator<Container, Geometry>::type
     >::type                                          iterator;
    typedef typename details::neighbor::const_iterator
    <Container, Geometry>::type                       const_iterator;
    typedef std::reverse_iterator<iterator>           reverse_iterator;
    typedef std::reverse_iterator<const_iterator>     const_reverse_iterator;

    iterator
    begin()
    { return details::neighbor::begin(*container_, geometry_, target_); }

    const_iterator
    begin() const
    { return details::neighbor::const_begin(*container_, geometry_, target_); }

    const_iterator
    cbegin() const
    { return details::neighbor::const_begin(*container_, geometry_, target_); }

    iterator
    end()
    { return details::neighbor::end(*container_, geometry_, target_); }

    const_iterator
    end() const
    { return details::neighbor::const_end(*container_, geometry_, target_); }

    const_iterator
    cend() const
    { return details::neighbor::const_end(*container_, geometry_, target_); }

    reverse_iterator
    rbegin()
    { return reverse_iterator(end()); }

    const_reverse_iterator
    rbegin() const
    { return reverse_iterator(cend()); }

    const_reverse_iterator
    crbegin() const
    { return reverse_iterator(cend()); }

    reverse_iterator
    rend()
    { return reverse_iterator(begin()); }

    const_reverse_iterator
    rend() const
    { return reverse_iterator(cbegin()); }

    const_reverse_iterator
    crend() const
    { return reverse_iterator(cbegin()); }

    iterator
    lower_bound(const distance_type& lower)
    { return details::neighbor::lower_bound(*container_, geometry_, target_, lower); }

    const_iterator
    lower_bound(const distance_type& lower) const
    { return details::neighbor::const_lower_bound(*container_, geometry_, target_, lower); }

    const_iterator
    clower_bound(const distance_type& lower) const
    { return details::neighbor::const_lower_bound(*container_, geometry_, target_, lower); }

    iterator
    upper_bound(const distance_type& upper)
    { return details::neighbor::upper_bound(*container_, geometry_, target_, upper); }

    const_iterator
    upper_bound(const distance_type& upper) const
    { return details::neighbor::const_upper_bound(*container_, geometry_, target_, upper); }

    const_iterator
    cupper_bound(const distance_type& upper) const
    { return details::neighbor::const_upper_bound(*container_, geometry_, target_, upper); }

    neighbor_view(Container& iterable, const Geometry& geometry,
                  const key_type& target)
      : target_(target), geometry_(geometry), container_(&iterable)
    { }

  private:
    key_type target_;
    Geometry geometry_;
    Container* container_;
  };

  // Specialization for constant container types.
  template <typename Container, typename Geometry>
  class neighbor_view<const Container, Geometry>
  {
    typedef typename spatial::container_traits<Container>  traits_type;

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

    typedef typename Geometry::distance_type          distance_type;

    typedef typename details::neighbor::const_iterator
    <Container, Geometry>::type                       iterator;
    typedef iterator                                  const_iterator;
    typedef std::reverse_iterator<iterator>           reverse_iterator;
    typedef reverse_iterator                          const_reverse_iterator;

    const_iterator
    begin() const
    { return details::neighbor::const_begin(*container_, geometry_, target_); }

    const_iterator
    cbegin() const
    { return details::neighbor::const_begin(*container_, geometry_, target_); }

    const_iterator
    end() const
    { return details::neighbor::const_end(*container_, geometry_, target_); }

    const_iterator
    cend() const
    { return details::neighbor::const_end(*container_, geometry_, target_); }

    const_reverse_iterator
    rbegin() const
    { return reverse_iterator(cend()); }

    const_reverse_iterator
    crbegin() const
    { return reverse_iterator(cend()); }

    const_reverse_iterator
    rend() const
    { return reverse_iterator(cbegin()); }

    const_reverse_iterator
    crend() const
    { return reverse_iterator(cbegin()); }

    const_iterator
    lower_bound(const distance_type& lower) const
    { return details::neighbor::const_lower_bound(*container_, geometry_, target_, lower); }

    const_iterator
    clower_bound(const distance_type& lower) const
    { return details::neighbor::const_lower_bound(*container_, geometry_, target_, lower); }

    const_iterator
    upper_bound(const distance_type& upper) const
    { return details::neighbor::const_upper_bound(*container_, geometry_, target_, upper); }

    const_iterator
    cupper_bound(const distance_type& upper) const
    { return details::neighbor::const_upper_bound(*container_, geometry_, target_, upper); }

    neighbor_view(const Container& iterable, const Geometry& geometry,
                  const key_type& target)
      : target_(target), geometry_(geometry), container_(&iterable)
    { }

  private:
    key_type target_;
    Geometry geometry_;
    const Container* container_;
  };
  //@}

  //@{
  template <typename Container>
  struct euclidian_neighbor_view
    : neighbor_view<Container, euclidian_double
                    <typename spatial::container_traits<Container>::key_type,
                     typename spatial::container_traits<Container>::compare_type> >
  {
    euclidian_neighbor_view
    (Container& iterable,
     const typename container_traits<Container>::key_type& origin)
      : neighbor_view<Container, euclidian_double
                      <typename container_traits<Container>::key_type,
                       typename container_traits<Container>::compare_type> >
    (iterable, euclidian_double
     <typename container_traits<Container>::key_type,
     typename container_traits<Container>::compare_type>
     (iterable.compare()), origin)
    { }
  };

  template <typename Container>
  struct euclidian_neighbor_view<const Container>
    : neighbor_view<const Container, euclidian_double
                    <typename spatial::container_traits<Container>::key_type,
                     typename spatial::container_traits<Container>::compare_type> >
  {
    euclidian_neighbor_view
    (const Container& iterable,
     const typename container_traits<Container>::key_type& origin)
      : neighbor_view<const Container, euclidian_double
                      <typename container_traits<Container>::key_type,
                       typename container_traits<Container>::compare_type> >
    (iterable, euclidian_double
     <typename container_traits<Container>::key_type,
     typename container_traits<Container>::compare_type>
     (iterable.compare()), origin)
    { }
  };
  //@}

  //@{
  template <typename Container>
  struct euclidian_float_neighbor_view
    : neighbor_view<Container, euclidian_float
                    <typename spatial::container_traits<Container>::key_type,
                     typename spatial::container_traits<Container>::compare_type> >
  {
    euclidian_float_neighbor_view
    (Container& iterable,
     const typename container_traits<Container>::key_type& origin)
      : neighbor_view<Container, euclidian_float
                      <typename container_traits<Container>::key_type,
                       typename container_traits<Container>::compare_type> >
    (iterable, euclidian_float
     <typename container_traits<Container>::key_type,
     typename container_traits<Container>::compare_type>
     (iterable.compare()), origin)
    { }
  };

  template <typename Container>
  struct euclidian_float_neighbor_view<const Container>
    : neighbor_view<const Container, euclidian_float
                    <typename spatial::container_traits<Container>::key_type,
                     typename spatial::container_traits<Container>::compare_type> >
  {
    euclidian_float_neighbor_view
    (const Container& iterable,
     const typename container_traits<Container>::key_type& origin)
      : neighbor_view<const Container, euclidian_float
                      <typename container_traits<Container>::key_type,
                       typename container_traits<Container>::compare_type> >
    (iterable, euclidian_float
     <typename container_traits<Container>::key_type,
     typename container_traits<Container>::compare_type>
     (iterable.compare()), origin)
    { }
  };
  //@}

  //@{
  template <typename Container>
  struct euclidian_square_neighbor_view
    : neighbor_view<Container, euclidian_square_double
                    <typename spatial::container_traits<Container>::key_type,
                     typename spatial::container_traits<Container>::compare_type> >
  {
    euclidian_square_neighbor_view
    (Container& iterable,
     const typename container_traits<Container>::key_type& origin)
      : neighbor_view<Container, euclidian_square_double
                      <typename container_traits<Container>::key_type,
                       typename container_traits<Container>::compare_type> >
    (iterable, euclidian_square_double
     <typename container_traits<Container>::key_type,
     typename container_traits<Container>::compare_type>
     (iterable.compare()), origin)
    { }
  };

  template <typename Container>
  struct euclidian_square_neighbor_view<const Container>
    : neighbor_view<const Container, euclidian_square_double
                    <typename spatial::container_traits<Container>::key_type,
                     typename spatial::container_traits<Container>::compare_type> >
  {
    euclidian_square_neighbor_view
    (const Container& iterable,
     const typename container_traits<Container>::key_type& origin)
      : neighbor_view<const Container, euclidian_square_double
                      <typename container_traits<Container>::key_type,
                       typename container_traits<Container>::compare_type> >
    (iterable, euclidian_square_double
     <typename container_traits<Container>::key_type,
     typename container_traits<Container>::compare_type>
     (iterable.compare()), origin)
    { }
  };
  //@}

  //@{
  template <typename Container>
  struct euclidian_square_float_neighbor_view
    : neighbor_view<Container, euclidian_square_float
                    <typename spatial::container_traits<Container>::key_type,
                     typename spatial::container_traits<Container>::compare_type> >
  {
    euclidian_square_float_neighbor_view
    (Container& iterable,
     const typename container_traits<Container>::key_type& origin)
      : neighbor_view<Container, euclidian_square_float
                      <typename container_traits<Container>::key_type,
                       typename container_traits<Container>::compare_type> >
    (iterable, euclidian_square_float
     <typename container_traits<Container>::key_type,
     typename container_traits<Container>::compare_type>
     (iterable.compare()), origin)
    { }
  };

  template <typename Container>
  struct euclidian_square_float_neighbor_view<const Container>
    : neighbor_view<const Container, euclidian_square_float
                    <typename spatial::container_traits<Container>::key_type,
                     typename spatial::container_traits<Container>::compare_type> >
  {
    euclidian_square_float_neighbor_view
    (const Container& iterable,
     const typename container_traits<Container>::key_type& origin)
      : neighbor_view<const Container, euclidian_square_float
                      <typename container_traits<Container>::key_type,
                       typename container_traits<Container>::compare_type> >
    (iterable, euclidian_square_float
     <typename container_traits<Container>::key_type,
     typename container_traits<Container>::compare_type>
     (iterable.compare()), origin)
    { }
  };
  //@}

  //@{
  template <typename Container, typename Distance>
  struct manhattan_neighbor_view
    : neighbor_view<Container, manhattan
                    <typename spatial::container_traits<Container>::key_type,
                     typename spatial::container_traits<Container>::compare_type,
                     Distance> >
  {
    manhattan_neighbor_view
    (Container& iterable,
     const typename container_traits<Container>::key_type& origin)
      : neighbor_view<Container, manhattan
                      <typename container_traits<Container>::key_type,
                       typename container_traits<Container>::compare_type,
                       Distance> >
    (iterable, manhattan
     <typename container_traits<Container>::key_type,
     typename container_traits<Container>::compare_type,
     Distance>
     (iterable.compare()), origin)
                       { }
  };

  template <typename Container, typename Distance>
  struct manhattan_neighbor_view<const Container, Distance>
    : neighbor_view<const Container, manhattan
                    <typename spatial::container_traits<Container>::key_type,
                     typename spatial::container_traits<Container>::compare_type,
                     Distance> >
  {
    manhattan_neighbor_view
    (const Container& iterable,
     const typename container_traits<Container>::key_type& origin)
      : neighbor_view<const Container, manhattan
                      <typename container_traits<Container>::key_type,
                       typename container_traits<Container>::compare_type,
                       Distance> >
    (iterable, manhattan
     <typename container_traits<Container>::key_type,
     typename container_traits<Container>::compare_type,
     Distance>
     (iterable.compare()), origin)
                       { }
  };
  //@}

} // namespace spatial

#include "spatial_neighbor.tpp"

#endif // SPATIAL_NEIGHBOR_HPP
