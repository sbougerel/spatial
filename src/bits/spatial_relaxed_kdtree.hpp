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
 */

/*
 *  Change Log:
 *
 *  - 2009-02-26 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 *
 */

#ifndef SPATIAL_RELAXED_KDTREE_HPP
#define SPATIAL_RELAXED_KDTREE_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#include <utility> // for std::pair
#include <algorithm> // for std::minmax

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
    // Detailed implementation of the relaxed kd-tree. Used by point_set,
    // point_multiset, point_map, point_multimap, box_set, box_multiset and their
    // equivalent in variant orders: variant_pointer_set, as chosen by the
    // templates. Not used by neighbor_point_set,
    // neighbor_point_multiset... Compare must provide strict unordered ordering
    // along each dimension! Each node maintains the count of its children nodes
    // plus one.
    template <typename Rank, typename Key, typename Compare,
	      typename Balancing, typename Alloc, bool Const_condition>
    class Relaxed_kdtree
      : public Kdtree_base<Rank, Key, Relaxed_kdtree_node<Key>, Compare,
			   Alloc, Const_condition>
    {
      typedef Relaxed_kdtree<Rank, Key, Compare, Balancing,
			     Alloc, Const_condition> Self;
      typedef Kdtree_base<Rank, Key, Relaxed_kdtree_node<Key>, Compare,
			  Alloc, Const_condition>    Base;

      typedef typename Base::Base_ptr            Base_ptr;
      typedef typename Base::Const_Base_ptr      Const_Base_ptr;
      typedef typename Base::Link_type           Link_type;
      typedef typename Base::Const_Link_type     Const_Link_type;

    public:
      typedef typename Base::key_type            key_type;
      typedef typename Base::pointer             pointer;
      typedef typename Base::const_pointer       const_pointer;
      typedef typename Base::reference           reference;
      typedef typename Base::const_reference     const_reference;
      typedef typename Base::node_type           node_type;
      typedef typename Base::size_type           size_type;
      typedef typename Base::difference_type     difference_type;
      typedef typename Base::allocator_type      allocator_type;
      typedef typename Base::node_allocator_type node_allocator_type;
      typedef typename Base::compare_type        compare_type;
      typedef typename Base::rank_type           rank_type;

      typedef typename Base::iterator            iterator;
      typedef typename Base::const_iterator      const_iterator;

    private:
      Balancing m_balancing;

    protected:
      Link_type
      create_node(const key_type& key)
      {
	Link_type node = Base::create_node(key);
	node->parent = 0;
	node->left = 0;
	node->right = 0;
	node->weight = 1;
	return node;
      }

      Link_type
      clone_node(Const_Base_ptr node)
      {
	Const_Link_type origin_node = static_cast<Const_Link_type>(node);
	Link_type new_node = create_node(origin_node->key_field);
	new_node->weight = origin_node->weight;
	return new_node;
      }

    public:
      // Functors accessors
      const Balancing&
      balancing() const
      { return m_balancing; }

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
       *  @brief  Insert the new node @c new_node into the tree located at node.
       *
       *  @param node_dim  The current dimension for the node.
       *  @param node      The node below which the new key shall be inserted.
       *  @param mew_node  The new node to insert
       */
      iterator
      insert_node(dimension_type node_dim, Base_ptr node, Base_ptr new_node);

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
      Base_ptr erase_node(dimension_type dim, Base_ptr node);

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
      void erase_node_finish(dimension_type dim, Base_ptr node);

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
      Base_ptr balance_node(dimension_type dim, Base_ptr node);

      /**
       *  @brief  Return true if the node is not balanced, false otherwise.
       *  @param node       The node to check
       *  @param more_left  Add @c more_left to the weight of the left nodes
       *  @param more_right Add @c more_right to the weight of the right nodes
       */
      bool is_node_unbalanced(Base_ptr node, weight_type more_left = 0,
			      weight_type more_right = 0);

    public:
      // allocation/deallocation
      Relaxed_kdtree()
	: Base(Rank(), Compare(), allocator_type()), m_balancing()
      { }

      explicit Relaxed_kdtree(const Rank& rank)
	: Base(rank, Compare(), allocator_type()), m_balancing()
      { }

      Relaxed_kdtree(const Rank& rank, const compare_type& compare)
	: Base(rank, compare, allocator_type()), m_balancing()
      { }

      Relaxed_kdtree(const Rank& rank, const compare_type& compare,
		     const Balancing& b)
	: Base(rank, compare, allocator_type()), m_balancing(b)
      { }

      Relaxed_kdtree(const Rank& rank, const compare_type& compare,
		     const Balancing& b, const allocator_type& alloc)
	: Base(rank, compare, alloc), m_balancing(b)
      { }

      /**
       *  @brief  Deep copy of @c other into the new tree.
       *
       *  This operation results in an identical the structure to the @p other
       *  tree. Therefore, all operations should behave similarly to both trees
       *  after the copy.
       */
      Relaxed_kdtree(const Relaxed_kdtree& other)
	: Base(other), m_balancing(other.balancing())
      {
	if (!other.empty())
	  { copy_structure(other); }
      }

      /**
       *  @brief  Assignment of @c other into the tree, with deep copy.
       *
       *  The copy preserve the structure of the tree @c other. Therefore, all
       *  operations should behave similarly to both trees after the copy.
       *
       *  @note  The allocator of the tree is not modified by the assignment.
       */
      Relaxed_kdtree&
      operator=(const Relaxed_kdtree& other)
      {
	if (&other != this)
	  {
	    clear();
	    Base::operator=(other);
	    m_balancing = other.m_balancing;
	    if (!other.empty())
	      { copy_structure(other); }
	  }
	return *this;
      }

    public:
      /**
       *  @brief  Returns the number of elements in the K-d tree.
       */
      size_type
      size() const
      {
	return Base::empty() ? 0
	  : static_cast<const Weighted_node*>(Base::get_root())->weight;
      }

      /**
       *  @brief  Returns the number of elements in the K-d tree. Same as size().
       *  @see size()
       */
      size_type
      count() const
      { return size(); }

      /**
       *  @brief  Erase all elements in the K-d tree.
       */
      void
      clear()
      { Base::clear(); }

      /**
       *  @brief  Swap the K-d tree content with other.
       */
      void swap(Self& other)
      { Base::swap(*static_cast<Base*>(&other)); }

      // Insertion
      /**
       *  @brief  Insert a single key @c key in the tree.
       */
      iterator
      insert(const key_type& key)
      {
	Link_type new_node = create_node(key);
	Base_ptr node = Base::get_root();
	if (Node_base::header(node))
	  {
	    // insert root node in empty tree
	    Base::set_leftmost(new_node);
	    Base::set_rightmost(new_node);
	    Base::set_root(new_node);
	    new_node->parent = node;
	    return iterator(new_node);
	  }
	else
	  { return insert_node(0, node, new_node); }
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
      erase(const_iterator position);

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
       *  @brief  Deletes any node that matches one of the keys in the sequence
       *  covered by the iterators.
       */
      template<typename InputIterator>
      void
      erase(InputIterator first, InputIterator last)
      { for (; first != last; ++first) { erase(*first); } }
    };

  } // namespace details
} // namespace spatial

#endif // SPATIAL_RELAXED_KDTREE_HPP
