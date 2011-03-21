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
   *  @brief  This balancing policy leaves the tree totally unbalanced.
   *
   *  This policy is simply a marker, and creating a container with this
   *  policy will, in fact, have the same result as using containers that do
   *  not implement balancing at all, such as spatial::frozen_pointset family
   *  of containers.
   */
  struct non_balancing { };

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
     *  @param x  The node to examine for rebalancing.
     *  @param d  The current dimension function to use for examination.
     *  @return  true indicate that reblancing must occurs, otherwise false.
     */
    template <typename Rank>
    bool
    operator()(const details::Weighted_node* x, const Rank& r) const
    {
      typedef std::pair<weight_type, weight_type> weight_pair;
      weight_type left_weight = (x->left != 0)
	? static_cast<const details::Weighted_node*>(x->left)->weight : 0;
      weight_type right_weight = (x->right != 0)
	? static_cast<const details::Weighted_node*>(x->right)->weight : 0;
      if (left_weight < right_weight)
	{
	  return (right_weight > r() && left_weight < (right_weight >> 1))
	    ? true : false;
	}
      else
	{
	  return (left_weight > r() && right_weight < (left_weight >> 1))
	    ? true : false;
	}
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
     *  @param x  The node to examine for rebalancing.
     *  @param d  The current dimension function to use for examination.
     *  @return  true indicate that reblancing must occurs, otherwise false.
     */
    template <typename Rank>
    bool
    operator()(const details::Weighted_node* x, const Rank& r) const
    {
      typedef std::pair<weight_type, weight_type> weight_pair;
      weight_type left_weight = (x->left != 0)
	? static_cast<const details::Weighted_node*>(x->left)->weight : 0;
      weight_type right_weight = (x->right != 0)
	? static_cast<const details::Weighted_node*>(x->right)->weight : 0;
      dimension_type rank = r();
      if (left_weight < right_weight)
	{
	  return (right_weight > rank && left_weight < (right_weight - rank))
	    ? true : false;
	}
      else
	{
	  return (left_weight > r() && right_weight < (left_weight - rank))
	    ? true : false;
	}
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
      create_node(const key_type& x)
      {
	Link_type node = Base::create_node(x);
	node->parent = 0;
	node->left = 0;
	node->right = 0;
	node->weight = 1;
	return node;
      }

    public:
      // Functors accessors
      const Balancing&
      balancing() const
      { return balancing; }

    private:
      void
      copy_structure(const Self& other);

      template <typename InputIterator>
      void
      copy_aux(InputIterator first, InputIterator last);

      /**
       *  @brief  Insert the key @c v into the tree located at node @c x or
       *  below.
       *
       *  @param k  The current dimension for the node.
       *  @param x  The node below which the new key shall be inserted.
       *  @param v  The key to insert
       */
      iterator
      insert_aux(dimension_type k, Base_ptr x, const key_type& v);

      /**
       *  @brief  Erase and replace the value at @x by the minimum or the maximum
       *  value in the right tree or left tree respectively.
       *
       *  If @c left_tree is @c true, look into the left tree to find the maximum
       *  value along the dimension @c k, and use this value to replace the value
       *  at @c x. If @c left_tree is @c false, look into the right tree to find
       *  the minimum value along the dimension @c k, and use this value to
       *  replace the value at @c x. Do this recursively until a leaf of the tree
       *  is destroyed.
       *
       *  @param left_tree  Determine which side of the tree shall be visited.
       *  @param k          The current dimension for the node @c x.
       *  @param x          The node whose value shall be replaced.
       *  @param key        Try balancing with key.
       */
      void
      balance_aux(bool left_tree, dimension_type k, Base_ptr x);

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

      Relaxed_kdtree(const Self& other)
	: Base(other), m_balancing(other.balancing())
      {
	if (!other.empty())
	  { copy_structure(other); }
      }

      Self&
      operator=(const Self& other)
      {
	if (&other != this)
	  {
	    clear();
	    Base::operator=(other);
	    // No need to copy balancing policy.
	    if (!other.empty())
	      { copy_structure(other); }
	  }
      }

    public:
      size_type
      size() const
      {
	return Base::empty() ? 0
	  : static_cast<const Weighted_node*>(Base::get_root())->weight;
      }

      size_type
      count() const
      { return size(); }

      void
      clear()
      { Base::clear(); }

      void swap(Self& other)
      {
	Base::swap(*static_cast<Base*>(&other));
      }

      // Insertion
      iterator
      insert(const key_type& key)
      {
	Base_ptr node = Base::get_root();
	if (node == Base::get_header())
	  {
	    // insert root node in empty tree
	    Link_type new_node = create_node(key);
	    node->parent = new_node;
	    node->left = new_node;
	    node->right = new_node;
	    new_node->parent = node;
	    return iterator(new_node);
	  }
	else
	  {
	    iterator i = insert_aux(0, node, key);
	    return i;
	  }
      }

      template<typename InputIterator>
      void
      insert(InputIterator first, InputIterator last)
      { copy_aux(first, last); }

      // Deletion
      void
      erase(const_iterator p);

      size_type
      erase(const key_type& x);

      template<typename InputIterator>
      void
      erase(InputIterator first, InputIterator last);
    };

  } // namespace details
} // namespace spatial

#endif // SPATIAL_RELAXED_KDTREE_HPP
