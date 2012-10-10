// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_kdtree.hpp
 *  @brief  Kdtree class is defined in this file and its implementation is in
 *  the corresponding *.tpp file.
 *
 *  The Kdtree class defines all the methods and algorithms to store, delete and
 *  iterate over nodes in a Kdtree. This class is the bare definition of the
 *  kdtree and must be rebalanced by the user after nodes have been inserted.
 *
 *  @see Kdtree
 */

#ifndef SPATIAL_KDTREE_HPP
#define SPATIAL_KDTREE_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#include "spatial_traits.hpp"
#include "spatial_exceptions.hpp"
#include "spatial_assert.hpp"
#include "spatial_details.hpp"
#include "spatial_function.hpp"
#include "spatial_node.hpp"
#include "spatial_mapping.hpp"
#include "spatial_region.hpp"
#include "spatial_neighbor.hpp"

namespace spatial
{
  namespace details
  {

    /**
     *  Detailed implementation of the kd-tree. Used by point_set,
     *  point_multiset, point_map, point_multimap, box_set, box_multiset and
     *  their equivalent in variant orders: variant_pointer_set, as chosen by
     *  the templates. Not used by neighbor_point_set,
     *  neighbor_point_multiset... Compare must provide strict unordered
     *  ordering along each dimension! Each node maintains the count of its
     *  children nodes plus one.
     */
    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Alloc>
    class Kdtree
    {
      typedef Kdtree<Rank, Key, Value, Compare, Alloc>  Self;

    public:
      // Container intrincsic types
      typedef Rank                                    rank_type;
      typedef typename mutate<Key>::type              key_type;
      typedef typename mutate<Value>::type            value_type;
      typedef Kdtree_link<Key, Value>                 mode_type;
      typedef Compare                                 key_compare;
      typedef ValueCompare<value_type, key_compare>   value_compare;
      typedef Alloc                                   allocator_type;

      // Container iterator related types
      typedef Value*                                  pointer;
      typedef const Value*                            const_pointer;
      typedef Value&                                  reference;
      typedef const Value&                            const_reference;
      typedef std::size_t                             size_type;
      typedef std::ptrdiff_t                          difference_type;

      // Container iterators
      // Conformant to C++ ISO standard, if Key and Value are the same type then
      // iterator and const_iterator shall be the same.
      typedef Node_iterator<mode_type>                iterator;
      typedef Const_node_iterator<mode_type>          const_iterator;
      typedef std::reverse_iterator<iterator>         reverse_iterator;
      typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;

    private:
      typedef typename Alloc::template rebind
      <Kdtree_link<Key, Value> >::other               Link_allocator;
      typedef typename Alloc::template rebind
      <value_type>::other                             Value_allocator;

      // The types used to deal with nodes
      typedef typename mode_type::node_ptr            node_ptr;
      typedef typename mode_type::const_node_ptr      const_node_ptr;
      typedef typename mode_type::link_ptr            link_ptr;
      typedef typename mode_type::const_link_ptr      const_link_ptr;

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
        Implementation(const rank_type& rank, const key_compare& compare,
                       const Link_allocator& alloc)
          : Rank(rank), count_(compare, 0),
            header_(alloc, Node<mode_type>()) { initialize(); }

        Implementation(const Implementation& impl)
          : Rank(impl), count_(impl.count_.base(), impl.count_()),
            header_(impl.header_.base(), impl.header_()) { initialize(); }

        void initialize()
        {
          count_() = 0;
          header_().parent = &header_();
          header_().left = &header_(); // the end marker, *must* not change!
          header_().right = &header_();
          leftmost_ = &header_();      // the substitute left most pointer
        }

        Compress<key_compare, size_type>           count_;
        Compress<Link_allocator, Node<mode_type> > header_;
        Node<mode_type>* leftmost_;
      } impl_;

    private:
      // Internal accessors
      node_ptr get_header()
      { return static_cast<node_ptr>(&impl_.header_()); }

      const_node_ptr get_header() const
      { return static_cast<const_node_ptr>(&impl_.header_()); }

      node_ptr get_leftmost()
      { return impl_.leftmost_; }

      const_node_ptr get_leftmost() const
      { return impl_.leftmost_; }

      void set_leftmost(node_ptr x)
      { impl_.leftmost_ = x; }

      node_ptr get_rightmost()
      { return impl_.header_().right; }

      const_node_ptr get_rightmost() const
      { return impl_.header_().right; }

      void set_rightmost(node_ptr x)
      { impl_.header_().right = x; }

      node_ptr get_root()
      { return impl_.header_().parent; }

      const_node_ptr get_root() const
      { return impl_.header_().parent; }

      void set_root(node_ptr x)
      { impl_.header_().parent = x; }

      rank_type& get_rank()
      { return *static_cast<Rank*>(&impl_); }

      key_compare& get_compare()
      { return impl_.count_.base(); }

      Link_allocator& get_link_allocator()
      { return impl_.header_.base(); }

      Value_allocator get_value_allocator() const
      { return impl_.header_.base(); }

    private:
      // Allocation/Deallocation of nodes
      struct safe_allocator // RAII for exception-safe memory management
      {
        Link_allocator& alloc;
        link_ptr link;

        safe_allocator(Link_allocator& a) : alloc(a), link(0)
        { link = alloc.allocate(1); } // may throw
        ~safe_allocator() { if (link) { alloc.deallocate(link, 1); } }
        link_ptr release() { link_ptr p = link; link=0; return p; }
      };

      node_ptr
      create_node(const value_type& value)
      {
        safe_allocator safe(get_link_allocator());
        // the following may throw but we have RAII on safe_allocator
        get_value_allocator().construct(mutate_pointer(&safe.link->value),
                                        value);
        link_ptr node = safe.release();
        // leave parent uninitialized: its value will change during insertion.
        node->left = 0;
        node->right = 0;
        return node; // silently casts into the parent node pointer
      }

      /**
       *  @brief  Destroy and deallocate @c node.
       */
      void
      destroy_node(node_ptr node)
      {
        get_value_allocator().destroy(mutate_pointer(&value(node)));
        get_link_allocator().deallocate(link(node), 1);
      }

      /**
       *  @brief  Destroy and deallocate all nodes in the container.
       */
      void
      destroy_all_nodes();

    private:
      /**
       *  @brief  Insert a node already allocated into the tree.
       */
      iterator insert_node(node_ptr target_node);

      /**
       *  @brief  Insert all the nodes in @p [first,last) into the tree, by
       *  first sorting the nodes according to the dimension of interest.
       *
       *  This function is semi-recursive. It iterates when walking down left
       *  nodes and recurse when walking down right nodes.
       */
      void rebalance_node_insert
      (typename std::vector<node_ptr>::iterator first,
       typename std::vector<node_ptr>::iterator last, dimension_type dim);

      /**
       *  @brief  Copy the exact sturcture of the sub-tree pointed to by @c
       *  other_node into the current empty tree.
       *
       *  The structural copy preserve all characteristics of the sub-tree
       *  pointed to by @c other_node.
       */
      void copy_structure(const Self& other);

      /**
       *  @brief  Copy the content of @p other to the tree and rebalances the
       *  values in the tree resulting in most query having an @c O(log(n))
       *  order of complexity.
       */
      void copy_rebalance(const Self& other);

      /**
       *  @brief  Erase the node located at @c node with current dimension
       *  @c node_dim.
       */
      void erase_node(dimension_type node_dim, node_ptr node);

    public:
      // Iterators standard interface
      iterator begin()
      { iterator it; it.node = get_leftmost(); return it; }

      const_iterator begin() const
      { const_iterator it; it.node = get_leftmost(); return it; }

      const_iterator cbegin() const { return begin(); }

      iterator end()
      { return iterator(get_header()); }

      const_iterator end() const
      { return const_iterator(get_header()); }

      const_iterator cend() const { return end(); }

      reverse_iterator rbegin()
      { return reverse_iterator(end()); }

      const_reverse_iterator rbegin() const
      { return const_reverse_iterator(end()); }

      const_reverse_iterator crbegin() const
      { return rbegin(); }

      reverse_iterator rend()
      { return reverse_iterator(begin()); }

      const_reverse_iterator rend() const
      { return const_reverse_iterator(begin()); }

      const_reverse_iterator crend() const
      { return rend(); }

    public:
      /**
       *  Returns the rank used to create the tree.
       */
      const rank_type& rank() const
      { return *static_cast<const Rank*>(&impl_); }

      /**
       *  Returns the dimension of the tree.
       */
      dimension_type dimension() const
      { return rank()(); }

      /**
       *  Returns the compare function used for the key.
       */
      const key_compare& key_comp() const
      { return impl_.count_.base(); }

      /**
       *  Returns the compare function used for the value.
       */
      const value_compare& value_comp() const
      { return value_compare(impl_.count_.base()); }

      /**
       *  Returns the allocator used by the tree.
       */
      allocator_type
      get_allocator() const { return get_value_allocator(); }

      /**
       *  @brief  True if the tree is empty.
       */
      bool empty() const { return (get_header() == get_root()); }

      /**
       *  @brief  Returns the number of elements in the K-d tree.
       */
      size_type size() const { return impl_.count_(); }

      /**
       *  @brief  Returns the number of elements in the K-d tree. Same as size().
       *  @see size()
       */
      size_type count() const { return impl_.count_(); }

      /**
       *  @brief  Erase all elements in the K-d tree.
       */
      void clear()
      { destroy_all_nodes(); impl_.initialize(); }

      /**
       *  @brief  The maximum number of elements that can be allocated.
       */
      size_type max_size() const
      { return impl_.header_.base().max_size(); }

      //@{
      /**
       *  Find the first node that matches with \c key and returns an iterator
       *  to it found, otherwise it returns an iterator to the element past the
       *  end of the container.
       *
       *  Notice that this function returns an iterator only to one of the
       *  elements with that key. To obtain the entire range of elements with a
       *  given value, you can use \ref equal_range.
       *
       *  If this function is called on an empty container, returns an iterator
       *  past the end of the container.
       *
       *  \fractime
       *  \param key the value to be searched for.
       *  \return An iterator to that value or an iterator to the element past
       *  the end of the container.
       */
      iterator
      find(const key_type& key)
      {
        equal_iterator eq = begin_equal(*this, key);
        iterator it; it.node = eq.node; return it;
      }

      const_iterator
      find(const key_type& key) const
      {
        const_equal_iterator eq = begin_equal(*this, key);
        const_iterator it; it.node = eq.node; return it;
      }
      //@}

    public:
      Kdtree()
        : impl_(rank_type(), key_compare(), allocator_type())
      { }

      explicit Kdtree(const rank_type& r)
        : impl_(r, key_compare(), allocator_type())
      { }

      explicit Kdtree(const key_compare& c)
        : impl_(rank_type(), c, allocator_type())
      { }

      Kdtree(const rank_type& r, const key_compare& c)
        : impl_(r, c, allocator_type())
      { }

      Kdtree(const rank_type& r, const key_compare& c, const allocator_type& a)
        : impl_(r, c, a)
      { }

      /**
       *  Deep copy of \c other into the new tree.
       *
       *  If \c balancing is \c false or unspecified, the copy preserve the
       *  structure of \c other tree. Therefore, all operations should behave
       *  similarly to both trees after the copy.
       *
       *  If \c balancing is @c true, the new tree is a balanced copy of a \c
       *  other tree, resulting in \Ofrac order of complexity on most search
       *  functions.
       */
      Kdtree(const Self& other, bool balancing = false) : impl_(other.impl_)
      {
        if (!other.empty())
          {
            if (balancing) { copy_rebalance(other); }
            else { copy_structure(other); }
          }
      }

      /**
       *  @brief  Assignment of @c other into the tree, with deep copy.
       *
       *  The copy preserve the structure of the tree @c other. Therefore, all
       *  operations should behave similarly to both trees after the copy.
       *
       *  @note  The allocator of the tree is not modified by the assignment.
       */
      Self&
      operator=(const Self& other)
      {
        if (&other != this)
          {
            destroy_all_nodes();
            template_member_assign<rank_type>
              ::do_it(get_rank(), other.rank());
            template_member_assign<key_compare>
              ::do_it(get_compare(), other.key_comp());
            impl_.initialize();
            if (!other.empty()) { copy_structure(other); }
          }
        return *this;
      }

      /**
       *  @brief  Deallocate all nodes in the destructor.
       */
      ~Kdtree()
      { destroy_all_nodes(); }

    public:
      /**
       *  @brief  Swap the K-d tree content with others
       *
       *  The extra overhead of the test is not required in common cases:
       *  users intentionally swap different objects.
       *  @warning  This function do not test: (this != &other)
       */
      void
      swap(Self& other)
      {
        if (empty() && other.empty()) return;
        template_member_swap<rank_type>::do_it
          (get_rank(), other.get_rank());
        template_member_swap<key_compare>::do_it
          (get_compare(), other.get_compare());
        template_member_swap<Link_allocator>::do_it
          (get_link_allocator(), other.get_link_allocator());
        if (impl_.header_().parent == &impl_.header_())
          {
            impl_.header_().parent = &other.impl_.header_();
            impl_.header_().right = &other.impl_.header_();
            impl_.leftmost_ = &other.impl_.header_();
          }
        else if (other.impl_.header_().parent == &other.impl_.header_())
          {
            other.impl_.header_().parent = &impl_.header_();
            other.impl_.header_().right = &impl_.header_();
            other.impl_.leftmost_ = &impl_.header_();
          }
        std::swap(impl_.header_().parent, other.impl_.header_().parent);
        std::swap(impl_.header_().right, other.impl_.header_().right);
        std::swap(impl_.leftmost_, other.impl_.leftmost_);
        if (impl_.header_().parent != &impl_.header_())
          { impl_.header_().parent->parent = &impl_.header_(); }
        if (other.impl_.header_().parent != &other.impl_.header_())
          { other.impl_.header_().parent->parent = &other.impl_.header_(); }
        std::swap(impl_.count_(), other.impl_.count_());
      }

      // Mutable functions
      /**
       *  @brief  Rebalance the K-d tree near-optimally, resulting in @c log(n)
       *  order of complexity on most search functions.
       *
       *  This function is time & memory consuming. Internally, it creates a
       *  vector of pointer to the node, and thus requires a substantial amount of
       *  memory for a large K-d tree. Ideally, this function should be called
       *  only once, when all the elements you will be working on have been
       *  inserted in the tree.
       *
       *  If you need to insert and erase multiple elements continuously, consider
       *  using a different balancing policy instead.
       *
       *  @see Relaxed_kdtree
       */
      void rebalance();

      /**
       *  @brief  Insert a single @c value element in the container.
       */
      iterator
      insert(const value_type& value)
      {
        link_ptr tmp = link(create_node(value)); // may throw
        return insert_node(tmp);
      }

      /**
       *  @brief  Insert a serie of values in the container at once.
       *
       *  The parameter @c first and @c last only need to be a model of @c
       *  InputIterator. Elements are inserted in a single pass.
       */
      template<typename InputIterator>
      void
      insert(InputIterator first, InputIterator last)
      { for (; first != last; ++first) { insert(*first); } }

      /**
       *  @brief  Deletes the node pointed to by the iterator.
       *
       *  The iterator must be pointing to an existing node belonging to the
       *  related tree, or dire things may happen.
       */
      void
      erase(iterator pointer);

      /**
       *  @brief  Deletes all nodes that match key @c value.
       *  @see    find
       *  @param  value that will be compared with the tree nodes.
       *
       *  The type @c key_type must be equally comparable.
       */
      size_type
      erase(const key_type& value);

      /**
       *  @brief  Deletes any node that matches one of the keys in the sequence
       *  covered by the iterators.
       *
       *  This function differs greatly from the standard containers such as @c
       *  std::multiset or @c std::multimap, in which a whole range of iterators
       *  is cut out of the map, and iterators given in argument are the
       *  container's own.
       *
       *  In this function, iterators given <b>shall not</b> belong to the
       *  container as they would get invalidated as they are erased.
       */
      template <typename InputIterator>
      void
      erase(InputIterator first, InputIterator last)
      { for (; first != last; ++first) { erase(*first); } }
    };

    /**
     *  @brief  Swap the content of the tree @p left and @p right.
     */
    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Alloc>
    inline void swap
    (Kdtree<Rank, Key, Value, Compare, Alloc>& left,
     Kdtree<Rank, Key, Value, Compare, Alloc>& right)
    { left.swap(right); }

    /**
     *  Returns true if <tt>a.size() == b.size()</tt>, if <tt>a.dimension() ==
     *  b.dimension()</tt> for runtime containers.
     *
     *  @todo: this operation does not tally with the behavior of the standard
     *  containers and should be modified to also test that all elements of both
     *  containers are matching. At the moment this is difficult to do with
     *  better than @f[ O(2nlog(n)) @f] complexity. However this will change
     *  when mapping_iterator will get support for ordering over all dimensions.
     */
    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Alloc>
    inline bool
    operator==(const Kdtree<Rank, Key, Value, Compare, Alloc>& a,
               const Kdtree<Rank, Key, Value, Compare, Alloc>& b)
    { return (a.size() == b.size() && a.dimension() == b.dimension()); }

    /**
     *  Returns true if <tt>a.size() != b.size()</tt> or if <tt>a.dimension() !=
     *  b.dimension()</tt> for runtime containers.
     *
     *  More details are given on operator=.
     */
    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Alloc>
    inline bool
    operator!=(const Kdtree<Rank, Key, Value, Compare, Alloc>& a,
               const Kdtree<Rank, Key, Value, Compare, Alloc>& b)
    { return !(a.size() == b.size()); }

  } // namespace details
} // namespace spatial

#include "spatial_kdtree.tpp"

#endif // SPATIAL_KDTREE_HPP
