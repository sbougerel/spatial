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
    //! Specialization for iterators pointing to node using the relaxed
    //! invariant.
    //! \see increment<Container>(typename mapping<Container>::iterator&)
    template <typename Container>
    inline mapping_iterator<Container>&
    increment_mapping(mapping_iterator<Container>& iter,
                      details::relaxed_invariant_tag)
    {
      typedef typename mapping_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.mapping_dimension() < rank());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      node_ptr best = 0; // not null when best has been found
      node_ptr node = iter.node;
      dimension_type node_dim = iter.node_dim;
      dimension_type best_dim = iter.node_dim;
      if (node->left != 0)
        {
          do
            {
              node = node->left;
              node_dim = incr_dim(rank, node_dim);
            }
          while (node->left != 0
                 && (node_dim != iter.mapping_dimension()
                     || !cmp(node_dim,
                             const_key(node), const_key(iter.node))));
        }
      node_ptr ceiling = node->parent; // the upper limit of unvisited nodes
      bool sibling_visited = false;    // at ceiling, sibling node is visited
      if (less_by_ref(cmp, iter.mapping_dimension(),
                      const_key(iter.node), const_key(node)))
        { best = node; best_dim = node_dim; }
      do
        {
          if (node->right != 0
              && (node_dim != iter.mapping_dimension() || best == 0
                  || !cmp(node_dim, const_key(best), const_key(node))))
            {
              node = node->right;
              node_dim = incr_dim(rank, node_dim);
              while (node->left != 0
                     && (node_dim != iter.mapping_dimension()
                         || !cmp(node_dim,
                                 const_key(node), const_key(iter.node))))
                {
                  node = node->left;
                  node_dim = incr_dim(rank, node_dim);
                }
              if (less_by_ref(cmp, iter.mapping_dimension(),
                              const_key(iter.node), const_key(node))
                  && (best == 0
                      || less_by_ref(cmp, iter.mapping_dimension(),
                                     const_key(node), const_key(best))))
                { best = node; best_dim = node_dim; }
            }
          else
            {
              node_ptr p = node->parent;
              // go upward as long as we don't hit header or ceiling
              while (!header(p)
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
                      if (less_by_ref(cmp, iter.mapping_dimension(),
                                      const_key(iter.node), const_key(node))
                          && (best == 0
                              || less_by_ref(cmp, iter.mapping_dimension(),
                                             const_key(node),
                                             const_key(best))))
                        { best = node; best_dim = node_dim; }
                      sibling_visited = false;
                      ceiling = p;
                    }
                }
              if (!header(p) && p == ceiling)
                {
                  if (p->right == node) { node = p->left; }
                  else { node = p->right; }
                  sibling_visited = true;
                  // go to full left in unvisited sibling
                  while (node->left != 0
                         && (node_dim != iter.mapping_dimension()
                             || !cmp(node_dim, const_key(node),
                                     const_key(iter.node))))
                    {
                      node = node->left;
                      node_dim = incr_dim(rank, node_dim);
                    }
                  if (less_by_ref(cmp, iter.mapping_dimension(),
                                  const_key(iter.node), const_key(node))
                      && (best == 0
                          || less_by_ref(cmp, iter.mapping_dimension(),
                                         const_key(node), const_key(best))))
                    { best = node; best_dim = node_dim; }
                }
              else
                {
                  node = p;
                  node_dim = decr_dim(rank, node_dim);
                  if (!header(node))
                    {
                      if (less_by_ref(cmp, iter.mapping_dimension(),
                                      const_key(iter.node), const_key(node))
                          && (best == 0
                              || less_by_ref(cmp, iter.mapping_dimension(),
                                             const_key(node),
                                             const_key(best))))
                        { best = node; best_dim = node_dim; }
                    }
                }
            }
        }
      while (!header(node));
      if (best != 0) { iter.node = best; iter.node_dim = best_dim; }
      else { iter.node = node; iter.node_dim = node_dim; }
      SPATIAL_ASSERT_CHECK(header(node));
      SPATIAL_ASSERT_CHECK(node_dim == (rank() - 1));
      SPATIAL_ASSERT_CHECK(iter.mapping_dimension() < rank());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    //! Specialization for iterators pointing to node using the strict
    //! invariant.
    //! \see increment<Container>(typename mapping<Container>::iterator&)
    template <typename Container>
    inline mapping_iterator<Container>&
    increment_mapping(mapping_iterator<Container>& iter,
                      details::strict_invariant_tag)
    {
      typedef typename mapping_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.mapping_dimension() < rank());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      node_ptr best = 0; // not null when best has been found
      node_ptr node = iter.node;
      dimension_type node_dim = iter.node_dim;
      dimension_type best_dim = iter.node_dim;
      if (node->left != 0 && node_dim != iter.mapping_dimension()) // optimize
        {
          do
            {
              node = node->left;
              node_dim = incr_dim(rank, node_dim);
            }
          while (node->left != 0
                 && (node_dim != iter.mapping_dimension() // optimize
                     || cmp(node_dim, const_key(iter.node),
                            const_key(node))));
        }
      node_ptr ceiling = node->parent; // the upper limit of unvisited nodes
      bool sibling_visited = false;    // at ceiling, sibling node is visited
      if (less_by_ref(cmp, iter.mapping_dimension(), const_key(iter.node),
                      const_key(node)))
        { best = node; best_dim = node_dim; }
      do
        {
          if (node->right != 0
              && (node_dim != iter.mapping_dimension() || best == 0
                  || !cmp(node_dim, const_key(best), const_key(node))))
            {
              node = node->right;
              node_dim = incr_dim(rank, node_dim);
              while (node->left != 0
                     && (node_dim != iter.mapping_dimension()  // optimize
                         || cmp(node_dim,
                                const_key(iter.node), const_key(node))))
                {
                  node = node->left;
                  node_dim = incr_dim(rank, node_dim);
                }
              if (less_by_ref(cmp, iter.mapping_dimension(),
                              const_key(iter.node), const_key(node))
                  && (best == 0
                      || less_by_ref(cmp, iter.mapping_dimension(),
                                     const_key(node), const_key(best))))
                { best = node; best_dim = node_dim; }
            }
          else
            {
              node_ptr p = node->parent;
              // go upward as long as we don't hit header or ceiling
              while (!header(p)
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
                      if (less_by_ref(cmp, iter.mapping_dimension(),
                                      const_key(iter.node), const_key(node))
                          && (best == 0
                              || less_by_ref(cmp, iter.mapping_dimension(),
                                             const_key(node),
                                             const_key(best))))
                        { best = node; best_dim = node_dim; }
                      sibling_visited = false;
                      ceiling = p;
                    }
                }
              if (!header(p) && p == ceiling)
                {
                  if (p->right == node) { node = p->left; }
                  else { node = p->right; }
                  sibling_visited = true;
                  // go to full left in unvisited sibling
                  while (node->left != 0
                         && (node_dim != iter.mapping_dimension() // optimize
                             || cmp(node_dim,
                                    const_key(iter.node), const_key(node))))
                    {
                      node = node->left;
                      node_dim = incr_dim(rank, node_dim);
                    }
                  if (less_by_ref(cmp, iter.mapping_dimension(),
                                  const_key(iter.node), const_key(node))
                      && (best == 0
                          || less_by_ref(cmp, iter.mapping_dimension(),
                                         const_key(node), const_key(best))))
                    { best = node; best_dim = node_dim; }
                }
              else
                {
                  node = p;
                  node_dim = decr_dim(rank, node_dim);
                  if (!header(node))
                    {
                      if (less_by_ref(cmp, iter.mapping_dimension(),
                                      const_key(iter.node), const_key(node))
                          && (best == 0
                              || less_by_ref(cmp, iter.mapping_dimension(),
                                             const_key(node),
                                             const_key(best))))
                        { best = node; best_dim = node_dim; }
                    }
                }
            }
        }
      while (!header(node));
      if (best != 0) { iter.node = best; iter.node_dim = best_dim; }
      else { iter.node = node; iter.node_dim = node_dim; }
      SPATIAL_ASSERT_CHECK(header(node));
      SPATIAL_ASSERT_CHECK(node_dim == (rank() - 1));
      SPATIAL_ASSERT_CHECK(iter.mapping_dimension() < rank());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    //! Specialization for iterators pointed to node using the relaxed
    //! invariant.
    //! \see maximum<Container>(typename mapping<Container>::iterator&)
    template<typename Container>
    inline mapping_iterator<Container>&
    maximum_mapping(mapping_iterator<Container>& iter,
                    details::relaxed_invariant_tag)
    {
      typedef typename mapping_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.mapping_dimension() < rank());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      node_ptr end = iter.node->parent;
      while (iter.node->right != 0)
        {
          iter.node = iter.node->right;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      node_ptr best = iter.node;
      dimension_type best_dim = iter.node_dim;
      do
        {
          if (iter.node->left != 0
              && (iter.node_dim != iter.mapping_dimension()
                  || !cmp(iter.mapping_dimension(),
                          const_key(iter.node), const_key(best))))
            {
              iter.node = iter.node->left;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->right != 0)
                {
                  iter.node = iter.node->right;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
              if (less_by_ref(cmp, iter.mapping_dimension(),
                              const_key(best), const_key(iter.node)))
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
                  && less_by_ref(cmp, iter.mapping_dimension(),
                                 const_key(best), const_key(iter.node)))
                { best = iter.node; best_dim = iter.node_dim; }
            }
        }
      while (iter.node != end);
      iter.node = best; iter.node_dim = best_dim;
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      return iter;
    }

    //! Specialization for iterators pointed to node using the strict
    //! invariant.
    //! \see maximum<Container>(typename mapping_iterator<Container>&)
    template<typename Container>
    inline mapping_iterator<Container>&
    maximum_mapping(mapping_iterator<Container>& iter,
                    details::strict_invariant_tag)
    {
      typedef typename mapping_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.mapping_dimension() < rank());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      node_ptr end = iter.node->parent;
      while (iter.node->right != 0)
        {
          iter.node = iter.node->right;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      node_ptr best = iter.node;
      dimension_type best_dim = iter.node_dim;
      do
        {
          if (iter.node->left != 0
              && iter.node_dim != iter.mapping_dimension()) // optimize
            {
              iter.node = iter.node->left;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->right != 0)
                {
                  iter.node = iter.node->right;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
              if (less_by_ref(cmp, iter.mapping_dimension(),
                              const_key(best), const_key(iter.node)))
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
                  && less_by_ref(cmp, iter.mapping_dimension(),
                                 const_key(best), const_key(iter.node)))
                { best = iter.node; best_dim = iter.node_dim; }
            }
        }
      while (iter.node != end);
      iter.node = best; iter.node_dim = best_dim;
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      return iter;
    }

    //! Specialization for iterators pointed to node using the relaxed
    //! invariant.
    template<typename Container>
    inline mapping_iterator<Container>&
    lower_bound_mapping
    (mapping_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound,
     details::relaxed_invariant_tag)
    {
      typedef typename mapping_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.mapping_dimension() < rank());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      node_ptr end = iter.node->parent;
      node_ptr best = 0;
      dimension_type best_dim = 0;
      while (iter.node->left != 0
             && (iter.node_dim != iter.mapping_dimension()
                 || !cmp(iter.node_dim, const_key(iter.node), bound)))
        {
          iter.node = iter.node->left;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      if (!cmp(iter.mapping_dimension(), const_key(iter.node), bound))
        { best = iter.node; best_dim = iter.node_dim; }
      do
        {
          if (iter.node->right != 0
              && (iter.node_dim != iter.mapping_dimension() || best == 0
                  || !cmp(iter.node_dim, const_key(best),
                          const_key(iter.node))))
            {
              iter.node = iter.node->right;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->left != 0
                     && (iter.node_dim != iter.mapping_dimension()
                         || !cmp(iter.node_dim, const_key(iter.node), bound)))
                {
                  iter.node = iter.node->left;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
              if (!cmp(iter.mapping_dimension(), const_key(iter.node), bound)
                  && (best == 0
                      || less_by_ref(cmp, iter.mapping_dimension(),
                                     const_key(iter.node), const_key(best))))
                { best = iter.node; best_dim = iter.node_dim; }
            }
          else
            {
              node_ptr p = iter.node->parent;
              while (p != end && p->right == iter.node)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
              if (iter.node != end
                  && !cmp(iter.mapping_dimension(), const_key(iter.node), bound)
                  && (best == 0
                      || less_by_ref(cmp, iter.mapping_dimension(),
                                     const_key(iter.node), const_key(best))))
                { best = iter.node; best_dim = iter.node_dim; }
            }
        }
      while (iter.node != end);
      if (best != 0) { iter.node = best; iter.node_dim = best_dim; }
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    //! Specialization for iterators pointed to node using the strict
    //! invariant.
    template<typename Container>
    inline mapping_iterator<Container>&
    lower_bound_mapping
    (mapping_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound,
     details::strict_invariant_tag)
    {
      typedef typename mapping_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.mapping_dimension() < rank());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      node_ptr end = iter.node->parent;
      node_ptr best = 0;
      dimension_type best_dim = 0;
      while (iter.node->left != 0
             && (iter.node_dim != iter.mapping_dimension() // optimize
                 || cmp(iter.node_dim, bound, const_key(iter.node))))
        {
          iter.node = iter.node->left;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      if (!cmp(iter.mapping_dimension(), const_key(iter.node), bound))
        { best = iter.node; best_dim = iter.node_dim; }
      do
        {
          if (iter.node->right != 0
              && (iter.node_dim != iter.mapping_dimension() || best == 0
                  || !cmp(iter.mapping_dimension(),
                          const_key(best), const_key(iter.node))))
            {
              iter.node = iter.node->right;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->left != 0 // optimize
                     && (iter.node_dim != iter.mapping_dimension()
                         || cmp(iter.node_dim, bound,
                                const_key(iter.node))))
                {
                  iter.node = iter.node->left;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
              if (!cmp(iter.mapping_dimension(), const_key(iter.node), bound)
                  && (best == 0
                      || less_by_ref(cmp, iter.mapping_dimension(),
                                     const_key(iter.node), const_key(best))))
                { best = iter.node; best_dim = iter.node_dim; }
            }
          else
            {
              node_ptr p = iter.node->parent;
              while (p != end && p->right == iter.node)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
              if (iter.node != end
                  && !cmp(iter.mapping_dimension(), const_key(iter.node), bound)
                  && (best == 0
                      || less_by_ref(cmp, iter.mapping_dimension(),
                                     const_key(iter.node), const_key(best))))
                { best = iter.node; best_dim = iter.node_dim; }
            }
        }
      while (iter.node != end);
      if (best != 0) { iter.node = best; iter.node_dim = best_dim; }
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container>
    inline mapping_iterator<Container>&
    increment_mapping(mapping_iterator<Container>& iter)
    {
      return ::spatial::details::increment_mapping
        (iter, typename container_traits<Container>::mode_type
         ::invariant_category());
    }

    // The next largest key on the mapping dimension is likely to be found in the
    // children of the current best, so, descend into the children of node first.
    template <typename Container>
    inline mapping_iterator<Container>&
    decrement_mapping(mapping_iterator<Container>& iter)
    {
      typedef typename mapping_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.mapping_dimension() < rank());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      if (header(iter.node))
        {
          iter.node = iter.node->parent;
          iter.node_dim = 0; // root is always compared on dimension 0
          return maximum_mapping(iter);
        }
      node_ptr best = 0; // not null when best has been found
      node_ptr node = iter.node;
      dimension_type node_dim = iter.node_dim;
      dimension_type best_dim = iter.node_dim;
      if (node->right != 0)
        {
          do
            {
              node = node->right;
              node_dim = incr_dim(rank, node_dim);
            }
          while (node->right != 0
                 && (node_dim != iter.mapping_dimension()
                     || !cmp(node_dim,
                             const_key(iter.node), const_key(node))));
        }
      node_ptr ceiling = node->parent; // the upper limit of unvisited nodes
      bool sibling_visited = false;    // at ceiling, sibling node is visited
      if (less_by_ref(cmp, iter.mapping_dimension(),
                      const_key(node), const_key(iter.node)))
        { best = node; best_dim = node_dim; }
      do
        {
          if (node->left != 0
              && (node_dim != iter.mapping_dimension()
                  || best == 0
                  || !cmp(node_dim, const_key(node), const_key(best))))
            {
              node = node->left;
              node_dim = incr_dim(rank, node_dim);
              while (node->right != 0
                     && (node_dim != iter.mapping_dimension()
                         || !cmp(node_dim,
                                 const_key(iter.node), const_key(node))))
                {
                  node = node->right;
                  node_dim = incr_dim(rank, node_dim);
                }
              if (less_by_ref(cmp, iter.mapping_dimension(),
                              const_key(node), const_key(iter.node))
                  && (best == 0
                      || less_by_ref(cmp, iter.mapping_dimension(),
                                     const_key(best), const_key(node))))
                { best = node; best_dim = node_dim; }
            }
          else
            {
              node_ptr p = node->parent;
              // go upward as long as we don't hit header or ceiling
              while (!header(p)
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
                      if (less_by_ref(cmp, iter.mapping_dimension(),
                                      const_key(node), const_key(iter.node))
                          && (best == 0
                              || less_by_ref(cmp, iter.mapping_dimension(),
                                             const_key(best),
                                             const_key(node))))
                        { best = node; best_dim = node_dim; }
                      sibling_visited = false;
                      ceiling = p;
                    }
                }
              if (!header(p) && p == ceiling)
                {
                  if (p->right == node) { node = p->left; }
                  else { node = p->right; }
                  sibling_visited = true;
                  // go to full right in unvisited sibling
                  while (node->right != 0
                         && (node_dim != iter.mapping_dimension()
                             || !cmp(node_dim, const_key(iter.node),
                                     const_key(node))))
                    {
                      node = node->right;
                      node_dim = incr_dim(rank, node_dim);
                    }
                  if (less_by_ref(cmp, iter.mapping_dimension(),
                                  const_key(node), const_key(iter.node))
                      && (best == 0
                          || less_by_ref(cmp, iter.mapping_dimension(),
                                         const_key(best), const_key(node))))
                    { best = node; best_dim = node_dim; }
                }
              else
                {
                  node = p;
                  node_dim = decr_dim(rank, node_dim);
                  if (!header(node))
                    {
                      if (less_by_ref(cmp, iter.mapping_dimension(),
                                      const_key(node), const_key(iter.node))
                          && (best == 0
                              || less_by_ref(cmp, iter.mapping_dimension(),
                                             const_key(best),
                                             const_key(node))))
                        { best = node; best_dim = node_dim; }
                    }
                }
            }
        }
      while (!header(node));
      if (best != 0) { iter.node = best; iter.node_dim = best_dim; }
      else { iter.node = node; iter.node_dim = node_dim; }
      SPATIAL_ASSERT_CHECK(header(node));
      SPATIAL_ASSERT_CHECK(node_dim == (rank() - 1));
      SPATIAL_ASSERT_CHECK(iter.mapping_dimension() < rank());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    // Find the minimum from node and stop when reaching the parent. Iterate in
    // left-first fashion.
    template <typename Container>
    inline mapping_iterator<Container>&
    minimum_mapping(mapping_iterator<Container>& iter)
    {
      typedef typename mapping_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.mapping_dimension() < rank());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      node_ptr end = iter.node->parent;
      while (iter.node->left != 0)
        {
          iter.node = iter.node->left;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      node_ptr best = iter.node;
      dimension_type best_dim = iter.node_dim;
      do
        {
          if (iter.node->right != 0
              && (iter.node_dim != iter.mapping_dimension()
                  || !cmp(iter.mapping_dimension(),
                          const_key(best), const_key(iter.node))))
            {
              iter.node = iter.node->right;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->left != 0)
                {
                  iter.node = iter.node->left;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
              if (less_by_ref(cmp, iter.mapping_dimension(),
                              const_key(iter.node), const_key(best)))
                { best = iter.node; best_dim = iter.node_dim; }
            }
          else
            {
              node_ptr p = iter.node->parent;
              while (p != end && p->right == iter.node)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
              if (iter.node != end
                  && less_by_ref(cmp, iter.mapping_dimension(),
                                 const_key(iter.node), const_key(best)))
                { best = iter.node; best_dim = iter.node_dim; }
            }
        }
      while (iter.node != end);
      SPATIAL_ASSERT_CHECK(best_dim < rank());
      SPATIAL_ASSERT_CHECK(best != 0);
      SPATIAL_ASSERT_CHECK(!header(best));
      iter.node = best; iter.node_dim = best_dim;
      return iter;
    }

    template <typename Container>
    inline mapping_iterator<Container>&
    maximum_mapping(mapping_iterator<Container>& iter)
    {
      return ::spatial::details::maximum_mapping
        (iter, typename container_traits<Container>::mode_type
         ::invariant_category());
    }

    template <typename Container>
    inline mapping_iterator<Container>&
    lower_bound_mapping(mapping_iterator<Container>& iter,
                        const typename container_traits<Container>::key_type&
                        bound)
    {
      return ::spatial::details::lower_bound_mapping
        (iter, bound,
         typename container_traits<Container>::mode_type
         ::invariant_category());
    }

    // Walk tree nodes in right-first fashion, bouncing off values that are
    // higher than key.
    template <typename Container>
    inline mapping_iterator<Container>&
    upper_bound_mapping(mapping_iterator<Container>& iter,
                        const typename container_traits<Container>::key_type&
                        bound)
    {
      typedef typename mapping_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.mapping_dimension() < rank());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      node_ptr end = iter.node->parent;
      node_ptr best = 0;
      dimension_type best_dim = 0;
      while (iter.node->left != 0
             && (iter.node_dim != iter.mapping_dimension()
                 || !cmp(iter.node_dim, const_key(iter.node), bound)))
        {
          iter.node = iter.node->left;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      if (cmp(iter.mapping_dimension(), bound, const_key(iter.node)))
        { best = iter.node; best_dim = iter.node_dim; }
      do
        {
          if (iter.node->right != 0
              && (iter.node_dim != iter.mapping_dimension() || best == 0
                  || !cmp(iter.mapping_dimension(),
                          const_key(best), const_key(iter.node))))
            {
              iter.node = iter.node->right;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->left != 0
                     && (iter.node_dim != iter.mapping_dimension()
                         || !cmp(iter.node_dim, const_key(iter.node), bound)))
                {
                  iter.node = iter.node->left;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
              if (cmp(iter.mapping_dimension(), bound, const_key(iter.node))
                  && (best == 0
                      || less_by_ref(cmp, iter.mapping_dimension(),
                                     const_key(iter.node), const_key(best))))
                { best = iter.node; best_dim = iter.node_dim; }
            }
          else
            {
              node_ptr p = iter.node->parent;
              while (p != end && p->right == iter.node)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
              if (iter.node != end
                  && cmp(iter.mapping_dimension(), bound, const_key(iter.node))
                  && (best == 0
                      || less_by_ref(cmp, iter.mapping_dimension(),
                                     const_key(iter.node), const_key(best))))
                { best = iter.node; best_dim = iter.node_dim; }
            }
        }
      while (iter.node != end);
      if (best != 0) { iter.node = best; iter.node_dim = best_dim; }
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_MAPPING_TPP
