// -*- C++ -*-

/**
 *  @file   spatial_range.tpp
 *  @brief
 *
 *  Change Log:
 *
 *  - 2009-10-19 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_RANGE_TPP
#define SPATIAL_RANGE_TPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

namespace spatial
{
  namespace details
  {

    template<typename Rank, typename Key, typename Node,
             typename Predicate, bool Constant, typename Derived>
    inline void
    Range_iterator_base<Rank, Key, Node, Predicate, Constant, Derived>
    ::increment()
    {
      SPATIAL_ASSERT_CHECK(!Node_base::header(impl_.node_));
      SPATIAL_ASSERT_CHECK(impl_.node_ != 0);
      SPATIAL_ASSERT_CHECK(impl_.node_dim_() < rank()());
      do
        {
          if (impl_.node_->right != 0
              && predicate()(impl_.node_dim_(), SPATIAL_KEY(impl_.node_),
                             rank()()) != above)
            {
              impl_.node_ = impl_.node_->right;
              impl_.node_dim_() = incr_dim(rank(), impl_.node_dim_());
              while (impl_.node_->left != 0
                     && predicate()(impl_.node_dim_(), SPATIAL_KEY(impl_.node_),
                                    rank()()) != below)
                {
                  impl_.node_ = impl_.node_->left;
                  impl_.node_dim_() = incr_dim(rank(), impl_.node_dim_());
                }
            }
          else
            {
              Base_ptr p = impl_.node_->parent;
              while (!Node_base::header(p) && impl_.node_ == p->right)
                {
                  impl_.node_ = p;
                  impl_.node_dim_() = decr_dim(rank(), impl_.node_dim_());
                  p = impl_.node_->parent;
                }
              impl_.node_ = p;
              impl_.node_dim_() = decr_dim(rank(), impl_.node_dim_());
            }
        }
      while (!Node_base::header(impl_.node_)
             && match_all(rank(), SPATIAL_KEY(impl_.node_), predicate())
             == false);
      SPATIAL_ASSERT_CHECK(impl_.node_dim_() < rank()());
      SPATIAL_ASSERT_CHECK(impl_.node_ != 0);
    }

    template<typename Rank, typename Key, typename Node,
             typename Predicate, bool Constant, typename Derived>
    inline void
    Range_iterator_base<Rank, Key, Node, Predicate, Constant, Derived>
    ::decrement()
    {
      SPATIAL_ASSERT_CHECK(impl_.node_ != 0);
      SPATIAL_ASSERT_CHECK(impl_.node_dim_() < rank()());
      if (Node_base::header(impl_.node_))
        {
          operator=(maximum(rank(), predicate(), 0, impl_.node_->parent));
          return;
        }
      do
        {
          if (impl_.node_->left != 0
              && predicate()(impl_.node_dim_(), SPATIAL_KEY(impl_.node_),
                             rank()()) != below)
            {
              impl_.node_ = impl_.node_->left;
              impl_.node_dim_() = incr_dim(rank(), impl_.node_dim_());
              while (impl_.node_->right != 0
                     && predicate()(impl_.node_dim_(), SPATIAL_KEY(impl_.node_),
                                    rank()()) != above)
                {
                  impl_.node_ = impl_.node_->right;
                  impl_.node_dim_() = incr_dim(rank(), impl_.node_dim_());
                }
            }
          else
            {
              Base_ptr p = impl_.node_->parent;
              while (!Node_base::header(p) && impl_.node_ == p->left)
                {
                  impl_.node_ = p;
                  impl_.node_dim_() = decr_dim(rank(), impl_.node_dim_());
                  p = impl_.node_->parent;
                }
              impl_.node_ = p;
              impl_.node_dim_() = decr_dim(rank(), impl_.node_dim_());
            }
        }
      while (!Node_base::header(impl_.node_)
             && match_all(rank(), SPATIAL_KEY(impl_.node_), predicate())
             == false);
      SPATIAL_ASSERT_CHECK(impl_.node_dim_() < rank()());
      SPATIAL_ASSERT_CHECK(impl_.node_ != 0);
    }

    template<typename Rank, typename Key, typename Node,
             typename Predicate, bool Constant, typename Derived>
    inline Derived
    Range_iterator_base<Rank, Key, Node, Predicate, Constant, Derived>
    ::minimum
    (const Rank& rank, const Predicate& predicate,
     dimension_type node_dim, Base_ptr node)
    {
      SPATIAL_ASSERT_CHECK(node_dim < rank());
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      SPATIAL_ASSERT_CHECK(node != 0);
      Base_ptr end = node->parent;
      // Quick positioning according to in-order transversal.
      while (node->right != 0
             && predicate(node_dim, SPATIAL_KEY(node), rank()) == below)
        {
          node = node->right;
          node_dim = incr_dim(rank, node_dim);
        }
      while (node->left != 0
             && predicate(node_dim, SPATIAL_KEY(node), rank()) != below)
        {
          node = node->left;
          node_dim = incr_dim(rank, node_dim);
        }
      // Start algorithm.
      do
        {
          if (match_all(rank, SPATIAL_KEY(node), predicate) == true)
            { break; }
          if (node->right != 0
              && predicate(node_dim, SPATIAL_KEY(node), rank()) != above)
            {
              node = node->right;
              node_dim = incr_dim(rank, node_dim);
              while (node->left != 0
                     && predicate(node_dim, SPATIAL_KEY(node), rank()) != below)
                {
                  node = node->left;
                  node_dim = incr_dim(rank, node_dim);
                }
            }
          else
            {
              Base_ptr p = node->parent;
              while (p != end && node == p->right)
                {
                  node = p;
                  node_dim = decr_dim(rank, node_dim);
                  p = node->parent;
                }
              node = p;
              node_dim = decr_dim(rank, node_dim);
            }
        }
      while (node != end);
      SPATIAL_ASSERT_CHECK(node_dim < rank());
      SPATIAL_ASSERT_CHECK(node != 0);
      return Derived(rank, predicate, node_dim, static_cast<Link_type>(node));
    }

    template<typename Rank, typename Key, typename Node,
             typename Predicate, bool Constant, typename Derived>
    inline Derived
    Range_iterator_base<Rank, Key, Node, Predicate, Constant, Derived>
    ::maximum
    (const Rank& rank, const Predicate& predicate,
     dimension_type node_dim, Base_ptr node)
    {
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(node_dim < rank());
      SPATIAL_ASSERT_CHECK(!Node_base::header(node));
      Base_ptr end = node->parent;
      // Quick positioning according to in-order transversal.
      while (node->left != 0
             && predicate(node_dim, SPATIAL_KEY(node), rank()) == above)
        {
          node = node->left;
          node_dim = incr_dim(rank, node_dim);
        }
      while (node->right != 0
             && predicate(node_dim, SPATIAL_KEY(node), rank()) != above)
        {
          node = node->right;
          node_dim = incr_dim(rank, node_dim);
        }
      // Start algorithm.
      do
        {
          if (match_all(rank, SPATIAL_KEY(node), predicate) == true)
            { break; }
          if (node->left != 0
              && predicate(node_dim, SPATIAL_KEY(node), rank()) != below)
            {
              node = node->left;
              node_dim = incr_dim(rank, node_dim);
              while (node->right != 0
                     && predicate(node_dim, SPATIAL_KEY(node), rank()) != above)
                {
                  node = node->right;
                  node_dim = incr_dim(rank, node_dim);
                }
            }
          else
            {
              Base_ptr p = node->parent;
              while (p != end && node == p->left)
                {
                  node = p;
                  node_dim = decr_dim(rank, node_dim);
                  p = node->parent;
                }
              node = p;
              node_dim = decr_dim(rank, node_dim);
            }
        }
      while (node != end);
      SPATIAL_ASSERT_CHECK(node_dim < rank());
      SPATIAL_ASSERT_CHECK(node != 0);
      return Derived(rank, predicate, node_dim, static_cast<Link_type>(node));
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_RANGE_TPP
