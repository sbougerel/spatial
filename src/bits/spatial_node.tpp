// -*- C++ -*-

/**
 *  @file   spatial_node.tpp
 *  @brief
 *
 *  Change Log:
 *
 *  - 2009-10-19 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_NODE_TPP
#define SPATIAL_NODE_TPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

namespace spatial
{
  namespace details
  {

    template <typename Mode>
    inline typename Node<Mode>::ptr
    increment(typename Node<Mode>::ptr x)
    {
      SPATIAL_ASSERT_CHECK(!header(x));
      if (x->right != 0)
        {
          x = x->right;
          while (x->left != 0) { x = x->left; }
        }
      else
        {
          typename Node<Mode>::ptr p = x->parent;
          while (!header(p) && x == p->right)
            { x = p; p = x->parent; }
          x = p;
        }
      return x;
    }

    template <typename Mode>
    inline typename Node<Mode>::ptr
    decrement(typename Node<Mode>::ptr x)
    {
      SPATIAL_ASSERT_CHECK((!header(x) || x->parent != 0));
      if (header(x))
        { x = x->right; }
      else if (x->left != 0)
        {
          typename Node<Mode>::ptr y = x->left;
          while (y->right != 0) { y = y->right; }
          x = y;
        }
      else
        {
          typename Node<Mode>::ptr p = x->parent;
          while (!header(p) && x == p->left)
            { x = p; p = x->parent; }
          x = p;
        }
      return x;
    }

    template <typename Mode>
    inline void swap(Node<Mode>& a, Node<Mode>& b)
    {
      typedef typename Node<Mode>::ptr node_ptr;
      if (&a == &b) return;
      SPATIAL_ASSERT_CHECK(!header(&a));
      SPATIAL_ASSERT_CHECK(!header(&b));
      if (a.parent == &b)
        {
          if (header(b.parent))
            { b.parent->parent = &a; }
          else
            {
              if (b.parent->left == &b) { b.parent->left = &a; }
              else { b.parent->right = &a; }
            }
          if (a.left != 0) { a.left->parent = &b; }
          if (a.right != 0) { a.right->parent = &b; }
          a.parent = b.parent;
          b.parent = &a;
          node_ptr a_left = a.left;
          node_ptr a_right = a.right;
          if (b.left == &a)
            {
              if (b.right != 0) { b.right->parent = &a; }
              a.left = &b;
              a.right = b.right;
            }
          else
            {
              if (b.left != 0) { b.left->parent = &a; }
              a.left = b.left;
              a.right = &b;
            }
          b.left = a_left;
          b.right = a_right;
        }
      else if (b.parent == &a)
        {
          if (header(a.parent))
            { a.parent->parent = &b; }
          else
            {
              if (a.parent->left == &a) { a.parent->left = &b; }
              else { a.parent->right = &b; }
            }
          if (b.left != 0) { b.left->parent = &a; }
          if (b.right != 0) { b.right->parent = &a; }
          b.parent = a.parent;
          a.parent = &b;
          node_ptr b_left = b.left;
          node_ptr b_right = b.right;
          if (a.left == &b)
            {
              if (a.right != 0) { a.right->parent = &b; }
              b.left = &a;
              b.right = a.right;
            }
          else
            {
              if (a.left != 0) { a.left->parent = &b; }
              b.left = a.left;
              b.right = &a;
            }
          a.left = b_left;
          a.right = b_right;
        }
      else
        {
          if (header(a.parent))
            { a.parent->parent = &b; }
          else
            {
              if (a.parent->left == &a) { a.parent->left = &b; }
              else { a.parent->right = &b; }
            }
          if (header(b.parent))
            { b.parent->parent = &a; }
          else
            {
              if (b.parent->left == &b) { b.parent->left = &a; }
              else { b.parent->right = &a; }
            }
          if (a.left != 0) { a.left->parent = &b; }
          if (b.left != 0) { b.left->parent = &a; }
          if (a.right != 0) { a.right->parent = &b; }
          if (b.right != 0) { b.right->parent = &a; }
          std::swap(a.parent, b.parent);
          std::swap(a.left, b.left);
          std::swap(a.right, b.right);
        }
    }

    template <typename Mode>
    inline typename Node<Mode>::const_ptr
    preorder_increment(typename Node<Mode>::const_ptr x)
    {
      if (x->left != 0) { x = x->left; }
      else if (x->right != 0) { x = x->right; }
      else
        {
          typename Node<Mode>::const_ptr p = x->parent;
          while (!header(p) && (x == p->right || p->right == 0))
            { x = p; p = x->parent; }
          x = p;
          if (!header(p)) { x = x->right; }
        }
      return x;
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_NODE_TPP
