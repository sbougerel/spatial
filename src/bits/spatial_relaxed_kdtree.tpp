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

    template<typename Rank, typename Key, typename Compare,
	     typename Balancing, typename Alloc, bool ConstIterator>
    inline
    typename Relaxed_kdtree<Rank, Key, Compare,
			    Balancing, Alloc, ConstIterator>
    ::iterator
    Relaxed_kdtree<Rank, Key, Compare, Balancing,
		   Alloc, ConstIterator>
    ::insert_aux
    (dimension_type node_dim,
     typename Relaxed_kdtree<Rank, Key, Compare, Balancing,
     Alloc, ConstIterator>::Base_ptr node,
     const typename Relaxed_kdtree<Rank, Key, Compare, Balancing,
     Alloc, ConstIterator>::key_type& key)
    {
      Link_type new_node = create_node(key);
      const Compare& cmp = Base::compare();
      const Rank& rank = Base::rank();
      while (true)
	{
	  // Balancing equal values on both side of the tree
	  if (cmp(node_dim, key, SPATIAL_KEY(node))
	      || (!cmp(node_dim, SPATIAL_KEY(node), key)
		  && (node->left == 0
		      || (node->right != 0
			  && node->left->weight < node->right->weight))))
	    {
	      if (node->left != 0)
		{
		  ++node->left->weight;
		  if (m_balancing(node, rank))
		    {
		      --node->left->weight;
		      if (node->left == 0
			  || (node->right != 0
			      && node->left->weight < node->right->weight))
			{ balance_aux(true, node_dim, node); }
		      else
			{ balance_aux(false, node_dim, node); }
		      insert_aux(node_dim, node, SPATIAL_KEY(node));
		    }
		  else
		    { node = node->left; node_dim = incr_dim(rank, node_dim); }
		}
	      else
		{
		  node->left = new_node;
		  new_node->parent = node;
		  if (Base::get_leftmost() == node)
		    { Base::get_header()->left = new_node; }
		  break;
		}
	    }
	  else
	    {
	      if (node->right != 0)
		{
		  ++node->right->weight;
		  if (m_balancing(node, rank))
		    {
		      --node->right->weight;
		      key_type tmp = SPATIAL_KEY(node);
		      if (node->right == 0
			  || (node->left != 0
			      && node->right->weight < node->left->weight))
			{ balance_aux(false, node_dim, node); }
		      else
			{ balance_aux(true, node_dim, node); }
		      insert_aux(node_dim, node, tmp);
		    }
		  else
		    { node = node->right; node_dim = incr_dim(rank, node_dim); }
		}
	      else
		{
		  node->right = new_node;
		  new_node->parent = node;
		  if (Base::get_rightmost() == node)
		    { Base::get_header()->right = new_node; }
		  break;
		}
	    }
	}
      return iterator(new_node);
    }

    // insert the new node in place of current if it is the minimum/maximum!

    template<typename Rank, typename Key, typename Compare,
	     typename Balancing, typename Alloc, bool ConstIterator>
    inline void
    Relaxed_kdtree<Rank, Key, Compare, Balancing, Alloc, ConstIterator>
    ::balance_aux
    (bool left_tree, dimension_type top_dim,
     typename Relaxed_kdtree<Rank, Key, Compare, Balancing,
     Alloc, ConstIterator>::Base_ptr top)
    {
      const Compare& cmp = Base::compare();
      const Rank& rank = Base::rank();
      Base_ptr node = top;
      dimension_type node_dim = top_dim;
      if (left_tree)
	{
	  node = iterator::maximum
	    (top_dim, incr_dim(rank, top_dim), top->left, Base::get_header(),
	     rank, cmp).node;
	}
      else
	{
	  node = iterator::minimum
	    (top_dim, incr_dim(rank, top_dim), top->right, Base::get_header(),
	     rank, cmp).node;
	}
      Base_ptr up = node;
      while (up != top)
	{
	  --up->weight;
	  node_dim = decr_dim(rank, node_dim);
	  up = up->parent;
	}
      // SWAP instead!!
      std::memcpy(&static_cast<Link_type>(top)->key_field,
		  &static_cast<Link_type>(node)->key_field,
		  sizeof(key_type));
      if (node->left == 0 && node->right == 0)
	{
	  if (node->parent->left == node)
	    {
	      if (Base::get_leftmost() == node)
		{ Base::get_header()->left = node->parent; }
	      node->parent->left = 0;
	    }
	  else
	    {
	      if (Base::get_rightmost() == node)
		{ Base::get_header()->right = node->parent; }
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
	  top = node;
	}
      while(node != 0)
	{
	  if (left_tree)
	    {
	      node = iterator::maximum
		(node_dim, incr_dim(rank, node_dim), node->left,
		 Base::get_header(), rank, cmp).node;
	    }
	  else
	    {
	      node = view::details::mapping_iterator<Self>::type::minimum
		(node_dim, incr_dim(rank, node_dim), node->right,
		 Base::get_header(), rank, cmp).impl.node;
	    }
	  Base_ptr up = node;
	  while (up != top)
	    {
	      --up->weight;
	      node_dim = decr_dim(rank, node_dim);
	      up = up->parent;
	    }
	  // SWAP HERE TOO!!!!
	  std::memcpy(&static_cast<Link_type>(top)->key_field,
		      &static_cast<Link_type>(node)->key_field,
		      sizeof(key_type));
	  if (node->left == 0 && node->right == 0)
	    {
	      if (node->parent->left == node)
		{
		  if (Base::get_leftmost() == node)
		    { Base::get_header()->left = node->parent; }
		  node->parent->left = 0;
		}
	      else
		{
		  if (Base::get_rightmost() == node)
		    { Base::get_header()->right = node->parent; }
		  node->parent->right = 0;
		}
	      Base::destroy_node(static_cast<Link_type>(node));
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
	      top = node;
	    }
	}
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_RELAXED_KDTREE_TPP
