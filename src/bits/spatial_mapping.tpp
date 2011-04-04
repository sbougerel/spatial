// -*- C++ -*-

/**
 *  @file   spatial_mapping.tpp
 *  @brief  
 *
 *  Change Log:
 *
 *  - 2009-02-27 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_MAPPING_TPP
#define SPATIAL_MAPPING_TPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

namespace spatial
{
  namespace details
  {
    namespace mapping
    {
      template<typename Impl, typename Base_ptr, typename Link_type,
	       typename Rank, typename Key, typename Compare>
      inline void
      increment(Impl& impl, details::relaxed_invariant_tag)
      {
	const Rank& rank = *static_cast<const Rank*>(&impl);
	const Compare& cmp = impl.mapping_dim.base();
	SPATIAL_ASSERT_CHECK(impl.node != 0);
	SPATIAL_ASSERT_CHECK(!Node_base::header(impl.node));
	SPATIAL_ASSERT_CHECK(impl.mapping_dim() < rank());
	SPATIAL_ASSERT_CHECK(impl.node_dim < rank());
	Base_ptr best = 0; // not null when best has been found
	Base_ptr node = impl.node;
	dimension_type node_dim = impl.node_dim;
	dimension_type best_dim = impl.node_dim;
	if (node->left != 0 && node_dim != impl.mapping_dim())
	  {
	    do
	      {
		node = node->left;
		node_dim = incr_dim(rank, node_dim);
	      }
	    while (node->left != 0
		   && (node_dim != impl.mapping_dim()
		       || !cmp(node_dim, SPATIAL_KEY(node),
			      SPATIAL_KEY(impl.node))));
	  }
	Base_ptr ceiling = node->parent; // the upper limit of unvisited nodes
	bool sibling_visited = false;    // at ceiling, sibling node is visited
	if (less_by_ref(cmp, impl.mapping_dim(), SPATIAL_KEY(impl.node),
			SPATIAL_KEY(node)))
	  { best = node; best_dim = node_dim; }
	do
	  {
	    if (node->right != 0
		&& (node_dim != impl.mapping_dim() || best == 0
		    || !cmp(node_dim, SPATIAL_KEY(best), SPATIAL_KEY(node))))
	      {
		node = node->right;
		node_dim = incr_dim(rank, node_dim);
		while (node->left != 0
		       && (node_dim != impl.mapping_dim()
			   || !cmp(node_dim, SPATIAL_KEY(node),
				   SPATIAL_KEY(impl.node))))
		  {
		    node = node->left;
		    node_dim = incr_dim(rank, node_dim);
		  }
		if (less_by_ref(cmp, impl.mapping_dim(),
				SPATIAL_KEY(impl.node),
				SPATIAL_KEY(node))
		    && (best == 0
			|| less_by_ref(cmp, impl.mapping_dim(),
				       SPATIAL_KEY(node), SPATIAL_KEY(best))))
		  { best = node; best_dim = node_dim; }
	      }
	    else
	      {
		Base_ptr p = node->parent;
		// go upward as long as we don't hit header or ceiling
		while (!Node_base::header(p)
		       && ((p != ceiling)
			   ? (p->right == node)
			   // either sibling is visited or there is one child
			   : (sibling_visited
			      || (p->right == node && p->left == 0)
			      || (p->left == node && p->right == 0))))
		  {
		    node = p;
		    node_dim = decr_dim(rank, node_dim);
		    p = node->parent;
		    if (node == ceiling)
		      {
			if (less_by_ref(cmp, impl.mapping_dim(),
					SPATIAL_KEY(impl.node),
					SPATIAL_KEY(node))
			    && (best == 0
				|| less_by_ref(cmp, impl.mapping_dim(),
					       SPATIAL_KEY(node),
					       SPATIAL_KEY(best))))
			  { best = node; best_dim = node_dim; }
			sibling_visited = false;
			ceiling = p;
		      }
		  }
		if (!Node_base::header(p) && p == ceiling)
		  {
		    if (p->right == node) { node = p->left; }
		    else { node = p->right; }
		    sibling_visited = true;
		    // go to full left in unvisited sibling
		    while (node->left != 0
			   && (node_dim != impl.mapping_dim()
			       || !cmp(node_dim,
				       SPATIAL_KEY(node),
				       SPATIAL_KEY(impl.node))))
		      {
			node = node->left;
			node_dim = incr_dim(rank, node_dim);
		      }
		    if (less_by_ref(cmp, impl.mapping_dim(),
				    SPATIAL_KEY(impl.node),
				    SPATIAL_KEY(node))
			&& (best == 0
			    || less_by_ref(cmp, impl.mapping_dim(),
					   SPATIAL_KEY(node),
					   SPATIAL_KEY(best))))
		      { best = node; best_dim = node_dim; }
		  }
		else
		  {
		    node = p;
		    node_dim = decr_dim(rank, node_dim);
		    if (!Node_base::header(node))
		      {
			if (less_by_ref(cmp, impl.mapping_dim(),
					SPATIAL_KEY(impl.node),
					SPATIAL_KEY(node))
			    && (best == 0
				|| less_by_ref(cmp, impl.mapping_dim(),
					       SPATIAL_KEY(node),
					       SPATIAL_KEY(best))))
			  { best = node; best_dim = node_dim; }
		      }
		  }
	      }
	  }
	while (!Node_base::header(node));
	if (best != 0) { impl.node = best; impl.node_dim = best_dim; }
	else { impl.node = node; impl.node_dim = node_dim; }
	SPATIAL_ASSERT_CHECK(impl.mapping_dim() < rank());
	SPATIAL_ASSERT_CHECK(impl.node_dim < rank());
	SPATIAL_ASSERT_CHECK(impl.node != 0);
      }

      template<typename Impl, typename Base_ptr, typename Link_type,
	       typename Rank, typename Key, typename Compare>
      inline void
      increment(Impl& impl, details::strict_invariant_tag)
      {
	const Rank& rank = *static_cast<const Rank*>(&impl);
	const Compare& cmp = impl.mapping_dim.base();
	SPATIAL_ASSERT_CHECK(impl.node != 0);
	SPATIAL_ASSERT_CHECK(!Node_base::header(impl.node));
	SPATIAL_ASSERT_CHECK(impl.mapping_dim() < rank());
	SPATIAL_ASSERT_CHECK(impl.node_dim < rank());
	Base_ptr best = 0; // not null when best has been found
	Base_ptr node = impl.node;
	dimension_type node_dim = impl.node_dim;
	dimension_type best_dim = impl.node_dim;
	if (node->left != 0 && node_dim != impl.mapping_dim())
	  {
	    do
	      {
		node = node->left;
		node_dim = incr_dim(rank, node_dim);
	      }
	    while (node->left != 0
		   && (node_dim != impl.mapping_dim() // optimization
		       || cmp(node_dim, SPATIAL_KEY(impl.node),
			      SPATIAL_KEY(node))));
	  }
	Base_ptr ceiling = node->parent; // the upper limit of unvisited nodes
	bool sibling_visited = false;    // at ceiling, sibling node is visited
	if (less_by_ref(cmp, impl.mapping_dim(), SPATIAL_KEY(impl.node),
			SPATIAL_KEY(node)))
	  { best = node; best_dim = node_dim; }
	do
	  {
	    if (node->right != 0
		&& (node_dim != impl.mapping_dim() || best == 0
		    || !cmp(node_dim, SPATIAL_KEY(best), SPATIAL_KEY(node))))
	      {
		node = node->right;
		node_dim = incr_dim(rank, node_dim);
		while (node->left != 0
		       && (node_dim != impl.mapping_dim()  // optimization
			   || cmp(node_dim, SPATIAL_KEY(impl.node),
				  SPATIAL_KEY(node))))
		  {
		    node = node->left;
		    node_dim = incr_dim(rank, node_dim);
		  }
		if (less_by_ref(cmp, impl.mapping_dim(),
				SPATIAL_KEY(impl.node),
				SPATIAL_KEY(node))
		    && (best == 0
			|| less_by_ref(cmp, impl.mapping_dim(),
				       SPATIAL_KEY(node), SPATIAL_KEY(best))))
		  { best = node; best_dim = node_dim; }
	      }
	    else
	      {
		Base_ptr p = node->parent;
		// go upward as long as we don't hit header or ceiling
		while (!Node_base::header(p)
		       && ((p != ceiling)
			   ? (p->right == node)
			   // either sibling is visited or there is one child
			   : (sibling_visited
			      || (p->right == node && p->left == 0)
			      || (p->left == node && p->right == 0))))
		  {
		    node = p;
		    node_dim = decr_dim(rank, node_dim);
		    p = node->parent;
		    if (node == ceiling)
		      {
			if (less_by_ref(cmp, impl.mapping_dim(),
					SPATIAL_KEY(impl.node),
					SPATIAL_KEY(node))
			    && (best == 0
				|| less_by_ref(cmp, impl.mapping_dim(),
					       SPATIAL_KEY(node),
					       SPATIAL_KEY(best))))
			  { best = node; best_dim = node_dim; }
			sibling_visited = false;
			ceiling = p;
		      }
		  }
		if (!Node_base::header(p) && p == ceiling)
		  {
		    if (p->right == node) { node = p->left; }
		    else { node = p->right; }
		    sibling_visited = true;
		    // go to full left in unvisited sibling
		    while (node->left != 0
			   && (node_dim != impl.mapping_dim() // optimization
			       || cmp(node_dim,
				      SPATIAL_KEY(impl.node),
				      SPATIAL_KEY(node))))
		      {
			node = node->left;
			node_dim = incr_dim(rank, node_dim);
		      }
		    if (less_by_ref(cmp, impl.mapping_dim(),
				    SPATIAL_KEY(impl.node),
				    SPATIAL_KEY(node))
			&& (best == 0
			    || less_by_ref(cmp, impl.mapping_dim(),
					   SPATIAL_KEY(node),
					   SPATIAL_KEY(best))))
		      { best = node; best_dim = node_dim; }
		  }
		else
		  {
		    node = p;
		    node_dim = decr_dim(rank, node_dim);
		    if (!Node_base::header(node))
		      {
			if (less_by_ref(cmp, impl.mapping_dim(),
					SPATIAL_KEY(impl.node),
					SPATIAL_KEY(node))
			    && (best == 0
				|| less_by_ref(cmp, impl.mapping_dim(),
					       SPATIAL_KEY(node),
					       SPATIAL_KEY(best))))
			  { best = node; best_dim = node_dim; }
		      }
		  }
	      }
	  }
	while (!Node_base::header(node));
	if (best != 0) { impl.node = best; impl.node_dim = best_dim; }
	else { impl.node = node; impl.node_dim = node_dim; }
	SPATIAL_ASSERT_CHECK(impl.mapping_dim() < rank());
	SPATIAL_ASSERT_CHECK(impl.node_dim < rank());
	SPATIAL_ASSERT_CHECK(impl.node != 0);
      }

      // Find the maximum from node and stop when reaching the parent. Iterate in
      // right-first fashion.
      template<typename Iterator, typename Base_ptr, typename Link_type,
	       typename Rank, typename Key, typename Compare>
      inline Iterator
      maximum
      (const Rank& rank, const Compare& cmp,
       dimension_type mapping_dim, dimension_type node_dim,
       Base_ptr node, details::relaxed_invariant_tag)
      {
	SPATIAL_ASSERT_CHECK(node != 0);
	SPATIAL_ASSERT_CHECK(mapping_dim < rank());
	SPATIAL_ASSERT_CHECK(node_dim < rank());
	SPATIAL_ASSERT_CHECK(!Node_base::header(node));
	Base_ptr end = node->parent;
	while (node->right != 0)
	  {
	    node = node->right;
	    node_dim = incr_dim(rank, node_dim);
	  }
	Base_ptr best = node;
	dimension_type best_dim = node_dim;
	do
	  {
	    if (node->left != 0
		&& (node_dim != mapping_dim
		    || !cmp(mapping_dim, SPATIAL_KEY(node), SPATIAL_KEY(best))))
	      {
		node = node->left;
		node_dim = incr_dim(rank, node_dim);
		while (node->right != 0)
		  {
		    node = node->right;
		    node_dim = incr_dim(rank, node_dim);
		  }
		if (less_by_ref(cmp, mapping_dim,
				SPATIAL_KEY(best), SPATIAL_KEY(node)))
		  { best = node; best_dim = node_dim; }
	      }
	    else
	      {
		Base_ptr p = node->parent;
		while (p != end && p->left == node)
		  {
		    node = p;
		    node_dim = decr_dim(rank, node_dim);
		    p = node->parent;
		  }
		node = p; node_dim = decr_dim(rank, node_dim);
		if (node != end
		    && less_by_ref(cmp, mapping_dim,
				   SPATIAL_KEY(best), SPATIAL_KEY(node)))
		  { best = node; best_dim = node_dim; }
	      }
	  }
	while (node != end);
	SPATIAL_ASSERT_CHECK(best_dim < rank());
	SPATIAL_ASSERT_CHECK(!Node_base::header(best));
	SPATIAL_ASSERT_CHECK(best != 0);
	SPATIAL_ASSERT_CHECK(node != 0);
	return Iterator(rank, cmp, mapping_dim, best_dim,
			static_cast<Link_type>(best));
      }

      // Specialized template that works a little faster on regular kdtree tree
      // nodes since invariant is applied strictly.
      template<typename Iterator, typename Base_ptr, typename Link_type,
	       typename Rank, typename Key, typename Compare>
      inline Iterator
      maximum
      (const Rank& rank, const Compare& cmp,
       dimension_type mapping_dim, dimension_type node_dim,
       Base_ptr node, details::strict_invariant_tag)
      {
	SPATIAL_ASSERT_CHECK(node != 0);
	SPATIAL_ASSERT_CHECK(mapping_dim < rank());
	SPATIAL_ASSERT_CHECK(node_dim < rank());
	SPATIAL_ASSERT_CHECK(!Node_base::header(node));
	Base_ptr end = node->parent;
	while (node->right != 0)
	  {
	    node = node->right;
	    node_dim = incr_dim(rank, node_dim);
	  }
	Base_ptr best = node;
	dimension_type best_dim = node_dim;
	do
	  {
	    if (node->left != 0 && node_dim != mapping_dim) // optimization
	      {
		node = node->left;
		node_dim = incr_dim(rank, node_dim);
		while (node->right != 0)
		  {
		    node = node->right;
		    node_dim = incr_dim(rank, node_dim);
		  }
		if (less_by_ref(cmp, mapping_dim,
				SPATIAL_KEY(best), SPATIAL_KEY(node)))
		  { best = node; best_dim = node_dim; }
	      }
	    else
	      {
		Base_ptr p = node->parent;
		while (p != end && p->left == node)
		  {
		    node = p;
		    node_dim = decr_dim(rank, node_dim);
		    p = node->parent;
		  }
		node = p; node_dim = decr_dim(rank, node_dim);
		if (node != end
		    && less_by_ref(cmp, mapping_dim,
				   SPATIAL_KEY(best), SPATIAL_KEY(node)))
		  { best = node; best_dim = node_dim; }
	      }
	  }
	while (node != end);
	SPATIAL_ASSERT_CHECK(best_dim < rank());
	SPATIAL_ASSERT_CHECK(!Node_base::header(best));
	SPATIAL_ASSERT_CHECK(best != 0);
	SPATIAL_ASSERT_CHECK(node != 0);
	return Iterator(rank, cmp, mapping_dim, best_dim,
			static_cast<Link_type>(best));
      }

      // Walk tree nodes in left-first fashion, bouncing off values that are lower
      // than key.
      template<typename Iterator, typename Base_ptr, typename Link_type,
	       typename Rank, typename Key, typename Compare>
      inline Iterator
      lower_bound
      (const Rank& rank, const Compare& cmp,
       dimension_type mapping_dim, dimension_type node_dim,
       Base_ptr node, const Key& flag, details::relaxed_invariant_tag)
      {
	SPATIAL_ASSERT_CHECK(node != 0);
	SPATIAL_ASSERT_CHECK(mapping_dim < rank());
	SPATIAL_ASSERT_CHECK(node_dim < rank());
	SPATIAL_ASSERT_CHECK(!Node_base::header(node));
	Base_ptr end = node->parent;
	Base_ptr best = end;
	dimension_type best_dim = decr_dim(rank, node_dim);
	while (node->left != 0
	       && (node_dim != mapping_dim
		   || !cmp(node_dim, SPATIAL_KEY(node), flag)))
	  {
	    node = node->left;
	    node_dim = incr_dim(rank, node_dim);
	  }
	if (!cmp(mapping_dim, SPATIAL_KEY(node), flag))
	  { best = node; best_dim = node_dim; }
	do
	  {
	    if (node->right != 0
		&& (node_dim != mapping_dim
		    || best == 0
		    || !cmp(mapping_dim,
			    SPATIAL_KEY(best), SPATIAL_KEY(node))))
	      {
		node = node->right;
		node_dim = incr_dim(rank, node_dim);
		while (node->left != 0
		       && (node_dim != mapping_dim
			   || !cmp(node_dim, SPATIAL_KEY(node), flag)))
		  {
		    node = node->left;
		    node_dim = incr_dim(rank, node_dim);
		  }
		if (!cmp(mapping_dim, SPATIAL_KEY(node), flag)
		    && (best == 0
			|| less_by_ref(cmp, mapping_dim,
				       SPATIAL_KEY(node), SPATIAL_KEY(best))))
		  { best = node; best_dim = node_dim; }
	      }
	    else
	      {
		Base_ptr p = node->parent;
		while (p != end && p->right == node)
		  {
		    node = p;
		    node_dim = decr_dim(rank, node_dim);
		    p = node->parent;
		  }
		node = p; node_dim = decr_dim(rank, node_dim);
		if (node != end
		    && !cmp(mapping_dim, SPATIAL_KEY(node), flag)
		    && (best == 0
			|| less_by_ref(cmp, mapping_dim,
				       SPATIAL_KEY(node), SPATIAL_KEY(best))))
		  { best = node; best_dim = node_dim; }
	      }
	  }
	while (node != end);
	if (best == 0) { best = node; best_dim = node_dim; }
	SPATIAL_ASSERT_CHECK(best_dim < rank());
	SPATIAL_ASSERT_CHECK(best != 0);
	return Iterator(rank, cmp, mapping_dim, best_dim,
			static_cast<Link_type>(best));
      }

      // Specialized template that works a little faster on regular kdtree tree
      // nodes since invariant is applied strictly.
      template<typename Iterator, typename Base_ptr, typename Link_type,
	       typename Rank, typename Key, typename Compare>
      inline Iterator
      lower_bound
      (const Rank& rank, const Compare& cmp,
       dimension_type mapping_dim, dimension_type node_dim,
       Base_ptr node, const Key& flag, details::strict_invariant_tag)
      {
	SPATIAL_ASSERT_CHECK(node != 0);
	SPATIAL_ASSERT_CHECK(mapping_dim < rank());
	SPATIAL_ASSERT_CHECK(node_dim < rank());
	SPATIAL_ASSERT_CHECK(!Node_base::header(node));
	Base_ptr end = node->parent;
	Base_ptr best = end;
	dimension_type best_dim = decr_dim(rank, node_dim);
	while (node->left != 0
	       && (node_dim != mapping_dim // optimization
		   || cmp(node_dim, flag, SPATIAL_KEY(node))))
	  {
	    node = node->left;
	    node_dim = incr_dim(rank, node_dim);
	  }
	if (!cmp(mapping_dim, SPATIAL_KEY(node), flag))
	  { best = node; best_dim = node_dim; }
	do
	  {
	    if (node->right != 0
		&& (node_dim != mapping_dim
		    || best == 0
		    || !cmp(mapping_dim,
			    SPATIAL_KEY(best), SPATIAL_KEY(node))))
	      {
		node = node->right;
		node_dim = incr_dim(rank, node_dim);
		while (node->left != 0
		       && (node_dim != mapping_dim // optimization
			   || cmp(node_dim, flag, SPATIAL_KEY(node))))
		  {
		    node = node->left;
		    node_dim = incr_dim(rank, node_dim);
		  }
		if (!cmp(mapping_dim, SPATIAL_KEY(node), flag)
		    && (best == 0
			|| less_by_ref(cmp, mapping_dim,
				       SPATIAL_KEY(node), SPATIAL_KEY(best))))
		  { best = node; best_dim = node_dim; }
	      }
	    else
	      {
		Base_ptr p = node->parent;
		while (p != end && p->right == node)
		  {
		    node = p;
		    node_dim = decr_dim(rank, node_dim);
		    p = node->parent;
		  }
		node = p; node_dim = decr_dim(rank, node_dim);
		if (node != end
		    && !cmp(mapping_dim, SPATIAL_KEY(node), flag)
		    && (best == 0
			|| less_by_ref(cmp, mapping_dim,
				       SPATIAL_KEY(node), SPATIAL_KEY(best))))
		  { best = node; best_dim = node_dim; }
	      }
	  }
	while (node != end);
	if (best == 0) { best = node; best_dim = node_dim; }
	SPATIAL_ASSERT_CHECK(best_dim < rank());
	SPATIAL_ASSERT_CHECK(best != 0);
	return Iterator(rank, cmp, mapping_dim, best_dim,
			static_cast<Link_type>(best));
      }

    } // namespace mapping

    template<typename Rank, typename Key, typename Node,
	     typename Compare, bool Constant, typename Derived>
    inline void
    Mapping_iterator_base
    <Rank, Key, Node, Compare, Constant, Derived>
    ::increment()
    {
      details::mapping::increment
	<Mapping_iterator_impl, Base_ptr, Link_type, Rank, Key, Compare>
	(impl, invariant_category());
    }

    // The next largest key on the mapping dimension is likely to be found in the
    // children of the current best, so, descend into the children of node first.
    template<typename Rank, typename Key, typename Node,
	     typename Compare, bool Constant, typename Derived>
    inline void
    Mapping_iterator_base
    <Rank, Key, Node, Compare, Constant, Derived>
    ::decrement()
    {
      const Compare& cmp = this->compare();
      const Rank& rank = this->rank();
      SPATIAL_ASSERT_CHECK(impl.node != 0);
      SPATIAL_ASSERT_CHECK(impl.mapping_dim() < rank());
      SPATIAL_ASSERT_CHECK(impl.node_dim < rank());
      Base_ptr best = 0; // not null when best has been found
      Base_ptr node = impl.node;
      dimension_type node_dim = impl.node_dim;
      dimension_type best_dim = impl.node_dim;
      if (Node_base::header(node))
	{
	  operator=(maximum(rank, cmp,
			    impl.mapping_dim(), 0, node->parent));
	  return;
	}
      if (node->right != 0)
	{
	  do
	    {
	      node = node->right;
	      node_dim = incr_dim(rank, node_dim);
	    }
	  while (node->right != 0
		 && (node_dim != impl.mapping_dim()
		     || !cmp(node_dim, SPATIAL_KEY(impl.node),
			     SPATIAL_KEY(node))));
	}
      Base_ptr ceiling = node->parent; // the upper limit of unvisited nodes
      bool sibling_visited = false;    // at ceiling, sibling node is visited
      if (less_by_ref(cmp, impl.mapping_dim(),
		      SPATIAL_KEY(node), SPATIAL_KEY(impl.node)))
	{ best = node; best_dim = node_dim; }
      do
	{
	  if (node->left != 0
	      && (node_dim != impl.mapping_dim() || best == 0
		  || !cmp(node_dim, SPATIAL_KEY(node), SPATIAL_KEY(best))))
	    {
	      node = node->left;
	      node_dim = incr_dim(rank, node_dim);
	      while (node->right != 0
		     && (node_dim != impl.mapping_dim()
			 || !cmp(node_dim, SPATIAL_KEY(impl.node),
				 SPATIAL_KEY(node))))
		{
		  node = node->right;
		  node_dim = incr_dim(rank, node_dim);
		}
	      if (less_by_ref(cmp, impl.mapping_dim(), SPATIAL_KEY(node),
			      SPATIAL_KEY(impl.node))
		  && (best == 0
		      || less_by_ref(cmp, impl.mapping_dim(),
				     SPATIAL_KEY(best), SPATIAL_KEY(node))))
		{ best = node; best_dim = node_dim; }
	    }
	  else
	    {
	      Base_ptr p = node->parent;
	      // go upward as long as we don't hit header or ceiling
	      while (!Node_base::header(p)
		     && ((p != ceiling)
			 ? (p->left == node)
			 // either sibling is visited or there is one child
			 : (sibling_visited
			    || (p->right == node && p->left == 0)
			    || (p->left == node && p->right == 0))))
		{
		  node = p;
		  node_dim = decr_dim(rank, node_dim);
		  p = node->parent;
		  if (node == ceiling)
		    {
		      if (less_by_ref(cmp, impl.mapping_dim(),
				      SPATIAL_KEY(node),
				      SPATIAL_KEY(impl.node))
			  && (best == 0
			      || less_by_ref(cmp, impl.mapping_dim(),
					     SPATIAL_KEY(best),
					     SPATIAL_KEY(node))))
			{ best = node; best_dim = node_dim; }
		      sibling_visited = false;
		      ceiling = p;
		    }
		}
	      if (!Node_base::header(p) && p == ceiling)
		{
		  if (p->right == node) { node = p->left; }
		  else { node = p->right; }
		  sibling_visited = true;
		  // go to full right in unvisited sibling
		  while (node->right != 0
			 && (node_dim != impl.mapping_dim()
			     || !cmp(node_dim, SPATIAL_KEY(impl.node),
				     SPATIAL_KEY(node))))
		    {
		      node = node->right;
		      node_dim = incr_dim(rank, node_dim);
		    }
		  if (less_by_ref(cmp, impl.mapping_dim(),
				  SPATIAL_KEY(node), SPATIAL_KEY(impl.node))
		      && (best == 0
			  || less_by_ref(cmp, impl.mapping_dim(),
					 SPATIAL_KEY(best),
					 SPATIAL_KEY(node))))
		    { best = node; best_dim = node_dim; }
		}
	      else
		{
		  node = p;
		  node_dim = decr_dim(rank, node_dim);
		  if (!Node_base::header(node))
		    {
		      if (less_by_ref(cmp, impl.mapping_dim(),
				      SPATIAL_KEY(node),
				      SPATIAL_KEY(impl.node))
			  && (best == 0
			      || less_by_ref(cmp, impl.mapping_dim(),
					     SPATIAL_KEY(best),
					     SPATIAL_KEY(node))))
			{ best = node; best_dim = node_dim; }
		    }
		}
	    }
	}
      while (!Node_base::header(node));
      if (best != 0) { impl.node = best; impl.node_dim = best_dim; }
      else { impl.node = node; impl.node_dim = node_dim; }
      SPATIAL_ASSERT_CHECK(impl.mapping_dim() < rank());
      SPATIAL_ASSERT_CHECK(impl.node_dim < rank());
      SPATIAL_ASSERT_CHECK(impl.node != 0);
    }

    // Find the minimum from node and stop when reaching the parent. Iterate in
    // left-first fashion.
    template<typename Rank, typename Key, typename Node,
	     typename Compare, bool Constant, typename Derived>
    inline Derived
    Mapping_iterator_base
    <Rank, Key, Node, Compare, Constant, Derived>
    ::minimum
    (const Rank& rank, const Compare& cmp,
     dimension_type mapping_dim, dimension_type node_dim,
     Base_ptr node)
    {
      SPATIAL_ASSERT_CHECK(mapping_dim < rank());
      SPATIAL_ASSERT_CHECK(node_dim < rank());
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      Base_ptr end = node->parent;
      while (node->left != 0)
	{ node = node->left; node_dim = incr_dim(rank, node_dim); }
      Base_ptr best = node;
      dimension_type best_dim = node_dim;
      do
	{
	  if (node->right != 0
	      && (node_dim != mapping_dim
		  || !cmp(mapping_dim,
			  SPATIAL_KEY(best), SPATIAL_KEY(node))))
	    {
	      node = node->right;
	      node_dim = incr_dim(rank, node_dim);
	      while (node->left != 0)
		{
		  node = node->left;
		  node_dim = incr_dim(rank, node_dim);
		}
	      if (less_by_ref(cmp, mapping_dim,
			      SPATIAL_KEY(node), SPATIAL_KEY(best)))
		{ best = node; best_dim = node_dim; }
	    }
	  else
	    {
	      Base_ptr p = node->parent;
	      while (p != end && p->right == node)
		{
		  node = p;
		  node_dim = decr_dim(rank, node_dim);
		  p = node->parent;
		}
	      node = p;
	      node_dim = decr_dim(rank, node_dim);
	      if (node != end
		  && less_by_ref(cmp, mapping_dim,
				 SPATIAL_KEY(node), SPATIAL_KEY(best)))
		{ best = node; best_dim = node_dim; }
	    }
	}
      while (node != end);
      SPATIAL_ASSERT_CHECK(best_dim < rank());
      SPATIAL_ASSERT_CHECK(!Node_base::header(best));
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(best != 0);
      return Derived(rank, cmp, mapping_dim, best_dim,
		     static_cast<Link_type>(best));
    }

    template<typename Rank, typename Key, typename Node,
	     typename Compare, bool Constant, typename Derived>
    inline Derived
    Mapping_iterator_base
    <Rank, Key, Node, Compare, Constant, Derived>
    ::maximum
    (const Rank& rank, const Compare& cmp,
     dimension_type mapping_dim, dimension_type node_dim,
     Base_ptr node)
    {
      return details::mapping::maximum
	<Derived, Base_ptr, Link_type, Rank, Key, Compare>
	(rank, cmp, mapping_dim, node_dim, node,
	 invariant_category());
    }

    template<typename Rank, typename Key, typename Node,
	     typename Compare, bool Constant, typename Derived>
    inline Derived
    Mapping_iterator_base
    <Rank, Key, Node, Compare, Constant, Derived>
    ::lower_bound
    (const Rank& rank, const Compare& cmp,
     dimension_type mapping_dim, dimension_type node_dim,
     Base_ptr node, const Key& flag)
    {
      return details::mapping::lower_bound
	<Derived, Base_ptr, Link_type, Rank, Key, Compare>
	(rank, cmp, mapping_dim, node_dim, node, flag,
	 invariant_category());
    }

    // Walk tree nodes in right-first fashion, bouncing off values that are higher
    // than key.
    template<typename Rank, typename Key, typename Node,
	     typename Compare, bool Constant, typename Derived>
    inline Derived
    Mapping_iterator_base
    <Rank, Key, Node, Compare, Constant, Derived>
    ::upper_bound
    (const Rank& rank, const Compare& cmp,
     dimension_type mapping_dim, dimension_type node_dim,
     Base_ptr node, const Key& flag)
    {
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(mapping_dim < rank());
      SPATIAL_ASSERT_CHECK(node_dim < rank());
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      Base_ptr end = node->parent;
      Base_ptr best = end;
      dimension_type best_dim = decr_dim(rank, node_dim);
      while (node->left != 0
	     && (node_dim != mapping_dim
		 || cmp(node_dim, flag, SPATIAL_KEY(node))))
	{ node = node->left; node_dim = incr_dim(rank, node_dim); }
      if (cmp(mapping_dim, flag, SPATIAL_KEY(node)))
	{ best = node; best_dim = node_dim; }
      do
	{
	  if (node->right != 0
	      && (node_dim != mapping_dim
		  || best == 0
		  || !cmp(mapping_dim,
			  SPATIAL_KEY(best), SPATIAL_KEY(node))))
	    {
	      node = node->right;
	      node_dim = incr_dim(rank, node_dim);
	      while (node->left != 0
		     && (node_dim != mapping_dim
			 || cmp(node_dim, flag, SPATIAL_KEY(node))))
		{
		  node = node->left;
		  node_dim = incr_dim(rank, node_dim);
		}
	      if (cmp(mapping_dim, flag, SPATIAL_KEY(node))
		  && (best == 0
		      || less_by_ref(cmp, mapping_dim,
				     SPATIAL_KEY(node), SPATIAL_KEY(best))))
		{ best = node; best_dim = node_dim; }
	    }
	  else
	    {
	      Base_ptr p = node->parent;
	      while (p != end && p->right == node)
		{
		  node = p;
		  node_dim = decr_dim(rank, node_dim);
		  p = node->parent;
		}
	      node = p; node_dim = decr_dim(rank, node_dim);
	      if (node != end
		  && cmp(mapping_dim, flag, SPATIAL_KEY(node))
		  && (best == 0
		      || less_by_ref(cmp, mapping_dim,
				     SPATIAL_KEY(node), SPATIAL_KEY(best))))
		{ best = node; best_dim = node_dim; }
	    }
	}
      while (node != end);
      if (best == 0) { best = node; best_dim = node_dim; }
      SPATIAL_ASSERT_CHECK(best_dim < rank());
      SPATIAL_ASSERT_CHECK(best != 0);
      return Derived(rank, cmp, mapping_dim, best_dim,
		     static_cast<Link_type>(best));
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_MAPPING_TPP
