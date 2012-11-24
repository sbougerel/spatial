// -*- C++ -*-

/**
 *  @file   spatial_relaxed_kdtree.hpp
 *  @brief  Defines a \kdtree with a relaxed invariant. On a given dimension, if
 *  coordinates between a root node and a child node are equal, the child node
 *  may be placed either on the left or the right of the tree. The relaxed
 *  \kdtree is a self-balancing tree.
 *
 *  Balancing the tree occurs when the number of children in the left part of
 *  a node differs from that in the right part, by a significant margin. The
 *  \kdtree is then rebalanced slightly, by shifting one child node from one
 *  side to the other side.
 *
 *  Relaxed \kdtree are implemented as scapegoat tree, and so, for each node
 *  they store an additional size_t count of number of children in the
 *  node. Although they are self-balancing, they are not as memory efficient as
 *  regular \kdtree.
 *
 *  Additionally, scapegoat roatations are not as efficient as the one in the
 *  \rbtree. Sadly, \rbtree rotation cannot be applied for \kdtree, due to the
 *  complexity of in invariant. However scapegoat rotations still provide
 *  amortized insertion and deletion times on the tree, and allow for multiple
 *  balancing policies to be defined.
 *
 *  @see Relaxed_kdtree
 */

#ifndef SPATIAL_RELAXED_KDTREE_HPP
#define SPATIAL_RELAXED_KDTREE_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#include <utility> // for std::pair
#include <algorithm> // for std::minmax

#include "spatial_traits.hpp"
#include "spatial_exceptions.hpp"
#include "spatial_assert.hpp"
#include "spatial_details.hpp"
#include "spatial_function.hpp"
#include "spatial_node.hpp"
#include "spatial_ordered.hpp"
#include "spatial_mapping.hpp"
#include "spatial_region.hpp"
#include "spatial_neighbor.hpp"

namespace spatial
{
  /**
   *  @brief  This policy triggers rebalancing for the node when the
   *  difference in weight between left or right is more than a half. The
   *  default policy for rebalancing.
   *
   *  In effect, this policy leaves the tree roughly balanced: the path from
   *  the root to the furthest leaf is no more than twice as long as the path
   *  from the root to the nearest leaf.
   *
   *  This policy is adequate in many cases cause it prevents worse-case
   *  insertion or deletion time, and worst-case run-time on many search
   *  algorithms, and does not require a large amount of rebalancing.
   */
  struct loose_balancing
  {
    /**
     *  @brief  Rebalancing predicate.
     *  @param r     The current dimension function to use for examination.
     *  @param left  The weight at the left
     *  @param right The weight at the right
     *  @return  true indicate that reblancing must occurs, otherwise false.
     */
    template <typename Rank>
    bool
    operator()(const Rank&, weight_type left, weight_type right) const
    {
      if (left < right)
        { return (left < (right >> 1)) ? true : false; }
      else
        { return (right < (left >> 1)) ? true : false; }
    }
  };

  /**
   *  @brief  A policy that balances a node if the difference in weight
   *  between left and right is higher than the current rank of the tree.
   *
   *  The dimension is choosen as a limiter because balancing the tree even
   *  more strictly will not have an impact on most search algorithm, since
   *  dimensions at each level of the \kdtree are rotated.
   */
  struct tight_balancing
  {
    /**
     *  @brief  Rebalancing predicate.
     *  @param r     The current dimension function to use for examination.
     *  @param left  The weight at the left
     *  @param right The weight at the right
     *  @return true Indicate that reblancing must occurs, otherwise false.
     */
    template <typename Rank>
    bool
    operator()(const Rank& r, weight_type left, weight_type right) const
    {
      weight_type rank = static_cast<weight_type>(r());
      if (left < right)
        { return (right - left > rank) ? true : false; }
      else
        { return (left - right > rank) ? true : false; }
    }
  };

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
              typename Balancing, typename Alloc>
    class Relaxed_kdtree
    {
      typedef Relaxed_kdtree<Rank, Key, Value, Compare, Balancing,
                             Alloc>                   Self;

    public:
      // Container intrincsic types
      typedef Rank                                    rank_type;
      typedef typename mutate<Key>::type              key_type;
      typedef typename mutate<Value>::type            value_type;
      typedef Relaxed_kdtree_link<Key, Value>         mode_type;
      typedef Compare                                 key_compare;
      typedef ValueCompare<value_type, key_compare>   value_compare;
      typedef Alloc                                   allocator_type;
      typedef Balancing                               balancing_policy;

      // Container iterator related types
      typedef Value*                                  pointer;
      typedef const Value*                            const_pointer;
      typedef Value&                                  reference;
      typedef const Value&                            const_reference;
      typedef std::size_t                             size_type;
      typedef std::ptrdiff_t                          difference_type;

      // Container iterators
      // Conformant to C++ standard, if Key and Value are the same type then
      // iterator and const_iterator shall be the same.
      typedef Node_iterator<mode_type>                iterator;
      typedef Const_node_iterator<mode_type>          const_iterator;
      typedef std::reverse_iterator<iterator>         reverse_iterator;
      typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;

    private:
      typedef typename Alloc::template rebind
      <Relaxed_kdtree_link<Key, Value> >::other       Link_allocator;
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
      struct Implementation : rank_type
      {
        Implementation(const rank_type& rank, const key_compare& compare,
                       const Balancing& balance, const Link_allocator& alloc)
          : Rank(rank), compare_(balance, compare),
            header_(alloc, Node<mode_type>()) { initialize(); }

        Implementation(const Implementation& impl)
          : Rank(impl), compare_(impl.compare_.base(), impl.compare_()),
            header_(impl.header_.base()) { initialize(); }

        void initialize()
        {
          header_().parent = &header_();
          header_().left = &header_(); // the end marker, *must* not change!
          header_().right = &header_();
          leftmost_ = &header_();      // the substitute left most pointer
        }

        ::spatial::details::Compress<Balancing, key_compare> compare_;
        ::spatial::details::Compress<Link_allocator, Node<mode_type> > header_;
        node_ptr leftmost_;
      } impl_;

    private:
      // Internal mutable accessor
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
      { return impl_.compare_(); }

      balancing_policy& get_balancing()
      { return impl_.compare_.base(); }

      Link_allocator& get_link_allocator()
      { return impl_.header_.base(); }

      Value_allocator get_value_allocator() const
      { return impl_.header_.base(); }

    private:
      // Allocation/Deallocation of nodes
      struct safe_allocator // RAII for exception-safe memory management
      {
        Link_allocator* alloc;
        link_ptr link;
        safe_allocator(Link_allocator& a)
          : alloc(&a), link(0)
        { link = alloc->allocate(1); } // may throw
        ~safe_allocator() { if (link) { alloc->deallocate(link, 1); } }
        link_ptr release() { link_ptr p = link; link=0; return p; }
      };

      node_ptr
      create_node(const value_type& value)
      {
        safe_allocator safe(get_link_allocator());
        // the following may throw. But we have RAII on safe_allocator
        get_value_allocator().construct(mutate_pointer(&safe.link->value),
                                        value);
        link_ptr node = safe.release();
        // leave parent uninitialized: its value will change during insertion.
        node->left = 0;
        node->right = 0;
        node->weight = 1;
        return node; // silently cast into base type node_ptr.
      }

      node_ptr
      clone_node(const_node_ptr node, const_node_ptr parent)
      {
        node_ptr new_node = create_node(const_value(node), parent);
        link(new_node).weight = const_link(node).weight;
        return new_node; // silently cast into base type node_ptr.
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
       *  @brief  Copy the exact sturcture of the sub-tree pointed to by @c
       *  other_node into the current empty tree.
       *
       *  The structural copy preserve all characteristics of the sub-tree
       *  pointed to by @c other_node.
       */
      void
      copy_structure(const Self& other);

      /**
       *  Insert the new node @c new_node into the tree located at node. If the
       *  last parameter is 0, the node will also be created.
       *
       *  @param node_dim  The current dimension for the node.
       *  @param node      The node below which the new key shall be inserted.
       *  @param mew_node  The new node to insert
       */
      iterator
      insert_node(dimension_type node_dim, node_ptr node, node_ptr new_node);

      /**
       *  Erase the node pointed by @c node.
       *
       *  @c node cannot be null or a root node, or else dire things may
       *  happen. This function does not destroy the node and it does not
       *  decrement weight to the parents of node.
       *
       *  @param dim      The current dimension for @c node
       *  @param node     The node to erase
       *  @return         The address of the node that has replaced the erased
       *                  one or @c node if it was a leaf.
       */
      node_ptr erase_node(dimension_type dim, node_ptr node);

      /**
       *  Erase the node pointed by @c node and balance tree up to
       *  header. Finish the work started by erase_node by reducing weight in
       *  parent of @c node, up to the header.
       *
       *  @c node cannot be null or a root node, or else dire things may
       *  happen. This function does not destroy the node.
       *
       *  @param dim      The current dimension for @c node
       *  @param node     The node to erase
       */
      void erase_node_balance(dimension_type dim, node_ptr node);

      /**
       *  Attempt to balance the current node.
       *
       *  @c node cannot be null or a root node, or else dire things may
       *  happen. This function does not destroy the node and it does not
       *  modify weight of the parents of node.
       *
       *  @param dim      The current dimension for @c node
       *  @param node     The node to erase
       *  @return         The address of the node that has replaced the current
       *                  node.
       */
      node_ptr balance_node(dimension_type dim, node_ptr node);

      /**
       *  @brief  Return true if the node is not balanced, false otherwise.
       *  @param node       The node to check
       *  @param more_left  Add @c more_left to the weight of the left nodes
       *  @param more_right Add @c more_right to the weight of the right nodes
       */
      bool is_node_unbalanced(node_ptr node, weight_type more_left = 0,
                              weight_type more_right = 0) const;

    public:
      // Iterators standard interface
      iterator begin()
      { iterator it; it.node = get_leftmost(); return it; }

      const_iterator begin() const
      { const_iterator it; it.node = get_leftmost(); return it; }

      const_iterator cbegin() const
      { return begin(); }

      iterator end()
      { return iterator(get_header()); }

      const_iterator end() const
      { return const_iterator(get_header()); }

      const_iterator cend() const
      { return end(); }

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
      // Functors accessors
      /**
       *  Returns the balancing policy for the container.
       */
      const balancing_policy&
      balancing() const
      { return impl_.compare_.base(); }

      /**
       *  Returns the rank type used internally to get the number of dimensions
       *  in the container.
       */
      const rank_type&
      rank() const
      { return *static_cast<const rank_type*>(&impl_); }

      /**
       *  Returns the dimension of the container.
       */
      dimension_type
      dimension() const
      { return rank()(); }

      /**
       *  Returns the compare function used for the key.
       */
      const key_compare&
      key_comp() const
      { return impl_.compare_(); }

      /**
       *  Returns the compare function used for the value.
       */
      const value_compare&
      value_comp() const
      { return value_compare(impl_.compare_()); }

      /**
       *  Returns the allocator used by the tree.
       */
      allocator_type
      get_allocator() const { return get_value_allocator(); }

      /**
       *  @brief  True if the tree is empty.
       */
      bool
      empty() const
      { return ( get_root() == get_header() ); }

      /**
       *  @brief  Returns the number of elements in the K-d tree.
       */
      size_type
      size() const
      {
        return empty() ? 0
          : static_cast<const_link_ptr>(get_root())->weight;
      }

      /**
       *  @brief  Returns the number of elements in the K-d tree. Same as size().
       *  @see size()
       */
      size_type
      count() const
      { return size(); }

      /**
       *  @brief  The maximum number of elements that can be allocated.
       */
      size_type
      max_size() const
      { return impl_.header_.base().max_size(); }

      //@{
      /**
       *  @brief  Find the first node that matches with @c key and returns an
       *  iterator to it found, otherwise it returns an iterator to the element
       *  past the end of the container.
       *
       *  Notice that this function returns an iterator only to one of the
       *  elements with that key. To obtain the entire range of elements with a
       *  given value, you can use @ref equal_range().
       *
       *  @param key The value to be searched for.
       *  @return An iterator to that value or an iterator to the element past
       *  the end of the container.
       */
      iterator
      find(const key_type& key)
      { return equal_begin(*this, key); }

      const_iterator
      find(const key_type& key) const
      { return equal_begin(*this, key); }
      //@}

    public:
      Relaxed_kdtree()
        : impl_(rank_type(), key_compare(), balancing_policy(),
                Link_allocator()) { }

      explicit Relaxed_kdtree(const rank_type& r)
        : impl_(r, Compare(), Balancing(), Link_allocator())
      { }

      Relaxed_kdtree(const rank_type& r, const key_compare& c)
        : impl_(r, c, Balancing(), Link_allocator())
      { }

      Relaxed_kdtree(const rank_type& r, const key_compare& c,
                     const balancing_policy& b)
        : impl_(r, c, b, Link_allocator())
      { }

      Relaxed_kdtree(const rank_type& r, const key_compare& c,
                     const balancing_policy& b, const allocator_type& a)
        : impl_(r, c, b, a)
      { }

      /**
       *  @brief  Deep copy of @c other into the new tree.
       *
       *  This operation results in an identical the structure to the @p other
       *  tree. Therefore, all operations should behave similarly to both trees
       *  after the copy.
       */
      Relaxed_kdtree(const Relaxed_kdtree& other) : impl_(other.impl_)
      { if (!other.empty()) { copy_structure(other); } }

      /**
       *  @brief  Assignment of @c other into the tree, with deep copy.
       *
       *  The copy preserve the structure of the tree @c other. Therefore, all
       *  operations should behave similarly to both trees after the copy.
       *
       *  @note  Allocator is not modified with this assignment and remains the
       *  same.
       */
      Relaxed_kdtree&
      operator=(const Relaxed_kdtree& other)
      {
        if (&other != this)
          {
            destroy_all_nodes();
            template_member_assign<rank_type>
              ::do_it(get_rank(), other.rank());
            template_member_assign<key_compare>
              ::do_it(get_compare(), other.key_comp());
            template_member_assign<balancing_policy>
              ::do_it(get_balancing(), other.balancing());
            impl_.initialize();
            if (!other.empty()) { copy_structure(other); }
          }
        return *this;
      }

      /**
       *  @brief  Deallocate all nodes in the destructor.
       */
      ~Relaxed_kdtree()
      { destroy_all_nodes(); }

    public:
      // Mutable functions
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
        template_member_swap<balancing_policy>::do_it
          (get_balancing(), other.get_balancing());
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
      }

      /**
       *  @brief  Erase all elements in the K-d tree.
       */
      void
      clear()
      {
        destroy_all_nodes();
        impl_.initialize();
      }

      /**
       *  @brief  Insert a single key @c key in the tree.
       */
      iterator
      insert(const value_type& value)
      {
        node_ptr target_node = create_node(value); // may throw
        node_ptr node = get_root();
        if (header(node))
          {
            // insert root node in empty tree
            set_leftmost(target_node);
            set_rightmost(target_node);
            set_root(target_node);
            target_node->parent = node;
            return iterator(target_node);
          }
        else
          { return insert_node(0, node, target_node); }
      }

      /**
       *  @brief  Insert a serie of values in the tree at once.
       */
      template<typename InputIterator>
      void
      insert(InputIterator first, InputIterator last)
      { for (; first != last; ++first) { insert(*first); } }

      // Deletion
      /**
       *  @brief  Deletes the node pointed to by the iterator.
       *
       *  The iterator must be pointing to an existing node belonging to the
       *  related tree, or dire things may happen.
       */
      void
      erase(iterator position);

      /**
       *  @brief  Deletes all nodes that match key @c value.
       *  @see    find
       *  @param  value that will be compared with the tree nodes.
       *
       *  The type @c key_type must be equally comparable.
       */
      size_type
      erase(const key_type& key);

      /**
       *  Deletes the range of nodes pointed to by the tree's own iterators.
       *
       *  This function leverages on the fact that when nodes are erased in the
       *  tree, the in-memory locations of other nodes do not change. Only the
       *  position of the node relative to other nodes in the tree changes.
       */
      void
      erase(const Bidirectional_iterator<mode_type, rank_type>& first,
            const Bidirectional_iterator<mode_type, rank_type>& last)
      {
        while (first != last)
          {
            iterator tmp = first++;
            erase(tmp); // this could be optimized to pass around node_dim;
            // Since the tree might be rebalanced, node_dim must be updated...
            first.node_dim = modulo(first.node, rank());
          }
      }
    };

    /**
     *  @brief  Swap the content of the relaxed @kdtree @p left and @p right.
     */
    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Balancing, typename Alloc>
    inline void swap
    (Relaxed_kdtree<Rank, Key, Value, Compare, Balancing, Alloc>& left,
     Relaxed_kdtree<Rank, Key, Value, Compare, Balancing, Alloc>& right)
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
              typename Balancing, typename Alloc>
    inline bool
    operator==(const Relaxed_kdtree<Rank, Key, Value, Compare, Balancing,
                                    Alloc>& a,
               const Relaxed_kdtree<Rank, Key, Value, Compare, Balancing,
                                    Alloc>& b)
    { return (a.size() == b.size() && a.dimension() == b.dimension()); }

    /**
     *  Returns true if <tt>a.size() != b.size()</tt> or if <tt>a.dimension() !=
     *  b.dimension()</tt> for runtime containers.
     *
     *  More details are given on operator=.
     */
    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Balancing, typename Alloc>
    inline bool
    operator!=(const Relaxed_kdtree<Rank, Key, Value, Compare, Balancing,
                                    Alloc>& a,
               const Relaxed_kdtree<Rank, Key, Value, Compare, Balancing,
                                    Alloc>& b)
    { return !(a.size() == b.size()); }

  } // namespace details
} // namespace spatial

#include "spatial_relaxed_kdtree.tpp"

#endif // SPATIAL_RELAXED_KDTREE_HPP
