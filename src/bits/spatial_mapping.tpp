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
    inline typename mapping<Container>::iterator&
    increment(typename mapping<Container>::iterator& iter,
              details::relaxed_invariant_tag)
    {
      typedef typename mapping<Container>::iterator::node_ptr node_ptr;
      const Rank& rank = *static_cast<const Rank*>(&iter.data);
      const Compare& cmp = iter.data.mapping_dim.base();

      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.data.mapping_dim() < rank());
      SPATIAL_ASSERT_CHECK(iter.data.node_dim < rank());

      node_ptr best = 0; // not null when best has been found
      node_ptr node = iter.node;
      dimension_type node_dim = iter.data.node_dim;
      dimension_type best_dim = iter.data.node_dim;
      if (node->left != 0)
        {
          do
            {
              node = node->left;
              node_dim = incr_dim(rank, node_dim);
            }
          while (node->left != 0
                 && (node_dim != iter.data.mapping_dim()
                     || !cmp(node_dim, key(node), key(iter.node))));
        }
      Node_ptr ceiling = node->parent; // the upper limit of unvisited nodes
      bool sibling_visited = false;    // at ceiling, sibling node is visited
      if (less_by_ref(cmp, iter.data.mapping_dim(), key(iter.node), key(node)))
        { best = node; best_dim = node_dim; }
      do
        {
          if (node->right != 0
              && (node_dim != iter.data.mapping_dim() || best == 0
                  || !cmp(node_dim, key(best), key(node))))
            {
              node = node->right;
              node_dim = incr_dim(rank, node_dim);
              while (node->left != 0
                     && (node_dim != iter.data.mapping_dim()
                         || !cmp(node_dim, key(node), key(iter.node))))
                {
                  node = node->left;
                  node_dim = incr_dim(rank, node_dim);
                }
              if (less_by_ref(cmp, iter.data.mapping_dim(),
                              key(iter.node), key(node))
                  && (best == 0
                      || less_by_ref(cmp, iter.data.mapping_dim(),
                                     key(node), key(best))))
                { best = node; best_dim = node_dim; }
            }
          else
            {
              Node_ptr p = node->parent;
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
                      if (less_by_ref(cmp, iter.data.mapping_dim(),
                                      key(iter.node), key(node))
                          && (best == 0
                              || less_by_ref(cmp, iter.data.mapping_dim(),
                                             key(node), key(best))))
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
                         && (node_dim != iter.data.mapping_dim()
                             || !cmp(node_dim, key(node), key(iter.node))))
                    {
                      node = node->left;
                      node_dim = incr_dim(rank, node_dim);
                    }
                  if (less_by_ref(cmp, iter.data.mapping_dim(),
                                  key(iter.node), key(node))
                      && (best == 0
                          || less_by_ref(cmp, iter.data.mapping_dim(),
                                         key(node), key(best))))
                    { best = node; best_dim = node_dim; }
                }
              else
                {
                  node = p;
                  node_dim = decr_dim(rank, node_dim);
                  if (!header(node))
                    {
                      if (less_by_ref(cmp, iter.data.mapping_dim(),
                                      key(iter.node), key(node))
                          && (best == 0
                              || less_by_ref(cmp, iter.data.mapping_dim(),
                                             key(node), key(best))))
                        { best = node; best_dim = node_dim; }
                    }
                }
            }
        }
      while (!header(node));
      if (best != 0) { iter.node = best; iter.data.node_dim = best_dim; }
      else { iter.node = node; iter.data.node_dim = node_dim; }

      SPATIAL_ASSERT_CHECK(iter.data.mapping_dim() < rank());
      SPATIAL_ASSERT_CHECK(iter.data.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    //! Specialization for iterators pointing to node using the strict
    //! invariant.
    //! \see increment<Container>(typename mapping<Container>::iterator&)
    template <typename Container>
    inline typename mapping<Container>::iterator&
    increment(typename mapping<Container>::iterator& iter,
              details::strict_invariant_tag)
    {
      typedef typename mapping<Container>::iterator::node_ptr node_ptr;
      const Rank& rank = *static_cast<const Rank*>(&iter.data);
      const Compare& cmp = iter.data.mapping_dim.base();

      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.data.mapping_dim() < rank());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());

      node_ptr best = 0; // not null when best has been found
      node_ptr node = iter.node;
      dimension_type node_dim = iter.node_dim;
      dimension_type best_dim = iter.node_dim;
      if (node->left != 0 && node_dim != iter.mapping_dim()) // optimization
        {
          do
            {
              node = node->left;
              node_dim = incr_dim(rank, node_dim);
            }
          while (node->left != 0
                 && (node_dim != iter.data.mapping_dim() // optimization
                     || cmp(node_dim, key(iter.node),
                            key(node))));
        }
      Node_ptr ceiling = node->parent; // the upper limit of unvisited nodes
      bool sibling_visited = false;    // at ceiling, sibling node is visited
      if (less_by_ref(cmp, iter.data.mapping_dim(), key(iter.node),
                      key(node)))
        { best = node; best_dim = node_dim; }
      do
        {
          if (node->right != 0
              && (node_dim != iter.data.mapping_dim() || best == 0
                  || !cmp(node_dim, key(best), key(node))))
            {
              node = node->right;
              node_dim = incr_dim(rank, node_dim);
              while (node->left != 0
                     && (node_dim != iter.data.mapping_dim()  // optimization
                         || cmp(node_dim, key(iter.node), key(node))))
                {
                  node = node->left;
                  node_dim = incr_dim(rank, node_dim);
                }
              if (less_by_ref(cmp, iter.data.mapping_dim(),
                              key(iter.node), key(node))
                  && (best == 0
                      || less_by_ref(cmp, iter.data.mapping_dim(),
                                     key(node), key(best))))
                { best = node; best_dim = node_dim; }
            }
          else
            {
              Node_ptr p = node->parent;
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
                      if (less_by_ref(cmp, iter.data.mapping_dim(),
                                      key(iter.node), key(node))
                          && (best == 0
                              || less_by_ref(cmp, iter.data.mapping_dim(),
                                             key(node), key(best))))
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
                         && (node_dim != iter.data.mapping_dim() // optimization
                             || cmp(node_dim, key(iter.node), key(node))))
                    {
                      node = node->left;
                      node_dim = incr_dim(rank, node_dim);
                    }
                  if (less_by_ref(cmp, iter.data.mapping_dim(),
                                  key(iter.node), key(node))
                      && (best == 0
                          || less_by_ref(cmp, iter.data.mapping_dim(),
                                         key(node), key(best))))
                    { best = node; best_dim = node_dim; }
                }
              else
                {
                  node = p;
                  node_dim = decr_dim(rank, node_dim);
                  if (!header(node))
                    {
                      if (less_by_ref(cmp, iter.data.mapping_dim(),
                                      key(iter.node), key(node))
                          && (best == 0
                              || less_by_ref(cmp, iter.data.mapping_dim(),
                                             key(node), key(best))))
                        { best = node; best_dim = node_dim; }
                    }
                }
            }
        }
      while (!header(node));
      if (best != 0) { iter.node = best; iter.data.node_dim = best_dim; }
      else { iter.node = node; iter.data.node_dim = node_dim; }

      SPATIAL_ASSERT_CHECK(iter.data.mapping_dim() < rank());
      SPATIAL_ASSERT_CHECK(iter.data.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    //! Specialization for iterators pointed to node using the relaxed
    //! invariant.
    //! \see maximum<Container>(typename mapping<Container>::iterator&)
    template<typename Container>
    inline mapping<Container>::iterator&
    maximum(typename mapping<Container>::iterator& iter,
            details::relaxed_invariant_tag)
    {
      typedef typename mapping<Container>::iterator::node_ptr node_ptr;
      const Rank& rank = *static_cast<const Rank*>(&iter.data);
      const Compare& cmp = iter.data.mapping_dim.base();

      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.data.mapping_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.data.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));

      node_ptr end = iter.iter.node->parent;
      while (iter.node->right != 0)
        {
          iter.node = iter.node->right;
          iter.data.node_dim = incr_dim(rank, iter.data.node_dim);
        }
      node_ptr best = iter.node;
      dimension_type best_dim = iter.data.node_dim;
      do
        {
          if (iter.node->left != 0
              && (iter.data.node_dim != mapping_dim
                  || !cmp(mapping_dim, key(iter.node), key(best))))
            {
              iter.node = iter.node->left;
              iter.data.node_dim = incr_dim(rank, iter.data.node_dim);
              while (iter.node->right != 0)
                {
                  iter.node = iter.node->right;
                  iter.data.node_dim = incr_dim(rank, iter.data.node_dim);
                }
              if (less_by_ref(cmp, mapping_dim, key(best), key(iter.node)))
                { best = iter.node; best_dim = iter.data.node_dim; }
            }
          else
            {
              Node_ptr p = iter.node->parent;
              while (p != end && p->left == iter.node)
                {
                  iter.node = p;
                  iter.data.node_dim = decr_dim(rank, iter.data.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.data.node_dim = decr_dim(rank, iter.data.node_dim);
              if (iter.node != end
                  && less_by_ref(cmp, mapping_dim, key(best), key(iter.node)))
                { best = iter.node; best_dim = iter.data.node_dim; }
            }
        }
      while (iter.node != end);
      iter.node = best; iter.data.node_dim = best_dim;

      SPATIAL_ASSERT_CHECK(iter.data.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      return iter;
    }

    //! Specialization for iterators pointed to node using the strict
    //! invariant.
    //! \see maximum<Container>(typename mapping<Container>::iterator&)
    template<typename Container>
    inline mapping<Container>::iterator&
    maximum(typename mapping<Container>::iterator& iter,
            details::strict_invariant_tag)
    {
      typedef typename mapping<Container>::iterator::node_ptr node_ptr;

      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.data.mapping_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.data.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));

      node_ptr end = iter.node->parent;
      while (iter.node->right != 0)
        {
          iter.node = iter.node->right;
          iter.data.node_dim = incr_dim(rank, iter.data.node_dim);
        }
      node_ptr best = iter.node;
      dimension_type best_dim = iter.data.node_dim;
      do
        {
          if (iter.node->left != 0
              && iter.data.node_dim != iter.data.mapping_dim) // optimization
            {
              iter.node = iter.node->left;
              iter.data.node_dim = incr_dim(rank, iter.data.node_dim);
              while (iter.node->right != 0)
                {
                  iter.node = iter.node->right;
                  iter.data.node_dim = incr_dim(rank, iter.data.node_dim);
                }
              if (less_by_ref(cmp, iter.data.mapping_dim,
                              key(best), key(iter.node)))
                { best = iter.node; best_dim = iter.data.node_dim; }
            }
          else
            {
              node_ptr p = iter.node->parent;
              while (p != end && p->left == iter.node)
                {
                  iter.node = p;
                  iter.data.node_dim = decr_dim(rank, iter.data.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.data.node_dim = decr_dim(rank, iter.data.node_dim);
              if (iter.node != end
                  && less_by_ref(cmp, iter.data.mapping_dim,
                                 key(best), key(iter.node)))
                { best = iter.node; best_dim = iter.data.node_dim; }
            }
        }
      while (iter.node != end);
      iter.node = best; iter.data.node_dim = best_dim;

      SPATIAL_ASSERT_CHECK(iter.data.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      return iter;
    }

    //! Specialization for iterators pointed to node using the relaxed
    //! invariant.
    //! \see lower_bound<Container>(typename mapping<Container>::iterator&,
    //! const typename Container::key_type&)
    template<typename Container>
    inline mapping<Container>::iterator&
    lower_bound(typename mapping<Container>::iterator& iter,
                const typename Container::key_type& bound,
                details::relaxed_invariant_tag)
    {
      typedef typename mapping<Container>::iterator::node_ptr node_ptr;

      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.data.mapping_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.data.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));

      Node_ptr end = iter.node->parent;
      Node_ptr best = 0;
      dimension_type best_dim = 0;
      while (iter.node->left != 0
             && (iter.data.node_dim != iter.data.mapping_dim
                 || !cmp(iter.data.node_dim, key(iter.node), bound)))
        {
          iter.node = iter.node->left;
          iter.data.node_dim = incr_dim(rank, iter.data.node_dim);
        }
      if (!cmp(iter.data.mapping_dim, key(iter.node), bound))
        { best = iter.node; best_dim = iter.data.node_dim; }
      do
        {
          if (iter.node->right != 0
              && (iter.data.node_dim != iter.data.mapping_dim || best == 0
                  || !cmp(iter.data.mapping_dim, key(best), key(iter.node))))
            {
              iter.node = iter.node->right;
              iter.data.node_dim = incr_dim(rank, iter.data.node_dim);
              while (iter.node->left != 0
                     && (iter.data.node_dim != iter.data.mapping_dim
                         || !cmp(iter.data.node_dim, key(iter.node), bound)))
                {
                  iter.node = iter.node->left;
                  iter.data.node_dim = incr_dim(rank, iter.data.node_dim);
                }
              if (!cmp(iter.data.mapping_dim, key(iter.node), bound)
                  && (best == 0 || less_by_ref(cmp, iter.data.mapping_dim,
                                               key(iter.node), key(best))))
                { best = iter.node; best_dim = iter.data.node_dim; }
            }
          else
            {
              Node_ptr p = iter.node->parent;
              while (p != end && p->right == iter.node)
                {
                  iter.node = p;
                  iter.data.node_dim = decr_dim(rank, iter.data.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p; iter.data.node_dim = decr_dim(rank, iter.data.node_dim);
              if (iter.node != end
                  && !cmp(iter.data.mapping_dim, key(iter.node), bound)
                  && (best == 0 || less_by_ref(cmp, iter.data.mapping_dim,
                                               key(iter.node), key(best))))
                { best = iter.node; best_dim = iter.data.node_dim; }
            }
        }
      while (iter.node != end);
      if (best != 0) { iter.node = best; iter.data.node_dim = best_dim; }

      SPATIAL_ASSERT_CHECK(iter.data.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    //! Specialization for iterators pointed to node using the strict
    //! invariant.
    //! \see lower_bound<Container>(typename mapping<Container>::iterator&,
    //! const typename Container::key_type&)
    template<typename Container>
    inline mapping<Container>::iterator&
    lower_bound(typename mapping<Container>::iterator& iter,
                const typename Container::key_type& bound,
                details::strict_invariant_tag)
    {
      typedef typename mapping<Container>::iterator::node_ptr node_ptr;

      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.data.mapping_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.data.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));

      Node_ptr end = iter.node->parent;
      Node_ptr best = 0;
      dimension_type best_dim = 0;
      while (iter.node->left != 0
             && (iter.data.node_dim != iter.data.mapping_dim // optimization
                 || cmp(iter.data.node_dim, bound, key(iter.node))))
        {
          iter.node = iter.node->left;
          iter.data.node_dim = incr_dim(rank, iter.data.node_dim);
        }
      if (!cmp(iter.data.mapping_dim, key(iter.node), bound))
        { best = iter.node; best_dim = iter.data.node_dim; }
      do
        {
          if (iter.node->right != 0
              && (iter.data.node_dim != iter.data.mapping_dim || best == 0
                  || !cmp(iter.data.mapping_dim, key(best), key(iter.node))))
            {
              iter.node = iter.node->right;
              iter.data.node_dim = incr_dim(rank, iter.data.node_dim);
              while (iter.node->left != 0 // optimization
                     && (iter.data.node_dim != iter.data.mapping_dim
                         || cmp(iter.data.node_dim, bound, key(iter.node))))
                {
                  iter.node = iter.node->left;
                  iter.data.node_dim = incr_dim(rank, iter.data.node_dim);
                }
              if (!cmp(iter.data.mapping_dim, key(iter.node), bound)
                  && (best == 0 || less_by_ref(cmp, iter.data.mapping_dim,
                                               key(iter.node), key(best))))
                { best = iter.node; best_dim = iter.data.node_dim; }
            }
          else
            {
              Node_ptr p = iter.node->parent;
              while (p != end && p->right == iter.node)
                {
                  iter.node = p;
                  iter.data.node_dim = decr_dim(rank, iter.data.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p; iter.data.node_dim = decr_dim(rank, iter.data.node_dim);
              if (iter.node != end
                  && !cmp(iter.data.mapping_dim, key(iter.node), bound)
                  && (best == 0 || less_by_ref(cmp, iter.data.mapping_dim,
                                               key(iter.node), key(best))))
                { best = iter.node; best_dim = iter.data.node_dim; }
            }
        }
      while (iter.node != end);
      if (best != 0) { iter.node = best; iter.data.node_dim = best_dim; }

      SPATIAL_ASSERT_CHECK(iter.data.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.data.node != 0);
      return iter;
    }

    template <typename Container>
    inline typename mapping<Container>::iterator&
    increment(typename mapping<Container>::iterator& iter)
    {
      return ::spatial::details::increment
        (iter, Container::mode_type::invariant_category());
    }

    // The next largest key on the mapping dimension is likely to be found in the
    // children of the current best, so, descend into the children of node first.
    template<typename Rank, typename Key, typename Value, typename Node,
             typename Compare, bool Constant, typename Derived>
    inline void
    Mapping_iterator_base<Rank, Key, Value, Node, Compare, Constant, Derived>
    ::decrement()
    {
      SPATIAL_ASSERT_CHECK(impl_.node_ != 0);
      SPATIAL_ASSERT_CHECK(impl_.mapping_dim_() < rank()());
      SPATIAL_ASSERT_CHECK(impl_.node_dim_ < rank()());
      node_ptr best = 0; // not null when best has been found
      node_ptr node = impl_.node_;
      dimension_type node_dim = impl_.node_dim_;
      dimension_type best_dim = impl_.node_dim_;
      if (header(node))
        {
          operator=(maximum(rank(), compare(),
                            impl_.mapping_dim_(), 0, node->parent));
          return;
        }
      if (node->right != 0)
        {
          do
            {
              node = node->right;
              node_dim = incr_dim(rank(), node_dim);
            }
          while (node->right != 0
                 && (node_dim != impl_.mapping_dim_()
                     || !compare()(node_dim, key(impl_.node_), key(node))));
        }
      node_ptr ceiling = node->parent; // the upper limit of unvisited nodes
      bool sibling_visited = false;    // at ceiling, sibling node is visited
      if (less_by_ref(compare(), impl_.mapping_dim_(),
                      key(node), key(impl_.node_)))
        { best = node; best_dim = node_dim; }
      do
        {
          if (node->left != 0
              && (node_dim != impl_.mapping_dim_() || best == 0
                  || !compare()(node_dim,
                                key(node), key(best))))
            {
              node = node->left;
              node_dim = incr_dim(rank(), node_dim);
              while (node->right != 0
                     && (node_dim != impl_.mapping_dim_()
                         || !compare()(node_dim, key(impl_.node_), key(node))))
                {
                  node = node->right;
                  node_dim = incr_dim(rank(), node_dim);
                }
              if (less_by_ref(compare(), impl_.mapping_dim_(),
                              key(node), key(impl_.node_))
                  && (best == 0
                      || less_by_ref(compare(), impl_.mapping_dim_(),
                                     key(best), key(node))))
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
                  node_dim = decr_dim(rank(), node_dim);
                  p = node->parent;
                  if (node == ceiling)
                    {
                      if (less_by_ref(compare(), impl_.mapping_dim_(),
                                      key(node), key(impl_.node_))
                          && (best == 0
                              || less_by_ref(compare(), impl_.mapping_dim_(),
                                             key(best), key(node))))
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
                         && (node_dim != impl_.mapping_dim_()
                             || !compare()(node_dim,
                                           key(impl_.node_), key(node))))
                    {
                      node = node->right;
                      node_dim = incr_dim(rank(), node_dim);
                    }
                  if (less_by_ref(compare(), impl_.mapping_dim_(),
                                  key(node), key(impl_.node_))
                      && (best == 0
                          || less_by_ref(compare(), impl_.mapping_dim_(),
                                         key(best), key(node))))
                    { best = node; best_dim = node_dim; }
                }
              else
                {
                  node = p;
                  node_dim = decr_dim(rank(), node_dim);
                  if (!header(node))
                    {
                      if (less_by_ref(compare(), impl_.mapping_dim_(),
                                      key(node), key(impl_.node_))
                          && (best == 0
                              || less_by_ref(compare(), impl_.mapping_dim_(),
                                             key(best), key(node))))
                        { best = node; best_dim = node_dim; }
                    }
                }
            }
        }
      while (!header(node));
      if (best != 0) { impl_.node_ = best; impl_.node_dim_ = best_dim; }
      else { impl_.node_ = node; impl_.node_dim_ = node_dim; }
      SPATIAL_ASSERT_CHECK(impl_.mapping_dim_() < rank()());
      SPATIAL_ASSERT_CHECK(impl_.node_dim_ < rank()());
      SPATIAL_ASSERT_CHECK(impl_.node_ != 0);
    }

    // Find the minimum from node and stop when reaching the parent. Iterate in
    // left-first fashion.
    template<typename Rank, typename Key, typename Value, typename Node,
             typename Compare, bool Constant, typename Derived>
    inline Derived
    Mapping_iterator_base<Rank, Key, Value, Node, Compare, Constant, Derived>
    ::minimum
    (const Rank& rank, const Compare& cmp, dimension_type mapping_dim,
     dimension_type node_dim, node_ptr node)
    {
      SPATIAL_ASSERT_CHECK(mapping_dim < rank());
      SPATIAL_ASSERT_CHECK(node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      node_ptr end = node->parent;
      while (node->left != 0)
        { node = node->left; node_dim = incr_dim(rank, node_dim); }
      node_ptr best = node;
      dimension_type best_dim = node_dim;
      do
        {
          if (node->right != 0
              && (node_dim != mapping_dim
                  || !cmp(mapping_dim,
                          key(best), key(node))))
            {
              node = node->right;
              node_dim = incr_dim(rank, node_dim);
              while (node->left != 0)
                {
                  node = node->left;
                  node_dim = incr_dim(rank, node_dim);
                }
              if (less_by_ref(cmp, mapping_dim,
                              key(node), key(best)))
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
                  && less_by_ref(cmp, mapping_dim,
                                 key(node), key(best)))
                { best = node; best_dim = node_dim; }
            }
        }
      while (node != end);
      SPATIAL_ASSERT_CHECK(best_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(best));
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(best != 0);
      return Derived(rank, cmp, mapping_dim, best_dim, link(best));
    }

    template<typename Rank, typename Key, typename Value, typename Node,
             typename Compare, bool Constant, typename Derived>
    inline Derived
    Mapping_iterator_base<Rank, Key, Value, Node, Compare, Constant, Derived>
    ::maximum
    (const Rank& rank, const Compare& cmp, dimension_type mapping_dim,
     dimension_type node_dim, node_ptr node)
    {
      return details::mapping::maximum
        <Derived, node_ptr, Rank, Compare>
        (rank, cmp, mapping_dim, node_dim, node, invariant_category());
    }

    template<typename Rank, typename Key, typename Value, typename Node,
             typename Compare, bool Constant, typename Derived>
    inline Derived
    Mapping_iterator_base<Rank, Key, Value, Node, Compare, Constant, Derived>
    ::lower_bound
    (const Rank& rank, const Compare& cmp, dimension_type mapping_dim,
     dimension_type node_dim, node_ptr node, const Key& flag)
    {
      return details::mapping::lower_bound
        <Derived, node_ptr, Rank, Key, Compare>
        (rank, cmp, mapping_dim, node_dim, node, flag, invariant_category());
    }

    // Walk tree nodes in right-first fashion, bouncing off values that are higher
    // than key.
    template<typename Rank, typename Key, typename Value, typename Node,
             typename Compare, bool Constant, typename Derived>
    inline Derived
    Mapping_iterator_base<Rank, Key, Value, Node, Compare, Constant, Derived>
    ::upper_bound
    (const Rank& rank, const Compare& cmp, dimension_type mapping_dim,
     dimension_type node_dim, node_ptr node, const Key& flag)
    {
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(mapping_dim < rank());
      SPATIAL_ASSERT_CHECK(node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(node));
      node_ptr end = node->parent;
      node_ptr best = 0;
      dimension_type best_dim = 0;
      while (node->left != 0
             && (node_dim != mapping_dim
                 || cmp(node_dim, flag, key(node))))
        { node = node->left; node_dim = incr_dim(rank, node_dim); }
      if (cmp(mapping_dim, flag, key(node)))
        { best = node; best_dim = node_dim; }
      do
        {
          if (node->right != 0
              && (node_dim != mapping_dim || best == 0
                  || !cmp(mapping_dim, key(best), key(node))))
            {
              node = node->right;
              node_dim = incr_dim(rank, node_dim);
              while (node->left != 0 && (node_dim != mapping_dim
                                         || cmp(node_dim, flag, key(node))))
                {
                  node = node->left;
                  node_dim = incr_dim(rank, node_dim);
                }
              if (cmp(mapping_dim, flag, key(node))
                  && (best == 0 || less_by_ref(cmp, mapping_dim,
                                               key(node), key(best))))
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
              node = p; node_dim = decr_dim(rank, node_dim);
              if (node != end
                  && cmp(mapping_dim, flag, key(node))
                  && (best == 0 || less_by_ref(cmp, mapping_dim,
                                               key(node), key(best))))
                { best = node; best_dim = node_dim; }
            }
        }
      while (node != end);
      if (best == 0) { best = node; best_dim = node_dim; }
      SPATIAL_ASSERT_CHECK(best_dim < rank());
      SPATIAL_ASSERT_CHECK(best != 0);
      return Derived(rank, cmp, mapping_dim, best_dim, link(best));
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_MAPPING_TPP
