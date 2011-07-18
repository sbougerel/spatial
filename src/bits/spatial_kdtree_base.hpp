// -*- C++ -*-

/**
 *  @file   spatial_kdtree_base.hpp
 *  @brief
 *
 *  Change Log:
 *
 *  - 2009-09-19 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_KDTREE_BASE_HPP
#define SPATIAL_KDTREE_BASE_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

namespace spatial
{
  namespace details
  {

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
       typename container_traits<Container>::node_type,
       equal_bounds<typename container_traits<Container>::key_type,
                    typename container_traits<Container>::compare_type> >
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
       typename container_traits<Container>::node_type,
       equal_bounds<typename container_traits<Container>::key_type,
                    typename container_traits<Container>::compare_type> >
        type;
    };

    /*
     * Forward declaration spell.
     */
    template <typename Rank, typename Key, typename Node,
              typename Compare, typename Alloc, bool ConstantIterator>
    class Kdtree_base;

    //@{
    /**
     * Accessor to header of the kd-tree. These functions are useful outside of
     * the k-d tree to initialize iterators.
     */
    template <typename Rank, typename Key, typename Node,
              typename Compare, typename Alloc, bool ConstantIterator>
    inline typename Kdtree_base<Rank, Key, Node, Compare, Alloc,
                                ConstantIterator>::node_type*
    get_end(Kdtree_base<Rank, Key, Node, Compare, Alloc,
                        ConstantIterator>& value)
    {
      return static_cast
        <typename Kdtree_base<Rank, Key, Node, Compare, Alloc,
                              ConstantIterator>::node_type*>
        (value.get_header());
    }

    template <typename Rank, typename Key, typename Node,
              typename Compare, typename Alloc, bool ConstantIterator>
    inline const typename Kdtree_base<Rank, Key, Node, Compare, Alloc,
                                      ConstantIterator>::node_type*
    get_end(const Kdtree_base<Rank, Key, Node, Compare, Alloc,
                              ConstantIterator>& value)
    {
      return static_cast
        <const typename Kdtree_base<Rank, Key, Node, Compare, Alloc,
                                    ConstantIterator>::node_type*>
        (value.get_header());
    }
    //@}

    //@{
    /**
     * Accessor to the root value of the kd-tree. These functions are useful
     * outside of the k-d tree to initialize iterators.
     */
    template <typename Rank, typename Key, typename Node,
              typename Compare, typename Alloc, bool ConstantIterator>
    inline typename Kdtree_base<Rank, Key, Node, Compare, Alloc,
                                ConstantIterator>::node_type*
    get_begin(Kdtree_base<Rank, Key, Node, Compare, Alloc,
                          ConstantIterator>& value)
    {
      return static_cast
        <typename Kdtree_base<Rank, Key, Node, Compare, Alloc,
                              ConstantIterator>::node_type*>
        (value.get_root());
    }

    template <typename Rank, typename Key, typename Node,
              typename Compare, typename Alloc, bool ConstantIterator>
    inline const typename Kdtree_base<Rank, Key, Node, Compare, Alloc,
                                      ConstantIterator>::node_type*
    get_begin(const Kdtree_base<Rank, Key, Node, Compare, Alloc,
                                ConstantIterator>& value)
    {
      return static_cast
        <const typename Kdtree_base<Rank, Key, Node, Compare, Alloc,
                                    ConstantIterator>::node_type*>
        (value.get_root());
    }
    //@}

    /*
     *
     */
    template <typename Rank, typename Key, typename Node,
              typename Compare, typename Alloc, bool ConstantIterator>
    class Kdtree_base
    {
      typedef Kdtree_base<Rank, Key, Node, Compare, Alloc,
                          ConstantIterator> Self;

    protected:
      typedef typename Alloc::template rebind
      <Node>::other                        Node_allocator;

      typedef Node_base::Base_ptr          Base_ptr;
      typedef Node_base::Const_Base_ptr    Const_Base_ptr;
      typedef Node*                        Link_type;
      typedef const Node*                  Const_Link_type;

    public:
      typedef Key                          key_type;
      typedef typename details::condition
      <ConstantIterator, const Key*,
       Key*>::type                         pointer;
      typedef const Key*                   const_pointer;
      typedef typename details::condition
      <ConstantIterator, const Key&,
       Key&>::type                         reference;
      typedef const Key&                   const_reference;
      typedef Node                         node_type;
      typedef std::size_t                  size_type;
      typedef std::ptrdiff_t               difference_type;
      typedef Alloc                        allocator_type;
      typedef Node_allocator               node_allocator_type;
      typedef Compare                      compare_type;
      typedef Rank                         rank_type;
      typedef typename true_or_false_type
      <ConstantIterator>::type             const_iterator_tag;

      // Conformant to C++ standard, if Key and Value are the same type then
      // iterator and const_iterator shall be the same.
      typedef typename details::condition
      <ConstantIterator, Const_Node_iterator<Key, Node>,
       Node_iterator<Key, Node> >::type         iterator;
      typedef Const_Node_iterator<Key, Node>    const_iterator;
      typedef std::reverse_iterator<iterator>   reverse_iterator;
      typedef std::reverse_iterator
      <const_iterator>                          const_reverse_iterator;

      typedef typename details::condition
      <ConstantIterator, typename details::const_equal_iterator<Self>::type,
       typename details::equal_iterator<Self>::type >
      ::type                                    equal_iterator;
      typedef typename
      details::const_equal_iterator<Self>::type const_equal_iterator;

    private:
      /**
       *  @brief The tree header.
       *
       *  The header node contains pointers to the root, the right most node and
       *  the header node marker (which is the left node of the header). The
       *  header class also contains the pointer to the left most node of the
       *  tree, since the place is already used by the header node marker.
       */
      struct Implementation : Rank
      {
        Implementation(const Rank& rank, const Compare& compare,
                       const Node_allocator& alloc)
          : Rank(rank), compare_(compare), header_(alloc, Header_node())
        { initialize(); }

        Implementation(const Implementation& impl)
          : Rank(impl), compare_(impl.compare),
            header_(impl.header_.base(), impl.header_())
        { initialize(); }

        void initialize()
        {
          header_().parent = &header_();
          header_().left = &header_();     // the end marker, *must* not change!
          header_().right = &header_();
          header_().leftmost = &header_(); // the substitute left most pointer
        }

        Compare compare_;
        details::Compress<Node_allocator, Header_node> header_;
      } impl_;

    public:
      /**
       *  @brief  Build a base k-d tree, setting header node and template
       *  paramters.
       */
      Kdtree_base(const Rank& other_rank = Rank(),
                  const Compare& other_compare = Compare(),
                  const Alloc& other_alloc = Alloc())
        : impl_(other_rank, other_compare, other_alloc)
      { }

      /**
       *  @brief  Copy a base k-d tree attributes from the argument @c other and
       *  set the header.
       */
      Kdtree_base(const Self& other)
        : impl_(other.rank(), other.compare(), other.allocator())
      { }

      /**
       *  @brief  Copy a base k-d tree attributes from the argument @c other.
       */
      Self&
      operator=(const Self& other)
      {
        *static_cast<Rank*>(&impl_) = other.rank();
        impl_.compare_ = other.compare();
        return *this;
      }

      /**
       *  @brief  Deallocate all nodes in the destructor.
       */
      ~Kdtree_base()
      { destroy_all_nodes(); }

    private:
      // Internal allocation
      struct safe_allocator // RAII for exception-safe memory management
      {
        Node_allocator& alloc;
        Link_type ptr;

        safe_allocator(Node_allocator& a) : alloc(a), ptr(a.allocate(1)) { }
        ~safe_allocator() { if (ptr) { alloc.deallocate(ptr, 1); } }
        Link_type release() { Link_type p = ptr; ptr=0; return p; }
      };

      Node_allocator&
      get_node_allocator()
      { return impl_.header_.base(); }

    protected:
      void initialize()
      {
        impl_.initialize();
      }

      // Allocation interface for derived classes
      const Node_allocator&
      get_node_allocator() const
      { return impl_.header_.base(); }

      allocator_type
      get_allocator() const
      { return static_cast<allocator_type>(get_node_allocator()); }

      Link_type
      create_node(const key_type& x)
      {
        safe_allocator safe(get_node_allocator());
        get_allocator().construct(&safe.ptr->value, x);
        return safe.release();
      }

      /**
       *  @brief  Destroy and deallocate @node.
       */
      void
      destroy_node(Link_type node)
      {
        get_allocator().destroy(&node->value);
        get_node_allocator().deallocate(node, 1);
      }

      /**
       *  @brief  Destroy and deallocate all nodes in the container.
       */
      void
      destroy_all_nodes();

    protected:
      // External accessor
      friend Link_type
      get_end<>(Self& value);

      friend Const_Link_type
      get_end<>(const Self& value);

      friend Link_type
      get_begin<>(Self& value);

      friend Const_Link_type
      get_begin<>(const Self& value);

      // Internal accessors
      Base_ptr
      get_header()
      { return static_cast<Base_ptr>(&impl_.header_()); }

      Const_Base_ptr
      get_header() const
      { return static_cast<Const_Base_ptr>(&impl_.header_()); }

      Base_ptr
      get_leftmost()
      { return impl_.header_().leftmost; }

      Const_Base_ptr
      get_leftmost() const
      { return impl_.header_().leftmost; }

      void
      set_leftmost(Base_ptr x)
      { impl_.header_().leftmost = x; }

      Base_ptr
      get_rightmost()
      { return impl_.header_().right; }

      Const_Base_ptr
      get_rightmost() const
      { return impl_.header_().right; }

      void
      set_rightmost(Base_ptr x)
      { impl_.header_().right = x; }

      Base_ptr
      get_root()
      { return impl_.header_().parent; }

      Const_Base_ptr
      get_root() const
      { return impl_.header_().parent; }

      void
      set_root(Base_ptr x)
      { impl_.header_().parent = x; }

      rank_type&
      get_rank()
      { return *static_cast<Rank*>(&impl_); }

      Compare&
      get_compare()
      { return impl_.compare_; }

    public:
      // Read-only public accessors
      const rank_type&
      rank() const
      { return *static_cast<const Rank*>(&impl_); }

      dimension_type
      dimension() const
      { return rank()(); }

      const Compare&
      compare() const
      { return impl_.compare_; }

      allocator_type
      allocator() const
      { return get_allocator(); }

      const node_allocator_type&
      node_allocator() const
      { return get_node_allocator(); }

    public:
      // Iterators standard interface
      iterator
      begin()
      {
        iterator it; it.node = get_leftmost();
        return it;
      }

      const_iterator
      begin() const
      {
        const_iterator it; it.node = get_leftmost();
        return it;
      }

      const_iterator
      cbegin() const
      { return begin(); }

      iterator
      end()
      {
        iterator it; it.node = get_header();
        return it;
      }

      const_iterator
      end() const
      {
        const_iterator it; it.node = get_header();
        return it;
      }

      const_iterator
      cend() const
      { return end(); }

      reverse_iterator
      rbegin()
      { return reverse_iterator(end()); }

      const_reverse_iterator
      rbegin() const
      { return const_reverse_iterator(end()); }

      const_reverse_iterator
      crbegin() const
      { return rbegin(); }

      reverse_iterator
      rend()
      { return reverse_iterator(begin()); }

      const_reverse_iterator
      rend() const
      { return const_reverse_iterator(begin()); }

      const_reverse_iterator
      crend() const
      { return rend(); }

    public:
      // Base functions
      /**
       *  @brief  Swap the K-d tree content with others
       *  @note  This function do not test: (this != &other)
       *
       *  The extra overhead of the test is not required in common cases:
       *  users intentionally swap different objects.
       */
      void
      swap(Kdtree_base& other)
      {
        if (empty() && other.empty()) return;
        details::template_member_swap<Rank>::do_it
          (get_rank(), other.get_rank());
        details::template_member_swap<Compare>::do_it
          (get_compare(), other.get_compare());
        details::template_member_swap<Node_allocator>::do_it
          (get_node_allocator(), other.get_node_allocator());
        if (impl_.header_().parent == &impl_.header_())
          {
            impl_.header_().parent = &other.impl_.header_();
            impl_.header_().right = &other.impl_.header_();
            impl_.header_().leftmost = &other.impl_.header_();
          }
        else if (other.impl_.header_().parent == &other.impl_.header_())
          {
            other.impl_.header_().parent = &impl_.header_();
            other.impl_.header_().right = &impl_.header_();
            other.impl_.header_().leftmost = &impl_.header_();
          }
        std::swap(impl_.header_().parent, other.impl_.header_().parent);
        std::swap(impl_.header_().right, other.impl_.header_().right);
        std::swap(impl_.header_().leftmost, other.impl_.header_().leftmost);
        if (impl_.header_().parent != &impl_.header_())
          { impl_.header_().parent->parent = &impl_.header_(); }
        if (other.impl_.header_().parent != &other.impl_.header_())
          { other.impl_.header_().parent->parent = &other.impl_.header_(); }
      }

      /**
       *  @brief  Remove all the elements from the tree.
       */
      void
      clear()
      {
        destroy_all_nodes();
        impl_.initialize();
      }

      /**
       *  @brief  True if the tree is empty.
       */
      bool
      empty() const
      { return ( get_root() == get_header() ); }

      /**
       *  @brief  The maximum number of elements that can be allocated.
       */
      size_type
      max_size() const
      { return get_node_allocator().max_size(); }

      //@{
      /**
       *  @brief  Find all nodes with the same coordinate as @c value and return
       *  the first that matches @c predicate.
       *
       *  The type @c key_type must be equally comparable.
       */
      template<typename Predicate>
      iterator
      find_if(const key_type& value, const Predicate& predicate);

      template<typename Predicate>
      const_iterator
      find_if(const key_type& value, const Predicate& predicate) const;
      //@}

      //@{
      /**
       *  @brief  Find all nodes with the same coordinate as @c value and return
       *  the first that is equal to @c value.
       *
       *  The type @c key_type must be equally comparable.
       */
      iterator
      find(const key_type& value)
      { return find_if(value, std::equal_to<key_type>()); }

      const_iterator
      find(const key_type& value) const
      { return find_if(value, std::equal_to<key_type>()); }
      //@}

      //@{
      /**
       *  @brief  Return a pair of iterator around keys of similar coordinates.
       *
       *  @attention These iterator are not similar to the other iterator, but
       *  are special types of iterators can only be used to list the equal
       *  objects in the container.
       */
      std::pair<equal_iterator, equal_iterator>
      equal_range(const key_type& key)
      {
        equal_bounds<key_type, compare_type> pred(compare(), key);
        equal_iterator first = details::range::begin(*this, pred);
        equal_iterator last = details::range::end(*this, pred);
        return std::make_pair(first, last);
      }

      std::pair<const_equal_iterator, const_equal_iterator>
      equal_range(const key_type& key) const
      {
        equal_bounds<key_type, compare_type> pred(compare(), key);
        const_equal_iterator first
          = details::range::const_begin(*this, pred);
        const_equal_iterator last
          = details::range::const_end(*this, pred);
        return std::make_pair(first, last);
      }
      //@}
    };

    /**
     *  @brief  Swap the contents of left and right trees.
     */
    template <typename Rank, typename Key, typename Node,
              typename Compare, typename Alloc, bool ConstantIterator>
    inline void swap
    (Kdtree_base
     <Rank, Key, Node, Compare, Alloc, ConstantIterator>& left,
     Kdtree_base
     <Rank, Key, Node, Compare, Alloc, ConstantIterator>& right)
    { left.swap(right); }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_KDTREE_BASE_HPP
