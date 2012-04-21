// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_node.hpp
 *  \brief  Defines the basic nodes and associated iterators.
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
     *  The basic node for any tree in the library. It contains only the
     *  information necessary to iterate through all nodes and the library, as
     *  well as access the values of a node. However it does not link to the
     *  value by itself.
     *
     *  All nodes in all containers in the library obey the invariant: if at the
     *  head, the \c left points to the head itself, always, by convention. This
     *  way, the header node can be identified readily. It is a very important
     *  feature of the library that only by looking at a node, one can tell
     *  whether the head of the tree has been reached or not.
     *
     *  Once at the head, the parent pointer will point to the root of the tree,
     *  while the right pointer will point to the right most node in the
     *  tree. Therefore, to store the left-most node in the tree, an additional
     *  pointer is required in each container.
     *
     *  This class also holds a model of \ref LinkMode to access the key
     *  and values of the node, without holding these values. It deliberately
     *  does not hold these values for 2 reasons:
     *
     *  \ul Not all nodes store their keys and values in the same way.
     *  \li Some nodes have more information than just key and values.
     *  \ul
     *
     *  Additionally, when deferencing a pointer from a node, only the minimum
     *  amount of information is transferred to the variable holding the node,
     *  which happens in several algorithms.
     *
     *  \tparam Mode A model of \ref LinkMode
     */
    template <typename Mode>
    struct Node
    {
      //! A pointer to any node, including the header node.
      typedef Node<Mode>*       ptr;
      //! A constant pointer to any node, including the header node.
      typedef const Node<Mode>* const_ptr;
      //! The mode type that indicate how to reach the key and/or the
      //! value from the link.
      typedef Mode              mode_type;

      /**
       *  A pointer to the parent of the current node. When we are at head, the
       *  parent pointer is equal to the current node. In all other cases it is
       *  different. When this value is null, the node has not been initialized
       *  and it is dangling.
       */
      ptr parent;

      /**
       *  A pointer to the left child node of the current node. If we are at the
       *  head, this pointer points to the left most node in the tree. If there are
       *  no node to the left, the pointer is null.
       */
      ptr left;

      /**
       *  A pointer to the right child node of the current node. If we are at the
       *  head, this pointer points to the right most node in the tree. If there are
       *  no node to the right, the pointer is null.
       */
      ptr right;
    };

    /**
     *  @brief  Check if node is a header node.
     */
    template <typename Mode>
    inline bool header(const Node<Mode>* x)
    { return (x->left == x); }

    //@{
    /**
     *  \brief  Reach the left most node.
     *
     *  Should not be used on header nodes.
     *  \tparam Mode a model of \ref LinkMode.
     *  \param x a pointer to a node.
     */
    template <typename Mode>
    inline Node<Mode>* minimum(Node<Mode>* x)
    {
      SPATIAL_ASSERT_CHECK(!header(x));
      while (x->left != 0) x = x->left; return x;
    }

    template <typename Mode>
    inline const Node<Mode>* minimum(const Node<Mode>* x)
    {
      SPATIAL_ASSERT_CHECK(!header(x));
      while (x->left != 0) x = x->left; return x;
    }
    //@}

    //@{
    /**
     *  @brief  Reach the left most node.
     *
     *  Should not be used on header nodes.
     *  \tparam Mode a model of \ref LinkMode.
     *  \param x a pointer to a node.
     */
    template <typename Mode>
    inline Node<Mode>* maximum(Node<Mode>* x)
    {
      SPATIAL_ASSERT_CHECK(!header(x));
      while (x->right != 0) x = x->right; return x;
    }

    template <typename Mode>
    inline const Node<Mode>* maximum(const Node<Mode>* x)
    {
      SPATIAL_ASSERT_CHECK(!header(x));
      while (x->right != 0) x = x->right; return x;
    }
    //@}

    //@{
    /**
     *  @brief  Reach the next node in symetric transversal order.
     *
     *  Should not be used on header nodes.
     *  \tparam Mode a model of \ref LinkMode.
     *  \param x a pointer to a node.
     */
    template <typename Mode> Node<Mode>*
    increment(Node<Mode>* x);

    template <typename Mode>
    inline const Node<Mode>*
    increment(const Node<Mode>* x)
    { return increment(const_cast<Node<Mode>*>(x)); }
    //@}

    //@{
    /**
     *  @brief  Reach the previous node in symetric transversal order.
     *
     *  Should not be used on empty trees, but can be used on header nodes when
     *  the tree is not empty.
     *  \tparam Mode a model of \ref LinkMode.
     *  \param x a pointer to a node.
     */
    template <typename Mode>
    typename Node<Mode>::ptr
    decrement(typename Node<Mode>::ptr x);

    template <typename Mode>
    inline typename Node<Mode>::const_ptr
    decrement(typename Node<Mode>::const_ptr x)
    { return decrement(const_cast<typename Node<Mode>::ptr>(x)); }
    //@}

    /**
     *  Reach the next node in preorder transversal.
     *
     *  Should not be used on empty trees or when the pointer to the node is a
     *  the head, which results in undefined behavior.
     *  \tparam Mode a model of \ref LinkMode.
     *  \param x a pointer to a node.
     */
    template <typename Mode> typename const Node<Mode>*
    preorder_increment(const Node<Mode>* x);

    /**
     *  Returns the modulo of a node's heigth by a container's rank. This, in
     *  effect, gives the current dimension along which the node's invarient is
     *  evaluated.
     *
     *  If \c x points to the header, by convention the highest dimension for a
     *  node invariant is returned.
     *
     *  \tparam Mode A model of \ref LinkMode.
     *  \tparam Rank A model of \ref Rank.
     *  \param x A constant pointer to a node.
     *  \param r An object of type Rank.
     */
    template <typename Mode, typename Rank>
    inline dimension_type
    modulo(typename Node<Mode>::const_ptr x, const Rank& r)
    {
      dimension_type d = r() - 1;
      while(!header(x)) { d = incr_dim(r, d); x = x->parent; }
      return d;
    }

    //@{
    /*
     *  The category of invariants for a \kdtree node: strict or relaxed.
     *
     *  This tag is an indicator for one of the library's most central concepts:
     *  different types of node use different sets of invariant, or "rules" on
     *  their nodes.
     *
     *  With \f$N\f$ the current node, \f$d\f$ the current dimension of
     *  comparison for the node (or: the node's dimension), \f$k(N)\f$ the
     *  function that associate a node to its key, \f$l(N)\f$ and \f$r(N)\f$ the
     *  functions that associate a node to its left node and right node
     *  respectively, then for each tag, the following invarient rules are
     *  respected: \f[ \begin{Bmatrix} k(N)_d \geq  k(l(N))_d \\ k(N)_d \leq
     *  k(r(N))_d \end{Bmatrix} \f] for relaxed invarient tags, and: \f[
     *  \begin{Bmatrix} k(N)_d \gt  k(l(N))_d \\ k(N)_d \leq k(r(N))_d
     *  \end{Bmatrix} \f] for strict invarient tags.
     *
     *  In other words, in relaxed invarient \kdtree, values equal to the node's
     *  value can be found both in the left and right sub-trees of the node,
     *  while in strict invarient \kdtree, value equal to the node's value can
     *  only be found on the right side of the node. This allows strict
     *  invarient \kdtree to be slightly faster during search if many similar
     *  values are present in the nodes. But it on the contrary, it makes them
     *  much harder to rebalance.
     *
     *  \see LibraryInternals
     */
    struct relaxed_invariant_tag { };
    struct strict_invariant_tag { };
    //@}

    /**
     *  Define the link type for a Kdtree that contains the value member.
     *
     *  This link also contains the linking information, so it is a model of the
     *  \ref LinkMode concept.
     *
     *  \tparam Key The key type that is held by the Kdtree_link.
     *  \tparam Value The value type that is held by the Kdtree_link.
     */
    template<typename Key, typename Value>
    struct Kdtree_link : Node<Kdtree_link<Key, Value> >
    {
      //! The link to the key type.
      typedef Key                                  key_type;
      //! The link to the value type.
      typedef Value                                value_type;
      //! The link to the node type, which is the node itself, since link
      //! information are also contained in this node.
      typedef Kdtree_link<Key, Value>              link_type;
      //! The link pointer which is often used, has a dedicated type.
      typedef link_type*                           link_ptr;
      //! The constant link pointer which is often used, has a dedicated type.
      typedef const link_type*                     const_link_ptr;
      //! The node pointer type deduced from the mode.
      typedef typename Node<link_type>::ptr        node_ptr;
      //! The constant node pointer deduced from the mode.
      typedef typename Node<link_type>::const_ptr  const_node_ptr;
      //! The category of invariant associated with this mode.
      typedef strict_invariant_tag                 invariant_category;

      /**
       *  The value of the node, required by the @ref Link concept.
       *
       *  In *-map containers, the value is necessarily a pair, with the first
       *  member being a key, and the second member being the mapped type. In
       *  *-set containers, the value and the key are one and the same thing.
       */
      Value value;
    };

    /**
     *  \brief  Define a weighted link type for the relaxed @kdtree.
     *
     *  This link also contains the linking information, so it is a model of the
     *  \ref LinkMode concept.
     *
     *  \tparam Key The key type that is held by the Kdtree_link.
     *  \tparam Value The value type that is held by the Kdtree_link.
     */
    template<typename Key, typename Value>
    struct Relaxed_kdtree_link : Node<Relaxed_kdtree_link<Key, Value> >
    {
      //! The link to the key type.
      typedef Key                                 key_type;
      //! The link to the value type.
      typedef Value                               value_type;
      //! The link type, which is also itself, since mode are also
      //! contained in this type.
      typedef Relaxed_kdtree_link<Key, Value>     link_type;
      //! The link pointer which is often used, has a dedicated type.
      typedef link_type*                          link_ptr;
      //! The constant link pointer which is often used, has a dedicated type.
      typedef const link_type*                    const_link_ptr;
      //! The node pointer type deduced from the mode.
      typedef typename Node<link_type>::ptr       node_ptr;
      //! The constant node pointer deduced from the mode.
      typedef typename Node<link_type>::const_ptr const_node_ptr;
      //! The category of invariant with associated with this mode.
      typedef relaxed_invariant_tag               invariant_category;

      //! The weight is equal to 1 plus the amount of child nodes below the
      //! current node. It is always equal to 1 at least.
      weight_type weight;

      /**
       *  The value of the node, required by the @ref Link concept.
       *
       *  In *-map containers, the value is necessarily a pair, with the first
       *  member being a key, and the second member being the mapped type. In
       *  *-set containers, the value and the key are one and the same thing.
       */
      Value value;
    };

    //@{
    /**
     *  This function converts a pointer on a node into a link for a \ref
     *  Kdtree_link type.
     *  \tparam Key the key type for the \ref Kdtree_link.
     *  \tparam Value the value type for the \ref Kdtree_link.
     *  \param node the node to convert to a link.
     */
    template <typename Key, typename Value>
    inline Kdtree_link<Key, Value>&
    link(Node<Kdtree_link<Key, Value> >& node)
    {
      return *static_cast<Kdtree_link<Key, Value>*>(&node);
    }

    template <typename Key, typename Value>
    inline const Kdtree_link<Key, Value>&
    const_link(const Node<Kdtree_link<Key, Value> >& node)
    {
      return *static_cast<const Kdtree_link<Key, Value>*>(&node);
    }
    //@}

    //@{
    /**
     *  This function converts a pointer on a node into a key for a \ref
     *  Kdtree_link type.
     *
     *  This overload is used when both the key type and the value type of the
     *  \ref Kdtree_link are similar.
     *  \tparam Value the value type for the \ref Kdtree_link.
     *  \param node the node to convert to a key.
     */
    template <typename Value>
    inline typename Kdtree_link<Value, Value>::key_type&
    key(Node<Kdtree_link<Value, Value> >& node)
    {
      return static_cast<Kdtree_link<Value, Value>*>(&node)->value;
    }

    template <typename Value>
    inline const typename Kdtree_link<Value, Value>::key_type&
    const_key(const Node<Kdtree_link<Value, Value> >& node)
    {
      return static_cast<const Kdtree_link<Value, Value>*>(&node)->value;
    }
    //@}

    //@{
    /**
     *  This function converts a pointer on a node into a key for a \ref
     *  Kdtree_link type.
     *  \tparam Key the key type for the \ref Kdtree_link.
     *  \tparam Value the value type for the \ref Kdtree_link.
     *  \param node the node to convert to a key.
     */
    template <typename Key, typename Value>
    inline typename Kdtree_link<Key, Value>::key_type&
    key(Node<Kdtree_link<Key, Value> >& node)
    {
      return static_cast<Kdtree_link<Key, Value>*>(&node)->value.first;
    }

    template <typename Key, typename Value>
    inline const typename Kdtree_link<Key, Value>::key_type&
    const_key(const Node<Kdtree_link<Key, Value> >& node)
    {
      return static_cast<const Kdtree_link<Key, Value>*>(&node)->value.first;
    }
    //@}

    //@{
    /**
     *  This function converts a pointer on a node into a value for a \ref
     *  Kdtree_link type.
     *  \tparam Key the key type for the \ref Kdtree_link.
     *  \tparam Value the value type for the \ref Kdtree_link.
     *  \param node the node to convert to a key.
     */
    template <typename Key, typename Value>
    inline typename Kdtree_link<Key, Value>::value_type&
    value(Node<Kdtree_link<Key, Value> >& node)
    {
      return static_cast<Kdtree_link<Key, Value>*>(&node)->value;
    }

    template <typename Key, typename Value>
    inline const typename Kdtree_link<Key, Value>::value_type&
    const_value(const Node<Kdtree_link<Key, Value> >& node)
    {
      return static_cast<const Kdtree_link<Key, Value>*>(&node)->value;
    }
    //@}

    //@{
    /**
     *  This function converts a pointer on a node into a link for a \ref
     *  Relaxed_kdtree_link type.
     *  \tparam Key the key type for the \ref Relaxed_kdtree_link.
     *  \tparam Value the value type for the \ref Relaxed_kdtree_link.
     *  \param node the node to convert to a key.
     */
    template <typename Key, typename Value>
    inline Relaxed_kdtree_link<Key, Value>&
    link(Node<Relaxed_kdtree_link<Key, Value> >& node)
    {
      return *static_cast<Relaxed_kdtree_link<Key, Value>*>(&node);
    }

    template <typename Key, typename Value>
    inline const Relaxed_kdtree_link<Key, Value>&
    const_link(const Node<Relaxed_kdtree_link<Key, Value> >& node)
    {
      return *static_cast<const Relaxed_kdtree_link<Key, Value>*>(&node);
    }
    //@}

    //@{
    /**
     *  This function converts a pointer on a node into a key for a @ref
     *  Relaxed_kdtree_link type.
     *  This overload is used when both the key type and the value type of the
     *  \ref Relaxed_kdtree_link are of the same type, e.g. in *-set containers.
     *  \tparam Value the value type for the \ref Relaxed_kdtree_link.
     *  \param node the node to convert to a key.
     */
    template <typename Value>
    inline typename Relaxed_kdtree_link<Value, Value>::key_type&
    key(Node<Relaxed_kdtree_link<Value, Value> >& node)
    {
      return static_cast<Relaxed_kdtree_link<Value, Value>*>(&node)->value;
    }

    template <typename Value>
    inline const typename Relaxed_kdtree_link<Value, Value>::key_type&
    const_key(const Node<Relaxed_kdtree_link<Value, Value> >& node)
    {
      return static_cast
        <const Relaxed_kdtree_link<Value, Value>*>(&node)->value;
    }
    //@}

    //@{
    /**
     *  This function converts a pointer on a node into a key for a @ref
     *  Relaxed_kdtree_link type.
     *  \tparam Key the key type for the \ref Relaxed_kdtree_link
     *  \tparam Value the value type for the \ref Relaxed_kdtree_link.
     *  \param node the node to convert to a key.
     */
    template <typename Key, typename Value>
    inline typename Relaxed_kdtree_link<Key, Value>::key_type&
    key(Node<Relaxed_kdtree_link<Key, Value> >& node)
    {
      return static_cast<Relaxed_kdtree_link<Key, Value>*>(&node)->value.first;
    }

    template <typename Key, typename Value>
    inline const typename Relaxed_kdtree_link<Key, Value>::key_type&
    const_key(const Node<Relaxed_kdtree_link<Key, Value> >& node)
    {
      return static_cast<const Relaxed_kdtree_link<Key, Value>*>
        (&node)->value.first;
    }
    //@}

    //@{
    /**
     *  This function converts a pointer on a node into a value for a @ref
     *  Relaxed_kdtree_link type.
     *  \tparam Key the key type for the \ref Relaxed_kdtree_link.
     *  \tparam Value the value type for the \ref Relaxed_kdtree_link.
     *  \param node the node to convert to a key.
     */
    template <typename Key, typename Value>
    inline typename Relaxed_kdtree_link<Key, Value>::value_type&
    value(Node<Relaxed_kdtree_link<Key, Value> >& node)
    {
      return static_cast<Relaxed_kdtree_link<Key, Value>*>(&node)->value;
    }

    template <typename Key, typename Value>
    inline const typename Relaxed_kdtree_link<Key, Value>::value_type&
    const_value(const Node<Relaxed_kdtree_link<Key, Value> >& node)
    {
      return static_cast<const Relaxed_kdtree_link<Key, Value>*>(&node)->value;
    }
    //@}

    //@{
    /**
     *  \brief  Swaps nodes position in the tree.
     *
     *  This function does not updates the left-most and right-most pointers of
     *  the tree where the nodes belongs to. This is left to the responsibility
     *  of the caller.
     *  \see Node
     *  \see Kdtree_link
     *  \see Relaxed_kdtree_link
     */
    template <typename Mode>
    void swap_node(Node<Mode>& a, Node<Mode>& b);

    template<typename Key, typename Value>
    inline void swap(Node<Kdtree_link<Key, Value> >& a,
                     Node<Kdtree_link<Key, Value> >& b)
    { swap_node(a, b); }

    template<typename Key, typename Value>
    inline void swap(Node<Relaxed_kdtree_link<Key, Value> >& a,
                     Node<Relaxed_kdtree_link<Key, Value> >& b)
    {
      std::swap(link(a).weight, link(b).weight);
      swap_node(a, b);
    }

    template <typename Mode>
    inline void swap
    (typename Node<Mode>::ptr& a, typename Node<Mode>::ptr& b)
    {
      swap(*a, *b);
      std::swap(a, b);
    }
    //@}

    /**
     *  \brief  A bidirectional iterator traversing all node in the tree in
     *  inorder traversal. This iterator provides mutable access to the nodes in
     *  the tree.
     *
     *  \tparam Node The node of the tree.
     */
    template<typename Mode>
    struct Node_iterator
    {
      typedef typename Mode::value_type          value_type;
      typedef value_type&                        reference;
      typedef value_type*                        pointer;
      typedef std::ptrdiff_t                     difference_type;
      typedef std::bidirectional_iterator_tag    iterator_category;
      typedef typename Node<Mode>::ptr           node_ptr;

    private:
      typedef Node_iterator<Mode>                Self;

    public:
      //! Create an uninintialized iterator. This iterator should not be used
      //! until is has been assigned.
      Node_iterator() : node() { }

      //! Build and assign an interator to a link pointer.
      explicit Node_iterator(node_ptr x) : node(x) { }

      //! Dereferance the iterator: return the value of the node.
      reference operator*() const
      { return value(node); }

      //! Dereferance the iterator: return the pointer to the value of the node.
      pointer operator->() const
      { return &value(node); }

      //! Moves the iterator to the next node in inorder transversal.
      Self& operator++()
      { node = increment(node); return *this; }

      //! Moves the iterator to the next node in inorder transversal and returns
      //! the iterator value before the move.
      Self operator++(int)
      { Self tmp = *this; node = increment(node); return tmp; }

      //! Moves the iterator to the previous node in inorder transversal.
      Self& operator--()
      { node = decrement(node); return *this; }

      //! Moves the iterator to the previous node in inorder transversal and
      //! returns the iterator value before the move.
      Self operator--(int)
      { Self tmp = *this; node = decrement(node); return tmp; }

      //! Check if 2 iterators are equal: pointing at the same node.
      bool operator==(const Self& x) const
      { return node == x.node; }

      //! Check if 2 iterators are different: pointing at different nodes.
      bool operator!=(const Self& x) const
      { return node != x.node; }

      //! The node pointed to by the iterator.
      node_ptr node;
    };

    /**
     *  \brief  A bidirectional iterator traversing all node in the tree in
     *  inorder traversal. This iterator provides constant access to the nodes
     *  in the tree.
     *
     *  \tparam Node The node of the tree.
     */
    template<typename Mode>
    struct Const_node_iterator
    {
      typedef typename Mode::value_type          value_type;
      typedef const value_type&                  reference;
      typedef const value_type*                  pointer;
      typedef std::ptrdiff_t                     difference_type;
      typedef std::bidirectional_iterator_tag    iterator_category;
      typedef typename Node<Mode>::const_ptr     node_ptr;

    private:
      typedef Const_node_iterator<Mode>          Self;
      typedef Node_iterator<Mode>                iterator;

    public:
      //! Create an uninintialized iterator. This iterator should not be used
      //! until is has been assigned.
      Const_node_iterator() : node() { }

      //! Build and assign an interator to a link pointer.
      explicit
      Const_node_iterator(node_ptr x) : node(x) { }

      //! Convert an iterator into a constant iterator.
      Const_node_iterator(const iterator& it) : node(it.node) { }

      //! Dereferance the iterator: return the value of the node.
      reference operator*() const
      { return value(node); }

      //! Dereferance the iterator: return the pointer to the value of the node.
      pointer operator->() const
      { return &value(node); }

      //! Moves the iterator to the next node in inorder transversal.
      Self& operator++()
      { node = increment(node); return *this; }

      //! Moves the iterator to the next node in inorder transversal and returns
      //! the iterator value before the move.
      Self operator++(int)
      { Self tmp = *this; node = increment(node); return tmp; }

      //! Moves the iterator to the previous node in inorder transversal.
      Self& operator--()
      { node = decrement(node); return *this; }

      //! Moves the iterator to the previous node in inorder transversal and
      //! returns the iterator value before the move.
      Self operator--(int)
      { Self tmp = *this; node = decrement(node); return tmp; }

      //! Check if 2 iterators are equal: pointing at the same node.
      bool operator==(const Self& x) const
      { return node == x.node; }

      //! Check if 2 iterators are different: pointing at different nodes.
      bool operator!=(const Self& x) const
      { return node != x.node; }

      //! The node pointed to by the iterator.
      node_ptr node;
    };

    /**
     *  A forward iterator that iterates through the node of the container in
     *  preorder transversal. It provides constant access to the node. It is
     *  used to clone the tree.
     *
     *  \tparam Node The type of Node in the tree.
     */
    template<typename Mode>
    struct Preorder_node_iterator
    {
      typedef typename Mode::value_type     value_type;
      typedef const value_type&             reference;
      typedef const value_type*             pointer;
      typedef std::ptrdiff_t                difference_type;
      typedef std::forward_iterator_tag     iterator_category;
      typedef typename Mode::const_ptr      node_ptr;

    private:
      typedef Preorder_node_iterator<Mode>  Self;

    public:
      //! Create an uninintialized iterator. This iterator should not be used
      //! until is has been assigned.
      Preorder_node_iterator() : node() { }

      //! Build and assign an interator to a link pointer.
      explicit
      Preorder_node_iterator(node_ptr x) : node(x) { }

      //! Dereferance the iterator: return the value of the node.
      reference operator*()
      { return value(node); }

      //! Dereferance the iterator: return the pointer to the value of the node.
      pointer operator->()
      { return &value(node); }

      //! Moves the iterator to the next node in preorder transversal.
      Self& operator++()
      { node = preorder_increment(node); return *this; }

      //! Moves the iterator to the next node in preorder transversal and returns
      //! the iterator value before the move.
      Self operator++(int)
      { Self tmp = *this; node = preorder_increment(node); return tmp; }

      //! Check if 2 iterators are equal: pointing at the same node.
      bool operator==(const Self& x) const
      { return node == x.node; }

      //! Check if 2 iterators are different: pointing at different nodes.
      bool operator!=(const Self& x) const
      { return node != x.node; }

      //! The node pointed to by the iterator.
      node_ptr node;
    };

    /**
     *  A common template for bidirectional iterators that work on identical
     *  \ref LinkMode "modes of linking".
     *
     *  This template defines all the necessary features of a bidirectional
     *  iterator. It relies on 2 pre-existing functions, \c increment and \c
     *  decrement to be implemented for the type of \c Iterator.
     *
     *  \tparam Mode      The mode used to \ref LinkMode "link nodes to their values".
     *  \tparam Iterator  The real type of iterator.
     *  \tparam Constant  True if the iterator is a constant iterator, false
     *  otherwise.
     */
    template <typename Mode, typename Iterator, bool Constant>
    struct bidirectional_iterator
    {
      //! The \c value_type can receive a copy of the reference pointed to be
      //! the iterator.
      typedef typename Mode::value_type                value_type;

      //! The reference type of the object pointed to by the iterator.
      typedef typename condition
      <Constant, const value_type&, value_type&>::type reference;

      //! The pointer type of the object pointed to by the iterator.
      typedef typename condition
      <Constant, const value_type*, value_type*>::type pointer;

      //! The difference_type returned by the distance between 2 iterators.
      typedef std::ptrdiff_t                           difference_type;

      //! The iterator category that is always \c bidirectional_iterator_tag.
      typedef std::bidirectional_iterator_tag          iterator_category;

      //! The type for the node pointed to by the iterator.
      typedef typename condition
      <Constant, typename Mode::const_node_ptr,
       typename Mode::node_ptr>::type                  node_ptr;

      //! Build an uninitialized iterator
      bidirectional_iterator() { }

      //! Initialize the node at construction time
      bidirectional_iterator(node_ptr x, dimension_type n)
	: node(x), node_dim(n) { }

      //! Returns the reference to the value pointed to by the iterator.
      reference operator*() const
      { return value(node); }

      //! Returns a pointer to the value pointed to by the iterator.
      pointer operator->() const
      { return &value(node); }

      //! Increments the iterator and returns the incremented value. Prefer to
      //! use this form in \c for loops.
      Iterator& operator++()
      {
        Iterator& x = *static_cast<Iterator*>(this);
        ::spatial::details::increment(x); return x;
      }

      //! Increments the iterator but returns the value of the iterator before
      //! the increment. Prefer to use the other form in \c for loops.
      Iterator operator++(int)
      {
        Iterator& x = *static_cast<Iterator*>(this);
        Iterator y(x);
        ::spatial::details::increment(x);
        return y;
      }

      //! Decrements the iterator and returns the decremented value. Prefer to
      //! use this form in \c for loops.
      Iterator& operator--()
      {
        Iterator& x = *static_cast<Iterator*>(this);
        ::spatial::details::decrement(x); return x;
      }

      //! Decrements the iterator but returns the value of the iterator before
      //! the decrement. Prefer to use the other form in \c for loops.
      Iterator operator--(int)
      {
        Iterator& x = *static_cast<Iterator*>(this);
        Iterator y(x);
        ::spatial::details::decrement(x);
        return y;
      }

      /**
       *  Two bidirectional iterators can always be compared for equility if
       *  they work on identical \ref LinkMode "link modes".
       *
       *  \tparam IteratorX The type of iterator on the right.
       *  \tparam ConstantX The constness of the iterator on the right.
       *  \param x The iterator on the right.
       */
      template <typename IteratorX, bool ConstantX>
      bool operator==
      (const bidirectional_iterator<Mode, IteratorX, ConstantX>& x) const
      { return node == x.node; }

      /**
       *  A bidirectional iterator can be compared with a node iterator if they
       *  work on identical \ref LinkMode "linking modes".
       *
       *  \param x The iterator on the right.
       */
      bool operator==(const Const_node_iterator<Mode>& x) const
      { return node == x.node; }

      /**
       *  Two bidirectional iterators can always be compared for inequility if
       *  they work on identical \ref LinkMode "linking modes".
       *
       *  \tparam IteratorX The type of iterator on the right.
       *  \tparam ConstantX The constness of the iterator on the right.
       *  \param x The iterator on the right.
       */
      template <typename IteratorX, bool ConstantX>
      bool operator!=
      (const bidirectional_iterator<Mode, IteratorX, ConstantX>& x) const
      { return node != x.node; }

      /**
       *  A bidirectional iterator can be compared for inequality with a node
       *  iterator if they work on identical \ref LinkMode "linking modes".
       *
       *  \param x The iterator on the right.
       */
      bool operator!=(const Const_node_iterator<Mode>& x) const
      { return node != x.node; }

      /**
       *  The pointer to the current node.
       *
       *  Modifying this attribute can potentially invalidate the iterator. Do
       *  not modify this attribute unless you know what you're doing. This
       *  iterator must always point to a valid node in the tree or to the end.
       */
      node_ptr node;

      /**
       *  The dimension of the current node.
       *
       *  Modifying this attribute can potentially invalidate the iterator. Do
       *  not modify this attribute unless you know what you're doing. This
       *  iterator must always point to a valid node in the tree or to the end.
       */
      dimension_type node_dim;
    };

  } // namespace details

} // namespace spatial

#include "spatial_node.tpp"

#endif // SPATIAL_NODE_HPP
