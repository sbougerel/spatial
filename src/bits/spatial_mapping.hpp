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
    template <typename Rank, typename Key, typename Node,
	      typename Compare, bool Constant, typename Derived>
    struct Mapping_iterator_base
    {
      typedef Key                             value_type;
      typedef typename details::condition
      <Constant, const Key&, Key&>::type      reference;
      typedef typename details::condition
      <Constant, const Key*, Key*>::type      pointer;
      typedef std::ptrdiff_t                  difference_type;
      typedef std::bidirectional_iterator_tag iterator_category;

    protected:
      typedef typename details::condition
      <Constant, Node_base::Const_Base_ptr,
       Node_base::Base_ptr>::type             Base_ptr;
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
      struct Mapping_iterator_impl : Rank
      {
	Mapping_iterator_impl()
	  : mapping_dim(), node_dim(), node() { }

	Mapping_iterator_impl
	(const Rank& rank, const Compare& compare,
	 dimension_type mapping_dim, dimension_type node_dim, Base_ptr node)
	  : Rank(rank), mapping_dim(compare, mapping_dim),
	    node_dim(node_dim), node(node)
	{ }

	/**
	 *  @brief  The current dimension of iteration.
	 *
	 *  You can modify this key if you suddenly want the iterator to change
	 *  dimension of iteration. However this field must always satisfy:
	 *
	 *     mapping_dim() < KeyDimension()
	 *
	 *  @see KeyDimension
	 */
	details::Compress<Compare, dimension_type> mapping_dim;

	/**
	 *  @brief  The dimension for node.
	 *
	 *  Modifing this key can potientially invalidate the iterator. Do not
	 *  modify this key unless you know what you're doing. See node.
	 */
	dimension_type node_dim;

	/**
	 *  @brief  The pointer to the current node.
	 *
	 *  Modifying this key can potentially invalidate the iterator. Do not
	 *  modify this key unless you know what you're doing. This iterator must
	 *  always point to a node in the tree or to the end.
	 */
	Base_ptr node;
      };

      const Rank&
      rank() const
      { return *static_cast<const Rank*>(&impl); }

      const Compare&
      compare() const
      { return impl.mapping_dim.base(); }

    private:
      void increment();

      void decrement();

    protected:
      Mapping_iterator_base() : impl() { }

      Mapping_iterator_base(const Mapping_iterator_impl& impl)
	: impl(impl)
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
      { return static_cast<Link_type>(impl.node)->key_field; }

      pointer
      operator->() const
      { return &static_cast<Link_type>(impl.node)->key_field; }

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

      Mapping_iterator_impl impl;
    };

    template <typename Rank, typename Key, typename Node,
	      typename Compare, bool Constant1, bool Constant2,
	      typename Derived1, typename Derived2>
    inline bool
    operator==(const Mapping_iterator_base
	       <Rank, Key, Node, Compare, Constant1, Derived1>& x,
	       const Mapping_iterator_base
	       <Rank, Key, Node, Compare, Constant2, Derived2>& y)
    { return x.impl.node == y.impl.node; }

    template <typename Rank, typename Key, typename Node,
	      typename Compare, bool Constant1, bool Constant2,
	      typename Derived1, typename Derived2>
    inline bool
    operator!=(const Mapping_iterator_base
	       <Rank, Key, Node, Compare, Constant1, Derived1>& x,
	       const Mapping_iterator_base
	       <Rank, Key, Node, Compare, Constant2, Derived2>& y)
    { return !(x == y); }

    /**
     *  @brief  A mutable iterator based on Mapping_iterator_base.
     */
    template <typename Rank, typename Key, typename Node,
	      typename Compare>
    struct Mapping_iterator
      : Mapping_iterator_base
    <Rank, Key, Node, Compare, false,
     Mapping_iterator<Rank, Key, Node, Compare> >
    {
    private:
      typedef Mapping_iterator_base
      <Rank, Key, Node, Compare, false,
       Mapping_iterator<Rank, Key, Node, Compare> >   Base;

    public:
      Mapping_iterator(const Rank& rank,
		       const Compare& compare, dimension_type mapping_dim,
		       dimension_type node_dim, typename Base::Link_type link)
	: Base(typename Base::Mapping_iterator_impl
	       (rank, compare, mapping_dim, node_dim, link))
      { }

      Mapping_iterator() { }
    };

    /**
     *  @brief  A constant iterator based on Mapping_iterator_base.
     */
    template <typename Rank, typename Key, typename Node,
	      typename Compare>
    struct Const_Mapping_iterator
      : Mapping_iterator_base
    <Rank, Key, Node, Compare, true,
     Const_Mapping_iterator<Rank, Key, Node, Compare> >
    {
    private:
      typedef Mapping_iterator
      <Rank, Key, Node, Compare>    iterator;

      typedef Mapping_iterator_base
      <Rank, Key, Node, Compare, true,
       Const_Mapping_iterator<Rank, Key, Node, Compare> > Base;

    public:
      Const_Mapping_iterator
      (const Rank& rank, const Compare& compare,
       dimension_type mapping_dim, dimension_type node_dim,
       typename Base::Link_type link)
	: Base(typename Base::Mapping_iterator_impl
	       (rank, compare, mapping_dim, node_dim, link))
      { }

      Const_Mapping_iterator() { }

      Const_Mapping_iterator(const iterator& i)
	: Base(typename Base::Mapping_iterator_impl
	       (i.rank(), i.compare(),
		i.impl.mapping_dim(), i.impl.node_dim, i.impl.node))
      { }
    };

  } // namespace details

  namespace view
  {
    namespace details
    {
      template<typename Container>
      struct mapping_iterator
      {
	typedef spatial::details::Mapping_iterator
	<typename container_traits<Container>::rank_type,
	 typename container_traits<Container>::key_type,
	 typename container_traits<Container>::node_type,
	 typename container_traits<Container>::compare_type>
	type;
      };

      template<typename Container>
      struct const_mapping_iterator
      {
	typedef spatial::details::Const_Mapping_iterator
	<typename container_traits<Container>::rank_type,
	 typename container_traits<Container>::key_type,
	 typename container_traits<Container>::node_type,
	 typename container_traits<Container>::compare_type>
	type;
      };

      template <typename Container>
      inline typename mapping_iterator<Container>::type
      end_mapping(Container& container, dimension_type mapping_dim)
      {
	except::check_dimension_argument(container.dimension(), mapping_dim);
	return typename mapping_iterator<Container>::type
	  (container.rank(), container.compare(),
	   mapping_dim, container.dimension() - 1,
	   get_end(container));
      }

      template <typename Container>
      inline typename const_mapping_iterator<Container>::type
      const_end_mapping
      (const Container& container, dimension_type mapping_dim)
      {
	return end_mapping(*const_cast<Container*>(&container), mapping_dim);
      }

      template <typename Container>
      inline typename mapping_iterator<Container>::type
      begin_mapping(Container& container, dimension_type mapping_dim)
      {
	// Guarentees begin(n) == end(n) if tree is empty
	if (container.empty())
	  { return end_mapping(container, mapping_dim); }
	else
	  {
	    except::check_dimension_argument
	      (container.dimension(), mapping_dim);
	    return mapping_iterator<Container>::type::minimum
	      (container.rank(), container.compare(),
	       mapping_dim, 0, container.end().node->parent);
	  }
      }

      template <typename Container>
      inline typename const_mapping_iterator<Container>::type
      const_begin_mapping
      (const Container& container, dimension_type mapping_dim)
      {
	return begin_mapping(*const_cast<Container*>(&container), mapping_dim);
      }

      template <typename Container>
      inline typename mapping_iterator<Container>::type
      lower_bound_mapping
      (Container& container, dimension_type mapping_dim,
       const typename container_traits<Container>::key_type& key)
      {
	if (container.empty())
	  { return end_mapping(container, mapping_dim); }
	else
	  {
	    except::check_dimension_argument
	      (container.dimension(), mapping_dim);
	    return mapping_iterator<Container>::type::lower_bound
	      (container.rank(), container.compare(),
	       mapping_dim, 0, container.end().node->parent, key);
	  }
      }

      template <typename Container>
      inline typename const_mapping_iterator<Container>::type
      const_lower_bound_mapping
      (const Container& container, dimension_type mapping_dim,
       const typename container_traits<Container>::key_type& key)
      {
	return lower_bound_mapping
	  (*const_cast<Container*>(&container), mapping_dim, key);
      }

      template <typename Container>
      inline typename mapping_iterator<Container>::type
      upper_bound_mapping
      (Container& container, dimension_type mapping_dim,
       const typename container_traits<Container>::key_type& key)
      {
	if (container.empty())
	  { return end_mapping(container, mapping_dim); }
	else
	  {
	    except::check_dimension_argument
	      (container.dimension(), mapping_dim);
	    return mapping_iterator<Container>::type::upper_bound
	      (container.rank(), container.compare(),
	       mapping_dim, 0, container.end().node->parent, key);
	  }
      }

      template <typename Container>
      inline typename const_mapping_iterator<Container>::type
      const_upper_bound_mapping
      (const Container& container, dimension_type mapping_dim,
       const typename container_traits<Container>::key_type& key)
      {
	return upper_bound_mapping
	  (*const_cast<Container*>(&container), mapping_dim, key);
      }
    } // namespace details

    /**
     *  @brief  Enforces the definition of the mapping_iterable tag into a type
     *  when used.
     */
    template <typename Type>
    struct mapping_iterable_traits
      : spatial::details::identity<typename Type::mapping_iterable> { };

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
    class mapping
    {
      typedef typename spatial::container_traits<Container>      traits_type;

    public:
      typedef typename traits_type::key_type            key_type;
      typedef typename traits_type::pointer             pointer;
      typedef typename traits_type::const_pointer       const_pointer;
      typedef typename traits_type::reference           reference;
      typedef typename traits_type::const_reference     const_reference;
      typedef typename traits_type::node_type           node_type;
      typedef typename traits_type::size_type           size_type;
      typedef typename traits_type::difference_type     difference_type;
      typedef typename traits_type::allocator_type      allocator_type;
      typedef typename traits_type::compare_type        compare_type;
      typedef typename traits_type::rank_type           rank_type;

      typedef typename spatial::details::condition
      <traits_type::const_iterator_tag::value,
       typename details::const_mapping_iterator<Container>::type,
       typename details::mapping_iterator<Container>::type
       >::type                                          iterator;
      typedef typename
      details::const_mapping_iterator<Container>::type  const_iterator;
      typedef std::reverse_iterator<iterator>           reverse_iterator;
      typedef std::reverse_iterator<const_iterator>     const_reverse_iterator;

      iterator
      begin()
      { return details::begin_mapping(*container, mapping_dim); }

      const_iterator
      begin() const
      { return details::const_begin_mapping(*container, mapping_dim); }

      const_iterator
      cbegin() const
      { return details::const_begin_mapping(*container, mapping_dim); }

      iterator
      end()
      { return details::end_mapping(*container, mapping_dim); }

      const_iterator
      end() const
      { return details::const_end_mapping(*container, mapping_dim); }

      const_iterator
      cend() const
      { return details::const_end_mapping(*container, mapping_dim); }

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
      { return details::lower_bound_mapping(*container, mapping_dim, key); }

      const_iterator
      lower_bound(const key_type& key) const
      { return details::const_lower_bound_mapping(*container, mapping_dim, key); }

      const_iterator
      clower_bound(const key_type& key) const
      { return details::const_lower_bound_mapping(*container, mapping_dim, key); }

      iterator
      upper_bound(const key_type& key)
      { return details::upper_bound_mapping(*container, mapping_dim, key); }

      const_iterator
      upper_bound(const key_type& key) const
      { return details::const_upper_bound_mapping(*container, mapping_dim, key); }

      const_iterator
      cupper_bound(const key_type& key) const
      { return details::const_upper_bound_mapping(*container, mapping_dim, key); }

      dimension_type mapping_dim;
      Container* container;

      mapping(typename mapping_iterable_traits<Container>
	      ::type& iterable, dimension_type mapping_dim)
	: mapping_dim(mapping_dim), container(&iterable)
      { }
    };

    /**
     *  @brief  Spcialization of the kdtree
     */
    template <typename Container>
    class mapping<const Container>
    {
      typedef typename spatial::container_traits<Container>    traits_type;

    public:
      typedef typename traits_type::key_type            key_type;
      typedef typename traits_type::const_pointer       pointer;
      typedef typename traits_type::const_pointer       const_pointer;
      typedef typename traits_type::const_reference     reference;
      typedef typename traits_type::const_reference     const_reference;
      typedef typename traits_type::node_type           node_type;
      typedef typename traits_type::size_type           size_type;
      typedef typename traits_type::difference_type     difference_type;
      typedef typename traits_type::allocator_type      allocator_type;
      typedef typename traits_type::compare_type        compare_type;
      typedef typename traits_type::rank_type           rank_type;

      typedef typename
      details::const_mapping_iterator<Container>::type  const_iterator;
      typedef const_iterator                            iterator;
      typedef std::reverse_iterator<const_iterator>     const_reverse_iterator;
      typedef const_reverse_iterator                    reverse_iterator;

      const_iterator
      begin() const
      { return details::const_begin_mapping(*container, mapping_dim); }

      const_iterator
      cbegin() const
      { return details::const_begin_mapping(*container, mapping_dim); }

      const_iterator
      end() const
      { return details::const_end_mapping(*container, mapping_dim); }

      const_iterator
      cend() const
      { return details::const_end_mapping(*container, mapping_dim); }

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
      { return details::const_lower_bound_mapping(*container, mapping_dim, key); }

      const_iterator
      clower_bound(const key_type& key) const
      { return details::const_lower_bound_mapping(*container, mapping_dim, key); }

      const_iterator
      upper_bound(const key_type& key) const
      { return details::const_upper_bound_mapping(*container, mapping_dim, key); }

      const_iterator
      cupper_bound(const key_type& key) const
      { return details::const_upper_bound_mapping(*container, mapping_dim, key); }

      dimension_type mapping_dim;
      const Container* container;

      mapping(const typename mapping_iterable_traits<Container>
	      ::type& iterable, dimension_type mapping_dim)
	: mapping_dim(mapping_dim), container(&iterable)
      { }
    };

  } // namespace view

} // namespace spatial

#endif // SPATIAL_MAPPING_HPP
