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

namespace spatial
{
  namespace details
  {

    template<typename Rank, typename Key, typename Compare,
	     typename Balancing, typename Alloc, bool ConstIterator>
    inline void
    Relaxed_kdtree<Rank, Key, Compare, Balancing, Alloc, ConstIterator>
    ::copy_structure(const Self& other)
    {
      SPATIAL_ASSERT_CHECK(!other.empty());
      SPATIAL_ASSERT_CHECK(Base::empty());
      Const_Base_ptr other_node = other.get_root();
      Base_ptr node
	= Base::create_node(SPATIAL_KEY_CONST(other_node)); // may throw
      node->left = node->right = 0;
      node->parent = Base::get_header();
      Base::set_root(node);
      try
	{
	  while(!Node_base::header(other_node))
	    {
	      if (other_node->left != 0)
		{
		  other_node = other_node->left;
		  Base_ptr target = clone_node(other_node);
		  target->parent = node;
		  target->left = target->right = 0;
		  node->left = target;
		  node = node->left;
		}
	      else if (other_node->right != 0)
		{
		  other_node = other_node->right;
		  Base_ptr target = clone_node(other_node);
		  target->parent = node;
		  target->right = target->left = 0;
		  node->right = target;
		  node = node->right;
		}
	      else
		{
		  Const_Base_ptr p = other_node->parent;
		  while (!Node_base::header(p)
			 && (other_node == p->right || p->right == 0))
		    {
		      other_node = p;
		      node = node->parent;
		      p = other_node->parent;
		    }
		  other_node = p;
		  node = node->parent;
		  if (!Node_base::header(p))
		    {
		      other_node = other_node->right;
		      Base_ptr target = clone_node(other_node);
		      target->parent = node;
		      target->right = target->left = 0;
		      node->right = target;
		      node = node->right;
		    }
		}
	    }
	  SPATIAL_ASSERT_CHECK(!Base::empty());
	  SPATIAL_ASSERT_CHECK(Node_base::header(other_node));
	  SPATIAL_ASSERT_CHECK(Node_base::header(node));
	}
      catch (...)
	{ clear(); throw; } // clean-up before re-throw
      Base::set_leftmost(Node_base::minimum(Base::get_root()));
      Base::set_rightmost(Node_base::maximum(Base::get_root()));
    }

    template<typename Rank, typename Key, typename Compare,
	     typename Balancing, typename Alloc, bool ConstIterator>
    inline
    typename Relaxed_kdtree<Rank, Key, Compare,
			    Balancing, Alloc, ConstIterator>
    ::iterator
    Relaxed_kdtree<Rank, Key, Compare, Balancing,
		   Alloc, ConstIterator>
    ::insert_node
    (dimension_type node_dim, Base_ptr node, Base_ptr new_node)
    {
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      const Compare& cmp = Base::compare();
      const Rank& rank = Base::rank();
      while (true)
	{
	  SPATIAL_ASSERT_CHECK
	    ((node->right
	      ? static_cast<Weighted_node*>(node->right)->weight : 0)
	     + (node->left
		? static_cast<Weighted_node*>(node->left)->weight: 0)
	     + 1 == static_cast<Weighted_node*>(node)->weight);
	  // Balancing equal values on both side of the tree
	  if (cmp(node_dim, SPATIAL_KEY(new_node), SPATIAL_KEY(node))
	      || (!cmp(node_dim, SPATIAL_KEY(node), SPATIAL_KEY(new_node))
		  && (node->left == 0
		      || (node->right != 0
			  && static_cast<Link_type>(node->left)->weight
			  < static_cast<Link_type>(node->right)->weight))))
	    {
	      if (node->left == 0)
		{
		  node->left = new_node;
		  new_node->parent = node;
		  if (Base::get_leftmost() == node)
		    { Base::set_leftmost(new_node); }
		  ++static_cast<Weighted_node*>(node)->weight;
		  break;
		}
	      else
		{
		  if(m_balancing(static_cast<Weighted_node*>(node), rank))
		    {
		      Base_ptr replace = erase_node(node_dim, node);
		      SPATIAL_ASSERT_CHECK(replace != node);
		      SPATIAL_ASSERT_CHECK(replace != 0);
		      insert_node(node_dim, replace, node); // recursive insert
		      node = replace;
		    }
		  else
		    {
		      ++static_cast<Weighted_node*>(node)->weight;
		      node = node->left;
		      node_dim = incr_dim(rank, node_dim);
		    }
		}
	    }
	  else
	    {
	      if (node->right == 0)
		{
		  node->right = new_node;
		  new_node->parent = node;
		  if (Base::get_rightmost() == node)
		    { Base::set_rightmost(new_node); }
		  ++static_cast<Weighted_node*>(node)->weight;
		  break;
		}
	      else
		{
		  if(m_balancing(static_cast<Weighted_node*>(node), rank))
		    {
		      Base_ptr replace = erase_node(node_dim, node);
		      SPATIAL_ASSERT_CHECK(replace != node);
		      SPATIAL_ASSERT_CHECK(replace != 0);
		      insert_node(node_dim, replace, node); // recursive insert
		      node = replace;
		    }
		  else
		    {
		      ++static_cast<Weighted_node*>(node)->weight;
		      node = node->right;
		      node_dim = incr_dim(rank, node_dim);
		    }
		}
	    }
	}
      SPATIAL_ASSERT_CHECK(new_node != 0);
      SPATIAL_ASSERT_CHECK(!Node_base::header(new_node));
      SPATIAL_ASSERT_CHECK(!Node_base::header(new_node->parent));
      return iterator(static_cast<Link_type>(new_node));
    }

    template<typename Rank, typename Key, typename Compare,
	     typename Balancing, typename Alloc, bool ConstIterator>
    inline typename Relaxed_kdtree<Rank, Key, Compare, Balancing, Alloc,
				   ConstIterator>::Base_ptr
    Relaxed_kdtree<Rank, Key, Compare, Balancing, Alloc, ConstIterator>
    ::erase_node
    (dimension_type node_dim, Base_ptr node)
    {
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      typedef Mapping_iterator<Rank, Key, node_type, Compare> mapping_iterator;
      const Compare& cmp = Base::compare();
      const Rank& rank = Base::rank();
      Base_ptr end = node->parent;
      while (node->right != 0 || node->left != 0)
	{
	  mapping_iterator candidate;
	  if (node->left != 0
	      && (node->right == 0
		  || static_cast<Link_type>(node->right)->weight
		  < static_cast<Link_type>(node->left)->weight))
	    {
	      candidate = mapping_iterator::maximum
		(rank, cmp, node_dim, incr_dim(rank, node_dim), node->left);
	      if (Base::get_leftmost() == candidate.impl.node)
		{ Base::set_leftmost(node); }
	      if (Base::get_rightmost() == node)
		{ Base::set_rightmost(candidate.impl.node); }
	    }
	  else
	    {
	      candidate = mapping_iterator::minimum
		(rank, cmp, node_dim, incr_dim(rank, node_dim), node->right);
	      if (Base::get_rightmost() == candidate.impl.node)
		{ Base::set_rightmost(node); }
	      if (Base::get_leftmost() == node)
		{ Base::set_leftmost(candidate.impl.node); }
	    }
	  spatial::details::swap(*static_cast<Link_type>(candidate.impl.node),
				 *static_cast<Link_type>(node));
	  node_dim = candidate.impl.node_dim;
	}
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(node->right == 0);
      SPATIAL_ASSERT_CHECK(node->left == 0);
      SPATIAL_ASSERT_CHECK(node->parent != 0);
      Base_ptr p = node->parent;
      if (Node_base::header(p))
	{
	  Base::set_root(Base::get_header());
	  Base::set_leftmost(Base::get_header());
	  Base::set_rightmost(Base::get_header());
	}
      else
	{
	  if (p->left == node)
	    {
	      p->left = 0;
	      if (Base::get_leftmost() == node) { Base::set_leftmost(p); }
	    }
	  else
	    {
	      p->right = 0;
	      if (Base::get_rightmost() == node) { Base::set_rightmost(p); }
	    }
	  // decrease count to all parents up to end
	  while(node->parent != end)
	    {
	      node = node->parent;
	      SPATIAL_ASSERT_CHECK(static_cast<Weighted_node*>(node)->weight > 1);
	      --static_cast<Weighted_node*>(node)->weight;
	    }
	}
      return node;
    }

    template<typename Rank, typename Key, typename Compare,
	     typename Balancing, typename Alloc, bool ConstIterator>
    inline void
    Relaxed_kdtree<Rank, Key, Compare, Balancing, Alloc, ConstIterator>
    ::erase
    (const_iterator pointer)
    {
      except::check_iterator_argument(pointer.node);
      const Rank& rank = Base::rank();
      dimension_type node_dim = rank() - 1;
      Const_Base_ptr node = pointer.node;
      Base_ptr p = const_cast<Base_ptr>(node->parent);
      while (!Node_base::header(node))
	{
	  node = node->parent;
	  node_dim = incr_dim(rank, node_dim);
	}
      except::check_invalid_iterator(node, Base::get_header());
      erase_node(node_dim, const_cast<Base_ptr>(pointer.node));
      while(!Node_base::header(p))
	{
	  SPATIAL_ASSERT_CHECK(static_cast<Weighted_node*>(p)->weight > 1);
	  --static_cast<Weighted_node*>(p)->weight;
	  p = p->parent;
	}
      destroy_node(static_cast<Link_type>(pointer.node));
    }

    template<typename Rank, typename Key, typename Compare,
	     typename Balancing, typename Alloc, bool ConstIterator>
    inline typename Relaxed_kdtree<Rank, Key, Compare, Balancing, Alloc,
				   ConstIterator>::size_type
    Relaxed_kdtree<Rank, Key, Compare, Balancing, Alloc, ConstIterator>
    ::erase
    (const key_type& key)
    {
      size_type cnt = 0;
      while (true)
	{
	  if (Base::empty()) break;
	  std::pair<typename Base::equal_iterator,
		    typename Base::equal_iterator>
	    found = Base::equal_range(key);
	  typename Base::equal_iterator begin = found.first;
	  for (; begin != found.second; ++begin)
	    {
	      /*
	       * If the compiler throws you an error at the line below, please,
	       * define the '==' operator for the type 'key_type'.
	       */
	      if (*begin == key)
		{
		  Base_ptr p = begin.impl.node->parent;
		  erase_node(begin.impl.node_dim(), begin.impl.node);
		  while(!Node_base::header(p))
		    {
		      SPATIAL_ASSERT_CHECK(static_cast<Weighted_node*>(p)->weight > 1);
		      --static_cast<Weighted_node*>(p)->weight;
		      p = p->parent;
		    }
		  destroy_node(static_cast<Link_type>(begin.impl.node));
		  ++cnt;
		  // We modified the tree, so we re-initialize the iterators
		  break;
		}
	    }
	  if (begin == found.second) break;
	}
      return cnt;
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_RELAXED_KDTREE_TPP
