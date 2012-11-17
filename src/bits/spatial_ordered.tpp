// -*- C++ -*-

/**
 *  @file   spatial_ordered.tpp
 *  @brief
 *
 *  Change Log:
 *
 *  - 2009-02-27 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_ORDERED_TPP
#define SPATIAL_ORDERED_TPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

namespace spatial
{
  namespace details
  {
    template <typename Cmp, typename Rank, typename Key>
    inline bool
    order_ref(const Cmp& cmp, const Rank& rank,
              const Key& a, const Key& b)
    {
      for (dimension_type d = 0; d < rank(); ++d)
        {
          if (cmp(d, a, b)) return true;
          if (cmp(d, b, a)) return false;
        }
      return (&a < &b);
    }

    template <typename Cmp, typename Key>
    inline bool
    order_less(const Cmp& cmp, dimension_type set_dim,
               const Key& a, const Key& b)
    {
      for (dimension_type d = 0; d <= set_dim; ++d)
        {
          if (cmp(d, a, b)) return true;
          if (cmp(d, b, a)) return false;
        }
      return false;
    }

    //! Specialization for iterators pointing to node using the relaxed
    //! invariant.
    //! \see increment<Container>(typename ordered<Container>::iterator&)
    template <typename Container>
    inline ordered_iterator<Container>&
    increment_ordered(ordered_iterator<Container>& iter,
                      relaxed_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      // Walk the tree in both directions: one step left, one step right, etc..
      dimension_type set_dim = 0; // number of values correctly positionned
      node_ptr best = 0;
      dimension_type best_dim = 0;
      node_ptr l_node, r_node; l_node = r_node = iter.node;
      dimension_type l_dim, r_dim; l_dim = r_dim = iter.node_dim;
      bool left_ended = false, right_ended = false;
      do
        {
          if (!left_ended)
            {
              if (l_node->left != 0
                  && (l_dim > set_dim
                      || !cmp(l_dim, const_key(l_node), const_key(iter.node))))
                {
                  l_node = l_node->left;
                  l_dim = incr_dim(rank, l_dim);
                  while (l_node->right != 0
                         && (l_dim > set_dim
                             || best == 0 || !cmp(l_dim, const_key(best),
                                                  const_key(l_node))))
                    { l_node = l_node->right; l_dim = incr_dim(rank, l_dim); }
                  if (order_ref(cmp, rank, const_key(iter.node),
                                const_key(l_node))
                      && (best == 0
                          || order_ref(cmp, rank, const_key(l_node),
                                       const_key(best))))
                    { best = l_node; best_dim = l_dim; }
                }
              else
                {
                  node_ptr p = l_node->parent;
                  while (!header(p) && p->left == l_node)
                    {
                      l_node = p;
                      l_dim = decr_dim(rank, l_dim);
                      p = l_node->parent;
                    }
                  l_node = p;
                  l_dim = decr_dim(rank, l_dim);
                  if (!header(l_node))
                    {
                      if (order_ref(cmp, rank, const_key(iter.node),
                                    const_key(l_node))
                          && (best == 0
                              || order_ref(cmp, rank, const_key(l_node),
                                           const_key(best))))
                        { best = l_node; best_dim = l_dim; }
                    }
                  else left_ended = true;
                }
            }
          if (!right_ended)
            {
              if (r_node->right != 0
                  && (r_dim > set_dim
                      || best == 0
                      || !cmp(r_dim, const_key(best), const_key(r_node))))
                {
                  r_node = r_node->right;
                  r_dim = incr_dim(rank, r_dim);
                  while (r_node->left != 0
                         && (r_dim > set_dim
                             || !cmp(r_dim, const_key(r_node),
                                     const_key(iter.node))))
                    { r_node = r_node->left; r_dim = incr_dim(rank, r_dim); }
                  if (order_ref(cmp, rank, const_key(iter.node),
                                const_key(r_node))
                      && (best == 0
                          || order_ref(cmp, rank, const_key(r_node),
                                       const_key(best))))
                    { best = r_node; best_dim = r_dim; }
                  continue;
                }
              else
                {
                  node_ptr p = r_node->parent;
                  while (!header(p) && p->right == r_node)
                    {
                      r_node = p;
                      r_dim = decr_dim(rank, r_dim);
                      p = r_node->parent;
                    }
                  r_node = p;
                  r_dim = decr_dim(rank, r_dim);
                  if (!header(r_node))
                    {
                      if (order_ref(cmp, rank, const_key(iter.node),
                                    const_key(r_node))
                          && (best == 0
                              || order_ref(cmp, rank, const_key(r_node),
                                           const_key(best))))
                        { best = r_node; best_dim = r_dim; }
                      continue;
                    }
                  else right_ended = true;
                }
            }
          if (left_ended) // continue to jump over this!
            {
              // stepping is over in both directions...
              if (++set_dim == rank()) break;
              right_ended = false;
              left_ended = false;
              l_node = r_node = iter.node;
              l_dim = r_dim = iter.node_dim;
            }
        } while(true);
      if (best != 0)
        { iter.node = best; iter.node_dim = best_dim; }
      else { iter.node = r_node; iter.node_dim = r_dim; }
      SPATIAL_ASSERT_CHECK(r_dim == (rank() - 1));
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    //! Specialization for iterators pointing to node using the strict
    //! invariant.
    //! \see increment<Container>(typename ordered<Container>::iterator&)
    template <typename Container>
    inline ordered_iterator<Container>&
    increment_ordered(ordered_iterator<Container>& iter,
                      strict_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      // Walk the tree in both directions: one step left, one step right, etc..
      dimension_type set_dim = 0; // number of values correctly positionned
      node_ptr best = 0;
      dimension_type best_dim = 0;
      node_ptr l_node, r_node; l_node = r_node = iter.node;
      dimension_type l_dim, r_dim; l_dim = r_dim = iter.node_dim;
      bool left_ended = false, right_ended = false;
      do
        {
          if (!left_ended)
            {
              if (l_node->left != 0
                  && (l_dim > set_dim
                      // strict invarient optimization
                      || cmp(l_dim, const_key(iter.node), const_key(l_node))))
                {
                  l_node = l_node->left;
                  l_dim = incr_dim(rank, l_dim);
                  while (l_node->right != 0
                         && (l_dim > set_dim
                             || best == 0
                             || !cmp(l_dim, const_key(best),
                                     const_key(l_node))))
                    { l_node = l_node->right; l_dim = incr_dim(rank, l_dim); }
                  if (order_ref(cmp, rank, const_key(iter.node),
                                const_key(l_node))
                      && (best == 0
                          || order_ref(cmp, rank, const_key(l_node),
                                       const_key(best))))
                    { best = l_node; best_dim = l_dim; }
                }
              else
                {
                  node_ptr p = l_node->parent;
                  while (!header(p) && p->left == l_node)
                    {
                      l_node = p;
                      l_dim = decr_dim(rank, l_dim);
                      p = l_node->parent;
                    }
                  l_node = p;
                  l_dim = decr_dim(rank, l_dim);
                  if (!header(l_node))
                    {
                      if (order_ref(cmp, rank, const_key(iter.node),
                                    const_key(l_node))
                          && (best == 0
                              || order_ref(cmp, rank, const_key(l_node),
                                            const_key(best))))
                        { best = l_node; best_dim = l_dim; }
                    }
                  else left_ended = true;
                }
            }
          if(!right_ended)
            {
              if (r_node->right != 0
                  && (r_dim > set_dim
                      || best == 0
                      || !cmp(r_dim, const_key(best), const_key(r_node))))
                {
                  r_node = r_node->right;
                  r_dim = incr_dim(rank, r_dim);
                  while (r_node->left
                         && (r_dim > set_dim
                             // strict invarient optimization
                             || cmp(r_dim, const_key(iter.node),
                                    const_key(r_node))))
                    { r_node = r_node->left; r_dim = incr_dim(rank, r_dim); }
                  if (order_ref(cmp, rank, const_key(iter.node),
                                const_key(r_node))
                      && (best == 0
                          || order_ref(cmp, rank, const_key(r_node),
                                       const_key(best))))
                    { best = r_node; best_dim = r_dim; }
                  continue;
                }
              else
                {
                  node_ptr p = r_node->parent;
                  while (!header(p) && p->right == r_node)
                    {
                      r_node = p;
                      r_dim = decr_dim(rank, r_dim);
                      p = r_node->parent;
                    }
                  r_node = p;
                  r_dim = decr_dim(rank, r_dim);
                  if (!header(r_node))
                    {
                      if (order_ref(cmp, rank, const_key(iter.node),
                                    const_key(r_node))
                          && (best == 0
                              || order_ref(cmp, rank, const_key(r_node),
                                           const_key(best))))
                        { best = r_node; best_dim = r_dim; }
                      continue;
                    }
                  else right_ended = true;
                }
            }
          if (left_ended) // continue to jump over this!
            {
              // stepping is over in both directions...
              if (++set_dim == rank()) break;
              right_ended = false;
              left_ended = false;
              l_node = r_node = iter.node;
              l_dim = r_dim = iter.node_dim;
            }
        } while(true);
      if (best != 0)
        { iter.node = best; iter.node_dim = best_dim; }
      else { iter.node = r_node; iter.node_dim = r_dim; }
      SPATIAL_ASSERT_CHECK(r_dim == (rank() - 1));
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container>
    inline ordered_iterator<Container>&
    increment_ordered(ordered_iterator<Container>& iter)
    {
      return increment_ordered
        (iter, typename container_traits<Container>::mode_type
         ::invariant_category());
    }

    // The next largest key on the ordered dimension is likely to be found in
    // the children of the current best, so, descend into the children of node
    // first.
    template <typename Container>
    inline ordered_iterator<Container>&
    decrement_ordered(ordered_iterator<Container>& iter,
                      relaxed_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      if (header(iter.node))
        {
          iter.node = iter.node->parent;
          iter.node_dim = 0; // root is always compared on dimension 0
          return maximum_ordered(iter);
        }
      // Walk the tree in both directions: one step left, one step right, etc..
      dimension_type set_dim = 0; // number of values correctly positionned
      node_ptr best = 0;
      dimension_type best_dim = 0;
      node_ptr l_node, r_node; l_node = r_node = iter.node;
      dimension_type l_dim, r_dim; l_dim = r_dim = iter.node_dim;
      bool left_ended = false, right_ended = false;
      do
        {
          if (!left_ended)
            {
              if (l_node->left != 0
                  && (l_dim > set_dim
                      || best == 0
                      || !cmp(l_dim, const_key(l_node), const_key(best))))
                {
                  l_node = l_node->left;
                  l_dim = incr_dim(rank, l_dim);
                  while (l_node->right
                         && (l_dim > set_dim
                             || !cmp(l_dim, const_key(iter.node),
                                     const_key(l_node))))
                    { l_node = l_node->right; l_dim = incr_dim(rank, l_dim); }
                  if (order_ref(cmp, rank, const_key(l_node),
                                const_key(iter.node))
                      && (best == 0
                          || order_ref(cmp, rank, const_key(best),
                                       const_key(l_node))))
                    { best = l_node; best_dim = l_dim; }
                }
              else
                {
                  node_ptr p = l_node->parent;
                  while (!header(p) && p->left == l_node)
                    {
                      l_node = p;
                      l_dim = decr_dim(rank, l_dim);
                      p = l_node->parent;
                    }
                  l_node = p;
                  l_dim = decr_dim(rank, l_dim);
                  if (!header(l_node))
                    {
                      if (order_ref(cmp, rank, const_key(l_node),
                                    const_key(iter.node))
                          && (best == 0
                              || order_ref(cmp, rank, const_key(best),
                                           const_key(l_node))))
                        { best = l_node; best_dim = l_dim; }
                    }
                  else left_ended = true;
                }
            }
          if (!right_ended)
            {
              if (r_node->right != 0
                  && (r_dim > set_dim
                      || !cmp(r_dim, const_key(iter.node), const_key(r_node))))
                {
                  r_node = r_node->right;
                  r_dim = incr_dim(rank, r_dim);
                  while (r_node->left
                         && (r_dim > set_dim
                             || best == 0
                             || !cmp(r_dim, const_key(r_node), const_key(best))))
                    { r_node = r_node->left; r_dim = incr_dim(rank, r_dim); }
                  if (order_ref(cmp, rank, const_key(r_node),
                                const_key(iter.node))
                      && (best == 0
                          || order_ref(cmp, rank, const_key(best),
                                       const_key(r_node))))
                    { best = r_node; best_dim = r_dim; }
                  continue;
                }
              else
                {
                  node_ptr p = r_node->parent;
                  while (!header(p) && p->right == r_node)
                    {
                      r_node = p;
                      r_dim = decr_dim(rank, r_dim);
                      p = r_node->parent;
                    }
                  r_node = p;
                  r_dim = decr_dim(rank, r_dim);
                  if (!header(r_node))
                    {
                      if (order_ref(cmp, rank, const_key(r_node),
                                    const_key(iter.node))
                          && (best == 0
                              || order_ref(cmp, rank, const_key(best),
                                           const_key(r_node))))
                        { best = r_node; best_dim = r_dim; }
                      continue;
                    }
                  else right_ended = true;
                }
            }
          if (left_ended) // continue to jump over this!
            {
              // stepping is over in both directions...
              if (++set_dim == rank()) break;
              right_ended = false;
              left_ended = false;
              l_node = r_node = iter.node;
              l_dim = r_dim = iter.node_dim;
            }
        } while(true);
      if (best != 0)
        { iter.node = best; iter.node_dim = best_dim; }
      else { iter.node = r_node; iter.node_dim = r_dim; }
      SPATIAL_ASSERT_CHECK(r_dim == (rank() - 1));
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    // The next largest key on the ordered dimension is likely to be found in
    // the children of the current best, so, descend into the children of node
    // first.
    template <typename Container>
    inline ordered_iterator<Container>&
    decrement_ordered(ordered_iterator<Container>& iter,
                      strict_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      if (header(iter.node))
        {
          iter.node = iter.node->parent;
          iter.node_dim = 0; // root is always compared on dimension 0
          return maximum_ordered(iter);
        }
      // Walk the tree in both directions: one step left, one step right, etc..
      dimension_type set_dim = 0; // number of values correctly positionned
      node_ptr best = 0;
      dimension_type best_dim = 0;
      node_ptr l_node, r_node; l_node = r_node = iter.node;
      dimension_type l_dim, r_dim; l_dim = r_dim = iter.node_dim;
      bool left_ended = false, right_ended = false;
      do
        {
          if (!left_ended)
            {
              if (l_node->left != 0
                  && (l_dim > set_dim
                      || best == 0
                      // optimization for strict invarient
                      || cmp(l_dim, const_key(best), const_key(l_node))))
                {
                  l_node = l_node->left;
                  l_dim = incr_dim(rank, l_dim);
                  while (l_node->right
                         && (l_dim > set_dim
                             || !cmp(l_dim, const_key(iter.node),
                                     const_key(l_node))))
                    { l_node = l_node->right; l_dim = incr_dim(rank, l_dim); }
                  if (order_ref(cmp, rank, const_key(l_node),
                                const_key(iter.node))
                      && (best == 0
                          || order_ref(cmp, rank, const_key(best),
                                       const_key(l_node))))
                    { best = l_node; best_dim = l_dim; }
                }
              else
                {
                  node_ptr p = l_node->parent;
                  while (!header(p) && p->left == l_node)
                    {
                      l_node = p;
                      l_dim = decr_dim(rank, l_dim);
                      p = l_node->parent;
                    }
                  l_node = p;
                  l_dim = decr_dim(rank, l_dim);
                  if (!header(l_node))
                    {
                      if (order_ref(cmp, rank, const_key(l_node),
                                    const_key(iter.node))
                          && (best == 0
                              || order_ref(cmp, rank, const_key(best),
                                           const_key(l_node))))
                        { best = l_node; best_dim = l_dim; }
                    }
                  else left_ended = true;
                }
            }
          if (!right_ended)
            {
              if (r_node->right
                  && (r_dim > set_dim
                      || !cmp(r_dim, const_key(iter.node), const_key(r_node))))
                {
                  r_node = r_node->right;
                  r_dim = incr_dim(rank, r_dim);
                  while (r_node->left
                         && (r_dim > set_dim
                             || best == 0
                             // optimization for strict invarient
                             || cmp(r_dim, const_key(best), const_key(r_node))))
                    { r_node = r_node->left; r_dim = incr_dim(rank, r_dim); }
                  if (order_ref(cmp, rank, const_key(r_node),
                                const_key(iter.node))
                      && (best == 0
                          || order_ref(cmp, rank, const_key(best),
                                       const_key(r_node))))
                    { best = r_node; best_dim = r_dim; }
                  continue;
                }
              else
                {
                  node_ptr p = r_node->parent;
                  while (!header(p) && p->right == r_node)
                    {
                      r_node = p;
                      r_dim = decr_dim(rank, r_dim);
                      p = r_node->parent;
                    }
                  r_node = p;
                  r_dim = decr_dim(rank, r_dim);
                  if (!header(r_node))
                    {
                      if (order_ref(cmp, rank, const_key(r_node),
                                    const_key(iter.node))
                          && (best == 0
                              || order_ref(cmp, rank, const_key(best),
                                           const_key(r_node))))
                        { best = r_node; best_dim = r_dim; }
                      continue;
                    }
                  else right_ended = true;
                }
            }
          if (left_ended) // continue to jump over this!
            {
              // stepping is over in both directions...
              if (++set_dim == rank()) break;
              right_ended = false;
              left_ended = false;
              l_node = r_node = iter.node;
              l_dim = r_dim = iter.node_dim;
            }
        } while(true);
      if (best != 0)
        { iter.node = best; iter.node_dim = best_dim; }
      else { iter.node = r_node; iter.node_dim = r_dim; }
      SPATIAL_ASSERT_CHECK(r_dim == (rank() - 1));
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container>
    inline ordered_iterator<Container>&
    decrement_ordered(ordered_iterator<Container>& iter)
    {
      return decrement_ordered
        (iter, typename container_traits<Container>::mode_type
         ::invariant_category());
    }

    // Find the minimum from node and stop when reaching the parent. Iterate in
    // left-first fashion.
    template <typename Container>
    inline ordered_iterator<Container>&
    minimum_ordered(ordered_iterator<Container>& iter)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      node_ptr end = iter.node->parent;
      dimension_type set_dim = 0;
      while (iter.node->left != 0)
        {
          iter.node = iter.node->left;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      node_ptr best = iter.node;
      dimension_type best_dim = iter.node_dim;
      do
        {
          node_ptr node = iter.node;
          dimension_type node_dim = iter.node_dim;
          do
            {
              if (node->right != 0
                  && (node_dim > set_dim
                      || !cmp(node_dim, const_key(best), const_key(node))))
                {
                  node = node->right;
                  node_dim = incr_dim(rank, node_dim);
                  while (node->left != 0)
                    { node = node->left; node_dim = incr_dim(rank, node_dim); }
                  if (order_ref(cmp, rank,
                                const_key(node), const_key(best)))
                    { best = node; best_dim = node_dim; }
                }
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
                  if (node != end
                      && order_ref(cmp, rank,
                                   const_key(node), const_key(best)))
                    { best = node; best_dim = node_dim; }
                }
            } while (node != end);
        } while (++set_dim < rank());
      iter.node = best; iter.node_dim = best_dim;
      SPATIAL_ASSERT_CHECK(best_dim < rank());
      SPATIAL_ASSERT_CHECK(best != 0);
      SPATIAL_ASSERT_CHECK(!header(best));
      return iter;
    }

    //! Specialization for iterators pointed to node using the relaxed
    //! invariant.
    //! \see maximum<Container>(typename ordered<Container>::iterator&)
    template<typename Container>
    inline ordered_iterator<Container>&
    maximum_ordered(ordered_iterator<Container>& iter,
                    relaxed_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      node_ptr end = iter.node->parent;
      dimension_type set_dim = 0;
      while (iter.node->right != 0)
        {
          iter.node = iter.node->right;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      node_ptr best = iter.node;
      dimension_type best_dim = iter.node_dim;
      do
        {
          node_ptr node = iter.node;
          dimension_type node_dim = iter.node_dim;
          do
            {
              if (node->left != 0
                  && (node_dim > set_dim
                      || !cmp(node_dim, const_key(node), const_key(best))))
                {
                  node = node->left;
                  node_dim = incr_dim(rank, node_dim);
                  while (node->right != 0)
                    { node = node->right; node_dim = incr_dim(rank, node_dim); }
                  if (order_ref(cmp, rank,
                                const_key(best), const_key(node)))
                    { best = node; best_dim = node_dim; }
                }
              else
                {
                  node_ptr p = node->parent;
                  while (p != end && p->left == node)
                    {
                      node = p;
                      node_dim = decr_dim(rank, node_dim);
                      p = node->parent;
                    }
                  node = p;
                  node_dim = decr_dim(rank, node_dim);
                  if (node != end
                      && order_ref(cmp, rank, const_key(best),
                                   const_key(node)))
                    { best = node; best_dim = node_dim; }
                }
            } while (node != end);
        } while (++set_dim < rank());
      iter.node = best; iter.node_dim = best_dim;
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      return iter;
    }

    //! Specialization for iterators pointed to node using the strict
    //! invariant.
    //! \see maximum<Container>(typename ordered_iterator<Container>&)
    template<typename Container>
    inline ordered_iterator<Container>&
    maximum_ordered(ordered_iterator<Container>& iter,
                    strict_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      node_ptr end = iter.node->parent;
      dimension_type set_dim = 0;
      while (iter.node->right != 0)
        {
          iter.node = iter.node->right;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      node_ptr best = iter.node;
      dimension_type best_dim = iter.node_dim;
      do
        {
          node_ptr node = iter.node;
          dimension_type node_dim = iter.node_dim;
          do
            {
              // optimization for strict invariant
              if (node->left != 0 && node_dim > set_dim)
                {
                  node = node->left;
                  node_dim = incr_dim(rank, node_dim);
                  while (node->right != 0)
                    { node = node->right; node_dim = incr_dim(rank, node_dim); }
                  if (order_ref(cmp, rank,
                                const_key(best), const_key(node)))
                    { best = node; best_dim = node_dim; }
                }
              else
                {
                  node_ptr p = node->parent;
                  while (p != end && p->left == node)
                    {
                      node = p;
                      node_dim = decr_dim(rank, node_dim);
                      p = node->parent;
                    }
                  node = p;
                  node_dim = decr_dim(rank, node_dim);
                  if (node != end
                      && order_ref(cmp, rank,
                                   const_key(best), const_key(node)))
                    { best = node; best_dim = node_dim; }
                }
            } while (node != end);
        } while (++set_dim < rank());
      iter.node = best; iter.node_dim = best_dim;
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      return iter;
    }

    template <typename Container>
    inline ordered_iterator<Container>&
    maximum_ordered(ordered_iterator<Container>& iter)
    {
      return maximum_ordered
        (iter, typename container_traits<Container>::mode_type
         ::invariant_category());
    }

    //! Specialization for iterators pointed to node using the relaxed
    //! invariant.
    template<typename Container>
    inline ordered_iterator<Container>&
    lower_bound_ordered
    (ordered_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound,
     relaxed_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      node_ptr end = iter.node->parent;
      dimension_type set_dim = 0;
      node_ptr best = 0;
      dimension_type best_dim = 0;
      node_ptr node;
      dimension_type node_dim;
      do
        {
          node = iter.node;
          node_dim = iter.node_dim;
          while (node->left != 0
                 && (node_dim > set_dim
                     || !cmp(node_dim, const_key(node), bound)))
            { node = node->left; node_dim = incr_dim(rank, node_dim); }
          if (!order_less(cmp, set_dim, const_key(node), bound))
            { best = node; best_dim = node_dim; }
          do
            {
              if (node->right != 0
                  && (node_dim > set_dim || best == 0
                      || !cmp(node_dim, const_key(best), const_key(node))))
                {
                  node = node->right;
                  node_dim = incr_dim(rank, node_dim);
                  while (node->left != 0
                         && (node_dim > set_dim
                             || !cmp(node_dim, const_key(node), bound)))
                    {
                      node = node->left;
                      node_dim = incr_dim(rank, node_dim);
                    }
                  if (!order_less(cmp, set_dim, const_key(node), bound)
                      && (best == 0
                          || order_ref(cmp, rank,
                                       const_key(node), const_key(best))))
                    { best = node; best_dim = node_dim; }
                }
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
                  if (node != end
                      && !order_less(cmp, set_dim, const_key(node), bound)
                      && (best == 0 || order_ref(cmp, rank, const_key(node),
                                                 const_key(best))))
                    { best = node; best_dim = node_dim; }
                }
            } while (node != end);
        } while (++set_dim < rank());
      if (best != 0) { iter.node = best; iter.node_dim = best_dim; }
      else { iter.node = node; iter.node_dim = node_dim; }
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    //! Specialization for iterators pointed to node using the strict
    //! invariant.
    template<typename Container>
    inline ordered_iterator<Container>&
    lower_bound_ordered
    (ordered_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound,
     strict_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      node_ptr end = iter.node->parent;
      dimension_type set_dim = 0;
      node_ptr best = 0;
      dimension_type best_dim = 0;
      node_ptr node;
      dimension_type node_dim;
      do
        {
          node = iter.node;
          node_dim = iter.node_dim;
          while (node->left != 0
                 && (node_dim > set_dim
                     // optimization for strict invarient
                     || cmp(node_dim, bound, const_key(node))))
            { node = node->left; node_dim = incr_dim(rank, node_dim); }
          if (!order_less(cmp, set_dim, const_key(node), bound))
            { best = node; best_dim = node_dim; }
          do
            {
              if (node->right != 0
                  && (node_dim > set_dim || best == 0
                      || !cmp(node_dim, const_key(best), const_key(node))))
                {
                  node = node->right;
                  node_dim = incr_dim(rank, node_dim);
                  while (node->left != 0
                         && (node_dim > set_dim
                             // optimization for strict invarient
                             || cmp(node_dim, bound, const_key(node))))
                    {
                      node = node->left;
                      node_dim = incr_dim(rank, node_dim);
                    }
                  if (!order_less(cmp, set_dim, const_key(node), bound)
                      && (best == 0
                          || order_ref(cmp, rank,
                                       const_key(node), const_key(best))))
                    { best = node; best_dim = node_dim; }
                }
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
                  if (node != end
                      && !order_less(cmp, set_dim, const_key(node), bound)
                      && (best == 0 || order_ref(cmp, rank, const_key(node),
                                                 const_key(best))))
                    { best = node; best_dim = node_dim; }
                }
            } while (node != end);
        } while (++set_dim < rank());
      if (best != 0) { iter.node = best; iter.node_dim = best_dim; }
      else { iter.node = node; iter.node_dim = node_dim; }
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container>
    inline ordered_iterator<Container>&
    lower_bound_ordered(ordered_iterator<Container>& iter,
                        const typename container_traits<Container>::key_type&
                        bound)
    {
      return lower_bound_ordered
        (iter, bound, typename container_traits<Container>::mode_type
         ::invariant_category());
    }

    // Walk tree nodes in right-first fashion, bouncing off values that are
    // higher than key.
    template <typename Container>
    inline ordered_iterator<Container>&
    upper_bound_ordered
    (ordered_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound,
     relaxed_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      node_ptr end = iter.node->parent;
      dimension_type set_dim = 0;
      node_ptr best = 0;
      dimension_type best_dim = 0;
      node_ptr node;
      dimension_type node_dim;
      do
        {
          node = iter.node;
          node_dim = iter.node_dim;
          while (node->left != 0
                 && (node_dim > set_dim
                     || !cmp(node_dim, const_key(node), bound)))
            { node = node->left; node_dim = incr_dim(rank, node_dim); }
          if (order_less(cmp, set_dim, bound, const_key(node)))
            { best = node; best_dim = node_dim; }
          do
            {
              if (node->right != 0
                  && (node_dim > set_dim || best == 0
                      || !cmp(node_dim, const_key(best), const_key(node))))
                {
                  node = node->right;
                  node_dim = incr_dim(rank, node_dim);
                  while (node->left != 0
                         && (node_dim > set_dim
                             || !cmp(node_dim, const_key(node), bound)))
                    {
                      node = node->left;
                      node_dim = incr_dim(rank, node_dim);
                    }
                  if (order_less(cmp, set_dim, bound, const_key(node))
                      && (best == 0
                          || order_ref(cmp, rank,
                                       const_key(node), const_key(best))))
                    { best = node; best_dim = node_dim; }
                }
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
                  if (node != end
                      && order_less(cmp, set_dim, bound, const_key(node))
                      && (best == 0
                          || order_ref(cmp, rank,
                                       const_key(node), const_key(best))))
                    { best = node; best_dim = node_dim; }
                }
            } while (node != end);
        } while (++set_dim < rank());
      if (best != 0) { iter.node = best; iter.node_dim = best_dim; }
      else { iter.node = node; iter.node_dim = node_dim; }
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    // Walk tree nodes in right-first fashion, bouncing off values that are
    // higher than key.
    template <typename Container>
    inline ordered_iterator<Container>&
    upper_bound_ordered
    (ordered_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound,
     strict_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      node_ptr end = iter.node->parent;
      dimension_type set_dim = 0;
      node_ptr best = 0;
      dimension_type best_dim = 0;
      node_ptr node;
      dimension_type node_dim;
      do
        {
          node = iter.node;
          node_dim = iter.node_dim;
          while (node->left != 0
                 && (node_dim > set_dim
                     // Optimization for strict invarient
                     || cmp(node_dim, bound, const_key(node))))
            { node = node->left; node_dim = incr_dim(rank, node_dim); }
          if (order_less(cmp, set_dim, bound, const_key(node)))
            { best = node; best_dim = node_dim; }
          do
            {
              if (node->right != 0
                  && (node_dim > set_dim || best == 0
                      || !cmp(node_dim, const_key(best), const_key(node))))
                {
                  node = node->right;
                  node_dim = incr_dim(rank, node_dim);
                  while (node->left != 0
                         && (node_dim > set_dim
                             // Optimization for strict invarient
                             || cmp(node_dim, bound, const_key(node))))
                    {
                      node = node->left;
                      node_dim = incr_dim(rank, node_dim);
                    }
                  if (order_less(cmp, set_dim, bound, const_key(node))
                      && (best == 0
                          || order_ref(cmp, rank,
                                       const_key(node), const_key(best))))
                    { best = node; best_dim = node_dim; }
                }
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
                  if (node != end
                      && order_less(cmp, set_dim, bound, const_key(node))
                      && (best == 0
                          || order_ref(cmp, rank,
                                       const_key(node), const_key(best))))
                    { best = node; best_dim = node_dim; }
                }
            } while (node != end);
        } while (++set_dim < rank());
      if (best != 0) { iter.node = best; iter.node_dim = best_dim; }
      else { iter.node = node; iter.node_dim = node_dim; }
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container>
    inline ordered_iterator<Container>&
    upper_bound_ordered(ordered_iterator<Container>& iter,
                        const typename container_traits<Container>::key_type&
                        bound)
    {
      return upper_bound_ordered
        (iter, bound, typename container_traits<Container>::mode_type
         ::invariant_category());
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_ORDERED_TPP
