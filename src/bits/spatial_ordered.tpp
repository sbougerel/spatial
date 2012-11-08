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
    template <typename Cmp, typename Key>
    inline bool
    less_order(const Cmp& cmp, dimension_type set_dim,
               const Key& a, const Key& b)
    {
      for (dimension_type d = 0; d <= set_dim; ++d)
        { if (cmp(d, b, a)) return false; }
      return (cmp(set_dim, a, b) || (&a < &b));
    }

    //! Specialization for iterators pointing to node using the relaxed
    //! invariant.
    //! \see increment<Container>(typename ordered<Container>::iterator&)
    template <typename Container>
    inline ordered_iterator<Container>&
    increment_ordered(ordered_iterator<Container>& iter,
                      details::relaxed_invariant_tag)
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
      node_ptr best = 0, end = 0;
      dimension_type best_dim = 0;
      node_ptr l_node, r_node; l_node = r_node = iter.node;
      dimension_type l_dim, r_dim; l_dim = r_dim = iter.node_dim;
      bool left_ended = false, right_ended = false;
      do
        {
          if (!left_ended)
            {
              if (l_node->left
                  && (l_dim > set_dim
                      || !cmp(l_dim, const_key(l_node), const_key(iter.node))))
                {
                  l_node = l_node->left;
                  l_dim = incr(rank, l_dim);
                  while (l_node->right
                         && (l_dim > set_dim
                             || best == 0 || !cmp(l_dim, const_key(best),
                                                  const_key(l_node))))
                    { l_node = l_node->right; l_dim = incr(rank, l_dim); }
                  if (best == 0
                      || less_order(cmp, set_dim, const_key(l_node),
                                    const_key(best)))
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
                      if (best == 0
                          || less_order(cmp, set_dim, const_key(l_node),
                                        const_key(best)))
                        { best = l_node; best_dim = l_dim; }
                    }
                  else left_ended = true;
                }
            }
          switch(right_ended)
            {
            case false:
              if (r_node->right
                  && (r_dim > set_dim
                      || best == 0
                      || !cmp(r_dim, const_key(best), const_key(r_node))))
                {
                  r_node = r_node->right;
                  r_dim = incr(rank, r_dim);
                  while (r_node->left
                         && (r_dim > set_dim
                             || !cmp(r_dim, const_key(r_node),
                                     const_key(iter.node))))
                    { r_node = r_node->left; r_dim = incr(rank, r_dim); }
                  if (best == 0
                      || less_order(cmp, set_dim, const_key(l_node),
                                    const_key(best)))
                    { best = l_node; best_dim = l_dim; }
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
                      if (best == 0
                          || less_order(cmp, set_dim, const_key(l_node),
                                        const_key(best)))
                        { best = l_node; best_dim = l_dim; }
                      // break: only when stepping right is not over...
                      break;
                    }
                  // no break: if stepping left is also over...
                  else right_ended = true;
                }
            case true:
              if (left_ended)
                {
                  // stepping is over in both directions...
                  ++set_dim;
                  if (set_dim == rank())
                    {
                      if (best != 0)
                        { iter.node = best; iter.node_dim = best_dim; }
                      else { iter.node = r_node; iter.node_dim = r_dim; }
                      SPATIAL_ASSERT_CHECK(r_dim == (rank() - 1));
                      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
                      SPATIAL_ASSERT_CHECK(iter.node != 0);
                      return iter;
                    }
                  right_ended = false;
                  left_ended = false;
                  l_node = r_node = iter.node;
                  l_dim = r_dim = iter.node_dim;
                }
            }
        } while(true);
    }

    //! Specialization for iterators pointing to node using the strict
    //! invariant.
    //! \see increment<Container>(typename ordered<Container>::iterator&)
    template <typename Container>
    inline ordered_iterator<Container>&
    increment_ordered(ordered_iterator<Container>& iter,
                      details::strict_invariant_tag)
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
      node_ptr best = 0, end = 0;
      dimension_type best_dim = 0;
      node_ptr l_node, r_node; l_node = r_node = iter.node;
      dimension_type l_dim, r_dim; l_dim = r_dim = iter.node_dim;
      bool left_ended = false, right_ended = false;
      do
        {
          if (!left_ended)
            {
              if (l_node->left
                  && (l_dim <= set_dim
                      // strict invarient optimization
                      || cmp(l_dim, const_key(iter.node), const_key(l_node))))
                {
                  l_node = l_node->left;
                  l_dim = incr(rank, l_dim);
                  while (l_node->right
                         && (l_dim > set_dim
                             || best == 0
                             || !cmp(l_dim, const_key(best),
                                     const_key(l_node))))
                    { l_node = l_node->right; l_dim = incr(rank, l_dim); }
                  if (best == 0
                      || less_order(cmp, set_dim, const_key(l_node),
                                    const_key(best)))
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
                      if (best == 0
                          || less_order(cmp, set_dim, const_key(l_node),
                                        const_key(best)))
                        { best = l_node; best_dim = l_dim; }
                    }
                  else left_ended = true;
                }
            }
          switch(right_ended)
            {
            case false:
              if (r_node->right
                  && (r_dim > set_dim
                      || best == 0
                      || !cmp(r_dim, const_key(best), const_key(r_node))))
                {
                  r_node = r_node->right;
                  r_dim = incr(rank, r_dim);
                  while (r_node->left
                         && (r_dim > set_dim
                             // strict invarient optimization
                             || cmp(r_dim, const_key(iter.node),
                                    const_key(r_node))))
                    { r_node = r_node->left; r_dim = incr(rank, r_dim); }
                  if (best == 0
                      || less_order(cmp, set_dim, const_key(l_node),
                                    const_key(best)))
                    { best = l_node; best_dim = l_dim; }
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
                      if (best == 0
                          || less_order(cmp, set_dim, const_key(l_node),
                                        const_key(best)))
                        { best = l_node; best_dim = l_dim; }
                      // break: only when stepping right is not over...
                      break;
                    }
                  // no break: if stepping left is also over...
                  else right_ended = true;
                }
            case true:
              if (left_ended)
                {
                  // stepping is over in both directions...
                  ++set_dim;
                  if (set_dim == rank())
                    {
                      if (best != 0)
                        { iter.node = best; iter.node_dim = best_dim; }
                      else { iter.node = r_node; iter.node_dim = r_dim; }
                      SPATIAL_ASSERT_CHECK(r_dim == (rank() - 1));
                      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
                      SPATIAL_ASSERT_CHECK(iter.node != 0);
                      return iter;
                    }
                  right_ended = false;
                  left_ended = false;
                  l_node = r_node = iter.node;
                  l_dim = r_dim = iter.node_dim;
                }
            }
        } while(true);
    }

    //! Specialization for iterators pointed to node using the relaxed
    //! invariant.
    //! \see maximum<Container>(typename ordered<Container>::iterator&)
    template<typename Container>
    inline ordered_iterator<Container>&
    maximum_ordered(ordered_iterator<Container>& iter,
                    details::relaxed_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      dimension_type set_dim = 0;
      while (iter.node->right != 0)
        {
          iter.node = iter.node->right;
          iter.node_dim = incr(rank, iter.node_dim);
        }
      node_ptr best = iter, end = iter->parent;
      dimension_type best_dim = iter.node_dim;
      while (iter.node != end)
        {
          if (iter.node->left != 0
              && (iter.node_dim > set_dim
                  || !cmp(iter.node_dim, const_key(iter.node),
                          const_key(best))))
            {
              iter.node = iter.node->left;
              iter.node_dim = incr(rank, iter.node_dim);
              while (iter.node->right)
                {
                  iter.node = iter.node->right;
                  iter.node_dim = incr(rank, iter.node_dim);
                }
              if (less_order(cmp, set_dim, const_key(best),
                             const_key(iter.node)))
                { best = iter.node; best_dim = iter.node_dim; }
            }
          else
            {
              node_ptr p = iter.node->parent;
              while (p != end && p->left == iter.node)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
              if (iter.node != end
                  && less_order(cmp, set_dim, const_key(best),
                                const_key(iter.node)))
                { best = iter.node; best_dim = iter.node_dim; }
            }
        }
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
                    details::strict_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));


      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      return iter;
    }

    //! Specialization for iterators pointed to node using the relaxed
    //! invariant.
    template<typename Container>
    inline ordered_iterator<Container>&
    lower_bound_ordered
    (ordered_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound,
     details::relaxed_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));


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
     details::strict_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));


      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container>
    inline ordered_iterator<Container>&
    increment_ordered(ordered_iterator<Container>& iter)
    {
      return ::spatial::details::increment_ordered
        (iter, typename container_traits<Container>::mode_type
         ::invariant_category());
    }

    // The next largest key on the ordered dimension is likely to be found in the
    // children of the current best, so, descend into the children of node first.
    template <typename Container>
    inline ordered_iterator<Container>&
    decrement_ordered(ordered_iterator<Container>& iter)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());


      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
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
      node_ptr best = 0;
      dimension_type best_dim = 0;


      SPATIAL_ASSERT_CHECK(best_dim < rank());
      SPATIAL_ASSERT_CHECK(best != 0);
      SPATIAL_ASSERT_CHECK(!header(best));
      iter.node = best; iter.node_dim = best_dim;
      return iter;
    }

    template <typename Container>
    inline ordered_iterator<Container>&
    maximum_ordered(ordered_iterator<Container>& iter)
    {
      return ::spatial::details::maximum_ordered
        (iter, typename container_traits<Container>::mode_type
         ::invariant_category());
    }

    template <typename Container>
    inline ordered_iterator<Container>&
    lower_bound_ordered(ordered_iterator<Container>& iter,
                        const typename container_traits<Container>::key_type&
                        bound)
    {
      return ::spatial::details::lower_bound_ordered
        (iter, bound,
         typename container_traits<Container>::mode_type
         ::invariant_category());
    }

    // Walk tree nodes in right-first fashion, bouncing off values that are
    // higher than key.
    template <typename Container>
    inline ordered_iterator<Container>&
    upper_bound_ordered(ordered_iterator<Container>& iter,
                        const typename container_traits<Container>::key_type&
                        bound)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));


      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_ORDERED_TPP
