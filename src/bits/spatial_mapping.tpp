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
      template<typename Impl, typename Node_ptr, typename Rank,
               typename Compare>
      inline void
      increment(Impl& impl_, details::relaxed_invariant_tag)
      {
        const Rank& rank = *static_cast<const Rank*>(&impl_);
        const Compare& cmp = impl_.mapping_dim_.base();
        SPATIAL_ASSERT_CHECK(impl_.node_ != 0);
        SPATIAL_ASSERT_CHECK(!header(impl_.node_));
        SPATIAL_ASSERT_CHECK(impl_.mapping_dim_() < rank());
        SPATIAL_ASSERT_CHECK(impl_.node_dim_ < rank());
        Node_ptr best = 0; // not null when best has been found
        Node_ptr node = impl_.node_;
        dimension_type node_dim = impl_.node_dim_;
        dimension_type best_dim = impl_.node_dim_;
        if (node->left != 0)
          {
            do
              {
                node = node->left;
                node_dim = incr_dim(rank, node_dim);
              }
            while (node->left != 0 && (node_dim != impl_.mapping_dim_()
                                       || !cmp(node_dim, key(node),
                                               key(impl_.node_))));
          }
        Node_ptr ceiling = node->parent; // the upper limit of unvisited nodes
        bool sibling_visited = false;    // at ceiling, sibling node is visited
        if (less_by_ref(cmp, impl_.mapping_dim_(), key(impl_.node_), key(node)))
          { best = node; best_dim = node_dim; }
        do
          {
            if (node->right != 0
                && (node_dim != impl_.mapping_dim_() || best == 0
                    || !cmp(node_dim, key(best), key(node))))
              {
                node = node->right;
                node_dim = incr_dim(rank, node_dim);
                while (node->left != 0
                       && (node_dim != impl_.mapping_dim_()
                           || !cmp(node_dim, key(node), key(impl_.node_))))
                  {
                    node = node->left;
                    node_dim = incr_dim(rank, node_dim);
                  }
                if (less_by_ref(cmp, impl_.mapping_dim_(),
                                key(impl_.node_), key(node))
                    && (best == 0
                        || less_by_ref(cmp, impl_.mapping_dim_(),
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
                        if (less_by_ref(cmp, impl_.mapping_dim_(),
                                        key(impl_.node_), key(node))
                            && (best == 0
                                || less_by_ref(cmp, impl_.mapping_dim_(),
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
                           && (node_dim != impl_.mapping_dim_()
                               || !cmp(node_dim, key(node), key(impl_.node_))))
                      {
                        node = node->left;
                        node_dim = incr_dim(rank, node_dim);
                      }
                    if (less_by_ref(cmp, impl_.mapping_dim_(),
                                    key(impl_.node_), key(node))
                        && (best == 0
                            || less_by_ref(cmp, impl_.mapping_dim_(),
                                           key(node), key(best))))
                      { best = node; best_dim = node_dim; }
                  }
                else
                  {
                    node = p;
                    node_dim = decr_dim(rank, node_dim);
                    if (!header(node))
                      {
                        if (less_by_ref(cmp, impl_.mapping_dim_(),
                                        key(impl_.node_), key(node))
                            && (best == 0
                                || less_by_ref(cmp, impl_.mapping_dim_(),
                                               key(node), key(best))))
                          { best = node; best_dim = node_dim; }
                      }
                  }
              }
          }
        while (!header(node));
        if (best != 0) { impl_.node_ = best; impl_.node_dim_ = best_dim; }
        else { impl_.node_ = node; impl_.node_dim_ = node_dim; }
        SPATIAL_ASSERT_CHECK(impl_.mapping_dim_() < rank());
        SPATIAL_ASSERT_CHECK(impl_.node_dim_ < rank());
        SPATIAL_ASSERT_CHECK(impl_.node_ != 0);
      }

      template<typename Impl, typename Node_ptr,
               typename Rank, typename Compare>
      inline void
      increment(Impl& impl, details::strict_invariant_tag)
      {
        const Rank& rank = *static_cast<const Rank*>(&impl);
        const Compare& cmp = impl.mapping_dim_.base();
        SPATIAL_ASSERT_CHECK(impl.node_ != 0);
        SPATIAL_ASSERT_CHECK(!header(impl.node_));
        SPATIAL_ASSERT_CHECK(impl.mapping_dim_() < rank());
        SPATIAL_ASSERT_CHECK(impl.node_dim_ < rank());
        Node_ptr best = 0; // not null when best has been found
        Node_ptr node = impl.node_;
        dimension_type node_dim = impl.node_dim_;
        dimension_type best_dim = impl.node_dim_;
        if (node->left != 0 && node_dim != impl.mapping_dim_()) // optimization
          {
            do
              {
                node = node->left;
                node_dim = incr_dim(rank, node_dim);
              }
            while (node->left != 0
                   && (node_dim != impl.mapping_dim_() // optimization
                       || cmp(node_dim, key(impl.node_),
                              key(node))));
          }
        Node_ptr ceiling = node->parent; // the upper limit of unvisited nodes
        bool sibling_visited = false;    // at ceiling, sibling node is visited
        if (less_by_ref(cmp, impl.mapping_dim_(), key(impl.node_),
                        key(node)))
          { best = node; best_dim = node_dim; }
        do
          {
            if (node->right != 0
                && (node_dim != impl.mapping_dim_() || best == 0
                    || !cmp(node_dim, key(best), key(node))))
              {
                node = node->right;
                node_dim = incr_dim(rank, node_dim);
                while (node->left != 0
                       && (node_dim != impl.mapping_dim_()  // optimization
                           || cmp(node_dim, key(impl.node_), key(node))))
                  {
                    node = node->left;
                    node_dim = incr_dim(rank, node_dim);
                  }
                if (less_by_ref(cmp, impl.mapping_dim_(),
                                key(impl.node_), key(node))
                    && (best == 0
                        || less_by_ref(cmp, impl.mapping_dim_(),
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
                        if (less_by_ref(cmp, impl.mapping_dim_(),
                                        key(impl.node_), key(node))
                            && (best == 0
                                || less_by_ref(cmp, impl.mapping_dim_(),
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
                           && (node_dim != impl.mapping_dim_() // optimization
                               || cmp(node_dim, key(impl.node_), key(node))))
                      {
                        node = node->left;
                        node_dim = incr_dim(rank, node_dim);
                      }
                    if (less_by_ref(cmp, impl.mapping_dim_(),
                                    key(impl.node_), key(node))
                        && (best == 0
                            || less_by_ref(cmp, impl.mapping_dim_(),
                                           key(node), key(best))))
                      { best = node; best_dim = node_dim; }
                  }
                else
                  {
                    node = p;
                    node_dim = decr_dim(rank, node_dim);
                    if (!header(node))
                      {
                        if (less_by_ref(cmp, impl.mapping_dim_(),
                                        key(impl.node_), key(node))
                            && (best == 0
                                || less_by_ref(cmp, impl.mapping_dim_(),
                                               key(node), key(best))))
                          { best = node; best_dim = node_dim; }
                      }
                  }
              }
          }
        while (!header(node));
        if (best != 0) { impl.node_ = best; impl.node_dim_ = best_dim; }
        else { impl.node_ = node; impl.node_dim_ = node_dim; }
        SPATIAL_ASSERT_CHECK(impl.mapping_dim_() < rank());
        SPATIAL_ASSERT_CHECK(impl.node_dim_ < rank());
        SPATIAL_ASSERT_CHECK(impl.node_ != 0);
      }

      // Find the maximum from node and stop when reaching the parent. Iterate
      // in right-first fashion.
      template<typename Iterator, typename Node_ptr, typename Rank,
               typename Compare>
      inline Iterator
      maximum
      (const Rank& rank, const Compare& cmp, dimension_type mapping_dim,
       dimension_type node_dim, Node_ptr node, details::relaxed_invariant_tag)
      {
        SPATIAL_ASSERT_CHECK(node != 0);
        SPATIAL_ASSERT_CHECK(mapping_dim < rank());
        SPATIAL_ASSERT_CHECK(node_dim < rank());
        SPATIAL_ASSERT_CHECK(!header(node));
        Node_ptr end = node->parent;
        while (node->right != 0)
          {
            node = node->right;
            node_dim = incr_dim(rank, node_dim);
          }
        Node_ptr best = node;
        dimension_type best_dim = node_dim;
        do
          {
            if (node->left != 0
                && (node_dim != mapping_dim
                    || !cmp(mapping_dim, key(node), key(best))))
              {
                node = node->left;
                node_dim = incr_dim(rank, node_dim);
                while (node->right != 0)
                  {
                    node = node->right;
                    node_dim = incr_dim(rank, node_dim);
                  }
                if (less_by_ref(cmp, mapping_dim, key(best), key(node)))
                  { best = node; best_dim = node_dim; }
              }
            else
              {
                Node_ptr p = node->parent;
                while (p != end && p->left == node)
                  {
                    node = p;
                    node_dim = decr_dim(rank, node_dim);
                    p = node->parent;
                  }
                node = p; node_dim = decr_dim(rank, node_dim);
                if (node != end
                    && less_by_ref(cmp, mapping_dim, key(best), key(node)))
                  { best = node; best_dim = node_dim; }
              }
          }
        while (node != end);
        SPATIAL_ASSERT_CHECK(best_dim < rank());
        SPATIAL_ASSERT_CHECK(!header(best));
        SPATIAL_ASSERT_CHECK(best != 0);
        SPATIAL_ASSERT_CHECK(node != 0);
        return Iterator(rank, cmp, mapping_dim, best_dim, link(best));
      }

      // Specialized template that works a little faster on regular kdtree tree
      // nodes since invariant is applied strictly.
      template<typename Iterator, typename Node_ptr, typename Rank,
               typename Compare>
      inline Iterator
      maximum
      (const Rank& rank, const Compare& cmp, dimension_type mapping_dim,
       dimension_type node_dim, Node_ptr node, details::strict_invariant_tag)
      {
        SPATIAL_ASSERT_CHECK(node != 0);
        SPATIAL_ASSERT_CHECK(mapping_dim < rank());
        SPATIAL_ASSERT_CHECK(node_dim < rank());
        SPATIAL_ASSERT_CHECK(!header(node));
        Node_ptr end = node->parent;
        while (node->right != 0)
          {
            node = node->right;
            node_dim = incr_dim(rank, node_dim);
          }
        Node_ptr best = node;
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
                                key(best), key(node)))
                  { best = node; best_dim = node_dim; }
              }
            else
              {
                Node_ptr p = node->parent;
                while (p != end && p->left == node)
                  {
                    node = p;
                    node_dim = decr_dim(rank, node_dim);
                    p = node->parent;
                  }
                node = p; node_dim = decr_dim(rank, node_dim);
                if (node != end
                    && less_by_ref(cmp, mapping_dim,
                                   key(best), key(node)))
                  { best = node; best_dim = node_dim; }
              }
          }
        while (node != end);
        SPATIAL_ASSERT_CHECK(best_dim < rank());
        SPATIAL_ASSERT_CHECK(!header(best));
        SPATIAL_ASSERT_CHECK(best != 0);
        SPATIAL_ASSERT_CHECK(node != 0);
        return Iterator(rank, cmp, mapping_dim, best_dim, link(best));
      }

      // Walk tree nodes in left-first fashion, bouncing off values that are
      // lower than key.
      template<typename Iterator, typename Node_ptr, typename Rank,
               typename Key, typename Compare>
      inline Iterator
      lower_bound
      (const Rank& rank, const Compare& cmp, dimension_type mapping_dim,
       dimension_type node_dim, Node_ptr node, const Key& flag,
       details::relaxed_invariant_tag)
      {
        SPATIAL_ASSERT_CHECK(node != 0);
        SPATIAL_ASSERT_CHECK(mapping_dim < rank());
        SPATIAL_ASSERT_CHECK(node_dim < rank());
        SPATIAL_ASSERT_CHECK(!header(node));
        Node_ptr end = node->parent;
        Node_ptr best = 0;
        dimension_type best_dim = 0;
        while (node->left != 0
               && (node_dim != mapping_dim
                   || !cmp(node_dim, key(node), flag)))
          {
            node = node->left;
            node_dim = incr_dim(rank, node_dim);
          }
        if (!cmp(mapping_dim, key(node), flag))
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
                                           || !cmp(node_dim, key(node), flag)))
                  {
                    node = node->left;
                    node_dim = incr_dim(rank, node_dim);
                  }
                if (!cmp(mapping_dim, key(node), flag)
                    && (best == 0 || less_by_ref(cmp, mapping_dim,
                                                 key(node), key(best))))
                  { best = node; best_dim = node_dim; }
              }
            else
              {
                Node_ptr p = node->parent;
                while (p != end && p->right == node)
                  {
                    node = p;
                    node_dim = decr_dim(rank, node_dim);
                    p = node->parent;
                  }
                node = p; node_dim = decr_dim(rank, node_dim);
                if (node != end
                    && !cmp(mapping_dim, key(node), flag)
                    && (best == 0 || less_by_ref(cmp, mapping_dim,
                                                 key(node), key(best))))
                  { best = node; best_dim = node_dim; }
              }
          }
        while (node != end);
        if (best == 0) { best = node; best_dim = node_dim; }
        SPATIAL_ASSERT_CHECK(best_dim < rank());
        SPATIAL_ASSERT_CHECK(best != 0);
        return Iterator(rank, cmp, mapping_dim, best_dim, link(best));
      }

      // Specialized template that works a little faster on regular kdtree tree
      // nodes since invariant is applied strictly.
      template<typename Iterator, typename Node_ptr, typename Rank,
               typename Key, typename Compare>
      inline Iterator
      lower_bound
      (const Rank& rank, const Compare& cmp, dimension_type mapping_dim,
       dimension_type node_dim, Node_ptr node, const Key& flag,
       details::strict_invariant_tag)
      {
        SPATIAL_ASSERT_CHECK(node != 0);
        SPATIAL_ASSERT_CHECK(mapping_dim < rank());
        SPATIAL_ASSERT_CHECK(node_dim < rank());
        SPATIAL_ASSERT_CHECK(!header(node));
        Node_ptr end = node->parent;
        Node_ptr best = 0;
        dimension_type best_dim = 0;
        while (node->left != 0
               && (node_dim != mapping_dim // optimization
                   || cmp(node_dim, flag, key(node))))
          {
            node = node->left;
            node_dim = incr_dim(rank, node_dim);
          }
        if (!cmp(mapping_dim, key(node), flag))
          { best = node; best_dim = node_dim; }
        do
          {
            if (node->right != 0
                && (node_dim != mapping_dim || best == 0
                    || !cmp(mapping_dim, key(best), key(node))))
              {
                node = node->right;
                node_dim = incr_dim(rank, node_dim);
                while (node->left != 0
                       && (node_dim != mapping_dim // optimization
                           || cmp(node_dim, flag, key(node))))
                  {
                    node = node->left;
                    node_dim = incr_dim(rank, node_dim);
                  }
                if (!cmp(mapping_dim, key(node), flag)
                    && (best == 0 || less_by_ref(cmp, mapping_dim,
                                                 key(node), key(best))))
                  { best = node; best_dim = node_dim; }
              }
            else
              {
                Node_ptr p = node->parent;
                while (p != end && p->right == node)
                  {
                    node = p;
                    node_dim = decr_dim(rank, node_dim);
                    p = node->parent;
                  }
                node = p; node_dim = decr_dim(rank, node_dim);
                if (node != end
                    && !cmp(mapping_dim, key(node), flag)
                    && (best == 0 || less_by_ref(cmp, mapping_dim,
                                                 key(node), key(best))))
                  { best = node; best_dim = node_dim; }
              }
          }
        while (node != end);
        if (best == 0) { best = node; best_dim = node_dim; }
        SPATIAL_ASSERT_CHECK(best_dim < rank());
        SPATIAL_ASSERT_CHECK(best != 0);
        return Iterator(rank, cmp, mapping_dim, best_dim, link(best));
      }

    } // namespace mapping

    template<typename Rank, typename Key, typename Value, typename Node,
             typename Compare, bool Constant, typename Derived>
    inline void
    Mapping_iterator_base<Rank, Key, Value, Node, Compare, Constant, Derived>
    ::increment()
    {
      details::mapping::increment
        <Mapping_iterator_impl, node_ptr, Rank, Compare>
        (impl_, invariant_category());
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
