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

    template <typename Rank, typename Key, typename Node,
	      typename Compare, typename Alloc, bool ConstantIterator>
    class Kdtree_base
    {
      typedef Kdtree_base<Rank, Key, Node, Compare, Alloc,
			  ConstantIterator> Self;

    protected:
      typedef typename Alloc::template rebind
      <Node>::other                        Node_allocator;

      typedef Node_base*                   Base_ptr;
      typedef const Node_base*             Const_Base_ptr;
      typedef Node*                        Link_type;
      typedef const Node*                  Const_Link_type;

    public:
      typedef Key                          key_type;
      typedef Key*                         pointer;
      typedef const Key*                   const_pointer;
      typedef Key&                         reference;
      typedef const Key&                   const_reference;
      typedef Node                         node_type;
      typedef std::size_t                  size_type;
      typedef std::ptrdiff_t               difference_type;
      typedef Alloc                        allocator_type;
      typedef Node_allocator               node_allocator_type;
      typedef Compare                      compare_type;
      typedef Rank                         rank_type;

      typedef Node_iterator
      <Key, Node, ConstantIterator>            iterator;
      typedef Const_Node_iterator
      <Key, Node, ConstantIterator>            const_iterator;
      typedef std::reverse_iterator<iterator>  reverse_iterator;
      typedef std::reverse_iterator
      <const_iterator>                         const_reverse_iterator;

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
	  : Rank(rank), compare(compare), header(alloc, Header_node())
	{ initialize(); }

	Implementation(const Implementation& impl)
	  : Rank(impl), compare(impl.compare),
	    header(impl.header.base(), impl.header)
	{ initialize(); }

	void initialize()
	{
	  header().parent = &header();
	  header().left = &header();     // the end marker, *must* remain as is!
	  header().right = &header();
	  header().leftmost = &header(); // the substitute left most pointer
	}

	Compare compare;
	details::Compress<Node_allocator, Header_node> header;
      } m_impl;

    public:
      /**
       *  @brief  Build a base k-d tree, setting header node and template
       *  paramters.
       */
      Kdtree_base(const Rank& rank = Rank(),
		  const Compare& compare = Compare(),
		  const Alloc& alloc = Alloc())
	: m_impl(rank, compare, alloc)
      { }

      /**
       *  @brief  Copy a base k-d tree attributes from the argument @c other and
       *  set the header.
       */
      Kdtree_base(const Self& other)
	: m_impl(other.rank(), other.compare(), other.allocator())
      { }

      /**
       *  @brief  Copy a base k-d tree attributes from the argument @c other.
       */
      Self&
      operator=(const Self& other)
      {
	*static_cast<Rank*>(&m_impl) = other.rank();
	m_impl.compare = other.compare();
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
      { return m_impl.header.base(); }

    protected:
      void initialize()
      {
	m_impl.initialize();
      }

      // Allocation interface for derived classes
      const Node_allocator&
      get_node_allocator() const
      { return m_impl.header.base(); }

      allocator_type
      get_allocator() const
      { return static_cast<allocator_type>(get_node_allocator()); }

      Link_type
      create_node(const key_type& x)
      {
	safe_allocator safe(get_node_allocator());
	get_allocator().construct(&safe.ptr->key_field, x);
	return safe.release();
      }

      /**
       *  @brief  Destroy and deallocate @node.
       */
      void
      destroy_node(Link_type node)
      {
	get_allocator().destroy(&node->key_field);
	get_node_allocator().deallocate(node, 1);
      }

      /**
       *  @brief  Destroy and deallocate all nodes in the container.
       */
      void
      destroy_all_nodes();

    protected:
      // Internal accessors
      Base_ptr
      get_header()
      { return static_cast<Base_ptr>(&m_impl.header()); }

      Const_Base_ptr
      get_header() const
      { return static_cast<Const_Base_ptr>(&m_impl.header()); }

      Base_ptr
      get_leftmost()
      { return m_impl.header().leftmost; }

      Const_Base_ptr
      get_leftmost() const
      { return m_impl.header().leftmost; }

      void
      set_leftmost(Base_ptr x)
      { m_impl.header().leftmost = x; }

      Base_ptr
      get_rightmost()
      { return m_impl.header().right; }

      Const_Base_ptr
      get_rightmost() const
      { return m_impl.header().right; }

      void
      set_rightmost(Base_ptr x)
      { m_impl.header().right = x; }

      Base_ptr
      get_root()
      { return m_impl.header().parent; }

      Const_Base_ptr
      get_root() const
      { return m_impl.header().parent; }

      void
      set_root(Base_ptr x)
      { m_impl.header().parent = x; }

      rank_type&
      get_rank()
      { return *static_cast<Rank*>(&m_impl); }

      Compare&
      get_compare()
      { return m_impl.compare; }

    public:
      // Read-only public accessors
      const rank_type&
      rank() const
      { return *static_cast<const Rank*>(&m_impl); }

      dimension_type
      dimension() const
      { return rank()(); }

      const Compare&
      compare() const
      { return m_impl.compare; }

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
	if (m_impl.header().parent == &m_impl.header())
	  {
	    m_impl.header().parent = &other.m_impl.header();
	    m_impl.header().right = &other.m_impl.header();
	    m_impl.header().leftmost = &other.m_impl.header();
	  }
	else if (other.m_impl.header().parent == &other.m_impl.header())
	  {
	    other.m_impl.header().parent = &m_impl.header();
	    other.m_impl.header().right = &m_impl.header();
	    other.m_impl.header().leftmost = &m_impl.header();
	  }
	std::swap(m_impl.header().parent, other.m_impl.header().parent);
	std::swap(m_impl.header().right, other.m_impl.header().right);
	std::swap(m_impl.header().leftmost, other.m_impl.header().leftmost);
	if (m_impl.header().parent != &m_impl.header())
	  { m_impl.header().parent->parent = &m_impl.header(); }
	if (other.m_impl.header().parent != &other.m_impl.header())
	  { other.m_impl.header().parent->parent = &other.m_impl.header(); }
      }

      /**
       *  @brief  Remove all the elements from the tree.
       */
      void
      clear()
      {
	destroy_all_nodes();
	m_impl.initialize();
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
