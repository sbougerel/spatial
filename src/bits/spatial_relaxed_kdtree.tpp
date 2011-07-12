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

    template<typename Rank, typename Key, typename Compare,
             typename Balancing, typename Alloc, bool ConstIterator>
    inline void
    Relaxed_kdtree<Rank, Key, Compare, Balancing, Alloc, ConstIterator>
    ::copy_structure(const Self& other)
    {
      SPATIAL_ASSERT_CHECK(!other.empty());
      SPATIAL_ASSERT_CHECK(Base::empty());
      Const_Base_ptr other_node = other.get_root();
      Base_ptr node = clone_node(other_node); // may throw
      node->parent = Base::get_header();
      Base::set_root(node);
      try
        {
          while(!Node_base::header(other_node))
            {
              if (other_node->left != 0)
                {
                  other_node = other_node->left;
                  Base_ptr target = clone_node(other_node);
                  target->parent = node;
                  node->left = target;
                  node = node->left;
                }
              else if (other_node->right != 0)
                {
                  other_node = other_node->right;
                  Base_ptr target = clone_node(other_node);
                  target->parent = node;
                  node->right = target;
                  node = node->right;
                }
              else
                {
                  Const_Base_ptr p = other_node->parent;
                  while (!Node_base::header(p)
                         && (other_node == p->right || p->right == 0))
                    {
                      other_node = p;
                      node = node->parent;
                      p = other_node->parent;
                    }
                  other_node = p;
                  node = node->parent;
                  if (!Node_base::header(p))
                    {
                      other_node = other_node->right;
                      Base_ptr target = clone_node(other_node);
                      target->parent = node;
                      node->right = target;
                      node = node->right;
                    }
                }
            }
          SPATIAL_ASSERT_CHECK(!Base::empty());
          SPATIAL_ASSERT_CHECK(Node_base::header(other_node));
          SPATIAL_ASSERT_CHECK(Node_base::header(node));
        }
      catch (...)
        { clear(); throw; } // clean-up before re-throw
      Base::set_leftmost(Node_base::minimum(Base::get_root()));
      Base::set_rightmost(Node_base::maximum(Base::get_root()));
    }

    template<typename Rank, typename Key, typename Compare,
             typename Balancing, typename Alloc, bool ConstIterator>
    inline bool
    Relaxed_kdtree<Rank, Key, Compare, Balancing, Alloc, ConstIterator>
    ::is_node_unbalanced
    (Base_ptr node, weight_type more_left, weight_type more_right)
    {
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      // I believe it is not necessary to balance node when it weighs less than
      // twice the rank. This due to the k-d tree rotating invarient (each
      // dimension is considered in turn). Therefore, it makes little difference
      // for all algorithms to have balanced nodes if there are not enough nodes
      // to balance along all dimensions.
      //
      // Note that while I can theorize this for several algorithms (simple
      // iteration, mapping iteration, range iteration), I still have to perform
      // experiments that reflect this hypothesis.
      if (static_cast<Weighted_node*>(node)->weight <=
          static_cast<weight_type>(Base::dimension()) << 1)
        { return false; }
      return m_balancing(Base::rank(),
                         more_left + (node->left
                                      ? static_cast<Weighted_node*>
                                      (node->left)->weight : 0),
                         more_right + (node->right
                                       ? static_cast<Weighted_node*>
                                       (node->right)->weight : 0));
    }

    template<typename Rank, typename Key, typename Compare,
             typename Balancing, typename Alloc, bool ConstIterator>
    inline typename Relaxed_kdtree<Rank, Key, Compare, Balancing, Alloc,
                                   ConstIterator>::Base_ptr
    Relaxed_kdtree<Rank, Key, Compare, Balancing, Alloc, ConstIterator>
    ::balance_node
    (dimension_type node_dim, Base_ptr node)
    {
      Base_ptr p = node->parent; // Parent does not get swapped!
      bool left_node = (p->left == node);
      // erase first...
      erase_node(node_dim, node);
      Base_ptr replacing = Node_base::header(p) ? p->parent
        : (left_node ? p->left : p->right);
      // ...then re-insert.
      insert_node(node_dim, replacing, node);
      return Node_base::header(p) ? p->parent
        : (left_node ? p->left : p->right);
    }

    template<typename Rank, typename Key, typename Compare,
             typename Balancing, typename Alloc, bool ConstIterator>
    inline
    typename Relaxed_kdtree<Rank, Key, Compare, Balancing, Alloc, ConstIterator>
    ::iterator
    Relaxed_kdtree<Rank, Key, Compare, Balancing, Alloc, ConstIterator>
    ::insert_node
    (dimension_type node_dim, Base_ptr node, Base_ptr new_node)
    {
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      const Compare& cmp = Base::compare();
      while (true)
        {
          SPATIAL_ASSERT_CHECK
            ((node->right
              ? static_cast<Weighted_node*>(node->right)->weight : 0)
             + (node->left
                ? static_cast<Weighted_node*>(node->left)->weight: 0)
             + 1 == static_cast<Weighted_node*>(node)->weight);
          // Balancing equal values on either side of the tree
          if (cmp(node_dim, SPATIAL_KEY(new_node), SPATIAL_KEY(node))
              || (!cmp(node_dim, SPATIAL_KEY(node), SPATIAL_KEY(new_node))
                  && (node->left == 0
                      || (node->right != 0
                          && static_cast<Link_type>(node->left)->weight
                          < static_cast<Link_type>(node->right)->weight))))
            {
              if (node->left == 0)
                {
                  node->left = new_node;
                  new_node->parent = node;
                  if (Base::get_leftmost() == node)
                    { Base::set_leftmost(new_node); }
                  ++static_cast<Weighted_node*>(node)->weight;
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
                      ++static_cast<Weighted_node*>(node)->weight;
                      node = node->left;
                      node_dim = incr_dim(Base::rank(), node_dim);
                    }
                }
            }
          else
            {
              if (node->right == 0)
                {
                  node->right = new_node;
                  new_node->parent = node;
                  if (Base::get_rightmost() == node)
                    { Base::set_rightmost(new_node); }
                  ++static_cast<Weighted_node*>(node)->weight;
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
                      ++static_cast<Weighted_node*>(node)->weight;
                      node = node->right;
                      node_dim = incr_dim(Base::rank(), node_dim);
                    }
                }
            }
        }
      SPATIAL_ASSERT_CHECK(new_node != 0);
      SPATIAL_ASSERT_CHECK(!Node_base::header(new_node));
      SPATIAL_ASSERT_CHECK(!Node_base::header(new_node->parent));
      return iterator(static_cast<Link_type>(new_node));
    }

    template<typename Rank, typename Key, typename Compare,
             typename Balancing, typename Alloc, bool ConstIterator>
    inline typename Relaxed_kdtree<Rank, Key, Compare, Balancing, Alloc,
                                   ConstIterator>::Base_ptr
    Relaxed_kdtree<Rank, Key, Compare, Balancing, Alloc, ConstIterator>
    ::erase_node
    (dimension_type node_dim, Base_ptr node)
    {
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      typedef Mapping_iterator<Rank, Key, node_type, Compare> mapping_iterator;
      Base_ptr parent = node->parent;
      while (node->right != 0 || node->left != 0)
        {
          mapping_iterator candidate;
          if (node->left != 0
              && (node->right == 0
                  || static_cast<Link_type>(node->right)->weight
                  < static_cast<Link_type>(node->left)->weight))
            {
              candidate = mapping_iterator::maximum
                (Base::rank(), Base::compare(), node_dim,
                 incr_dim(Base::rank(), node_dim), node->left);
              if (Base::get_leftmost() == candidate.impl_.node_)
                { Base::set_leftmost(node); }
              if (Base::get_rightmost() == node)
                { Base::set_rightmost(candidate.impl_.node_); }
            }
          else
            {
              candidate = mapping_iterator::minimum
                (Base::rank(), Base::compare(), node_dim,
                 incr_dim(Base::rank(), node_dim), node->right);
              if (Base::get_rightmost() == candidate.impl_.node_)
                { Base::set_rightmost(node); }
              if (Base::get_leftmost() == node)
                { Base::set_leftmost(candidate.impl_.node_); }
            }
          spatial::details::swap(*static_cast<Link_type>(candidate.impl_.node_),
                                 *static_cast<Link_type>(node));
          node_dim = candidate.impl_.node_dim_;
        }
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(node->right == 0);
      SPATIAL_ASSERT_CHECK(node->left == 0);
      SPATIAL_ASSERT_CHECK(node->parent != 0);
      Base_ptr p = node->parent;
      if (Node_base::header(p))
        {
          Base::set_root(Base::get_header());
          Base::set_leftmost(Base::get_header());
          Base::set_rightmost(Base::get_header());
        }
      else
        {
          if (p->left == node)
            {
              p->left = 0;
              if (Base::get_leftmost() == node) { Base::set_leftmost(p); }
            }
          else
            {
              p->right = 0;
              if (Base::get_rightmost() == node) { Base::set_rightmost(p); }
            }
          // decrease count and rebalance parents up to parent
          while(node->parent != parent)
            {
              node = node->parent;
              node_dim = decr_dim(Base::rank(), node_dim);
              SPATIAL_ASSERT_CHECK(static_cast<Weighted_node*>(node)->weight > 1);
              --static_cast<Weighted_node*>(node)->weight;
              if(is_node_unbalanced(node))
                {
                  node = balance_node(node_dim, node);  // recursive!
                }
            }
        }
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      return node;
    }

    template<typename Rank, typename Key, typename Compare,
             typename Balancing, typename Alloc, bool ConstIterator>
    inline void
    Relaxed_kdtree<Rank, Key, Compare, Balancing, Alloc, ConstIterator>
    ::erase_node_finish
    (dimension_type node_dim, Base_ptr node)
    {
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      Base_ptr p = node->parent;
      erase_node(node_dim, node);
      node_dim = decr_dim(Base::rank(), node_dim);
      while(!Node_base::header(p))
        {
          SPATIAL_ASSERT_CHECK(static_cast<Weighted_node*>(p)->weight > 1);
          --static_cast<Weighted_node*>(p)->weight;
          if(is_node_unbalanced(p))
            { p = balance_node(node_dim, p); } // balance node
          p = p->parent;
          node_dim = decr_dim(Base::rank(), node_dim);
        }
    }

    template<typename Rank, typename Key, typename Compare,
             typename Balancing, typename Alloc, bool ConstIterator>
    inline void
    Relaxed_kdtree<Rank, Key, Compare, Balancing, Alloc, ConstIterator>
    ::erase
    (const_iterator target)
    {
      except::check_node_iterator_argument(target.node);
      dimension_type node_dim = Base::rank()() - 1;
      Const_Base_ptr node = target.node;
      while (!Node_base::header(node))
        {
          node = node->parent;
          node_dim = incr_dim(Base::rank(), node_dim);
        }
      except::check_iterator_argument(node, Base::get_header());
      erase_node_finish(node_dim, const_cast<Base_ptr>(target.node));
      destroy_node(static_cast<Link_type>(const_cast<Base_ptr>(target.node)));
    }

    template<typename Rank, typename Key, typename Compare,
             typename Balancing, typename Alloc, bool ConstIterator>
    inline typename Relaxed_kdtree<Rank, Key, Compare, Balancing, Alloc,
                                   ConstIterator>::size_type
    Relaxed_kdtree<Rank, Key, Compare, Balancing, Alloc, ConstIterator>
    ::erase
    (const key_type& key)
    {
      size_type cnt = 0;
      while (true)
        {
          if (Base::empty()) break;
          std::pair<typename Base::equal_iterator,
                    typename Base::equal_iterator>
            found = Base::equal_range(key);
          typename Base::equal_iterator iter = found.first;
          for (; iter != found.second; ++iter)
            {
              /*
               * If the compiler throws you an error at the line below, please,
               * define the '==' operator for the type 'key_type'.
               */
              if (*iter == key)
                {
                  erase_node_finish(iter.impl_.node_dim_(), iter.impl_.node_);
                  destroy_node(static_cast<Link_type>(iter.impl_.node_));
                  ++cnt;
                  // We modified the tree, so we re-initialize the iterators
                  break;
                }
            }
          if (iter == found.second) break;
        }
      return cnt;
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_RELAXED_KDTREE_TPP
