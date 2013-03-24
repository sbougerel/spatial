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

    template <typename Container, typename Metric>
    inline neighbor_iterator<Container, Metric>&
    increment_neighbor(neighbor_iterator<Container, Metric>& it)
    {
      typedef Node<typename Container::mode_type>* node_ptr;
      const typename container_traits<Container>::rank_type rank(it.rank());
      const typename container_traits<Container>::key_compare cmp(it.key_comp());
      const Metric met(it.metric());
      SPATIAL_ASSERT_CHECK(it.node_dim < rank());
      SPATIAL_ASSERT_CHECK(it.node != 0);
      SPATIAL_ASSERT_CHECK(!header(it.node));
      // In this algorithm, we seek to find the next nearest point to
      // origin. Thus assuming that this point exists, its distance to origin is
      // equal or greater than that of the previous nearest point to origin.
      // Since K-d tree are good at preserving locality, it is best to search
      // the next nearest point from the current nearest point, since these 2
      // points could be close to one another in the tree. In order to find the
      // next nearest, we perform in-order transveral, at the left and right
      // simultaneously.
      //
      // right iteration variables start with 'r'
      // left iteration variables start with 'l'
      node_ptr rn = it.node;
      node_ptr ln = it.node;
      dimension_type rn_dim = it.node_dim;
      dimension_type ln_dim = it.node_dim;
      bool rn_break = false;
      bool ln_break = false;
      // existing values used for comparison
      node_ptr curr = it.node;
      node_ptr near_node = 0;
      dimension_type near_dim = 0;
      typename Metric::distance_type near_distance = 0;
      typename Metric::distance_type tmp;
      do
        {
          // One step to the right side...
          if (rn_break) { goto left_side; }
          if (rn->right != 0
              && (!cmp(rn_dim, target_key(it), const_key(rn))
                  || near_node == 0
                  || near_distance >= met.distance_to_plane
                  (rank(), rn_dim, target_key(it), const_key(rn))))
            {
              rn = rn->right;
              rn_dim = incr_dim(rank, rn_dim);
              while(rn->left != 0
                    && (!cmp(rn_dim, const_key(rn), target_key(it))
                        || near_node == 0
                        || near_distance >= met.distance_to_plane
                        (rank(), rn_dim, target_key(it), const_key(rn))))
                {
                  rn = rn->left;
                  rn_dim = incr_dim(rank, rn_dim);
                }
            }
          else
            {
              node_ptr p = rn->parent;
              while (!header(p) && p->right == rn)
                {
                  rn = p;
                  rn_dim = decr_dim(rank, rn_dim);
                  p = rn->parent;
                }
              rn = p;
              rn_dim = decr_dim(rank, rn_dim);
            }
          if (header(rn))
            { if (ln_break) break; rn_break = true; goto left_side; }
          tmp = met.distance_to_key(rank(), target_key(it), const_key(rn));
          if (tmp < distance(it) || (tmp == distance(it) && rn < curr))
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
              && (!cmp(ln_dim, const_key(ln), target_key(it))
                  || near_node == 0
                  || near_distance >= met.distance_to_plane
                  (rank(), ln_dim, target_key(it), const_key(ln))))
            {
              ln = ln->left;
              ln_dim = incr_dim(rank, ln_dim);
              while(ln->right != 0
                    && (!cmp(ln_dim, target_key(it), const_key(ln))
                        || near_node == 0
                        || near_distance >= met.distance_to_plane
                        (rank(), ln_dim, target_key(it), const_key(ln))))
                {
                  ln = ln->right;
                  ln_dim = incr_dim(rank, ln_dim);
                }
            }
          else
            {
              node_ptr p = ln->parent;
              while (!header(p) && p->left == ln)
                {
                  ln = p;
                  ln_dim = decr_dim(rank, ln_dim);
                  p = ln->parent;
                }
              ln = p;
              ln_dim = decr_dim(rank, ln_dim);
            }
          if (header(ln))
            { if (rn_break) break; ln_break = true; continue; }
          tmp = met.distance_to_key(rank(), target_key(it), const_key(ln));
          if (tmp < distance(it) || (tmp == distance(it) && ln < curr))
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
      SPATIAL_ASSERT_CHECK(near_dim < rank());
      SPATIAL_ASSERT_CHECK(near_node != curr);
      SPATIAL_ASSERT_CHECK(near_node == 0 || distance(it) < near_distance
                           || (distance(it) == near_distance
                               && curr < near_node));
      SPATIAL_ASSERT_CHECK(rn_dim == (rank() - 1));
      SPATIAL_ASSERT_CHECK(ln_dim == (rank() - 1));
      if (near_node != 0)
        {
          it.node = near_node;
          it.node_dim = near_dim;
          it.distance() = near_distance;
        }
      else
        {
          it.node = rn; // rn points past-the-end
          it.node_dim = rn_dim;
        }
      return it;
    }

    // The next largest key on the neighbor dimension is likely to be found in
    // the children of the current best, so, descend into the children of node
    // first.
    template <typename Container, typename Metric>
    inline neighbor_iterator<Container, Metric>&
    decrement_neighbor(neighbor_iterator<Container, Metric>& it)
    {
      typedef Node<typename Container::mode_type>* node_ptr;
      const typename container_traits<Container>::rank_type rank(it.rank());
      const typename container_traits<Container>::key_compare cmp(it.key_comp());
      const Metric met(it.metric());
      SPATIAL_ASSERT_CHECK(it.node_dim < rank());
      SPATIAL_ASSERT_CHECK(it.node != 0);
      // Must come back from an end position for reverse iteration...
      if (header(it.node))
        {
          it.node = it.node->parent;
          it.node_dim = 0; // root is always compared on dimension 0
          return maximum_neighbor(it);
        }
      // As in 'increment', we follow the same convention: we traverse the tree
      // in-order to the left and the right simultaneously.
      //
      // right iteration variables start with 'r'
      // left iteration variables start with 'l'
      node_ptr rn = it.node;
      node_ptr ln = it.node;
      dimension_type rn_dim = it.node_dim;
      dimension_type ln_dim = it.node_dim;
      bool rn_break = false;
      bool ln_break = false;
      // existing values used for comparison
      node_ptr curr = it.node;
      node_ptr near_node = 0;
      dimension_type near_dim = 0;
      typename Metric::distance_type near_distance = 0;
      typename Metric::distance_type tmp;
      do
        {
          // In-order traversal that starts with all nodes before 'curr'
          if (ln_break) { goto right_side; }
          if (ln->left != 0
              && (!cmp(ln_dim, const_key(ln), target_key(it))
                  || distance(it) >= met.distance_to_plane
                  (rank(), ln_dim, target_key(it), const_key(ln))))
            {
              ln = ln->left;
              ln_dim = incr_dim(rank, ln_dim);
              while(ln->right != 0
                    && (!cmp(ln_dim, target_key(it), const_key(ln))
                        || distance(it) >= met.distance_to_plane
                        (rank(), ln_dim, target_key(it), const_key(ln))))
                {
                  ln = ln->right;
                  ln_dim = incr_dim(rank, ln_dim);
                }
            }
          else
            {
              node_ptr p = ln->parent;
              while (!header(p) && p->left == ln)
                {
                  ln = p;
                  ln_dim = decr_dim(rank, ln_dim);
                  p = ln->parent;
                }
              ln = p;
              ln_dim = decr_dim(rank, ln_dim);
            }
          if (header(ln))
            { if (rn_break) break; ln_break = true; goto right_side; }
          tmp = met.distance_to_key(rank(), target_key(it), const_key(ln));
          if (tmp > distance(it) || (tmp == distance(it) && ln > curr))
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
          // Now, in-order traversal that starts with all nodes after 'curr'
          if (rn_break) { continue; }
          if (rn->right != 0
              && (!cmp(rn_dim, target_key(it), const_key(rn))
                  || distance(it) >= met.distance_to_plane
                  (rank(), rn_dim, target_key(it), const_key(rn))))
            {
              rn = rn->right;
              rn_dim = incr_dim(rank, rn_dim);
              while(rn->left != 0
                    && (!cmp(rn_dim, const_key(rn), target_key(it))
                        || distance(it) >= met.distance_to_plane
                        (rank(), rn_dim, target_key(it), const_key(rn))))
                {
                  rn = rn->left;
                  rn_dim = incr_dim(rank, rn_dim);
                }
            }
          else
            {
              node_ptr p = rn->parent;
              while (!header(p) && p->right == rn)
                {
                  rn = p;
                  rn_dim = decr_dim(rank, rn_dim);
                  p = rn->parent;
                }
              rn = p;
              rn_dim = decr_dim(rank, rn_dim);
            }
          if (header(rn))
            { if(ln_break) break; rn_break = true; continue; }
          tmp = met.distance_to_key(rank(), target_key(it), const_key(rn));
          if (tmp > distance(it) || (tmp == distance(it) && rn > curr))
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
      SPATIAL_ASSERT_CHECK(near_dim < rank());
      SPATIAL_ASSERT_CHECK(near_node != curr);
      SPATIAL_ASSERT_CHECK(near_node == 0
                           || distance(it) > near_distance
                           || (distance(it) == near_distance
                               && curr > near_node));
      SPATIAL_ASSERT_CHECK(rn_dim == (rank() - 1));
      SPATIAL_ASSERT_CHECK(ln_dim == (rank() - 1));
      if (near_node != 0)
        {
          it.node = near_node;
          it.node_dim = near_dim;
          it.distance() = near_distance;
        }
      else
        {
          it.node = rn; // rn points past-the-end here
          it.node_dim = rn_dim;
        }
      return it;
    }

    // Find the minimum from node and stop when reaching the parent. Iterate
    // in left-first fashion.
    template <typename Container, typename Metric>
    inline neighbor_iterator<Container, Metric>&
    minimum_neighbor(neighbor_iterator<Container, Metric>& it)
    {
      typedef Node<typename Container::mode_type>* node_ptr;
      const typename container_traits<Container>::rank_type rank(it.rank());
      const typename container_traits<Container>::key_compare cmp(it.key_comp());
      const Metric met(it.metric());
      SPATIAL_ASSERT_CHECK(it.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(it.node));
      SPATIAL_ASSERT_CHECK(it.node != 0);
      node_ptr node = it.node;
      dimension_type node_dim = it.node_dim;
      node_ptr near_node = node;
      node_ptr end = node->parent;
      dimension_type near_dim = node_dim;
      typename Metric::distance_type near_distance
        = met.distance_to_key(rank(), target_key(it), const_key(node));
      typename Metric::distance_type tmp;
      // Depth traversal starts with left first
      while(node->left != 0
            && (!cmp(node_dim, const_key(node), target_key(it))
                || near_distance
                >= met.distance_to_plane(rank(), node_dim, target_key(it),
                                         const_key(node))))
        {
          node = node->left;
          node_dim = incr_dim(rank, node_dim);
          tmp = met.distance_to_key(rank(), target_key(it), const_key(node));
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
              && (!cmp(node_dim, target_key(it), const_key(node))
                  || near_distance
                  >= met.distance_to_plane(rank(), node_dim, target_key(it),
                                           const_key(node))))
            {
              node = node->right;
              node_dim = incr_dim(rank, node_dim);
              tmp = met.distance_to_key(rank(), target_key(it), const_key(node));
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
                    && (!cmp(node_dim, const_key(node), target_key(it))
                        || near_distance
                        >= met.distance_to_plane(rank(), node_dim, target_key(it),
                                                 const_key(node))))
                {
                  node = node->left;
                  node_dim = incr_dim(rank, node_dim);
                  tmp = met.distance_to_key(rank(), target_key(it),
                                            const_key(node));
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
              node_ptr p = node->parent;
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
      SPATIAL_ASSERT_CHECK(!header(near_node));
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(near_node != 0);
      it.node = near_node; it.node_dim = near_dim;
      it.distance() = near_distance;
      return it;
    }

    // Find the minimum from node and stop when reaching the parent. Iterate
    // in left-first fashion.
    template <typename Container, typename Metric>
    inline neighbor_iterator<Container, Metric>&
    maximum_neighbor(neighbor_iterator<Container, Metric>& it)
    {
      typedef Node<typename Container::mode_type>* node_ptr;
      const Metric met(it.metric());
      const typename container_traits<Container>::rank_type rank(it.rank());
      SPATIAL_ASSERT_CHECK(it.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(it.node));
      SPATIAL_ASSERT_CHECK(it.node != 0);
      node_ptr node = it.node;
      node_ptr end = node->parent;
      dimension_type node_dim = it.node_dim;
      typename Metric::distance_type tmp;
      // Finding the maximum is, for lack of a better algorithm, equivalent to a
      // O(n) search. An alternative has been explored: being able to find if a
      // node is in a cell that is smaller than the current 'far_node' node
      // found.  However, with the data at hand, computing the cell turned out
      // to be more expensive than doing a simple iteration over all nodes in
      // the tree.  Maybe, one day we'll find a better algorithm that also has
      // no impact on the memory footprint of the tree (although I doubt these 2
      // conditions will ever be met. Probably there will be a tradeoff.)
      //
      // Iterate from left most to right most, and stop at node's parent.
      while (node->left != 0)
        { node = node->left; node_dim = incr_dim(rank, node_dim); }
      node_ptr far_node = node;
      dimension_type far_dim = node_dim;
      typename Metric::distance_type far_distance
        = met.distance_to_key(rank(), target_key(it), const_key(node));
      do {
        if (node->right != 0)
          {
            node = node->right; node_dim = incr_dim(rank, node_dim);
            while (node->left != 0)
              { node = node->left; node_dim = incr_dim(rank, node_dim); }
          }
        else
          {
            node_ptr p = node->parent;
            while (p != end && node == p->right)
              {
                node = p; p = node->parent;
                node_dim = decr_dim(rank, node_dim);
              }
            node = p;
            node_dim = decr_dim(rank, node_dim);
          }
        if (node == end) { break; }
        tmp = met.distance_to_key(rank(), target_key(it), const_key(node));
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
      SPATIAL_ASSERT_CHECK(!header(far_node));
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(far_node != 0);
      it.node = far_node; it.node_dim = far_dim;
      it.distance() = far_distance;
      return it;
    }

    template <typename Container, typename Metric>
    inline neighbor_iterator<Container, Metric>&
    lower_bound_neighbor(neighbor_iterator<Container, Metric>& it,
                         const typename Metric::distance_type bound)
    {
      typedef Node<typename Container::mode_type>* node_ptr;
      const typename container_traits<Container>::rank_type rank(it.rank());
      const typename container_traits<Container>::key_compare cmp(it.key_comp());
      const Metric met(it.metric());
      SPATIAL_ASSERT_CHECK(it.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(it.node));
      SPATIAL_ASSERT_CHECK(it.node != 0);
      node_ptr node = it.node;
      node_ptr end = node->parent;
      node_ptr near_node = 0;
      dimension_type node_dim = it.node_dim;
      dimension_type near_dim = node_dim;
      typename Metric::distance_type tmp;
      typename Metric::distance_type near_distance
        = met.distance_to_key(rank(), target_key(it), const_key(node));
      if (near_distance >= bound) { near_node = node; }
      // Depth traversal starts with left first
      while(node->left != 0
            && (!cmp(node_dim, const_key(node), target_key(it))
                || near_node == 0 || near_distance
                >= met.distance_to_plane(rank(), node_dim, target_key(it),
                                         const_key(node))))
        {
          node = node->left;
          node_dim = incr_dim(rank, node_dim);
          tmp = met.distance_to_key(rank(), target_key(it), const_key(node));
          if (tmp < bound) { continue; }
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
              && (!cmp(node_dim, target_key(it), const_key(node))
                  || near_node == 0 || near_distance
                  >= met.distance_to_plane(rank(), node_dim, target_key(it),
                                           const_key(node))))
            {
              node = node->right;
              node_dim = incr_dim(rank, node_dim);
              tmp = met.distance_to_key(rank(), target_key(it), const_key(node));
              if (tmp >= bound)
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
                    && (!cmp(node_dim, const_key(node), target_key(it))
                        || near_node == 0
                        || near_distance
                        >= met.distance_to_plane(rank(), node_dim, target_key(it),
                                                 const_key(node))))
                {
                  node = node->left;
                  node_dim = incr_dim(rank, node_dim);
                  tmp = met.distance_to_key(rank(), target_key(it),
                                            const_key(node));
                  if (tmp >= bound)
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
              node_ptr p = node->parent;
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
      it.node = near_node; it.node_dim = near_dim;
      it.distance() = near_distance;
      return it;
    }

    template <typename Container, typename Metric>
    inline neighbor_iterator<Container, Metric>&
    upper_bound_neighbor(neighbor_iterator<Container, Metric>& it,
                         const typename Metric::distance_type bound)
    {
      typedef Node<typename Container::mode_type>* node_ptr;
      const typename container_traits<Container>::rank_type rank(it.rank());
      const typename container_traits<Container>::key_compare cmp(it.key_comp());
      const Metric met(it.metric());
      SPATIAL_ASSERT_CHECK(it.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(it.node));
      SPATIAL_ASSERT_CHECK(it.node != 0);
      node_ptr node = it.node;
      node_ptr end = node->parent;
      node_ptr near_node = 0;
      dimension_type node_dim = it.node_dim;
      dimension_type near_dim = node_dim;
      typename Metric::distance_type tmp;
      typename Metric::distance_type near_distance
        = met.distance_to_key(rank(), target_key(it), const_key(node));
      if (near_distance > bound) { near_node = node; }
      // Depth traversal starts with left first
      while(node->left != 0
            && (!cmp(node_dim, const_key(node), target_key(it))
                || near_node == 0
                || near_distance
                >= met.distance_to_plane(rank(), node_dim, target_key(it),
                                         const_key(node))))
        {
          node = node->left;
          node_dim = incr_dim(rank, node_dim);
          tmp = met.distance_to_key(rank(), target_key(it), const_key(node));
          if (tmp <= bound) { continue; }
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
              && (!cmp(node_dim, target_key(it), const_key(node))
                  || near_node == 0 || near_distance
                  >= met.distance_to_plane(rank(), node_dim, target_key(it),
                                           const_key(node))))
            {
              node = node->right;
              node_dim = incr_dim(rank, node_dim);
              tmp = met.distance_to_key(rank(), target_key(it), const_key(node));
              if (tmp > bound)
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
                    && (!cmp(node_dim, const_key(node), target_key(it))
                        || near_node == 0
                        || near_distance
                        >= met.distance_to_plane(rank(), node_dim, target_key(it),
                                                 const_key(node))))
                {
                  node = node->left;
                  node_dim = incr_dim(rank, node_dim);
                  tmp = met.distance_to_key(rank(), target_key(it),
                                            const_key(node));
                  if (tmp > bound)
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
              node_ptr p = node->parent;
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
      it.node = near_node; it.node_dim = near_dim;
      it.distance() = near_distance;
      return it;
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_NEIGHBOR_TPP
