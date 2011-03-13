// -*- C++ -*-

/**
 *  @file   spatial_kdtree_base.tpp
 *  @brief  
 *
 *  Change Log:
 *
 *  - 2009-10-20 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_KDTREE_BASE_TPP
#define SPATIAL_KDTREE_BASE_TPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

namespace spatial
{
  namespace details
  {

    template<typename Rank, typename Key, typename Node,
	     typename Compare, typename Alloc, bool ConstantIterator>
    inline void
    Kdtree_base<Rank, Key, Node, Compare, Alloc, ConstantIterator>
    ::destroy_all_nodes()
    {
      Base_ptr node = get_root();
      while (!Node_base::header(node))
	{
	  if (node->left != 0) { node = node->left; }
	  else if (node->right != 0) { node = node->right; }
	  else
	    {
	      Base_ptr p = node->parent;
	      if (Node_base::header(p))
		{
		  set_root(get_header());
		  set_leftmost(get_header());
		  set_rightmost(get_header());
		}
	      else
		{
		  if (p->left == node) { p->left = 0; }
		  else { p->right = 0; }
		}
	      SPATIAL_ASSERT_CHECK(node != 0);
	      SPATIAL_ASSERT_CHECK(p != 0);
	      destroy_node(static_cast<Link_type>(node));
	      node = p;
	    }
	}
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_KDTREE_BASE_TPP
