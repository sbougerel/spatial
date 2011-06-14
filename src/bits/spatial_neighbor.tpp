// -*- C++ -*-

/**
 *  @file   spatial_mapping.tpp
 *  @brief  
 *
 *  Change Log:
 *
 *  - 2010-05-03 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_NEIGHBOR_TPP
#define SPATIAL_NEIGHBOR_TPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#include <bitset>

namespace spatial
{
  namespace details
  {

    template <typename Rank, typename Key, typename Node, typename Compare,
	      typename Geometry, bool Constant, typename Derived>
    inline void
    Neighbor_iterator_base
    <Rank, Key, Node, Compare, Geometry, Constant, Derived>
    ::increment()
    {
      SPATIAL_ASSERT_CHECK(impl.node_dim < rank()());
      SPATIAL_ASSERT_CHECK(!Node_base::header(impl.node));
      SPATIAL_ASSERT_CHECK(impl.node != 0);
      /*
	In this algorithm, we seek to find the next nearest point to
	origin. Thus assuming that this point exists, its distance to origin is
	equal or greater than that of the previous nearest point to origin.
	Since K-d tree are good at preserving locality, it is best to search the
	next nearest point from the current nearest point, since these 2 points
	could be close to one another in the tree. In order to find the next
	nearest, we perform in-order transveral, at the left and right
	simultaneously.
      */
      const Compare& cmp = compare();
      const Key& origin = impl.origin();
      const Rank& rk = rank();
      const Geometry& geo = geometry();
      Base_ptr rn = impl.node;
      dimension_type rn_dim = impl.node_dim;
      bool rn_break = false;
      Base_ptr ln = impl.node;
      dimension_type ln_dim = impl.node_dim;
      bool ln_break = false;
      Base_ptr curr = impl.node;
      distance_type curr_distance = distance();
      Base_ptr near_node = 0;
      dimension_type near_dim = 0;
      distance_type near_distance = 0;
      distance_type tmp;
      do
	{
	  // One step to the right side...
	  if (rn_break) { goto left_side; }
	  if (rn->right != 0
	      && (!cmp(rn_dim, origin, SPATIAL_KEY(rn))
		  || near_node == 0
		  || near_distance
		  >= geo.distance_to_plane
		  (rk(), rn_dim, origin, SPATIAL_KEY(rn))))
	    {
	      rn = rn->right;
	      rn_dim = incr_dim(rk, rn_dim);
	      while(rn->left != 0
		    && (!cmp(rn_dim, SPATIAL_KEY(rn), origin)
			|| near_node == 0
			|| near_distance
			>= geo.distance_to_plane
			(rk(), rn_dim, origin, SPATIAL_KEY(rn))))
		{
		  rn = rn->left;
		  rn_dim = incr_dim(rk, rn_dim);
		}
	    }
	  else
	    {
	      Base_ptr p = rn->parent;
	      while (!Node_base::header(p) && p->right == rn)
		{
		  rn = p;
		  rn_dim = decr_dim(rk, rn_dim);
		  p = rn->parent;
		}
	      rn = p;
	      rn_dim = decr_dim(rk, rn_dim);
	    }
	  if (Node_base::header(rn))
	    { if (ln_break) break; rn_break = true; goto left_side; }
	  tmp = geo.distance_to_key(rk(), origin, SPATIAL_KEY(rn));
	  if (tmp < curr_distance || (tmp == curr_distance && rn < curr))
	    { goto left_side; }
	  if (near_node == 0 || tmp < near_distance)
	    {
	      near_node = rn;
	      near_dim = rn_dim;
	      near_distance = tmp;
	    }
	  else if (tmp == near_distance && rn < near_node)
	    {
	      near_node = rn;
	      near_dim = rn_dim;
	    }
	left_side:
	  // One step to the left side...
	  if (ln_break) { continue; }
	  if (ln->left != 0
	      && (!cmp(ln_dim, SPATIAL_KEY(ln), origin)
		  || near_node == 0
		  || near_distance
		  >= geo.distance_to_plane
		  (rk(), ln_dim, origin, SPATIAL_KEY(ln))))
	    {
	      ln = ln->left;
	      ln_dim = incr_dim(rk, ln_dim);
	      while(ln->right != 0
		    && (!cmp(ln_dim, origin, SPATIAL_KEY(ln))
			|| near_node == 0
			|| near_distance
			>= geo.distance_to_plane
			(rk(), ln_dim, origin, SPATIAL_KEY(ln))))
		{
		  ln = ln->right;
		  ln_dim = incr_dim(rk, ln_dim);
		}
	    }
	  else
	    {
	      Base_ptr p = ln->parent;
	      while (!Node_base::header(p) && p->left == ln)
		{
		  ln = p;
		  ln_dim = decr_dim(rk, ln_dim);
		  p = ln->parent;
		}
	      ln = p;
	      ln_dim = decr_dim(rk, ln_dim);
	    }
	  if (Node_base::header(ln))
	    { if (rn_break) break; ln_break = true; continue; }
	  tmp = geo.distance_to_key(rk(), origin, SPATIAL_KEY(ln));
	  if (tmp < curr_distance || (tmp == curr_distance && ln < curr))
	    { continue; }
	  if (near_node == 0 || tmp < near_distance)
	    {
	      near_node = ln;
	      near_dim = ln_dim;
	      near_distance = tmp;
	    }
	  else if (tmp == near_distance && ln < near_node)
	    {
	      near_node = ln;
	      near_dim = ln_dim;
	    }
	}
      while(true);
      SPATIAL_ASSERT_CHECK(near_dim < rk());
      SPATIAL_ASSERT_CHECK(near_node != curr);
      SPATIAL_ASSERT_CHECK(near_node == 0
			   || curr_distance < near_distance
			   || (curr_distance == near_distance && curr < near_node));
      SPATIAL_ASSERT_CHECK(rn_dim < rk());
      SPATIAL_ASSERT_CHECK(ln_dim < rk());
      SPATIAL_ASSERT_CHECK(Node_base::header(rn));
      SPATIAL_ASSERT_CHECK(Node_base::header(ln));
      SPATIAL_ASSERT_CHECK(ln == rn);
      SPATIAL_ASSERT_CHECK(ln_dim == rn_dim);
      if (near_node != 0)
	{
	  impl.node = near_node;
	  impl.node_dim = near_dim;
	  impl.distance() = near_distance;
	}
      else
	{
	  impl.node = rn;
	  impl.node_dim = rn_dim;
	  impl.distance() = 0;
	}
    }

    // The next largest key on the neighbor dimension is likely to be found in
    // the children of the current best, so, descend into the children of node
    // first.
    template <typename Rank, typename Key, typename Node, typename Compare,
	      typename Geometry, bool Constant, typename Derived>
    inline void
    Neighbor_iterator_base
    <Rank, Key, Node, Compare, Geometry, Constant, Derived>
    ::decrement()
    {
      SPATIAL_ASSERT_CHECK(impl.node_dim < rank()());
      SPATIAL_ASSERT_CHECK(impl.node != 0);
      // Must come back from an end position for reverse iteration...
      if (Node_base::header(impl.node))
	{
	  Derived iter = maximum(rank(), compare(), geometry(), impl.origin(),
				 0, impl.node->parent);
	  impl.node = iter.impl.node;
	  impl.node_dim = iter.impl.node_dim;
	  impl.distance() = iter.impl.distance();
	  return;
	}
      /*
	As in 'increment', we follow the same convention: we traverse the tree
	in-order to the left and the right simultaneously.
      */
      const Compare& cmp = compare();
      const Key& origin = impl.origin();
      const Rank& rk = rank();
      const Geometry& geo = geometry();
      Base_ptr rn = impl.node;
      dimension_type rn_dim = impl.node_dim;
      Base_ptr ln = impl.node;
      dimension_type ln_dim = impl.node_dim;
      Base_ptr curr = impl.node;
      distance_type curr_distance = distance();
      Base_ptr near_node = 0;
      dimension_type near_dim = 0;
      distance_type near_distance = 0;
      distance_type tmp;
      bool ln_break = false;
      bool rn_break = false;
      do
	{
	  // In-order traversal that starts with all nodes before 'curr'
	  if (ln_break) { goto right_side; }
	  if (ln->left != 0
	      && (!cmp(ln_dim, SPATIAL_KEY(ln), origin)
		  || curr_distance
		  >= geo.distance_to_plane
		  (rk(), ln_dim, origin, SPATIAL_KEY(ln))))
	    {
	      ln = ln->left;
	      ln_dim = incr_dim(rk, ln_dim);
	      while(ln->right != 0
		    && (!cmp(ln_dim, origin, SPATIAL_KEY(ln))
			|| curr_distance
			>= geo.distance_to_plane
			(rk(), ln_dim, origin, SPATIAL_KEY(ln))))
		{
		  ln = ln->right;
		  ln_dim = incr_dim(rk, ln_dim);
		}
	    }
	  else
	    {
	      Base_ptr p = ln->parent;
	      while (!Node_base::header(p) && p->left == ln)
		{
		  ln = p;
		  ln_dim = decr_dim(rk, ln_dim);
		  p = ln->parent;
		}
	      ln = p;
	      ln_dim = decr_dim(rk, ln_dim);
	    }
	  if (Node_base::header(ln))
	    { if (rn_break) break; ln_break = true; goto right_side; }
	  tmp = geo.distance_to_key(rk(), origin, SPATIAL_KEY(ln));
	  if (tmp > curr_distance || (tmp == curr_distance && ln > curr))
	    { goto right_side; }
	  if (near_node == 0 || tmp > near_distance)
	    {
	      near_node = ln;
	      near_dim = ln_dim;
	      near_distance = tmp;
	    }
	  else if (tmp == near_distance && ln > near_node)
	    {
	      near_node = ln;
	      near_dim = ln_dim;
	    }
	right_side:
	  // Now, in-order traversal that finished with all nodes after 'curr'
	  if (rn_break) { continue; }
	  if (rn->right != 0
	      && (!cmp(rn_dim, origin, SPATIAL_KEY(rn))
		  || curr_distance
		  >= geo.distance_to_plane
		  (rk(), rn_dim, origin, SPATIAL_KEY(rn))))
	    {
	      rn = rn->right;
	      rn_dim = incr_dim(rk, rn_dim);
	      while(rn->left != 0
		    && (!cmp(rn_dim, SPATIAL_KEY(rn), origin)
			|| curr_distance
			>= geo.distance_to_plane
			(rk(), rn_dim, origin, SPATIAL_KEY(rn))))
		{
		  rn = rn->left;
		  rn_dim = incr_dim(rk, rn_dim);
		}
	    }
	  else
	    {
	      Base_ptr p = rn->parent;
	      while (!Node_base::header(p) && p->right == rn)
		{
		  rn = p;
		  rn_dim = decr_dim(rk, rn_dim);
		  p = rn->parent;
		}
	      rn = p;
	      rn_dim = decr_dim(rk, rn_dim);
	    }
	  if (Node_base::header(rn))
	    { if(ln_break) break; rn_break = true; continue; }
	  tmp = geo.distance_to_key(rk(), origin, SPATIAL_KEY(rn));
	  if (tmp > curr_distance || (tmp == curr_distance && rn > curr))
	    { continue; }
	  if (near_node == 0 || tmp > near_distance)
	    {
	      near_node = rn;
	      near_dim = rn_dim;
	      near_distance = tmp;
	    }
	  else if (tmp == near_distance && rn > near_node)
	    {
	      near_node = rn;
	      near_dim = rn_dim;
	    }
	}
      while(true);
      SPATIAL_ASSERT_CHECK(near_dim < rk());
      SPATIAL_ASSERT_CHECK(near_node != curr);
      SPATIAL_ASSERT_CHECK(near_node == 0
			   || curr_distance > near_distance
			   || (curr_distance == near_distance && curr > near_node));
      SPATIAL_ASSERT_CHECK(rn_dim < rk());
      SPATIAL_ASSERT_CHECK(ln_dim < rk());
      SPATIAL_ASSERT_CHECK(Node_base::header(rn));
      SPATIAL_ASSERT_CHECK(Node_base::header(ln));
      SPATIAL_ASSERT_CHECK(rn == ln);
      SPATIAL_ASSERT_CHECK(rn_dim == ln_dim);
      if (near_node != 0)
	{
	  impl.node = near_node;
	  impl.node_dim = near_dim;
	  impl.distance() = near_distance;
	}
      else
	{
	  impl.node = rn;
	  impl.node_dim = rn_dim;
	  impl.distance() = 0;
	}
    }

    // Find the minimum from node and stop when reaching the parent. Iterate in
    // left-first fashion.
    template <typename Rank, typename Key, typename Node, typename Compare,
	      typename Geometry, bool Constant, typename Derived>
    inline Derived
    Neighbor_iterator_base
    <Rank, Key, Node, Compare, Geometry, Constant, Derived>
    ::minimum
    (const Rank& rank, const Compare& cmp, const Geometry& geometry,
     const Key& origin, dimension_type node_dim, Base_ptr node)
    {
      SPATIAL_ASSERT_CHECK(node_dim < rank());
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      Base_ptr near_node = node;
      Base_ptr end = node->parent;
      dimension_type near_dim = node_dim;
      distance_type near_distance
	= geometry.distance_to_key(rank(), origin, SPATIAL_KEY(node));
      distance_type tmp;
      // Depth traversal starts with left first
      while(node->left != 0
	    && (!cmp(node_dim, SPATIAL_KEY(node), origin)
		|| near_distance
		>= geometry.distance_to_plane(rank(), node_dim, origin,
					      SPATIAL_KEY(node))))
	{
	  node = node->left;
	  node_dim = incr_dim(rank, node_dim);
	  tmp = geometry.distance_to_key(rank(), origin, SPATIAL_KEY(node));
	  if (tmp < near_distance)
	    {
	      near_node = node;
	      near_dim = node_dim;
	      near_distance = tmp;
	    }
	  else if (tmp == near_distance
		   // to provide total ordering among the nodes, we use the node
		   // pointer as a fall back comparison mechanism
		   && node < near_node)
	    {
	      near_node = node;
	      near_dim = node_dim;
	    }
	}
      // In-order, right, left, then all the way up
      do
	{
	  if (node->right != 0
	      && (!cmp(node_dim, origin, SPATIAL_KEY(node))
		  || near_distance
		  >= geometry.distance_to_plane(rank(), node_dim, origin,
						SPATIAL_KEY(node))))
	    {
	      node = node->right;
	      node_dim = incr_dim(rank, node_dim);
	      tmp = geometry.distance_to_key(rank(), origin, SPATIAL_KEY(node));
	      if (tmp < near_distance)
		{
		  near_node = node;
		  near_dim = node_dim;
		  near_distance = tmp;
		}
	      else if (tmp == near_distance && node < near_node)
		{
		  near_node = node;
		  near_dim = node_dim;
		}
	      while(node->left != 0
		    && (!cmp(node_dim, SPATIAL_KEY(node), origin)
			|| near_distance
			>= geometry.distance_to_plane(rank(), node_dim, origin,
						      SPATIAL_KEY(node))))
		{
		  node = node->left;
		  node_dim = incr_dim(rank, node_dim);
		  tmp = geometry.distance_to_key(rank(), origin, SPATIAL_KEY(node));
		  if (tmp < near_distance)
		    {
		      near_node = node;
		      near_dim = node_dim;
		      near_distance = tmp;
		    }
		  else if (tmp == near_distance && node < near_node)
		    {
		      near_node = node;
		      near_dim = node_dim;
		    }
		}
	    }
	  // No more nodes to visit, so go up!
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
	    }
	}
      while(node != end);
      SPATIAL_ASSERT_CHECK(near_dim < rank());
      SPATIAL_ASSERT_CHECK(!Node_base::header(near_node));
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(near_node != 0);
      return Derived(rank, cmp, geometry, origin, near_dim,
		     static_cast<Link_type>(near_node), near_distance);
    }

    template <typename Rank, typename Key, typename Node, typename Compare,
	      typename Geometry, bool Constant, typename Derived>
    inline Derived
    Neighbor_iterator_base
    <Rank, Key, Node, Compare, Geometry, Constant, Derived>
    ::maximum
    (const Rank& rank, const Compare& cmp, const Geometry& geometry,
     const Key& origin, dimension_type node_dim, Base_ptr node)
    {
      SPATIAL_ASSERT_CHECK(node_dim < rank());
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      Base_ptr end = node->parent;
      distance_type tmp;
      // Finding the maximum is, for lack of a better algorithm, equivalent to a
      // O(n) search. An alternative has been explored: being able to find if a
      // node is in a cell that is smaller than the current 'far_node' node found.
      // However, with the data at hand, computing the cell turned out to be
      // more expensive than doing a simple iteration over all nodes in the
      // tree.  May be, one day we'll find a better algorithm that also has no
      // impact on the memory footprint of the tree (although I doubt these 2
      // conditions will ever be met. Probably there will be a tradeoff.)
      //
      // Iterate from left most to right most, and stop at node's parent.
      while (node->left != 0)
	{ node = node->left; node_dim = incr_dim(rank, node_dim); }
      Base_ptr far_node = node;
      dimension_type far_dim = node_dim;
      distance_type far_distance
	= geometry.distance_to_key(rank(), origin, SPATIAL_KEY(node));
      do {
	if (node->right != 0)
	  {
	    node = node->right; node_dim = incr_dim(rank, node_dim);
	    while (node->left != 0)
	      { node = node->left; node_dim = incr_dim(rank, node_dim); }
	  }
	else
	  {
	    Base_ptr p = node->parent;
	    while (p != end && node == p->right)
	      {
		node = p; p = node->parent;
		node_dim = decr_dim(rank, node_dim);
	      }
	    node = p;
	    node_dim = decr_dim(rank, node_dim);
	  }
	if (node == end) { break; }
	tmp = geometry.distance_to_key(rank(), origin, SPATIAL_KEY(node));
	if (tmp > far_distance
	    || (tmp == far_distance && node > far_node))
	  {
	    far_node = node;
	    far_dim = node_dim;
	    far_distance = tmp;
	  }
      }
      while(true);
      SPATIAL_ASSERT_CHECK(far_dim < rank());
      SPATIAL_ASSERT_CHECK(!Node_base::header(far_node));
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(far_node != 0);
      return Derived(rank, cmp, geometry, origin, far_dim,
		     static_cast<Link_type>(far_node), far_distance);
    }

    template <typename Rank, typename Key, typename Node, typename Compare,
	      typename Geometry, bool Constant, typename Derived>
    inline Derived
    Neighbor_iterator_base
    <Rank, Key, Node, Compare, Geometry, Constant, Derived>
    ::lower_bound
    (const Rank& rank, const Compare& cmp, const Geometry& geometry,
     const Key& origin, distance_type limit,
     dimension_type node_dim, Base_ptr node)
    {
      SPATIAL_ASSERT_CHECK(node_dim < rank());
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      Base_ptr end = node->parent;
      Base_ptr near_node = 0;
      dimension_type near_dim = node_dim;
      distance_type tmp;
      distance_type near_distance
	= geometry.distance_to_key(rank(), origin, SPATIAL_KEY(node));
      if (near_distance >= limit) { near_node = node; }
      // Depth traversal starts with left first
      while(node->left != 0
	    && (!cmp(node_dim, SPATIAL_KEY(node), origin)
		|| near_node == 0
		|| near_distance
		>= geometry.distance_to_plane(rank(), node_dim, origin,
					      SPATIAL_KEY(node))))
	{
	  node = node->left;
	  node_dim = incr_dim(rank, node_dim);
	  tmp = geometry.distance_to_key(rank(), origin, SPATIAL_KEY(node));
	  if (tmp < limit) { continue; }
	  if (near_node == 0 || tmp < near_distance)
	    {
	      near_node = node;
	      near_dim = node_dim;
	      near_distance = tmp;
	    }
	  else if (tmp == near_distance
		   // to provide total ordering among the nodes, we use the node
		   // pointer as a fall back comparison mechanism
		   && node < near_node)
	    {
	      near_node = node;
	      near_dim = node_dim;
	    }
	}
      // In-order, right, left, then all the way up
      do
	{
	  if (node->right != 0
	      && (!cmp(node_dim, origin, SPATIAL_KEY(node))
		  || near_node == 0
		  || near_distance
		  >= geometry.distance_to_plane(rank(), node_dim, origin,
						SPATIAL_KEY(node))))
	    {
	      node = node->right;
	      node_dim = incr_dim(rank, node_dim);
	      tmp = geometry.distance_to_key(rank(), origin, SPATIAL_KEY(node));
	      if (tmp >= limit)
		{
		  if (near_node == 0 || tmp < near_distance)
		    {
		      near_node = node;
		      near_dim = node_dim;
		      near_distance = tmp;
		    }
		  else if (tmp == near_distance && node < near_node)
		    {
		      near_node = node;
		      near_dim = node_dim;
		    }
		}
	      while(node->left != 0
		    && (!cmp(node_dim, SPATIAL_KEY(node), origin)
			|| near_node == 0
			|| near_distance
			>= geometry.distance_to_plane(rank(), node_dim, origin,
						      SPATIAL_KEY(node))))
		{
		  node = node->left;
		  node_dim = incr_dim(rank, node_dim);
		  tmp = geometry.distance_to_key(rank(), origin,
						 SPATIAL_KEY(node));
		  if (tmp >= limit)
		    {
		      if (near_node == 0 || tmp < near_distance)
			{
			  near_node = node;
			  near_dim = node_dim;
			  near_distance = tmp;
			}
		      else if (tmp == near_distance && node < near_node)
			{
			  near_node = node;
			  near_dim = node_dim;
			}
		    }
		}
	    }
	  // No more nodes to visit, so go up!
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
	    }
	}
      while(node != end);
      if (near_node == 0)
	{ near_node = node; near_dim = node_dim; }
      SPATIAL_ASSERT_CHECK(near_dim < rank());
      SPATIAL_ASSERT_CHECK(node != 0);
      return Derived(rank, cmp, geometry, origin, near_dim,
		     static_cast<Link_type>(near_node), near_distance);
    }

    template <typename Rank, typename Key, typename Node, typename Compare,
	      typename Geometry, bool Constant, typename Derived>
    inline Derived
    Neighbor_iterator_base
    <Rank, Key, Node, Compare, Geometry, Constant, Derived>
    ::upper_bound
    (const Rank& rank, const Compare& cmp, const Geometry& geometry,
     const Key& origin, distance_type limit,
     dimension_type node_dim, Base_ptr node)
    {
      SPATIAL_ASSERT_CHECK(node_dim < rank());
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      Base_ptr end = node->parent;
      Base_ptr near_node = 0;
      dimension_type near_dim = node_dim;
      distance_type tmp;
      distance_type near_distance
	= geometry.distance_to_key(rank(), origin, SPATIAL_KEY(node));
      if (near_distance > limit) { near_node = node; }
      // Depth traversal starts with left first
      while(node->left != 0
	    && (!cmp(node_dim, SPATIAL_KEY(node), origin)
		|| near_node == 0
		|| near_distance
		>= geometry.distance_to_plane(rank(), node_dim, origin,
					      SPATIAL_KEY(node))))
	{
	  node = node->left;
	  node_dim = incr_dim(rank, node_dim);
	  tmp = geometry.distance_to_key(rank(), origin, SPATIAL_KEY(node));
	  if (tmp <= limit) { continue; }
	  if (near_node == 0 || tmp < near_distance)
	    {
	      near_node = node;
	      near_dim = node_dim;
	      near_distance = tmp;
	    }
	  else if (tmp == near_distance
		   // to provide total ordering among the nodes, we use the node
		   // pointer as a fall back comparison mechanism
		   && node < near_node)
	    {
	      near_node = node;
	      near_dim = node_dim;
	    }
	}
      // In-order, right, left, then all the way up
      do
	{
	  if (node->right != 0
	      && (!cmp(node_dim, origin, SPATIAL_KEY(node))
		  || near_node == 0
		  || near_distance
		  >= geometry.distance_to_plane(rank(), node_dim, origin,
						SPATIAL_KEY(node))))
	    {
	      node = node->right;
	      node_dim = incr_dim(rank, node_dim);
	      tmp = geometry.distance_to_key(rank(), origin, SPATIAL_KEY(node));
	      if (tmp > limit)
		{
		  if (near_node == 0 || tmp < near_distance)
		    {
		      near_node = node;
		      near_dim = node_dim;
		      near_distance = tmp;
		    }
		  else if (tmp == near_distance && node < near_node)
		    {
		      near_node = node;
		      near_dim = node_dim;
		    }
		}
	      while(node->left != 0
		    && (!cmp(node_dim, SPATIAL_KEY(node), origin)
			|| near_node == 0
			|| near_distance
			>= geometry.distance_to_plane(rank(), node_dim, origin,
						      SPATIAL_KEY(node))))
		{
		  node = node->left;
		  node_dim = incr_dim(rank, node_dim);
		  tmp = geometry.distance_to_key(rank(), origin,
						 SPATIAL_KEY(node));
		  if (tmp > limit)
		    {
		      if (near_node == 0 || tmp < near_distance)
			{
			  near_node = node;
			  near_dim = node_dim;
			  near_distance = tmp;
			}
		      else if (tmp == near_distance && node < near_node)
			{
			  near_node = node;
			  near_dim = node_dim;
			}
		    }
		}
	    }
	  // No more nodes to visit, so go up!
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
	    }
	}
      while(node != end);
      if (near_node == 0)
	{ near_node = node; near_dim = node_dim; }
      SPATIAL_ASSERT_CHECK(near_dim < rank());
      SPATIAL_ASSERT_CHECK(node != 0);
      return Derived(rank, cmp, geometry, origin, near_dim,
		     static_cast<Link_type>(near_node), near_distance);
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_NEIGHBOR_TPP
