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

    template <typename Rank, typename Key, typename Compare,
              typename Alloc, bool ConstantIterator>
    inline
    typename Kdtree<Rank, Key, Compare, Alloc, ConstantIterator>::iterator
    Kdtree<Rank, Key, Compare, Alloc, ConstantIterator>::insert_node
    (Base_ptr target_node)
    {
      SPATIAL_ASSERT_CHECK(target_node != 0);
      Base_ptr node = Base::get_root();
      dimension_type node_dim = 0;
      target_node->left = target_node->right = 0;
      const Compare& cmp = Base::compare();
      const Rank& rk = Base::rank();
      if (Node_base::header(node))
        {
          SPATIAL_ASSERT_CHECK(m_count == 0);
          target_node->parent = Base::get_header();
          Base::set_root(target_node);
          Base::set_leftmost(target_node);
          Base::set_rightmost(target_node);
          ++m_count;
        }
      else
        {
          while (true)
            {
              if (cmp(node_dim, SPATIAL_KEY(target_node), SPATIAL_KEY(node)))
                {
                  if (node->left != 0)
                    {
                      node = node->left;
                      node_dim = incr_dim(rk, node_dim);
                    }
                  else
                    {
                      node->left = target_node;
                      target_node->parent = node;
                      if (node == Base::get_leftmost())
                        { Base::set_leftmost(target_node); }
                      ++m_count;
                      break;
                    }
                }
              else
                {
                  if (node->right != 0)
                    {
                      node = node->right;
                      node_dim = incr_dim(rk, node_dim);
                    }
                  else
                    {
                      node->right = target_node;
                      target_node->parent = node;
                      if (node == Base::get_rightmost())
                        { Base::set_rightmost(target_node); }
                      ++m_count;
                      break;
                    }
                }
            }
        }
      SPATIAL_ASSERT_CHECK(Base::empty() == false);
      SPATIAL_ASSERT_CHECK(m_count != 0);
      SPATIAL_ASSERT_CHECK(target_node->right == 0);
      SPATIAL_ASSERT_CHECK(target_node->left == 0);
      SPATIAL_ASSERT_CHECK(target_node->parent != 0);
      SPATIAL_ASSERT_CHECK(target_node != 0);
      return iterator(static_cast<Link_type>(target_node));
    }

    template <typename Rank, typename Key, typename Compare,
              typename Alloc, bool ConstantIterator>
    inline void
    Kdtree<Rank, Key, Compare, Alloc, ConstantIterator>
    ::copy_structure(const Self& other)
    {
      SPATIAL_ASSERT_CHECK(!other.empty());
      SPATIAL_ASSERT_CHECK(Base::empty());
      Const_Base_ptr other_node = other.get_root();
      Base_ptr node
        = Base::create_node(SPATIAL_KEY_CONST(other_node)); // may throw
      node->left = node->right = 0;
      node->parent = Base::get_header();
      Base::set_root(node);
      try
        {
          while(!Node_base::header(other_node))
            {
              if (other_node->left != 0)
                {
                  other_node = other_node->left;
                  Base_ptr target
                    = Base::create_node(SPATIAL_KEY_CONST(other_node));
                  target->parent = node;
                  target->left = target->right = 0;
                  node->left = target;
                  node = node->left;
                }
              else if (other_node->right != 0)
                {
                  other_node = other_node->right;
                  Base_ptr target
                    = Base::create_node(SPATIAL_KEY_CONST(other_node));
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
                      Base_ptr target
                        = Base::create_node(SPATIAL_KEY_CONST(other_node));
                      target->parent = node;
                      target->right = target->left = 0;
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
      m_count = other.size();
    }

    template <typename Rank, typename Key, typename Compare,
              typename Alloc, bool ConstantIterator>
    inline void
    Kdtree<Rank, Key, Compare, Alloc, ConstantIterator>
    ::copy_rebalance(const Self& other)
    {
      SPATIAL_ASSERT_CHECK(Base::empty());
      SPATIAL_ASSERT_CHECK(!other.empty());
      std::vector<Base_ptr> ptr_store;
      ptr_store.reserve(size()); // may throw
      try
        {
          for(const_iterator i = other.begin(); i != other.end(); ++i)
            { ptr_store.push_back(Base::create_node(*i)); } // may throw
        }
      catch (...)
        {
          for(typename std::vector<Base_ptr>::iterator i = ptr_store.begin();
              i != ptr_store.end(); ++i)
            { destroy_node(static_cast<Link_type>(*i)); }
          throw;
        }
      rebalance_node_insert(ptr_store.begin(), ptr_store.end(), 0);
      SPATIAL_ASSERT_CHECK(!Base::empty());
    }

    template<typename Compare, typename Base_ptr, typename Link_type>
    struct mapping_compare
    {
      Compare compare;
      dimension_type dimension;

      mapping_compare(const Compare& c, dimension_type d)
        : compare(c), dimension(d) { }

      bool
      operator() (const Base_ptr& x, const Base_ptr& y) const
      {
        return compare(dimension, SPATIAL_KEY(x), SPATIAL_KEY(y));
      }
    };

    template <typename Rank, typename Key, typename Compare,
              typename Alloc, bool ConstantIterator>
    inline void
    Kdtree<Rank, Key, Compare, Alloc, ConstantIterator>
    ::rebalance_node_insert
    (typename std::vector<Base_ptr>::iterator first,
     typename std::vector<Base_ptr>::iterator last, dimension_type dim)
    {
      SPATIAL_ASSERT_CHECK(first != last);
      SPATIAL_ASSERT_CHECK(dim < Base::dimension());
      typedef mapping_compare<Compare, Base_ptr, Link_type> less;
      do
        {
          ptrdiff_t half = (last - first) >> 1;
          std::nth_element(first, first + half, last,
                           less(Base::compare(), dim));
          insert_node(*(first + half));
          dim = incr_dim(Base::rank(), dim);
          if (first + half + 1 != last)
            { rebalance_node_insert(first + half + 1, last, dim); }
          last = first + half;
        }
      while(first != last);
      SPATIAL_ASSERT_CHECK(!Base::empty());
    }

    template <typename Rank, typename Key, typename Compare,
              typename Alloc, bool ConstantIterator>
    inline void
    Kdtree<Rank, Key, Compare, Alloc, ConstantIterator>::rebalance()
    {
      if (Base::empty()) return;
      std::vector<Base_ptr> ptr_store;
      ptr_store.reserve(size()); // may throw
      for(iterator i = Base::begin(); i != Base::end(); ++i)
        { ptr_store.push_back(i.node); }
      Base::initialize();
      m_count = 0;
      rebalance_node_insert(ptr_store.begin(), ptr_store.end(), 0);
      SPATIAL_ASSERT_CHECK(!Base::empty());
      SPATIAL_ASSERT_CHECK(size() != 0);
    }

    template <typename Rank, typename Key, typename Compare,
              typename Alloc, bool ConstantIterator>
    inline void
    Kdtree<Rank, Key, Compare, Alloc, ConstantIterator>::erase_node
    (dimension_type node_dim, Base_ptr node)
    {
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      typedef Mapping_iterator<Rank, Key, node_type, Compare> mapping_iterator;
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
              if (Base::get_rightmost() == node)
                { Base::set_rightmost(Node_base::maximum(node->right)); }
              Base_ptr seeker = node->right;
              if (Base::get_leftmost() == seeker) { Base::set_leftmost(node); }
              else
                {
                  while (seeker->left != 0)
                    {
                      seeker = seeker->left;
                      if (Base::get_leftmost() == seeker)
                        { Base::set_leftmost(node); break; }
                    }
                }
            }
          candidate = mapping_iterator::minimum
            (Base::rank(), Base::compare(), node_dim,
             incr_dim(Base::rank(), node_dim), node->right);
          if (Base::get_rightmost() == candidate.impl_.node_)
            { Base::set_rightmost(node); }
          if (Base::get_leftmost() == node)
            { Base::set_leftmost(candidate.impl_.node_); }
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
          SPATIAL_ASSERT_CHECK(m_count == 1);
          Base::set_root(Base::get_header());
          Base::set_leftmost(Base::get_header());
          Base::set_rightmost(Base::get_header());
        }
      else if (p->left == node)
        {
          p->left = 0;
          if (Base::get_leftmost() == node) { Base::set_leftmost(p); }
        }
      else
        {
          p->right = 0;
          if (Base::get_rightmost() == node) { Base::set_rightmost(p); }
        }
      --m_count;
      SPATIAL_ASSERT_CHECK((Base::get_header() == Base::get_root())
                           ? (m_count == 0) : true);
      destroy_node(static_cast<Link_type>(node));
    }

    template <typename Rank, typename Key, typename Compare,
              typename Alloc, bool ConstantIterator>
    inline void
    Kdtree<Rank, Key, Compare, Alloc, ConstantIterator>::erase
    (const_iterator target)
    {
      except::check_iterator_argument(target.node);
      dimension_type node_dim = Base::rank()() - 1;
      Const_Base_ptr node = target.node;
      while (!Node_base::header(node))
        {
          node = node->parent;
          node_dim = incr_dim(Base::rank(), node_dim);
        }
      except::check_invalid_iterator(node, Base::get_header());
      erase_node(node_dim, const_cast<Base_ptr>(target.node));
    }

    template <typename Rank, typename Key, typename Compare,
              typename Alloc, bool ConstantIterator>
    inline typename Kdtree<Rank, Key, Compare, Alloc,
                           ConstantIterator>::size_type
    Kdtree<Rank, Key, Compare, Alloc, ConstantIterator>::erase
    (const key_type& value)
    {
      size_type cnt = 0;
      while (true)
        {
          if (Base::empty()) break;
          std::pair<typename Base::equal_iterator,
                    typename Base::equal_iterator>
            found = Base::equal_range(value);
          typename Base::equal_iterator iter = found.first;
          for (; iter != found.second; ++iter)
            {
              /*
               * If the compiler throws you an error at the line below, please,
               * define the '==' operator for the type 'key_type'.
               */
              if (*iter == value)
                {
                  erase_node(iter.impl_.node_dim_(), iter.impl_.node_);
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
