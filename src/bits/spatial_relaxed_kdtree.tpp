// -*- C++ -*-

/**
 *  @file   spatial_relaxed_kdtree.tpp
 *  @brief  
 *
 *  Change Log:
 *
 *  - 2009-02-27 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_RELAXED_KDTREE_TPP
#define SPATIAL_RELAXED_KDTREE_TPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#include <cstring> // for memcpy

namespace spatial
{
  namespace details
  {

    template<typename Dimension, typename Key, typename Compare,
	     typename Balance_policy, typename Alloc, bool ConstIterator>
    inline
    typename Relaxed_kdtree<Dimension, Key, Compare,
			    Balance_policy, Alloc, ConstIterator>
    ::unordered_iterator
    Relaxed_kdtree<Dimension, Key, Compare, Balance_policy,
		   Alloc, ConstIterator>::insert_aux
    (dimension_type k,
     typename Relaxed_kdtree<Dimension, Key, Compare, Balance_policy,
     Alloc, ConstIterator>::Base_ptr x,
     const typename Relaxed_kdtree<Dimension, Key, Compare, Balance_policy,
     Alloc, ConstIterator>::key_type& v)
    {
      Link_type new_node = create_node(v);
      while (true)
	{
	  // Balancing equal values on both side of the tree
	  if (compare()(k, v, static_cast<Const_Link_type>(x)->key_field)
	      || (!compare()(k, static_cast<Const_Link_type>(x)->key_field, v)
		  && (x->left == 0 || (x->right != 0
				       && x->left->weight < x->right->weight))))
	    {
	      if (x->left != 0)
		{
		  ++x->left->weight;
		  bool balance = balance_policy()(x, dimension_function());
		  if (balance)
		    {
		      --x->left->weight;
		      key_type tmp = static_cast<Link_type>(x)->key_field;
		      if (x->left == 0 || (x->right != 0
					   && x->left->weight < x->right->weight))
			{ balance_aux(true, k, x); }
		      else
			{ balance_aux(false, k, x); }
		      insert_aux(k, x, tmp);
		    }
		  else
		    { x = x->left; k = (k + 1) % dimension(); }
		}
	      else
		{
		  x->left = new_node;
		  new_node->parent = x;
		  if (get_leftmost() == x)
		    { get_header()->left = new_node; }
		  break;
		}
	    }
	  else
	    {
	      if (x->right != 0)
		{
		  ++x->right->weight;
		  bool balance = balance_policy()(x, dimension_function());
		  if (balance)
		    {
		      --x->right->weight;
		      key_type tmp = static_cast<Link_type>(x)->key_field;
		      if (x->right == 0 || (x->left != 0
					    && x->right->weight < x->left->weight))
			{ balance_aux(false, k, x); }
		      else
			{ balance_aux(true, k, x); }
		      insert_aux(k, x, tmp);
		    }
		  else
		    { x = x->right; k = (k + 1) % dimension(); }
		}
	      else
		{
		  x->right = new_node;
		  new_node->parent = x;
		  if (get_rightmost() == x)
		    { get_header()->right = new_node; }
		  break;
		}
	    }
	}
      return unordered_iterator(new_node);
    }

    // insert the new node in place of current if it is the minimum/maximum!

    template<typename Dimension, typename Key, typename Compare,
	     typename Balance_policy, typename Alloc, bool ConstIterator>
    inline void
    Relaxed_kdtree<Dimension, Key, Compare, Balance_policy, Alloc, ConstIterator>
    ::balance_aux
    (bool left_tree, dimension_type k,
     typename Relaxed_kdtree<Dimension, Key, Compare, Balance_policy,
     Alloc, ConstIterator>::Base_ptr x)
    {
      Base_ptr node = x;
      dimension_type node_dim = k;
      if (left_tree)
	{
	  node = iterator::maximum(k, (k + 1) % dimension(),
				   node->left, get_header(),
				   dimension_function(), compare()).node;
	}
      else
	{
	  node = iterator::minimum(k, (k + 1) % dimension(),
				   node->right, get_header(),
				   dimension_function(), compare()).node;
	}
      Base_ptr up = node;
      while (up != x)
	{
	  --up->weight;
	  node_dim = (node_dim + 1) % dimension();
	  up = up->parent;
	}
      std::memcpy(&static_cast<Link_type>(x)->key_field,
		  &static_cast<Link_type>(node)->key_field,
		  sizeof(key_type));
      if (node->left == 0 && node->right == 0)
	{
	  if (node->parent->left == node)
	    {
	      if (get_leftmost() == node)
		{ get_header()->left = node->parent; }
	      node->parent->left = 0;
	    }
	  else
	    {
	      if (get_rightmost() == node)
		{ get_header()->right = node->parent; }
	      node->parent->right = 0;
	    }
	  destroy_node(static_cast<Link_type>(node));
	  node = 0;
	}
      else
	{
	  if (node->left == 0
	      || (node->right != 0
		  && node->left->weight < node->right->weight))
	    { left_tree = false; }
	  else
	    { left_tree = true; }
	  x = node;
	}
      while(node != 0)
	{
	  if (left_tree)
	    {
	      node = mapping_iterator::maximum
		(node_dim, (node_dim + 1) % dimension(), x->left,
		 get_header(), dimension_function(), compare()).node;
	    }
	  else
	    {
	      node = mapping_iterator::minimum
		(node_dim, (node_dim + 1) % dimension(), x->right,
		 get_header(), dimension_function(), compare()).node;
	    }
	  Base_ptr up = node;
	  while (up != x)
	    {
	      --up->weight;
	      node_dim = (node_dim + 1) % dimension();
	      up = up->parent;
	    }
	  std::memcpy(&static_cast<Link_type>(x)->key_field,
		      &static_cast<Link_type>(node)->key_field,
		      sizeof(key_type));
	  if (node->left == 0 && node->right == 0)
	    {
	      if (node->parent->left == node)
		{
		  if (get_leftmost() == node)
		    { get_header()->left = node->parent; }
		  node->parent->left = 0;
		}
	      else
		{
		  if (get_rightmost() == node)
		    { get_header()->right = node->parent; }
		  node->parent->right = 0;
		}
	      destroy_node(static_cast<Link_type>(node));
	      node = 0;
	    }
	  else
	    {
	      if (node->left == 0
		  || (node->right != 0
		      && node->left->weight < node->right->weight))
		{ left_tree = false; }
	      else
		{ left_tree = true; }
	      x = node;
	    }
	}
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_RELAXED_KDTREE_TPP
