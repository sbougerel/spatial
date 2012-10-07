// -*- C++ -*-

/**
 *  @file   spatial_relaxed_kdtree.tpp
 *  @brief
 *
 *  Change Log:
 *
 *  - 2009-02-27 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_RELAXED_KDTREE_TPP
#define SPATIAL_RELAXED_KDTREE_TPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

namespace spatial
{
  namespace details
  {

    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Balancing, typename Alloc>
    inline void
    Relaxed_kdtree<Rank, Key, Value, Compare, Balancing, Alloc>
    ::destroy_all_nodes()
    {
      node_ptr node = get_root();
      while (!header(node))
        {
          if (node->left != 0) { node = node->left; }
          else if (node->right != 0) { node = node->right; }
          else
            {
              node_ptr p = node->parent;
              if (header(p))
                {
                  set_root(get_header());
                  set_leftmost(get_header());
                  set_rightmost(get_header());
                }
              else
                {
                  if (p->left == node) { p->left = 0; }
                  else { p->right = 0; }
                }
              SPATIAL_ASSERT_CHECK(node != 0);
              SPATIAL_ASSERT_CHECK(p != 0);
              destroy_node(node);
              node = p;
            }
        }
    }

    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Balancing, typename Alloc>
    inline void
    Relaxed_kdtree<Rank, Key, Value, Compare, Balancing, Alloc>
    ::copy_structure
    (const Self& other)
    {
      SPATIAL_ASSERT_CHECK(!other.empty());
      SPATIAL_ASSERT_CHECK(empty());
      const_node_ptr other_node = other.get_root();
      node_ptr node = clone_node(other_node); // may throw
      node->parent = get_header();
      set_root(node);
      try
        {
          while(!header(other_node))
            {
              if (other_node->left != 0)
                {
                  other_node = other_node->left;
                  node_ptr target = clone_node(other_node);
                  target->parent = node;
                  node->left = target;
                  node = node->left;
                }
              else if (other_node->right != 0)
                {
                  other_node = other_node->right;
                  node_ptr target = clone_node(other_node);
                  target->parent = node;
                  node->right = target;
                  node = node->right;
                }
              else
                {
                  const_node_ptr p = other_node->parent;
                  while (!header(p)
                         && (other_node == p->right || p->right == 0))
                    {
                      other_node = p;
                      node = node->parent;
                      p = other_node->parent;
                    }
                  other_node = p;
                  node = node->parent;
                  if (!header(p))
                    {
                      other_node = other_node->right;
                      node_ptr target = clone_node(other_node);
                      target->parent = node;
                      node->right = target;
                      node = node->right;
                    }
                }
            }
          SPATIAL_ASSERT_CHECK(!empty());
          SPATIAL_ASSERT_CHECK(header(other_node));
          SPATIAL_ASSERT_CHECK(header(node));
        }
      catch (...)
        { clear(); throw; } // clean-up before re-throw
      set_leftmost(minimum(get_root()));
      set_rightmost(maximum(get_root()));
    }

    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Balancing, typename Alloc>
    inline bool
    Relaxed_kdtree<Rank, Key, Value, Compare, Balancing, Alloc>
    ::is_node_unbalanced
    (node_ptr node, weight_type more_left, weight_type more_right) const
    {
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(!header(node));
      // I believe it is not necessary to balance node when it weighs less than
      // the current dimension. This due to the k-d tree rotating invarient
      // (each dimension is considered in turn). Therefore, it makes little
      // difference for all algorithms to have balanced nodes if there are not
      // enough nodes to start pruning along all dimensions.
      //
      // Note that while I can theorize this for several algorithms (simple
      // iteration, mapping iteration, range iteration), I still have to perform
      // experiments that reflect this hypothesis.
      return (const_link(node)->weight > dimension() && balancing()
        (rank(),
         more_left + (node->left ? const_link(node->left)->weight : 0),
         more_right + (node->right ? const_link(node->right)->weight : 0)));
    }

    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Balancing, typename Alloc>
    inline
    typename Relaxed_kdtree<Rank, Key, Value, Compare, Balancing, Alloc>::node_ptr
    Relaxed_kdtree<Rank, Key, Value, Compare, Balancing, Alloc>
    ::balance_node
    (dimension_type node_dim, node_ptr node)
    {
      const_node_ptr p = node->parent; // Parent is not swapped, node is!
      bool left_node = (p->left == node);
      // erase first...
      erase_node(node_dim, node);
      node_ptr replacing = header(p) ? p->parent
        : (left_node ? p->left : p->right);
      // ...then re-insert.
      insert_node(node_dim, replacing, node);
      return header(p) ? p->parent : (left_node ? p->left : p->right);
    }

    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Balancing, typename Alloc>
    inline
    typename Relaxed_kdtree<Rank, Key, Value, Compare, Balancing, Alloc>::iterator
    Relaxed_kdtree<Rank, Key, Value, Compare, Balancing, Alloc>
    ::insert_node
    (dimension_type node_dim, node_ptr node, node_ptr target_node)
    {
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(!header(node));
      while (true)
        {
          SPATIAL_ASSERT_CHECK
            ((node->right ? const_link(node->right)->weight : 0)
             + (node->left ? const_link(node->left)->weight: 0)
             + 1 == const_link(node)->weight);
          // Balancing equal values on either side of the tree
          if (key_comp()(node_dim, const_key(target_node), const_key(node))
              || (!key_comp()(node_dim,
                              const_key(node), const_key(target_node))
                  && (node->left == 0
                      || (node->right != 0
                          && const_link(node->left)->weight
                          < const_link(node->right)->weight))))
            {
              if (node->left == 0)
                {
                  node->left = target_node;
                  target_node->parent = node;
                  if (get_leftmost() == node)
                    { set_leftmost(target_node); }
                  ++link(node)->weight;
                  break;
                }
              else
                {
                  if(is_node_unbalanced(node, 1, 0))
                    {
                      node = balance_node(node_dim, node); // recursive!
                    }
                  else
                    {
                      ++link(node)->weight;
                      node = node->left;
                      node_dim = incr_dim(rank(), node_dim);
                    }
                }
            }
          else
            {
              if (node->right == 0)
                {
                  node->right = target_node;
                  target_node->parent = node;
                  if (get_rightmost() == node)
                    { set_rightmost(target_node); }
                  ++link(node)->weight;
                  break;
                }
              else
                {
                  if(is_node_unbalanced(node, 0, 1))
                    {
                      node = balance_node(node_dim, node);  // recursive!
                    }
                  else
                    {
                      ++link(node)->weight;
                      node = node->right;
                      node_dim = incr_dim(rank(), node_dim);
                    }
                }
            }
        }
      SPATIAL_ASSERT_CHECK(target_node != 0);
      SPATIAL_ASSERT_CHECK(!header(target_node));
      SPATIAL_ASSERT_CHECK(!header(target_node->parent));
      SPATIAL_ASSERT_CHECK(target_node->right == 0);
      SPATIAL_ASSERT_CHECK(target_node->left == 0);
      SPATIAL_ASSERT_CHECK(target_node->parent != 0);
      return iterator(target_node);
    }

    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Balancing, typename Alloc>
    inline
    typename Relaxed_kdtree<Rank, Key, Value, Compare, Balancing, Alloc>
    ::node_ptr
    Relaxed_kdtree<Rank, Key, Value, Compare, Balancing, Alloc>
    ::erase_node
    (dimension_type node_dim, node_ptr node)
    {
      typedef Iterator_mapping<Self> mapping_iterator;
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(!header(node));
      node_ptr parent = node->parent;
      while (node->right != 0 || node->left != 0)
        {
          mapping_iterator candidate(*this, 0, 0, 0);
          if (node->left != 0
              && (node->right == 0
                  || const_link(node->right)->weight
                  < const_link(node->left)->weight))
            {
              candidate.data.mapping_dim() = node_dim;
              candidate.node_dim = incr_dim(rank(), node_dim);
              candidate.node = node->left;
              candidate = maximum_mapping(candidate);
              if (get_leftmost() == candidate.node)
                { set_leftmost(node); }
              if (get_rightmost() == node)
                { set_rightmost(candidate.node); }
            }
          else
            {
              candidate.data.mapping_dim() = node_dim;
              candidate.node_dim = incr_dim(rank(), node_dim);
              candidate.node = node->right;
              candidate = minimum_mapping(candidate);
              if (get_rightmost() == candidate.node)
                { set_rightmost(node); }
              if (get_leftmost() == node)
                { set_leftmost(candidate.node); }
            }
          swap_node(node, candidate.node);
                  node = candidate.node;
          node_dim = candidate.node_dim;
        }
      SPATIAL_ASSERT_CHECK(!header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(node->right == 0);
      SPATIAL_ASSERT_CHECK(node->left == 0);
      SPATIAL_ASSERT_CHECK(node->parent != 0);
      node_ptr p = node->parent;
      if (header(p))
        {
          set_root(get_header());
          set_leftmost(get_header());
          set_rightmost(get_header());
        }
      else
        {
          if (p->left == node)
            {
              p->left = 0;
              if (get_leftmost() == node) { set_leftmost(p); }
            }
          else
            {
              p->right = 0;
              if (get_rightmost() == node) { set_rightmost(p); }
            }
          // decrease count and rebalance parents up to parent
          while(node->parent != parent)
            {
              node = node->parent;
              node_dim = decr_dim(rank(), node_dim);
              SPATIAL_ASSERT_CHECK(const_link(node)->weight > 1);
              --link(node)->weight;
              if(is_node_unbalanced(node))
                {
                  node = balance_node(node_dim, node);  // recursive!
                }
            }
        }
      SPATIAL_ASSERT_CHECK(!header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      return node;
    }

    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Balancing, typename Alloc>
    inline void
    Relaxed_kdtree<Rank, Key, Value, Compare, Balancing, Alloc>
    ::erase_node_balance
    (dimension_type node_dim, node_ptr node)
    {
      SPATIAL_ASSERT_CHECK(!header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      node_ptr p = node->parent;
      erase_node(node_dim, node);
      node_dim = decr_dim(rank(), node_dim);
      while(!header(p))
        {
          SPATIAL_ASSERT_CHECK(const_link(p)->weight > 1);
          --link(p)->weight;
          if(is_node_unbalanced(p))
            { p = balance_node(node_dim, p); } // balance node
          p = p->parent;
          node_dim = decr_dim(rank(), node_dim);
        }
    }

    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Balancing, typename Alloc>
    inline void
    Relaxed_kdtree<Rank, Key, Value, Compare, Balancing, Alloc>
    ::erase
    (iterator target)
    {
      except::check_node_iterator(target.node);
      node_ptr node = target.node;
      dimension_type node_dim = rank()() - 1;
      while(!header(node))
        {
          node_dim = details::incr_dim(rank(), node_dim);
          node = node->parent;
        }
      except::check_iterator(node, get_header());
      erase_node_balance(node_dim, target.node);
      destroy_node(target.node);
    }

    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Balancing, typename Alloc>
    inline
    typename Relaxed_kdtree<Rank, Key, Value, Compare, Balancing, Alloc>
    ::size_type
    Relaxed_kdtree<Rank, Key, Value, Compare, Balancing, Alloc>
    ::erase
    (const key_type& key)
    {
      size_type cnt = 0;
      while (true)
        {
          if (empty()) break;
          typename equal_region<Self>::iterator_pair found
            = equal_region_range(*this, key);
          if (found.first == found.second) break; // no node matching this key
          erase_node_balance(found.first.data.node_dim, found.first.node);
          destroy_node(found.first.node);
          ++cnt;
        }
      return cnt;
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_RELAXED_KDTREE_TPP
