// -*- C++ -*-

/**
 *  @file   spatial_range.tpp
 *  @brief  
 *
 *  Change Log:
 *
 *  - 2009-10-19 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_RANGE_TPP
#define SPATIAL_RANGE_TPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

namespace spatial
{
  namespace details
  {

    template<typename Rank, typename Key, typename Node,
	     typename Predicate, bool Constant, typename Derived>
    inline void
    Range_iterator_base<Rank, Key, Node, Predicate, Constant, Derived>
    ::increment()
    {
      Predicate predicate = this->predicate();
      Rank rank = this->rank();
      SPATIAL_ASSERT_CHECK(!Node_base::header(impl.node));
      SPATIAL_ASSERT_CHECK(impl.node != 0);
      SPATIAL_ASSERT_CHECK(impl.node_dim() < rank());
      do
	{
	  if (impl.node->right != 0
	      && match(impl.node_dim(), SPATIAL_KEY(impl.node),
			  predicate) != above)
	    {
	      impl.node = impl.node->right;
	      impl.node_dim() = incr_dim(rank, impl.node_dim());
	      while (impl.node->left != 0
		     && match(impl.node_dim(), SPATIAL_KEY(impl.node),
				 predicate) != below)
		{
		  impl.node = impl.node->left;
		  impl.node_dim() = incr_dim(rank, impl.node_dim());
		}
	    }
	  else
	    {
	      Base_ptr p = impl.node->parent;
	      while (!Node_base::header(p) && impl.node == p->right)
		{
		  impl.node = p;
		  impl.node_dim() = decr_dim(rank, impl.node_dim());
		  p = impl.node->parent;
		}
	      impl.node = p;
	      impl.node_dim() = decr_dim(rank, impl.node_dim());
	    }
	}
      while (!Node_base::header(impl.node)
	     && match_all(rank, SPATIAL_KEY(impl.node),
			     predicate) == false);
      SPATIAL_ASSERT_CHECK(impl.node_dim() < rank());
      SPATIAL_ASSERT_CHECK(impl.node != 0);
    }

    template<typename Rank, typename Key, typename Node,
	     typename Predicate, bool Constant, typename Derived>
    inline void
    Range_iterator_base<Rank, Key, Node, Predicate, Constant, Derived>
    ::decrement()
    {
      Predicate predicate = this->predicate();
      Rank rank = this->rank();
      SPATIAL_ASSERT_CHECK(impl.node != 0);
      SPATIAL_ASSERT_CHECK(impl.node_dim() < rank());
      if (Node_base::header(impl.node))
	{
	  operator=(maximum(rank, predicate, 0, impl.node->parent));
	  return;
	}
      do
	{
	  if (impl.node->left != 0
	      && match(impl.node_dim(), SPATIAL_KEY(impl.node),
			  predicate) != below)
	    {
	      impl.node = impl.node->left;
	      impl.node_dim() = incr_dim(rank, impl.node_dim());
	      while (impl.node->right != 0
		     && match(impl.node_dim(), SPATIAL_KEY(impl.node),
				 predicate) != above)
		{
		  impl.node = impl.node->right;
		  impl.node_dim() = incr_dim(rank, impl.node_dim());
		}
	    }
	  else
	    {
	      Base_ptr p = impl.node->parent;
	      while (!Node_base::header(p) && impl.node == p->left)
		{
		  impl.node = p;
		  impl.node_dim() = decr_dim(rank, impl.node_dim());
		  p = impl.node->parent;
		}
	      impl.node = p;
	      impl.node_dim() = decr_dim(rank, impl.node_dim());
	    }
	}
      while (!Node_base::header(impl.node)
	     && match_all(rank, SPATIAL_KEY(impl.node),
			     predicate) == false);
      SPATIAL_ASSERT_CHECK(impl.node_dim() < rank());
      SPATIAL_ASSERT_CHECK(impl.node != 0);
    }

    template<typename Rank, typename Key, typename Node,
	     typename Predicate, bool Constant, typename Derived>
    inline Derived
    Range_iterator_base<Rank, Key, Node, Predicate, Constant, Derived>
    ::minimum
    (const Rank& rank, const Predicate& predicate,
     dimension_type node_dim, Base_ptr node)
    {
      SPATIAL_ASSERT_CHECK(node_dim < rank());
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      Base_ptr end = node->parent;
      // Quick positioning according to in-order transversal.
      while (node->right != 0
	     && match(node_dim, SPATIAL_KEY(node), predicate) == below)
	{
	  node = node->right;
	  node_dim = incr_dim(rank, node_dim);
	}
      while (node->left != 0
	     && match(node_dim, SPATIAL_KEY(node), predicate) != below)
	{
	  node = node->left;
	  node_dim = incr_dim(rank, node_dim);
	}
      // Start algorithm.
      do
	{
	  if (match_all(rank, SPATIAL_KEY(node), predicate) == true)
	    { break; }
	  if (node->right != 0
	      && match(node_dim, SPATIAL_KEY(node), predicate) != above)
	    {
	      node = node->right;
	      node_dim = incr_dim(rank, node_dim);
	      while (node->left != 0
		     && match(node_dim, SPATIAL_KEY(node), predicate) != below)
		{
		  node = node->left;
		  node_dim = incr_dim(rank, node_dim);
		}
	    }
	  else
	    {
	      Base_ptr p = node->parent;
	      while (p != end && node == p->right)
		{
		  node = p;
		  node_dim = decr_dim(rank, node_dim);
		  p = node->parent;
		}
	      node = p;
	      node_dim = decr_dim(rank, node_dim);
	    }
	}
      while (node != end);
      SPATIAL_ASSERT_CHECK(node_dim < rank());
      SPATIAL_ASSERT_CHECK(node != 0);
      return Derived(rank, predicate, node_dim,
		     static_cast<Link_type>(node));
    }

    template<typename Rank, typename Key, typename Node,
	     typename Predicate, bool Constant, typename Derived>
    inline Derived
    Range_iterator_base<Rank, Key, Node, Predicate, Constant, Derived>
    ::maximum
    (const Rank& rank, const Predicate& predicate,
     dimension_type node_dim, Base_ptr node)
    {
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(node_dim < rank());
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      Base_ptr end = node->parent;
      // Quick positioning according to in-order transversal.
      while (node->left != 0
	     && match(node_dim, SPATIAL_KEY(node), predicate) == above)
	{
	  node = node->left;
	  node_dim = incr_dim(rank, node_dim);
	}
      while (node->right != 0
	     && match(node_dim, SPATIAL_KEY(node), predicate) != above)
	{
	  node = node->right;
	  node_dim = incr_dim(rank, node_dim);
	}
      // Start algorithm.
      do
	{
	  if (match_all(rank, SPATIAL_KEY(node), predicate) == true)
	    { break; }
	  if (node->left != 0
	      && match(node_dim, SPATIAL_KEY(node), predicate) != below)
	    {
	      node = node->left;
	      node_dim = incr_dim(rank, node_dim);
	      while (node->right != 0
		     && match(node_dim, SPATIAL_KEY(node), predicate) != above)
		{
		  node = node->right;
		  node_dim = incr_dim(rank, node_dim);
		}
	    }
	  else
	    {
	      Base_ptr p = node->parent;
	      while (p != end && node == p->left)
		{
		  node = p;
		  node_dim = decr_dim(rank, node_dim);
		  p = node->parent;
		}
	      node = p;
	      node_dim = decr_dim(rank, node_dim);
	    }
	}
      while (node != end);
      SPATIAL_ASSERT_CHECK(node_dim < rank());
      SPATIAL_ASSERT_CHECK(node != 0);
      return Derived(rank, predicate, node_dim,
		     static_cast<Link_type>(node));
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_RANGE_TPP
