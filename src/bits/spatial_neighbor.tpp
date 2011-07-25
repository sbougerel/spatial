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

    template <typename Rank, typename Key, typename Value, typename Node,
              typename Compare, typename Geometry, bool Constant,
              typename Derived>
    inline void
    Neighbor_iterator_base
    <Rank, Key, Value, Node, Compare, Geometry, Constant, Derived>
    ::increment()
    {
      SPATIAL_ASSERT_CHECK(impl_.node_dim_ < rank()());
      SPATIAL_ASSERT_CHECK(!Node_base::header(impl_.node_));
      SPATIAL_ASSERT_CHECK(impl_.node_ != 0);
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
      // right iteration variables start with 'r'
      Base_ptr rn = impl_.node_;
      dimension_type rn_dim = impl_.node_dim_;
      bool rn_break = false;
      // left iteration variables start with 'l'
      Base_ptr ln = impl_.node_;
      dimension_type ln_dim = impl_.node_dim_;
      bool ln_break = false;
      // existing values used for comparison
      Base_ptr curr = impl_.node_;
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
              && (!compare()(rn_dim, target(), Link_().key(rn))
                  || near_node == 0
                  || near_distance
                  >= geometry().distance_to_plane
                  (rank()(), rn_dim, target(), Link_().key(rn))))
            {
              rn = rn->right;
              rn_dim = incr_dim(rank(), rn_dim);
              while(rn->left != 0
                    && (!compare()(rn_dim, Link_().key(rn), target())
                        || near_node == 0
                        || near_distance
                        >= geometry().distance_to_plane
                        (rank()(), rn_dim, target(), Link_().key(rn))))
                {
                  rn = rn->left;
                  rn_dim = incr_dim(rank(), rn_dim);
                }
            }
          else
            {
              Base_ptr p = rn->parent;
              while (!Node_base::header(p) && p->right == rn)
                {
                  rn = p;
                  rn_dim = decr_dim(rank(), rn_dim);
                  p = rn->parent;
                }
              rn = p;
              rn_dim = decr_dim(rank(), rn_dim);
            }
          if (Node_base::header(rn))
            { if (ln_break) break; rn_break = true; goto left_side; }
          tmp = geometry().distance_to_key(rank()(), target(), Link_().key(rn));
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
              && (!compare()(ln_dim, Link_().key(ln), target())
                  || near_node == 0
                  || near_distance
                  >= geometry().distance_to_plane
                  (rank()(), ln_dim, target(), Link_().key(ln))))
            {
              ln = ln->left;
              ln_dim = incr_dim(rank(), ln_dim);
              while(ln->right != 0
                    && (!compare()(ln_dim, target(), Link_().key(ln))
                        || near_node == 0
                        || near_distance
                        >= geometry().distance_to_plane
                        (rank()(), ln_dim, target(), Link_().key(ln))))
                {
                  ln = ln->right;
                  ln_dim = incr_dim(rank(), ln_dim);
                }
            }
          else
            {
              Base_ptr p = ln->parent;
              while (!Node_base::header(p) && p->left == ln)
                {
                  ln = p;
                  ln_dim = decr_dim(rank(), ln_dim);
                  p = ln->parent;
                }
              ln = p;
              ln_dim = decr_dim(rank(), ln_dim);
            }
          if (Node_base::header(ln))
            { if (rn_break) break; ln_break = true; continue; }
          tmp = geometry().distance_to_key(rank()(), target(), Link_().key(ln));
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
      SPATIAL_ASSERT_CHECK(near_dim < rank()());
      SPATIAL_ASSERT_CHECK(near_node != curr);
      SPATIAL_ASSERT_CHECK(near_node == 0
                           || curr_distance < near_distance
                           || (curr_distance == near_distance
                               && curr < near_node));
      SPATIAL_ASSERT_CHECK(rn_dim < rank()());
      SPATIAL_ASSERT_CHECK(ln_dim < rank()());
      SPATIAL_ASSERT_CHECK(Node_base::header(rn));
      SPATIAL_ASSERT_CHECK(Node_base::header(ln));
      SPATIAL_ASSERT_CHECK(ln == rn);
      SPATIAL_ASSERT_CHECK(ln_dim == rn_dim);
      if (near_node != 0)
        {
          impl_.node_ = near_node;
          impl_.node_dim_ = near_dim;
          impl_.distance_() = near_distance;
        }
      else
        {
          impl_.node_ = rn;
          impl_.node_dim_ = rn_dim;
          impl_.distance_() = 0;
        }
    }

    // The next largest key on the neighbor dimension is likely to be found in
    // the children of the current best, so, descend into the children of node
    // first.
    template <typename Rank, typename Key, typename Value, typename Node,
              typename Compare, typename Geometry, bool Constant,
              typename Derived>
    inline void
    Neighbor_iterator_base
    <Rank, Key, Value, Node, Compare, Geometry, Constant, Derived>
    ::decrement()
    {
      SPATIAL_ASSERT_CHECK(impl_.node_dim_ < rank()());
      SPATIAL_ASSERT_CHECK(impl_.node_ != 0);
      // Must come back from an end position for reverse iteration...
      if (Node_base::header(impl_.node_))
        {
          Derived iter = maximum(rank(), compare(), geometry(), impl_.target_(),
                                 0, impl_.node_->parent);
          impl_.node_ = iter.impl_.node_;
          impl_.node_dim_ = iter.impl_.node_dim_;
          impl_.distance_() = iter.impl_.distance_();
          return;
        }
      /*
        As in 'increment', we follow the same convention: we traverse the tree
        in-order to the left and the right simultaneously.
      */
      // right iteration variables start with 'r'
      Base_ptr rn = impl_.node_;
      dimension_type rn_dim = impl_.node_dim_;
      bool rn_break = false;
      // left iteration variables start with 'l'
      Base_ptr ln = impl_.node_;
      dimension_type ln_dim = impl_.node_dim_;
      bool ln_break = false;
      // existing values used for comparison
      Base_ptr curr = impl_.node_;
      distance_type curr_distance = distance();
      Base_ptr near_node = 0;
      dimension_type near_dim = 0;
      distance_type near_distance = 0;
      distance_type tmp;
      do
        {
          // In-order traversal that starts with all nodes before 'curr'
          if (ln_break) { goto right_side; }
          if (ln->left != 0
              && (!compare()(ln_dim, Link_().key(ln), target())
                  || curr_distance
                  >= geometry().distance_to_plane
                  (rank()(), ln_dim, target(), Link_().key(ln))))
            {
              ln = ln->left;
              ln_dim = incr_dim(rank(), ln_dim);
              while(ln->right != 0
                    && (!compare()(ln_dim, target(), Link_().key(ln))
                        || curr_distance
                        >= geometry().distance_to_plane
                        (rank()(), ln_dim, target(), Link_().key(ln))))
                {
                  ln = ln->right;
                  ln_dim = incr_dim(rank(), ln_dim);
                }
            }
          else
            {
              Base_ptr p = ln->parent;
              while (!Node_base::header(p) && p->left == ln)
                {
                  ln = p;
                  ln_dim = decr_dim(rank(), ln_dim);
                  p = ln->parent;
                }
              ln = p;
              ln_dim = decr_dim(rank(), ln_dim);
            }
          if (Node_base::header(ln))
            { if (rn_break) break; ln_break = true; goto right_side; }
          tmp = geometry().distance_to_key(rank()(), target(), Link_().key(ln));
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
              && (!compare()(rn_dim, target(), Link_().key(rn))
                  || curr_distance
                  >= geometry().distance_to_plane
                  (rank()(), rn_dim, target(), Link_().key(rn))))
            {
              rn = rn->right;
              rn_dim = incr_dim(rank(), rn_dim);
              while(rn->left != 0
                    && (!compare()(rn_dim, Link_().key(rn), target())
                        || curr_distance
                        >= geometry().distance_to_plane
                        (rank()(), rn_dim, target(), Link_().key(rn))))
                {
                  rn = rn->left;
                  rn_dim = incr_dim(rank(), rn_dim);
                }
            }
          else
            {
              Base_ptr p = rn->parent;
              while (!Node_base::header(p) && p->right == rn)
                {
                  rn = p;
                  rn_dim = decr_dim(rank(), rn_dim);
                  p = rn->parent;
                }
              rn = p;
              rn_dim = decr_dim(rank(), rn_dim);
            }
          if (Node_base::header(rn))
            { if(ln_break) break; rn_break = true; continue; }
          tmp = geometry().distance_to_key(rank()(), target(), Link_().key(rn));
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
      SPATIAL_ASSERT_CHECK(near_dim < rank()());
      SPATIAL_ASSERT_CHECK(near_node != curr);
      SPATIAL_ASSERT_CHECK(near_node == 0
                           || curr_distance > near_distance
                           || (curr_distance == near_distance
                               && curr > near_node));
      SPATIAL_ASSERT_CHECK(rn_dim < rank()());
      SPATIAL_ASSERT_CHECK(ln_dim < rank()());
      SPATIAL_ASSERT_CHECK(Node_base::header(rn));
      SPATIAL_ASSERT_CHECK(Node_base::header(ln));
      SPATIAL_ASSERT_CHECK(rn == ln);
      SPATIAL_ASSERT_CHECK(rn_dim == ln_dim);
      if (near_node != 0)
        {
          impl_.node_ = near_node;
          impl_.node_dim_ = near_dim;
          impl_.distance_() = near_distance;
        }
      else
        {
          impl_.node_ = rn;
          impl_.node_dim_ = rn_dim;
          impl_.distance_() = 0;
        }
    }

    // Find the minimum from node and stop when reaching the parent. Iterate in
    // left-first fashion.
    template <typename Rank, typename Key, typename Value, typename Node,
              typename Compare, typename Geometry, bool Constant,
              typename Derived>
    inline Derived
    Neighbor_iterator_base
    <Rank, Key, Value, Node, Compare, Geometry, Constant, Derived>
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
        = geometry.distance_to_key(rank(), origin, Link_().key(node));
      distance_type tmp;
      // Depth traversal starts with left first
      while(node->left != 0
            && (!cmp(node_dim, Link_().key(node), origin)
                || near_distance
                >= geometry.distance_to_plane(rank(), node_dim, origin,
                                              Link_().key(node))))
        {
          node = node->left;
          node_dim = incr_dim(rank, node_dim);
          tmp = geometry.distance_to_key(rank(), origin, Link_().key(node));
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
              && (!cmp(node_dim, origin, Link_().key(node))
                  || near_distance
                  >= geometry.distance_to_plane(rank(), node_dim, origin,
                                                Link_().key(node))))
            {
              node = node->right;
              node_dim = incr_dim(rank, node_dim);
              tmp = geometry.distance_to_key(rank(), origin, Link_().key(node));
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
                    && (!cmp(node_dim, Link_().key(node), origin)
                        || near_distance
                        >= geometry.distance_to_plane(rank(), node_dim, origin,
                                                      Link_().key(node))))
                {
                  node = node->left;
                  node_dim = incr_dim(rank, node_dim);
                  tmp = geometry.distance_to_key(rank(), origin,
                                                 Link_().key(node));
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

    template <typename Rank, typename Key, typename Value, typename Node,
              typename Compare, typename Geometry, bool Constant,
              typename Derived>
    inline Derived
    Neighbor_iterator_base
    <Rank, Key, Value, Node, Compare, Geometry, Constant, Derived>
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
        = geometry.distance_to_key(rank(), origin, Link_().key(node));
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
        tmp = geometry.distance_to_key(rank(), origin, Link_().key(node));
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

    template <typename Rank, typename Key, typename Value, typename Node,
              typename Compare, typename Geometry, bool Constant,
              typename Derived>
    inline Derived
    Neighbor_iterator_base
    <Rank, Key, Value, Node, Compare, Geometry, Constant, Derived>
    ::lower_bound
    (const Rank& rank, const Compare& cmp, const Geometry& geometry,
     const Key& origin, distance_type limit, dimension_type node_dim,
     Base_ptr node)
    {
      SPATIAL_ASSERT_CHECK(node_dim < rank());
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      Base_ptr end = node->parent;
      Base_ptr near_node = 0;
      dimension_type near_dim = node_dim;
      distance_type tmp;
      distance_type near_distance
        = geometry.distance_to_key(rank(), origin, Link_().key(node));
      if (near_distance >= limit) { near_node = node; }
      // Depth traversal starts with left first
      while(node->left != 0
            && (!cmp(node_dim, Link_().key(node), origin)
                || near_node == 0
                || near_distance
                >= geometry.distance_to_plane(rank(), node_dim, origin,
                                              Link_().key(node))))
        {
          node = node->left;
          node_dim = incr_dim(rank, node_dim);
          tmp = geometry.distance_to_key(rank(), origin, Link_().key(node));
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
              && (!cmp(node_dim, origin, Link_().key(node))
                  || near_node == 0
                  || near_distance
                  >= geometry.distance_to_plane(rank(), node_dim, origin,
                                                Link_().key(node))))
            {
              node = node->right;
              node_dim = incr_dim(rank, node_dim);
              tmp = geometry.distance_to_key(rank(), origin, Link_().key(node));
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
                    && (!cmp(node_dim, Link_().key(node), origin)
                        || near_node == 0
                        || near_distance
                        >= geometry.distance_to_plane(rank(), node_dim, origin,
                                                      Link_().key(node))))
                {
                  node = node->left;
                  node_dim = incr_dim(rank, node_dim);
                  tmp = geometry.distance_to_key(rank(), origin,
                                                 Link_().key(node));
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

    template <typename Rank, typename Key, typename Value, typename Node,
              typename Compare, typename Geometry, bool Constant,
              typename Derived>
    inline Derived
    Neighbor_iterator_base
    <Rank, Key, Value, Node, Compare, Geometry, Constant, Derived>
    ::upper_bound
    (const Rank& rank, const Compare& cmp, const Geometry& geometry,
     const Key& origin, distance_type limit, dimension_type node_dim,
     Base_ptr node)
    {
      SPATIAL_ASSERT_CHECK(node_dim < rank());
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      Base_ptr end = node->parent;
      Base_ptr near_node = 0;
      dimension_type near_dim = node_dim;
      distance_type tmp;
      distance_type near_distance
        = geometry.distance_to_key(rank(), origin, Link_().key(node));
      if (near_distance > limit) { near_node = node; }
      // Depth traversal starts with left first
      while(node->left != 0
            && (!cmp(node_dim, Link_().key(node), origin)
                || near_node == 0
                || near_distance
                >= geometry.distance_to_plane(rank(), node_dim, origin,
                                              Link_().key(node))))
        {
          node = node->left;
          node_dim = incr_dim(rank, node_dim);
          tmp = geometry.distance_to_key(rank(), origin, Link_().key(node));
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
              && (!cmp(node_dim, origin, Link_().key(node))
                  || near_node == 0
                  || near_distance
                  >= geometry.distance_to_plane(rank(), node_dim, origin,
                                                Link_().key(node))))
            {
              node = node->right;
              node_dim = incr_dim(rank, node_dim);
              tmp = geometry.distance_to_key(rank(), origin, Link_().key(node));
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
                    && (!cmp(node_dim, Link_().key(node), origin)
                        || near_node == 0
                        || near_distance
                        >= geometry.distance_to_plane(rank(), node_dim, origin,
                                                      Link_().key(node))))
                {
                  node = node->left;
                  node_dim = incr_dim(rank, node_dim);
                  tmp = geometry.distance_to_key(rank(), origin,
                                                 Link_().key(node));
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
