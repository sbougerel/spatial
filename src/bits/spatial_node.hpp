// -*- C++ -*-

/**
 *  @file   spatial_node.hpp
 *  @brief  Defines the basic nodes and associated iterators.
 *
 *  Change Log:
 *
 *  - 2009-10-19 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_NODE_HPP
#define SPATIAL_NODE_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

namespace spatial
{
  namespace details
  {
    /**
     *  @brief  The basic node for any tree in the library with the basic functions
     *  on these nodes.
     *
     *  All trees in the library have the following invariant: at the head, the
     *  left node points to the head itself. This way, the header node can be
     *  identified readily.
     */
    struct Node_base
    {
      typedef Node_base*       Base_ptr;
      typedef const Node_base* Const_Base_ptr;

      Base_ptr parent;
      Base_ptr left;
      Base_ptr right;

      /**
       *  @brief  Check if node is a header node.
       */
      static bool
      header(Const_Base_ptr x)
      { return (x->left == x); }

      //@{
      /**
       *  @brief  Reach the left most node.
       *
       *  Should not be used on empty trees.
       */
      static Base_ptr
      minimum(Base_ptr x)
      {
        while (x->left != 0) x = x->left;
        return x;
      }

      static Const_Base_ptr
      minimum(Const_Base_ptr x)
      {
        return minimum(const_cast<Base_ptr>(x));
      }
      //@}

      //@{
      /**
       *  @brief  Reach the left most node.
       *
       *  Should not be used on empty trees.
       */
      static Base_ptr
      maximum(Base_ptr x)
      {
        while (x->right != 0) x = x->right;
        return x;
      }

      static Const_Base_ptr
      maximum(Const_Base_ptr x)
      {
        return maximum(const_cast<Base_ptr>(x));
      }
      //@}

      //@{
      /**
       *  @brief  Reach the next node in symetric transversal order.
       *
       *  Should not be used on empty trees.
       */
      static Base_ptr
      increment(Base_ptr x);

      static Const_Base_ptr
      increment(Const_Base_ptr x)
      {
        return increment(const_cast<Base_ptr>(x));
      }
      //@}

      //@{
      /**
       *  @brief  Reach the previous node in symetric transversal order.
       *
       *  Should not be used on empty trees.
       */
      static Base_ptr
      decrement(Base_ptr x);

      static Const_Base_ptr
      decrement(Const_Base_ptr x)
      {
        return decrement(const_cast<Base_ptr>(x));
      }
      //@}

      /**
       *  @brief  Reach the next node in preorder transversal.
       *
       *  Should not be used on empty trees.
       */
      static Const_Base_ptr
      preorder_increment(Const_Base_ptr x);
    };

    /**
     *  @brief  Define the header node type for all tree types.
     *
     *  In the header node, the left node is always pointing to the header node
     *  itself. Thus identifying the header node is equivalent to:
     *
     *    (node->left == node)
     *
     *  This would return true if node is the header node of the tree.
     */
    struct Header_node : Node_base
    {
      Node_base::Base_ptr leftmost;
    };

    /**
     *  @brief  Define the node type for a Kdtree that contains the value member.
     */
    template<typename Value>
    struct Kdtree_node : Node_base
    {
      Value value;
    };

    /**
     *  @brief  Define a weighted node type for the relaxed k-d tree.
     */
    template<typename Value>
    struct Relaxed_kdtree_node : Node_base
    {
      weight_type weight;
      Value value;
    };

    //@{
    /**
     *  @brief  Swaps nodes position in the tree.
     *  @relates Node_base
     *
     *  This function does not updates the left-most and right-most pointers of
     *  the tree where the nodes belongs to. This is left to the responsibility of
     *  the caller.
     */
    void swap(Node_base& a, Node_base& b);
    template<typename Value>
    inline void swap(Kdtree_node<Value>& a, Kdtree_node<Value>& b)
    {
      swap(*static_cast<Node_base::Base_ptr>(&a),
           *static_cast<Node_base::Base_ptr>(&b));
    }
    template<typename Value>
    inline void swap(Relaxed_kdtree_node<Value>& a, Relaxed_kdtree_node<Value>& b)
    {
      std::swap(a.weight, b.weight);
      swap(*static_cast<Node_base::Base_ptr>(&a),
           *static_cast<Node_base::Base_ptr>(&b));
    }
    //@}

    /**
     *  @brief  A bidirectional iterator traversing all node in the tree in
     *  inorder traversal. This iterator provides mutable access to the nodes in
     *  the tree.
     */
    template<typename Value, typename Node>
    struct Node_iterator
    {
      typedef Value                            value_type;
      typedef Value&                           reference;
      typedef Value*                           pointer;
      typedef std::ptrdiff_t                   difference_type;
      typedef std::bidirectional_iterator_tag  iterator_category;

    private:
      typedef Node_iterator<Value, Node>       Self;
      typedef Node_base::Base_ptr              Base_ptr;
      typedef Node*                            Link_type;

    public:
      Node_iterator() : node() { }

      explicit
      Node_iterator(Link_type x) : node(x) { }

      reference
      operator*() const
      { return static_cast<Link_type>(node)->value; }

      pointer
      operator->() const
      { return &static_cast<Link_type>(node)->value; }

      Self&
      operator++()
      {
        node = Node_base::increment(node);
        return *this;
      }

      Self
      operator++(int)
      {
        Self tmp = *this;
        node = Node_base::increment(node);
        return tmp;
      }

      Self&
      operator--()
      {
        node = Node_base::decrement(node);
        return *this;
      }

      Self
      operator--(int)
      {
        Self tmp = *this;
        node = Node_base::decrement(node);
        return tmp;
      }

      bool
      operator==(const Self& x) const
      { return node == x.node; }

      bool
      operator!=(const Self& x) const
      { return node != x.node; }

      Base_ptr node;
    };

    /**
     *  @brief  A bidirectional iterator traversing all node in the tree in
     *  inorder traversal. This iterator provides constant access to the nodes
     *  in the tree.
     */
    template<typename Value, typename Node>
    struct Const_Node_iterator
    {
      typedef Value                             value_type;
      typedef const Value&                      reference;
      typedef const Value*                      pointer;
      typedef std::ptrdiff_t                    difference_type;
      typedef std::bidirectional_iterator_tag   iterator_category;

    private:
      typedef Const_Node_iterator<Value, Node>  Self;
      typedef Node_base::Const_Base_ptr         Base_ptr;
      typedef const Node*                       Link_type;

      typedef Node_iterator<Value, Node>        iterator;

    public:
      Const_Node_iterator() : node() { }

      explicit
      Const_Node_iterator(Link_type x) : node(x) { }

      Const_Node_iterator(const iterator& it) : node(it.node) { }

      reference
      operator*() const
      { return static_cast<Link_type>(node)->value; }

      pointer
      operator->() const
      { return &static_cast<Link_type>(node)->value; }

      Self&
      operator++()
      {
        node = Node_base::increment(node);
        return *this;
      }

      Self
      operator++(int)
      {
        Self tmp = *this;
        node = Node_base::increment(node);
        return tmp;
      }

      Self&
      operator--()
      {
        node = Node_base::decrement(node);
        return *this;
      }

      Self
      operator--(int)
      {
        Self tmp = *this;
        node = Node_base::decrement(node);
        return tmp;
      }

      bool
      operator==(const Self& x) const
      { return node == x.node; }

      bool
      operator!=(const Self& x) const
      { return node != x.node; }

      Base_ptr node;
    };

    /**
     *  A forward iterator that iterates through the node of the container in
     *  preorder transversal. It provides constant access to the node. It is
     *  used mainly to clone the tree.
     */
    template<typename Value, typename Node>
    struct Preorder_node_iterator
    {
      typedef Value                                value_type;
      typedef const Value&                         reference;
      typedef const Value*                         pointer;
      typedef std::ptrdiff_t                       difference_type;
      typedef std::forward_iterator_tag            iterator_category;

    private:
      typedef Preorder_node_iterator<Value, Node>  Self;
      typedef Node_base::Const_Base_ptr            Base_ptr;
      typedef const Node*                          Link_type;

    public:
      Preorder_node_iterator() : node() { }

      explicit
      Preorder_node_iterator(Link_type x) : node(x) { }

      reference
      operator*()
      { return static_cast<Link_type>(node)->value; }

      pointer
      operator->()
      { return &static_cast<Link_type>(node)->value; }

      Self&
      operator++()
      {
        node = Node_base::preorder_increment(node);
        return *this;
      }

      Self
      operator++(int)
      {
        Self tmp = *this;
        node = Node_base::preorder_increment(node);
        return tmp;
      }

      bool
      operator==(const Self& x) const
      { return node == x.node; }

      bool
      operator!=(const Self& x) const
      { return node != x.node; }

      Base_ptr node;
    };

    //@{
    /**
     *  Help with retrieval of the key_type from the value_type and the
     *  node_type in a container.
     */
    template <typename Key, typename Value, typename Node>
    struct Linker
    {
      const Node* link(Node_base::Const_Base_ptr ptr) const
      { return static_cast<const Node*>(ptr); }
      Node* link(Node_base::Base_ptr ptr) const
      { return static_cast<Node*>(ptr); }

      const Key& key(Node_base::Const_Base_ptr ptr) const
      { return link(ptr)->value.first; }
      const Key& key(Node_base::Base_ptr ptr) const
      { return link(ptr)->value.first; }

      const Value& val(Node_base::Const_Base_ptr ptr) const
      { return link(ptr)->value; }
      Value& val(Node_base::Base_ptr ptr) const
      { return link(ptr)->value; }

      const Key& pick(const Value& value) const { return value.first; }
      Key& pick(Value& value) const { return value.first; }
    };

    // Specialization for containers where Key is same as Value
    template <typename Key, typename Node>
    struct Linker<Key, Key, Node>
    {
      const Node* link(Node_base::Const_Base_ptr ptr) const
      { return static_cast<const Node*>(ptr); }
      Node* link(Node_base::Base_ptr ptr) const
      { return static_cast<Node*>(ptr); }

      const Key& key(Node_base::Const_Base_ptr ptr) const
      { return link(ptr)->value; }
      const Key& key(Node_base::Base_ptr ptr) const
      { return link(ptr)->value; }

      const Key& val(Node_base::Const_Base_ptr ptr) const
      { return link(ptr)->value; }
      Key& val(Node_base::Base_ptr ptr) const
      { return link(ptr)->value; }

      const Key& pick(const Key& value) const { return value; }
      Key& pick(Key& value) const { return value; }
    };
    //@}

  } // namespace details
} // namespace spatial

#include "spatial_node.tpp"

#endif // SPATIAL_NODE_HPP
