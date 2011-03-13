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
	In this algorithm, we seek to find the next nearest pointo origin. Thus
	assuming that this point exists, its distance to origin is equal or
	greater than that of the previous nearest point to origin.  Since K-d
	tree are good at preserving locality, it is best to search the next
	nearest point from the current nearest point, since these 2 points could
	be close to one another in the tree. In order to find the next nearest,
	we choose to walk all the nodes at the right of the current nearest,
	then all the nodes at the left. We set this has a convention: when we
	seeking to find a greater distance, we search right first then left. We
	do the contrary otherwise.
      */
      const Compare& cmp = compare();
      const Key& origin = impl.origin();
      const Rank& rk = rank();
      const Geometry& geo = geometry();
      Base_ptr node = impl.node;
      dimension_type node_dim = impl.node_dim;
      Base_ptr curr = node;
      dimension_type curr_dim = node_dim;
      distance_type curr_distance = distance();
      Base_ptr near = 0;
      dimension_type near_dim = 0;
      distance_type near_distance = 0;
      distance_type tmp;
      // In-order traversal that starts with all nodes after 'node'
      do
	{
	  if (node->right != 0
	      && (!cmp(node_dim, origin, SPATIAL_KEY(node))
		  || near == 0
		  || near_distance
		  >= geo.distance_to_plane
		  (rk(), node_dim, origin, SPATIAL_KEY(node))))
	    {
	      node = node->right;
	      node_dim = incr_dim(rk, node_dim);
	      while(node->left != 0
		    && (!cmp(node_dim, SPATIAL_KEY(node), origin)
			|| near == 0
			|| near_distance
			>= geo.distance_to_plane
			(rk(), node_dim, origin, SPATIAL_KEY(node))))
		{
		  node = node->left;
		  node_dim = incr_dim(rk, node_dim);
		}
	    }
	  else
	    {
	      Base_ptr p = node->parent;
	      while (!Node_base::header(p) && p->right == node)
		{
		  node = p;
		  node_dim = decr_dim(rk, node_dim);
		  p = node->parent;
		}
	      node = p;
	      node_dim = decr_dim(rk, node_dim);
	    }
	  if (Node_base::header(node)) break;
	  tmp = geo.distance_to_key(rk(), origin, SPATIAL_KEY(node));
	  if (tmp < curr_distance || (tmp == curr_distance && node < curr))
	    { continue; }
	  if (near == 0 || tmp < near_distance)
	    {
	      near = node;
	      near_dim = node_dim;
	      near_distance = tmp;
	    }
	  else if (tmp == near_distance && node < near)
	    {
	      near = node;
	      near_dim = node_dim;
	    }
	}
      while(true);
      // Now, in-order traversal that finished with all nodes before 'node'
      node = curr;
      node_dim = curr_dim;
      do
	{
	  if (node->left != 0
	      && (!cmp(node_dim, SPATIAL_KEY(node), origin)
		  || near == 0
		  || near_distance
		  >= geo.distance_to_plane
		  (rk(), node_dim, origin, SPATIAL_KEY(node))))
	    {
	      node = node->left;
	      node_dim = incr_dim(rk, node_dim);
	      while(node->right != 0
		    && (!cmp(node_dim, origin, SPATIAL_KEY(node))
			|| near == 0
			|| near_distance
			>= geo.distance_to_plane
			(rk(), node_dim, origin, SPATIAL_KEY(node))))
		{
		  node = node->right;
		  node_dim = incr_dim(rk, node_dim);
		}
	    }
	  else
	    {
	      Base_ptr p = node->parent;
	      while (!Node_base::header(p) && p->left == node)
		{
		  node = p;
		  node_dim = decr_dim(rk, node_dim);
		  p = node->parent;
		}
	      node = p;
	      node_dim = decr_dim(rk, node_dim);
	    }
	  if (Node_base::header(node)) break;
	  tmp = geo.distance_to_key(rk(), origin, SPATIAL_KEY(node));
	  if (tmp < curr_distance || (tmp == curr_distance && node < curr))
	    { continue; }
	  if (near == 0 || tmp < near_distance)
	    {
	      near = node;
	      near_dim = node_dim;
	      near_distance = tmp;
	    }
	  else if (tmp == near_distance && node < near)
	    {
	      near = node;
	      near_dim = node_dim;
	    }
	}
      while(true);
      SPATIAL_ASSERT_CHECK(near_dim < rk());
      SPATIAL_ASSERT_CHECK(near != curr);
      SPATIAL_ASSERT_CHECK(near == 0
			   || curr_distance < near_distance
			   || (curr_distance == near_distance && curr < near));
      SPATIAL_ASSERT_CHECK(node_dim < rk());
      SPATIAL_ASSERT_CHECK(Node_base::header(node));
      if (near != 0)
	{
	  impl.node = near;
	  impl.node_dim = near_dim;
	  impl.distance() = near_distance;
	}
      else
	{
	  impl.node = node;
	  impl.node_dim = node_dim;
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
	in-order by starting with node before the current node, then the nodes
	after the current node.
      */
      const Compare& cmp = compare();
      const Key& origin = impl.origin();
      const Rank& rk = rank();
      const Geometry& geo = geometry();
      Base_ptr node = impl.node;
      dimension_type node_dim = impl.node_dim;
      Base_ptr curr = node;
      dimension_type curr_dim = node_dim;
      distance_type curr_distance = distance();
      Base_ptr near = 0;
      dimension_type near_dim = 0;
      distance_type near_distance = 0;
      distance_type tmp;
      // In-order traversal that starts with all nodes after 'node'
      do
	{
	  if (node->left != 0
	      && (!cmp(node_dim, SPATIAL_KEY(node), origin)
		  || curr_distance
		  >= geo.distance_to_plane
		  (rk(), node_dim, origin, SPATIAL_KEY(node))))
	    {
	      node = node->left;
	      node_dim = incr_dim(rk, node_dim);
	      while(node->right != 0
		    && (!cmp(node_dim, origin, SPATIAL_KEY(node))
			|| curr_distance
			>= geo.distance_to_plane
			(rk(), node_dim, origin, SPATIAL_KEY(node))))
		{
		  node = node->right;
		  node_dim = incr_dim(rk, node_dim);
		}
	    }
	  else
	    {
	      Base_ptr p = node->parent;
	      while (!Node_base::header(p) && p->left == node)
		{
		  node = p;
		  node_dim = decr_dim(rk, node_dim);
		  p = node->parent;
		}
	      node = p;
	      node_dim = decr_dim(rk, node_dim);
	    }
	  if (Node_base::header(node)) break;
	  tmp = geo.distance_to_key(rk(), origin, SPATIAL_KEY(node));
	  if (tmp > curr_distance || (tmp == curr_distance && node > curr))
	    { continue; }
	  if (near == 0 || tmp > near_distance)
	    {
	      near = node;
	      near_dim = node_dim;
	      near_distance = tmp;
	    }
	  else if (tmp == near_distance && node > near)
	    {
	      near = node;
	      near_dim = node_dim;
	    }
	}
      while(true);
      // Now, in-order traversal that finished with all nodes before 'node'
      node = curr;
      node_dim = curr_dim;
      do
	{
	  if (node->right != 0
	      && (!cmp(node_dim, origin, SPATIAL_KEY(node))
		  || curr_distance
		  >= geo.distance_to_plane
		  (rk(), node_dim, origin, SPATIAL_KEY(node))))
	    {
	      node = node->right;
	      node_dim = incr_dim(rk, node_dim);
	      while(node->left != 0
		    && (!cmp(node_dim, SPATIAL_KEY(node), origin)
			|| curr_distance
			>= geo.distance_to_plane
			(rk(), node_dim, origin, SPATIAL_KEY(node))))
		{
		  node = node->left;
		  node_dim = incr_dim(rk, node_dim);
		}
	    }
	  else
	    {
	      Base_ptr p = node->parent;
	      while (!Node_base::header(p) && p->right == node)
		{
		  node = p;
		  node_dim = decr_dim(rk, node_dim);
		  p = node->parent;
		}
	      node = p;
	      node_dim = decr_dim(rk, node_dim);
	    }
	  if (Node_base::header(node)) break;
	  tmp = geo.distance_to_key(rk(), origin, SPATIAL_KEY(node));
	  if (tmp > curr_distance || (tmp == curr_distance && node > curr))
	    { continue; }
	  if (near == 0 || tmp > near_distance)
	    {
	      near = node;
	      near_dim = node_dim;
	      near_distance = tmp;
	    }
	  else if (tmp == near_distance && node > near)
	    {
	      near = node;
	      near_dim = node_dim;
	    }
	}
      while(true);
      SPATIAL_ASSERT_CHECK(near_dim < rk());
      SPATIAL_ASSERT_CHECK(near != curr);
      SPATIAL_ASSERT_CHECK(near == 0
			   || curr_distance > near_distance
			   || (curr_distance == near_distance && curr > near));
      SPATIAL_ASSERT_CHECK(node_dim < rk());
      SPATIAL_ASSERT_CHECK(Node_base::header(node));
      if (near != 0)
	{
	  impl.node = near;
	  impl.node_dim = near_dim;
	  impl.distance() = near_distance;
	}
      else
	{
	  impl.node = node;
	  impl.node_dim = node_dim;
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
      Base_ptr end = node->parent;
      Base_ptr near = node;
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
	      near = node;
	      near_dim = node_dim;
	      near_distance = tmp;
	    }
	  else if (tmp == near_distance
		   // to provide total ordering among the nodes, we use the node
		   // pointer as a fall back comparison mechanism
		   && node < near)
	    {
	      near = node;
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
		  near = node;
		  near_dim = node_dim;
		  near_distance = tmp;
		}
	      else if (tmp == near_distance && node < near)
		{
		  near = node;
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
		      near = node;
		      near_dim = node_dim;
		      near_distance = tmp;
		    }
		  else if (tmp == near_distance && node < near)
		    {
		      near = node;
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
      SPATIAL_ASSERT_CHECK(!Node_base::header(near));
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(near != 0);
      return Derived(rank, cmp, geometry, origin, near_dim,
		     static_cast<Link_type>(near), near_distance);
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
      // node is in a cell that is smaller than the current 'far' node found.
      // However, with the data at hand, computing the cell turned out to be
      // more expensive than doing a simple iteration over all nodes in the
      // tree.  May be, one day we'll find a better algorithm that also has no
      // impact on the memory footprint of the tree (although I doubt these 2
      // conditions will ever be met. Probably there will be a tradeoff.
      //
      // Iterate from left most to right most, and stop at node's parent.
      while (node->left != 0)
	{ node = node->left; node_dim = incr_dim(rank, node_dim); }
      Base_ptr far = node;
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
	    || (tmp == far_distance && node > far))
	  {
	    far = node;
	    far_dim = node_dim;
	    far_distance = tmp;
	  }
      }
      while(true);
      SPATIAL_ASSERT_CHECK(far_dim < rank());
      SPATIAL_ASSERT_CHECK(!Node_base::header(far));
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(far != 0);
      return Derived(rank, cmp, geometry, origin, far_dim,
		     static_cast<Link_type>(far), far_distance);
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
      Base_ptr near = 0;
      dimension_type near_dim = node_dim;
      distance_type tmp;
      distance_type near_distance
	= geometry.distance_to_key(rank(), origin, SPATIAL_KEY(node));
      if (near_distance >= limit) { near = node; }
      // Depth traversal starts with left first
      while(node->left != 0
	    && (!cmp(node_dim, SPATIAL_KEY(node), origin)
		|| near == 0
		|| near_distance
		>= geometry.distance_to_plane(rank(), node_dim, origin,
					      SPATIAL_KEY(node))))
	{
	  node = node->left;
	  node_dim = incr_dim(rank, node_dim);
	  tmp = geometry.distance_to_key(rank(), origin, SPATIAL_KEY(node));
	  if (tmp < limit) { continue; }
	  if (near == 0 || tmp < near_distance)
	    {
	      near = node;
	      near_dim = node_dim;
	      near_distance = tmp;
	    }
	  else if (tmp == near_distance
		   // to provide total ordering among the nodes, we use the node
		   // pointer as a fall back comparison mechanism
		   && node < near)
	    {
	      near = node;
	      near_dim = node_dim;
	    }
	}
      // In-order, right, left, then all the way up
      do
	{
	  if (node->right != 0
	      && (!cmp(node_dim, origin, SPATIAL_KEY(node))
		  || near == 0
		  || near_distance
		  >= geometry.distance_to_plane(rank(), node_dim, origin,
						SPATIAL_KEY(node))))
	    {
	      node = node->right;
	      node_dim = incr_dim(rank, node_dim);
	      tmp = geometry.distance_to_key(rank(), origin, SPATIAL_KEY(node));
	      if (tmp >= limit)
		{
		  if (near == 0 || tmp < near_distance)
		    {
		      near = node;
		      near_dim = node_dim;
		      near_distance = tmp;
		    }
		  else if (tmp == near_distance && node < near)
		    {
		      near = node;
		      near_dim = node_dim;
		    }
		}
	      while(node->left != 0
		    && (!cmp(node_dim, SPATIAL_KEY(node), origin)
			|| near == 0
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
		      if (near == 0 || tmp < near_distance)
			{
			  near = node;
			  near_dim = node_dim;
			  near_distance = tmp;
			}
		      else if (tmp == near_distance && node < near)
			{
			  near = node;
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
      if (near == 0)
	{ near = node; near_dim = node_dim; }
      SPATIAL_ASSERT_CHECK(near_dim < rank());
      SPATIAL_ASSERT_CHECK(node != 0);
      return Derived(rank, cmp, geometry, origin, near_dim,
		     static_cast<Link_type>(near), near_distance);
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
      Base_ptr near = 0;
      dimension_type near_dim = node_dim;
      distance_type tmp;
      distance_type near_distance
	= geometry.distance_to_key(rank(), origin, SPATIAL_KEY(node));
      if (near_distance > limit) { near = node; }
      // Depth traversal starts with left first
      while(node->left != 0
	    && (!cmp(node_dim, SPATIAL_KEY(node), origin)
		|| near == 0
		|| near_distance
		>= geometry.distance_to_plane(rank(), node_dim, origin,
					      SPATIAL_KEY(node))))
	{
	  node = node->left;
	  node_dim = incr_dim(rank, node_dim);
	  tmp = geometry.distance_to_key(rank(), origin, SPATIAL_KEY(node));
	  if (tmp <= limit) { continue; }
	  if (near == 0 || tmp < near_distance)
	    {
	      near = node;
	      near_dim = node_dim;
	      near_distance = tmp;
	    }
	  else if (tmp == near_distance
		   // to provide total ordering among the nodes, we use the node
		   // pointer as a fall back comparison mechanism
		   && node < near)
	    {
	      near = node;
	      near_dim = node_dim;
	    }
	}
      // In-order, right, left, then all the way up
      do
	{
	  if (node->right != 0
	      && (!cmp(node_dim, origin, SPATIAL_KEY(node))
		  || near == 0
		  || near_distance
		  >= geometry.distance_to_plane(rank(), node_dim, origin,
						SPATIAL_KEY(node))))
	    {
	      node = node->right;
	      node_dim = incr_dim(rank, node_dim);
	      tmp = geometry.distance_to_key(rank(), origin, SPATIAL_KEY(node));
	      if (tmp > limit)
		{
		  if (near == 0 || tmp < near_distance)
		    {
		      near = node;
		      near_dim = node_dim;
		      near_distance = tmp;
		    }
		  else if (tmp == near_distance && node < near)
		    {
		      near = node;
		      near_dim = node_dim;
		    }
		}
	      while(node->left != 0
		    && (!cmp(node_dim, SPATIAL_KEY(node), origin)
			|| near == 0
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
		      if (near == 0 || tmp < near_distance)
			{
			  near = node;
			  near_dim = node_dim;
			  near_distance = tmp;
			}
		      else if (tmp == near_distance && node < near)
			{
			  near = node;
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
      if (near == 0)
	{ near = node; near_dim = node_dim; }
      SPATIAL_ASSERT_CHECK(near_dim < rank());
      SPATIAL_ASSERT_CHECK(node != 0);
      return Derived(rank, cmp, geometry, origin, near_dim,
		     static_cast<Link_type>(near), near_distance);
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_NEIGHBOR_TPP
