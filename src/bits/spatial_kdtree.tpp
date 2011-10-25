// -*- C++ -*-

/**
 *  @file   spatial_kdtree.tpp
 *  @brief
 *
 *  Change Log:
 *
 *  - 2009-02-27 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_KDTREE_TPP
#define SPATIAL_KDTREE_TPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#include <vector>
#include <algorithm>

namespace spatial
{
  namespace details
  {

    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Alloc>
    inline void
    Kdtree<Rank, Key, Value, Compare, Alloc>
    ::destroy_all_nodes()
    {
      Node_base::Base_ptr node = get_root();
      while (!Node_base::header(node))
        {
          if (node->left != 0) { node = node->left; }
          else if (node->right != 0) { node = node->right; }
          else
            {
              Node_base::Base_ptr p = node->parent;
              if (Node_base::header(p))
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
              destroy_node(static_cast<Link_type>(node));
              node = p;
            }
        }
    }

    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Alloc>
    inline
    typename Kdtree<Rank, Key, Value, Compare, Alloc>::iterator
    Kdtree<Rank, Key, Value, Compare, Alloc>::insert_node
    (Base_ptr target_node)
    {
      SPATIAL_ASSERT_CHECK(target_node != 0);
      Base_ptr node = get_root();
      dimension_type node_dim = 0;
      if (Node_base::header(node))
        {
          SPATIAL_ASSERT_CHECK(impl_.count_() == 0);
          target_node->parent = get_header();
          set_root(target_node);
          set_leftmost(target_node);
          set_rightmost(target_node);
          ++impl_.count_();
        }
      else
        {
          while (true)
            {
              if (key_comp()(node_dim, Link_().key(target_node),
                             Link_().key(node)))
                {
                  if (node->left != 0)
                    {
                      node = node->left;
                      node_dim = incr_dim(rank(), node_dim);
                    }
                  else
                    {
                      node->left = target_node;
                      target_node->parent = node;
                      if (node == get_leftmost())
                        { set_leftmost(target_node); }
                      ++impl_.count_();
                      break;
                    }
                }
              else
                {
                  if (node->right != 0)
                    {
                      node = node->right;
                      node_dim = incr_dim(rank(), node_dim);
                    }
                  else
                    {
                      node->right = target_node;
                      target_node->parent = node;
                      if (node == get_rightmost())
                        { set_rightmost(target_node); }
                      ++impl_.count_();
                      break;
                    }
                }
            }
        }
      SPATIAL_ASSERT_CHECK(empty() == false);
      SPATIAL_ASSERT_CHECK(impl_.count_() != 0);
      SPATIAL_ASSERT_CHECK(target_node->right == 0);
      SPATIAL_ASSERT_CHECK(target_node->left == 0);
      SPATIAL_ASSERT_CHECK(target_node->parent != 0);
      return iterator(static_cast<Link_type>(target_node));
    }

    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Alloc>
    inline void
    Kdtree<Rank, Key, Value, Compare, Alloc>::copy_structure
    (const Self& other)
    {
      SPATIAL_ASSERT_CHECK(!other.empty());
      SPATIAL_ASSERT_CHECK(empty());
      Const_Base_ptr other_node = other.get_root();
      Base_ptr node = create_node(Link_().val(other_node)); // may throw
      node->parent = get_header();
      set_root(node);
      try
        {
          while(!Node_base::header(other_node))
            {
              if (other_node->left != 0)
                {
                  other_node = other_node->left;
                  Base_ptr target = create_node(Link_().val(other_node));
                  target->parent = node;
                  target->left = target->right = 0;
                  node->left = target;
                  node = node->left;
                }
              else if (other_node->right != 0)
                {
                  other_node = other_node->right;
                  Base_ptr target = create_node(Link_().val(other_node));
                  target->parent = node;
                  target->right = target->left = 0;
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
                      Base_ptr target = create_node(Link_().val(other_node));
                      target->parent = node;
                      target->right = target->left = 0;
                      node->right = target;
                      node = node->right;
                    }
                }
            }
          SPATIAL_ASSERT_CHECK(!empty());
          SPATIAL_ASSERT_CHECK(Node_base::header(other_node));
          SPATIAL_ASSERT_CHECK(Node_base::header(node));
        }
      catch (...)
        { clear(); throw; } // clean-up before re-throw
      set_leftmost(Node_base::minimum(get_root()));
      set_rightmost(Node_base::maximum(get_root()));
      impl_.count_() = other.size();
      SPATIAL_ASSERT_CHECK(size() != 0);
    }

    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Alloc>
    inline void
    Kdtree<Rank, Key, Value, Compare, Alloc>
    ::copy_rebalance(const Self& other)
    {
      SPATIAL_ASSERT_CHECK(empty());
      SPATIAL_ASSERT_CHECK(!other.empty());
      std::vector<Base_ptr> ptr_store;
      ptr_store.reserve(size()); // may throw
      try
        {
          for(const_iterator i = other.begin(); i != other.end(); ++i)
            { ptr_store.push_back(create_node(*i)); } // may throw
        }
      catch (...)
        {
          for(typename std::vector<Base_ptr>::iterator i = ptr_store.begin();
              i != ptr_store.end(); ++i)
            { destroy_node(static_cast<Link_type>(*i)); }
          throw;
        }
      rebalance_node_insert(ptr_store.begin(), ptr_store.end(), 0);
      SPATIAL_ASSERT_CHECK(!empty());
    }

    template<typename Compare, typename Base_ptr, typename Link_>
    struct mapping_compare
    {
      Compare compare;
      dimension_type dimension;

      mapping_compare(const Compare& c, dimension_type d)
        : compare(c), dimension(d) { }

      bool
      operator() (const Base_ptr& x, const Base_ptr& y) const
      {
        return compare(dimension, Link_().key(x), Link_().key(y));
      }
    };

    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Alloc>
    inline void
    Kdtree<Rank, Key, Value, Compare, Alloc>
    ::rebalance_node_insert
    (typename std::vector<Base_ptr>::iterator first,
     typename std::vector<Base_ptr>::iterator last, dimension_type dim)
    {
      SPATIAL_ASSERT_CHECK(first != last);
      SPATIAL_ASSERT_CHECK(dim < dimension());
      typedef mapping_compare<Compare, Base_ptr, Link_> less;
      do
        {
          ptrdiff_t half = (last - first) >> 1;
          std::nth_element(first, first + half, last, less(key_comp(), dim));
          Base_ptr node = *(first + half);
          node->right = node->left = 0;
          insert_node(node);
          dim = incr_dim(rank(), dim);
          if (first + half + 1 != last)
            { rebalance_node_insert(first + half + 1, last, dim); }
          last = first + half;
        }
      while(first != last);
      SPATIAL_ASSERT_CHECK(!empty());
    }

    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Alloc>
    inline void
    Kdtree<Rank, Key, Value, Compare, Alloc>
    ::rebalance()
    {
      if (empty()) return;
      std::vector<Base_ptr> ptr_store;
      ptr_store.reserve(size()); // may throw
      for(iterator i = begin(); i != end(); ++i)
        { ptr_store.push_back(i.node); }
      impl_.initialize();
      impl_.count_() = 0;
      rebalance_node_insert(ptr_store.begin(), ptr_store.end(), 0);
      SPATIAL_ASSERT_CHECK(!empty());
      SPATIAL_ASSERT_CHECK(size() != 0);
    }

    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Alloc>
    inline void
    Kdtree<Rank, Key, Value, Compare, Alloc>::erase_node
    (dimension_type node_dim, Base_ptr node)
    {
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      typedef Mapping_iterator<rank_type, key_type, value_type, node_type,
                               key_compare> mapping_iterator;
      while (node->right != 0 || node->left != 0)
        {
          // If there is nothing on the right, to preserve the invariant, we
          // need to shift the whole sub-tree to the right. This K-d tree
          // rotation is not documented anywhere I've searched. The previous
          // known rotation by J. L. Bentely for erasing nodes in the K-d tree
          // is incorrect for strict invariant (left nodes strictly less than
          // root node). This could explain while it is hard to find an
          // implementation of K-d Tree with the O(log(n)) erase function
          // predicted in his paper.
          mapping_iterator candidate;
          if (node->right == 0)
            {
              node->right = node->left;
              node->left = 0;
              if (get_rightmost() == node)
                { set_rightmost(Node_base::maximum(node->right)); }
              Base_ptr seeker = node->right;
              if (get_leftmost() == seeker) { set_leftmost(node); }
              else
                {
                  while (seeker->left != 0)
                    {
                      seeker = seeker->left;
                      if (get_leftmost() == seeker)
                        { set_leftmost(node); break; }
                    }
                }
            }
          candidate = mapping_iterator::minimum
            (rank(), key_comp(), node_dim, incr_dim(rank(), node_dim),
             node->right);
          if (get_rightmost() == candidate.impl_.node_)
            { set_rightmost(node); }
          if (get_leftmost() == node)
            { set_leftmost(candidate.impl_.node_); }
          spatial::details::swap(*static_cast<Link_type>(candidate.impl_.node_),
                                 *static_cast<Link_type>(node));
          node_dim = candidate.impl_.node_dim_;
        }
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(node->right == 0);
      SPATIAL_ASSERT_CHECK(node->left == 0);
      SPATIAL_ASSERT_CHECK(node->parent != 0);
      Base_ptr p = node->parent;
      if (Node_base::header(p))
        {
          SPATIAL_ASSERT_CHECK(impl_.count_() == 1);
          set_root(get_header());
          set_leftmost(get_header());
          set_rightmost(get_header());
        }
      else if (p->left == node)
        {
          p->left = 0;
          if (get_leftmost() == node) { set_leftmost(p); }
        }
      else
        {
          p->right = 0;
          if (get_rightmost() == node) { set_rightmost(p); }
        }
      --impl_.count_();
      SPATIAL_ASSERT_CHECK((get_header() == get_root())
                           ? (impl_.count_() == 0) : true);
      destroy_node(static_cast<Link_type>(node));
    }

    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Alloc>
    inline void
    Kdtree<Rank, Key, Value, Compare, Alloc>
    ::erase(iterator target)
    {
      except::check_node_iterator_argument(target.node);
      dimension_type node_dim = rank()() - 1;
      Const_Base_ptr node = target.node;
      while (!Node_base::header(node))
        {
          node = node->parent;
          node_dim = incr_dim(rank(), node_dim);
        }
      except::check_iterator_argument(node, get_header());
      erase_node(node_dim, target.node);
    }

    template <typename Rank, typename Key, typename Value, typename Compare,
              typename Alloc>
    inline
    typename Kdtree<Rank, Key, Value, Compare, Alloc>::size_type
    Kdtree<Rank, Key, Value, Compare, Alloc>::erase
    (const key_type& key)
    {
      size_type cnt = 0;
      while (true)
        {
          if (empty()) break;
          std::pair<equal_iterator, equal_iterator> found = equal_range(key);
          if (found.first == found.second) break; // no node matching this key
          erase_node(found.first.impl_.node_dim_(), found.first.impl_.node_);
          ++cnt;
        }
      return cnt;
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_RELAXED_KDTREE_TPP
