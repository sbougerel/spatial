// -*- C++ -*-

/**
 *  @file   spatial_mapping.hpp
 *  @brief
 *
 *  Change log:
 *
 *  - 31-10-2009 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 */

#ifndef SPATIAL_MAPPING_HPP
#define SPATIAL_MAPPING_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

namespace spatial
{
  namespace details
  {

    /**
     *  @brief  A heavy iterator implementation meant to optimize transversal of
     *  all nodes in the tree when projected on a single dimension.
     *
     *  With this iterator, a n-dimensional K-d tree behaves like n different sets
     *  with similar data. Iteration through the tree is very efficient when the
     *  dimension K-d tree is very small by comparison to the number of objects,
     *  but pretty inefficient otherwise, by comparison to a set.
     */
    template <typename Rank, typename Key, typename Value, typename Node,
              typename Compare, bool Constant, typename Derived>
    struct Mapping_iterator_base
    {
      typedef Value                              value_type;
      typedef typename condition
      <Constant, const Value&, Value&>::type     reference;
      typedef typename condition
      <Constant, const Value*, Value*>::type     pointer;
      typedef std::ptrdiff_t                     difference_type;
      typedef std::bidirectional_iterator_tag    iterator_category;

    protected:
      typedef typename condition
      <Constant, Node_base::Const_Base_ptr,
       Node_base::Base_ptr>::type                Base_ptr;
      typedef typename condition
      <Constant, const Node*, Node*>::type       Link_type;
      typedef typename node_traits<Node>
      ::invariant_category                       invariant_category;
      typedef Linker<Key, Value, Node>           Link_;

    public:
      /**
       *  @brief  The implementation that holds all members of the iterator.
       *
       *  To keep the iterator memory foot-print as low as possible, this class
       *  goes the extra mile to provide empty member optimization on all template
       *  paramerters.
       */
      struct Mapping_iterator_impl : Rank
      {
        Mapping_iterator_impl()
          : mapping_dim_(), node_dim_(), node_() { }

        Mapping_iterator_impl
        (const Rank& rank, const Compare& compare,
         dimension_type mapping_dim, dimension_type node_dim, Base_ptr node)
          : Rank(rank), mapping_dim_(compare, mapping_dim),
            node_dim_(node_dim), node_(node)
        { }

        /**
         *  @brief  The current dimension of iteration.
         *
         *  You can modify this key if you suddenly want the iterator to change
         *  dimension of iteration. However this field must always satisfy:
         *
         *     mapping_dim() < Rank()
         *
         *  Rank being the template rank provider for the iterator.
         */
        Compress<Compare, dimension_type> mapping_dim_;

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
      { return impl_.mapping_dim_.base(); }

    private:
      void increment();

      void decrement();

    protected:
      Mapping_iterator_base() : impl_() { }

      Mapping_iterator_base(const Mapping_iterator_impl& impl)
        : impl_(impl)
      { }

    public:
      /**
       *  @brief  From @c node, find the node with the minimum value along the
       *  dimension @c mapping_dim.
       *  @param mapping_dim  The mapping dimension.
       *  @param node_dim  The current dimension for the node x.
       *  @param node  The node from which to find the minimum.
       *  @param key_dimension  The number of dimensions of the key.
       *  @param compare  The comparison function for the key.
       *  @return  The iterator pointing the minimum, or to the parent of x.
       *
       *  @c node must be a tree node and not the header node or @c null.
       */
      static Derived
      minimum(const Rank& rank, const Compare& compare,
              dimension_type mapping_dim, dimension_type node_dim, Base_ptr node);

      /**
       *  @brief  From x, find the node with the maximum value along the dimension
       *  @c mapping_dim.
       *  @param mapping_dim  The mapping dimension.
       *  @param node_dim  The current dimension for the node x.
       *  @param node  The node from which to find the maximum.
       *  @param key_dimension  The number of dimensions of the key.
       *  @param compare  The comparison function for the key.
       *  @return  The iterator pointing the maximum, or to the parent of x.
       *
       *  @c node must be a tree node and not the header node or @c null.
       */
      static Derived
      maximum(const Rank& rank, const Compare& compare,
              dimension_type mapping_dim, dimension_type node_dim, Base_ptr node);

      /**
       *  @brief  From x, find the node with the lower bound of y, along dimension
       *  @c mapping_dim.
       *  @param mapping_dim  The mapping dimension.
       *  @param node_dim  The current dimension for the node x.
       *  @param node  The node from which to find the lower bound.
       *  @param flag  The key marking the limit of the lower bound.
       *  @param key_dimension  The number of dimensions of the key.
       *  @param compare  The comparison function for the key.
       *  @return  The iterator to lower bound, or to the parent of @c node.
       *
       *  @c node must be a tree node and not the header node or @c null.
       */
      static Derived
      lower_bound(const Rank& rank, const Compare& compare,
                  dimension_type mapping_dim, dimension_type node_dim,
                  Base_ptr node, const Key& flag);

      /**
       *  @brief  From x, find the node with the upper bound of y, along dimension
       *  @c mapping_dim.
       *  @param mapping_dim  The mapping dimension.
       *  @param node_dim  The current dimension for the node x.
       *  @param node  The node from which to find the upper bound.
       *  @param flag  The key marking the limit of the upper bound.
       *  @param key_dimension  The number of dimensions of the key.
       *  @param compare  The comparison function for the key.
       *  @return  The iterator to upper bound, or to the parent of @c node.
       *
       *  @c node must be a tree node and not the header node or @c null.
       */
      static Derived
      upper_bound(const Rank& rank, const Compare& compare,
                  dimension_type mapping_dim, dimension_type node_dim,
                  Base_ptr node, const Key& flag);

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

      Mapping_iterator_impl impl_;
    };

    template <typename Rank, typename Key, typename Value, typename Node,
              typename Compare, bool Constant1, bool Constant2,
              typename Derived1, typename Derived2>
    inline bool
    operator==(const Mapping_iterator_base
               <Rank, Key, Value, Node, Compare, Constant1, Derived1>& x,
               const Mapping_iterator_base
               <Rank, Key, Value, Node, Compare, Constant2, Derived2>& y)
    { return x.impl_.node_ == y.impl_.node_; }

    template <typename Rank, typename Key, typename Value, typename Node,
              typename Compare, bool Constant1, bool Constant2,
              typename Derived1, typename Derived2>
    inline bool
    operator!=(const Mapping_iterator_base
               <Rank, Key, Value, Node, Compare, Constant1, Derived1>& x,
               const Mapping_iterator_base
               <Rank, Key, Value, Node, Compare, Constant2, Derived2>& y)
    { return !(x == y); }

    /**
     *  @brief  A mutable iterator based on Mapping_iterator_base.
     */
    template <typename Rank, typename Key, typename Value, typename Node,
              typename Compare>
    struct Mapping_iterator
      : Mapping_iterator_base
    <Rank, Key, Value, Node, Compare, false,
     Mapping_iterator<Rank, Key, Value, Node, Compare> >
    {
    private:
      typedef Mapping_iterator_base
      <Rank, Key, Value, Node, Compare, false,
       Mapping_iterator<Rank, Key, Value, Node, Compare> >   Base;

    public:
      Mapping_iterator(const Rank& r, const Compare& c,
                       dimension_type mapping_dim, dimension_type node_dim,
                       typename Base::Link_type link)
        : Base(typename Base::Mapping_iterator_impl
               (r, c, mapping_dim, node_dim, link))
      { }

      Mapping_iterator() { }

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
     *  @brief  A constant iterator based on Mapping_iterator_base.
     */
    template <typename Rank, typename Key, typename Value, typename Node,
              typename Compare>
    struct Const_Mapping_iterator
      : Mapping_iterator_base
    <Rank, Key, Value, Node, Compare, true,
     Const_Mapping_iterator<Rank, Key, Value, Node, Compare> >
    {
    private:
      typedef Mapping_iterator
      <Rank, Key, Value, Node, Compare>    iterator;

      typedef Mapping_iterator_base
      <Rank, Key, Value, Node, Compare, true,
       Const_Mapping_iterator<Rank, Key, Value, Node, Compare> > Base;

    public:
      Const_Mapping_iterator
      (const Rank& r, const Compare& c, dimension_type mapping_dim,
       dimension_type node_dim, typename Base::Link_type link)
        : Base(typename Base::Mapping_iterator_impl
               (r, c, mapping_dim, node_dim, link))
      { }

      Const_Mapping_iterator() { }

      Const_Mapping_iterator(const iterator& i)
        : Base(typename Base::Mapping_iterator_impl
               (i.rank(), i.compare(), i.impl_.mapping_dim_(),
                i.impl_.node_dim_, i.impl_.node_))
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

    namespace mapping
    {
      template<typename Container>
      struct iterator
      {
        typedef spatial::details::Mapping_iterator
        <typename container_traits<Container>::rank_type,
         typename container_traits<Container>::key_type,
         typename container_traits<Container>::value_type,
         typename container_traits<Container>::node_type,
         typename container_traits<Container>::key_compare>
        type;
      };

      template<typename Container>
      struct const_iterator
      {
        typedef spatial::details::Const_Mapping_iterator
        <typename container_traits<Container>::rank_type,
         typename container_traits<Container>::key_type,
         typename container_traits<Container>::value_type,
         typename container_traits<Container>::node_type,
         typename container_traits<Container>::key_compare>
        type;
      };

      template <typename Container>
      inline typename iterator<Container>::type
      end(Container& container, dimension_type mapping_dim)
      {
        except::check_dimension_argument(container.dimension(), mapping_dim);
        return typename iterator<Container>::type
          (container.rank(), container.key_comp(),
           mapping_dim, container.dimension() - 1,
           get_end(container));
      }

      template <typename Container>
      inline typename const_iterator<Container>::type
      const_end
      (const Container& container, dimension_type mapping_dim)
      {
        return end(*const_cast<Container*>(&container), mapping_dim);
      }

      template <typename Container>
      inline typename iterator<Container>::type
      begin(Container& container, dimension_type mapping_dim)
      {
        // Guarentees begin(n) == end(n) if tree is empty
        if (container.empty())
          { return end(container, mapping_dim); }
        else
          {
            except::check_dimension_argument
              (container.dimension(), mapping_dim);
            return iterator<Container>::type::minimum
              (container.rank(), container.key_comp(),
               mapping_dim, 0, container.end().node->parent);
          }
      }

      template <typename Container>
      inline typename const_iterator<Container>::type
      const_begin
      (const Container& container, dimension_type mapping_dim)
      {
        return begin(*const_cast<Container*>(&container), mapping_dim);
      }

      template <typename Container>
      inline typename iterator<Container>::type
      lower_bound
      (Container& container, dimension_type mapping_dim,
       const typename container_traits<Container>::key_type& key)
      {
        if (container.empty())
          { return end(container, mapping_dim); }
        else
          {
            except::check_dimension_argument
              (container.dimension(), mapping_dim);
            return iterator<Container>::type::lower_bound
              (container.rank(), container.key_comp(),
               mapping_dim, 0, container.end().node->parent, key);
          }
      }

      template <typename Container>
      inline typename const_iterator<Container>::type
      const_lower_bound
      (const Container& container, dimension_type mapping_dim,
       const typename container_traits<Container>::key_type& key)
      {
        return lower_bound
          (*const_cast<Container*>(&container), mapping_dim, key);
      }

      template <typename Container>
      inline typename iterator<Container>::type
      upper_bound
      (Container& container, dimension_type mapping_dim,
       const typename container_traits<Container>::key_type& key)
      {
        if (container.empty())
          { return end(container, mapping_dim); }
        else
          {
            except::check_dimension_argument
              (container.dimension(), mapping_dim);
            return iterator<Container>::type::upper_bound
              (container.rank(), container.key_comp(),
               mapping_dim, 0, container.end().node->parent, key);
          }
      }

      template <typename Container>
      inline typename const_iterator<Container>::type
      const_upper_bound
      (const Container& container, dimension_type mapping_dim,
       const typename container_traits<Container>::key_type& key)
      {
        return upper_bound
          (*const_cast<Container*>(&container), mapping_dim, key);
      }

    } // namespace mapping
  } // namespace details

  /**
   *  @brief  View of the Kdtree that provides standard iterator accessors for
   *  kdtree types that inherit from @c Mapping_iterable<KdtreeType>. Types
   *  using this view must provide the type definition mapping_iterable, that
   *  links back to the type itself.
   *
   *  This is a poor man's way to implement simple concept checking using
   *  traits, until one day, it create a dependancy on Boost.
   *  @see details::mapping_iterable_traits
   */
  template <typename Container>
  class mapping_view
  {
    typedef typename spatial::container_traits<Container>      traits_type;

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
     typename details::mapping::const_iterator<Container>::type,
     typename details::mapping::iterator<Container>::type
     >::type                                          iterator;
    typedef typename
    details::mapping::const_iterator<Container>::type const_iterator;
    typedef std::reverse_iterator<iterator>           reverse_iterator;
    typedef std::reverse_iterator<const_iterator>     const_reverse_iterator;

    iterator
    begin()
    { return details::mapping::begin(*container_, mapping_dim_); }

    const_iterator
    begin() const
    { return details::mapping::const_begin(*container_, mapping_dim_); }

    const_iterator
    cbegin() const
    { return details::mapping::const_begin(*container_, mapping_dim_); }

    iterator
    end()
    { return details::mapping::end(*container_, mapping_dim_); }

    const_iterator
    end() const
    { return details::mapping::const_end(*container_, mapping_dim_); }

    const_iterator
    cend() const
    { return details::mapping::const_end(*container_, mapping_dim_); }

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
    lower_bound(const key_type& key)
    { return details::mapping::lower_bound(*container_, mapping_dim_, key); }

    const_iterator
    lower_bound(const key_type& key) const
    { return details::mapping::const_lower_bound(*container_, mapping_dim_, key); }

    const_iterator
    clower_bound(const key_type& key) const
    { return details::mapping::const_lower_bound(*container_, mapping_dim_, key); }

    iterator
    upper_bound(const key_type& key)
    { return details::mapping::upper_bound(*container_, mapping_dim_, key); }

    const_iterator
    upper_bound(const key_type& key) const
    { return details::mapping::const_upper_bound(*container_, mapping_dim_, key); }

    const_iterator
    cupper_bound(const key_type& key) const
    { return details::mapping::const_upper_bound(*container_, mapping_dim_, key); }

    mapping_view(Container& iterable, dimension_type mapping_dim)
      : mapping_dim_(mapping_dim), container_(&iterable)
    { }

  private:
    dimension_type mapping_dim_;
    Container* container_;
  };

  /**
   *  @brief  Specialization of the view for constant containers
   */
  template <typename Container>
  class mapping_view<const Container>
  {
    typedef typename spatial::container_traits<Container>    traits_type;

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
    typedef typename
    details::mapping::const_iterator<Container>::type const_iterator;
    typedef const_iterator                            iterator;
    typedef std::reverse_iterator<const_iterator>     const_reverse_iterator;
    typedef const_reverse_iterator                    reverse_iterator;

    const_iterator
    begin() const
    { return details::mapping::const_begin(*container_, mapping_dim_); }

    const_iterator
    cbegin() const
    { return details::mapping::const_begin(*container_, mapping_dim_); }

    const_iterator
    end() const
    { return details::mapping::const_end(*container_, mapping_dim_); }

    const_iterator
    cend() const
    { return details::mapping::const_end(*container_, mapping_dim_); }

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
    lower_bound(const key_type& key) const
    { return details::mapping::const_lower_bound(*container_, mapping_dim_, key); }

    const_iterator
    clower_bound(const key_type& key) const
    { return details::mapping::const_lower_bound(*container_, mapping_dim_, key); }

    const_iterator
    upper_bound(const key_type& key) const
    { return details::mapping::const_upper_bound(*container_, mapping_dim_, key); }

    const_iterator
    cupper_bound(const key_type& key) const
    { return details::mapping::const_upper_bound(*container_, mapping_dim_, key); }

    mapping_view(const Container& iterable, dimension_type mapping_dim)
      : mapping_dim_(mapping_dim), container_(&iterable)
    { }

  private:
    dimension_type mapping_dim_;
    const Container* container_;
  };

} // namespace spatial

#include "spatial_mapping.tpp"

#endif // SPATIAL_MAPPING_HPP
