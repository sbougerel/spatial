// -*- C++ -*-

/**
 *  @file   spatial_relaxed_kdtree.hpp
 *  @brief  
 *
 *  Change Log:
 *
 *  - 2009-02-26 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 *
 *
 *  Todo:
 *
 *  - Test with different rebalancing models, using the sliding balance
 *    algorithm, start with relaxed rebalance first.
 *
 *  - Standards allocators have hints. Use cache oblivious tree allocators!
 *
 *  - Try to store only when you are convinced that it is the right way to do!
 *
 *  - Red-black will be tricky to implement.... arggge... but it'll be hard with
 *    using paper before to compute complexity.
 *
 *  - Use random sets on random queries, cloudy set on cloudy queries, etc...
 *
 *  - try with a red/black first on paper to validate idea... add a third color
 *    to know in which direction to perform the switch? Interesting... but at
 *    which node should I perform the split????? How do I recolor????
 *
 *  - Put all the POD types at the beginning of the function for register
 *    optimization!
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
  namespace details
  {

    /**
     *  @brief  This balancing policy leaves the tree totally unbalanced.
     *
     *  With such policy, the tree will never be balanced. If you do not need
     *  balancing of the tree, you should consider using plain old k-d trees and
     *  not relaxed k-d trees.
     */
    struct unbalanced
    {
      /**
       *  @brief  Always return false, thus prevent rebalancing to occur.
       *  @return  false, always.
       */
      template <typename Dimension>
      bool
      operator()(const Relaxed_kdtree_node_base*,
		 const Dimension&) const
      { return false; }
    };

    /**
     *  @brief  Provide loose balacing with a fixed factor of 0.5.
     *
     *  This policy triggers rebalancing for the node is the smallest weight of
     *  left or right is less than half that of the biggest weight.
     */
    struct loosely_balanced
    {
      /**
       *  @brief  Rebalancing predicate.
       *  @param x  The node to examine for rebalancing.
       *  @param d  The current dimension function to use for examination.
       *  @return  true indicate that reblancing must occurs, otherwise false.
       */
      template <typename Dimension>
      bool
      operator()(const Relaxed_kdtree_node_base* x,
		 const Dimension& d) const
      {
	typedef std::pair<weight_type, weight_type> weight_pair;
	weight_type left_weight = (x->left != 0) ? x->left->weight : 0;
	weight_type right_weight = (x->right != 0) ? x->right->weight : 0;
	weight_pair p
	  = left_weight < right_weight ?
	  weight_pair(left_weight, right_weight) :
	  weight_pair(right_weight, left_weight);
	if(p.second > d() && p.first < (p.second >> 1) )
	  { return true; }
	else { return false; }
      }
    };

    /**
     *  @brief  A policy that resutls in a tightly balanced tree.
     *
     *  With such policy, the node is rebalanced if the smallest weight of left or
     *  right children is less than that of the biggest, by a difference equal the
     *  dimension of the tree plus 1.
     */
    struct tightly_balanced
    {
      /**
       *  @brief  Rebalancing predicate.
       *  @param x  The node to examine for rebalancing.
       *  @param d  The current dimension function to use for examination.
       *  @return  true indicate that reblancing must occurs, otherwise false.
       */
      template <typename Dimension>
      bool
      operator()(const Relaxed_kdtree_node_base* x,
		 const Dimension& d) const
      {
	typedef std::pair<weight_type, weight_type> weight_pair;
	weight_type left_weight = (x->left != 0) ? x->left->weight : 0;
	weight_type right_weight = (x->right != 0) ? x->right->weight : 0;
	weight_pair p
	  = left_weight < right_weight ?
	  weight_pair(left_weight, right_weight) :
	  weight_pair(right_weight, left_weight);
	if(p.second > (d() + 1) && p.first < (p.second - d() - 1))
	  { return true; }
	else { return false; }
      }
    };

    /**
     *  @brief  Provide tighter balacing with a fixed Difference.
     *
     *  With such policy, the node is rebalanced if the smallest weight of left
     *  and right children is different from the biggest by @Difference.
     */
    template <weight_type Difference>
    struct tight_balancing
    {
      /**
       *  @brief  Rebalancing predicate.
       *  @param x  The node to examine for rebalancing.
       *  @param d  The current dimension function to use for examination.
       *  @return  true indicate that reblancing must occurs, otherwise false.
       */
      template <typename Dimension>
      bool
      operator()(const Relaxed_kdtree_node_base* x,
		 const Dimension& d) const
      {
	typedef std::pair<weight_type, weight_type> weight_pair;
	weight_type left_weight = (x->left != 0) ? x->left->weight : 0;
	weight_type right_weight = (x->right != 0) ? x->right->weight : 0;
	weight_pair p
	  = left_weight < right_weight ?
	  weight_pair(left_weight, right_weight) :
	  weight_pair(right_weight, left_weight);
	if(p.second > Difference && p.first < (p.second - Difference))
	  { return true; }
	else { return false; }
      }
    };

    // Detailed implementation of the relaxed kd-tree. Used by point_set,
    // point_multiset, point_map, point_multimap, box_set, box_multiset and their
    // equivalent in variant orders: variant_pointer_set, as chosen by the
    // templates. Not used by neighbor_point_set,
    // neighbor_point_multiset... Compare must provide strict unordered ordering
    // along each dimension! Each node maintains the count of its children nodes
    // plus one.
    template <typename Dimension, typename Key, typename Compare,
	      typename Balance_policy, typename Alloc, bool Const_condition>
    class Relaxed_kdtree
      : public Kdtree_base<Dimension, Key, Relaxed_kdtree_node<Key>, Compare,
			   Alloc, Const_condition>,
						  private Balance_policy
    {
      typedef Relaxed_kdtree<Dimension, Key, Compare, Balance_policy,
			     Alloc, Const_condition> Self;
      typedef Kdtree_base<Dimension, Key, Relaxed_kdtree_node<Key>, Compare,
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

      typedef details::equal_iterator<Self>       equal_iterator;
      typedef details::const_equal_iterator<Self> const_equal_iterator;

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
      const Balance_policy&
      balance_policy() const
      { return *static_cast<const Balance_policy*>(this); }

    private:
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
	: Base(Dimension(), Compare(), allocator_type()), Balance_policy()
      { }

      explicit Relaxed_kdtree(const Dimension& d)
	: Base(d, Compare(), allocator_type()), Balance_policy()
      { }

      Relaxed_kdtree(const Dimension& d, const compare_type& c)
	: Base(d, c, allocator_type()), Balance_policy()
      { }

      Relaxed_kdtree(const Dimension& d, const compare_type& c,
		     const Balance_policy& b)
	: Base(d, c, allocator_type()), Balance_policy(b)
      { }

      Relaxed_kdtree(const Dimension& d, const compare_type& c,
		     const Balance_policy& b, const allocator_type& alloc)
	: Base(d, c, alloc), Balance_policy(b)
      { }

      Relaxed_kdtree(const Self& t)
	: Base(t), Balance_policy(t.balance_policy())
      {
	if (!t.empty())
	  {
	    // copy stuff now.
	  }
      }

      Self&
      operator=(const Self& t)
      {
	Base::operator=(t);
	node_count = 0;
	if (!t.empty())
	  {
	    // copy stuff now.
	  }
      }


    public:
      void swap(Self& t);

      // Insertion
      iterator
      insert(const key_type& v)
      {
	Base_ptr x = get_root();
	if (x == get_header())
	  {
	    Link_type new_node = create_node(v);
	    x->parent = new_node;
	    x->left = new_node;
	    x->right = new_node;
	    new_node->parent = x;
	    return unordered_iterator(new_node);
	  }
	else
	  {
	    iterator i = insert_aux(0, x, v);
	    ++x->weight; // increment after in case above alloc fails
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
      { return details::equal_range(*this, key); }

      std::pair<const_equal_iterator, const_equal_iterator>
      equal_range(const key_type& key) const
      { return details::const_equal_range(*this, key); }
      //@}
    };

  } // namespace details
} // namespace spatial

#endif // SPATIAL_RELAXED_KDTREE_HPP
