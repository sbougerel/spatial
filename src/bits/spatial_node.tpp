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

    inline Node_base::Base_ptr
    Node_base::increment(Node_base::Base_ptr x)
    {
      if (x->right != 0)
	{
	  x = x->right;
	  while (x->left != 0) { x = x->left; }
	}
      else
	{
	  Base_ptr p = x->parent;
	  while (!Node_base::header(p) && x == p->right)
	    { x = p; p = x->parent; }
	  x = p;
	}
      return x;
    }

    inline Node_base::Base_ptr
    Node_base::decrement(Node_base::Base_ptr x)
    {
      if (Node_base::header(x))
	{ x = x->right; }
      else if (x->left != 0)
	{
	  Base_ptr y = x->left;
	  while (y->right != 0) { y = y->right; }
	  x = y;
	}
      else
	{
	  Base_ptr p = x->parent;
	  while (!Node_base::header(p) && x == p->left)
	    { x = p; p = x->parent; }
	  x = p;
	}
      return x;
    }

    inline
    void swap(Node_base& a, Node_base& b)
    {
      if (&a == &b) return;
      SPATIAL_ASSERT_CHECK(!Node_base::header(&a));
      SPATIAL_ASSERT_CHECK(!Node_base::header(&b));
      if (a.parent == &b)
	{
	  if (Node_base::header(b.parent))
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
	  Node_base::Base_ptr a_left = a.left;
	  Node_base::Base_ptr a_right = a.right;
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
	  if (Node_base::header(a.parent))
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
	  Node_base::Base_ptr b_left = b.left;
	  Node_base::Base_ptr b_right = b.right;
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
	  if (Node_base::header(a.parent))
	    { a.parent->parent = &b; }
	  else
	    {
	      if (a.parent->left == &a) { a.parent->left = &b; }
	      else { a.parent->right = &b; }
	    }
	  if (Node_base::header(b.parent))
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

    inline Node_base::Const_Base_ptr
    Node_base::preorder_increment(Node_base::Const_Base_ptr x)
    {
      if (x->left != 0) { x = x->left; }
      else if (x->right != 0) { x = x->right; }
      else
	{
	  Const_Base_ptr p = x->parent;
	  while (!Node_base::header(p)
		 && (x == p->right || p->right == 0))
	    { x = p; p = x->parent; }
	  x = p;
	  if (!Node_base::header(p)) { x = x->right; }
	}
      return x;
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_NODE_TPP
