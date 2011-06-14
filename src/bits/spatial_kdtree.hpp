// -*- C++ -*-

/**
 *  @file   spatial_kdtree.hpp
 *  @brief  
 *
 *  Change Log:
 *
 *  - 2009-09-19 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_KDTREE_HPP
#define SPATIAL_KDTREE_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#include <functional> // std::equal_to

namespace spatial
{
  namespace details
  {

    template <typename Rank, typename Key,
	      typename Compare, typename Alloc, bool ConstantIterator>
    class Kdtree
      : public Kdtree_base<Rank, Key, Kdtree_node<Key>, Compare, Alloc,
			   ConstantIterator>
    {
      typedef Kdtree<Rank, Key, Compare,
		     Alloc, ConstantIterator>    Self;
      typedef Kdtree_base<Rank, Key, Kdtree_node<Key>, Compare, Alloc,
			  ConstantIterator>      Base;

      typedef typename Base::Base_ptr            Base_ptr;
      typedef typename Base::Const_Base_ptr      Const_Base_ptr;
      typedef typename Base::Link_type           Link_type;
      typedef typename Base::Const_Link_type     Const_Link_type;

    public:
      typedef typename Base::key_type            key_type;
      typedef typename Base::pointer             pointer;
      typedef typename Base::const_pointer       const_pointer;
      typedef typename Base::reference           reference;
      typedef typename Base::const_reference     const_reference;
      typedef typename Base::node_type           node_type;
      typedef typename Base::size_type           size_type;
      typedef typename Base::difference_type     difference_type;
      typedef typename Base::allocator_type      allocator_type;
      typedef typename Base::node_allocator_type node_allocator_type;
      typedef typename Base::compare_type        compare_type;
      typedef typename Base::rank_type           rank_type;

      typedef typename Base::iterator            iterator;
      typedef typename Base::const_iterator      const_iterator;

    private:
      /**
       *  @brief  Keeps tracks of the number of nodes in the tree.
       */
      size_type m_count;

    private:
      /**
       *  @brief  Insert a node already allocated into the tree.
       */
      iterator insert_node(Base_ptr target_node);

      /**
       *  @brief  Insert all the nodes in @p [first,last) into the tree, by
       *  first sorting the nodes according to the dimension of interest.
       *
       *  This function is semi-recursive. It iterates when walking down left
       *  nodes and recurse when walking down right nodes.
       */
      void rebalance_node_insert
        (std::vector<Base_ptr>::iterator first,
		 std::vector<Base_ptr>::iterator last, dimension_type dim);

      /**
       *  @brief  Copy the exact sturcture of the sub-tree pointed to by @c
       *  other_node into the current empty tree.
       *
       *  The structural copy preserve all characteristics of the sub-tree
       *  pointed to by @c other_node.
       */
      void copy_structure(const Self& other);

      /**
       *  @brief  Copy the content of @p other to the tree and rebalances the
       *  values in the tree resulting in most query having an @c O(log(n))
       *  order of complexity.
       */
      void copy_rebalance(const Self& other);

      /**
       *  @brief  Erase the node located at @c node with current dimension
       *  @c node_dim.
       */
      void erase_node(dimension_type node_dim, Base_ptr node);

    public:
      Kdtree()
	: Base(Rank(), Compare(), allocator_type()), m_count(0)
      { }

      explicit Kdtree(const Rank& rank)
	: Base(rank, Compare(), allocator_type()), m_count(0)
      { }

      explicit Kdtree(const compare_type& compare)
	: Base(Rank(), compare, allocator_type()), m_count(0)
      { }

      Kdtree(const Rank& rank, const compare_type& compare)
	: Base(rank, compare, allocator_type()), m_count(0)
      { }

      Kdtree(const Rank& rank, const compare_type& compare,
	     const allocator_type& alloc)
	: Base(rank, compare, alloc), m_count(0)
      { }

      /**
       *  @brief  Deep copy of @c other into the new tree.
       *
       *  If @p rebalancing is @c false, @c no_rebalance or unspecified, the copy
       *  preserve the structure of the @p other tree. Therefore, all operations
       *  should behave similarly to both trees after the copy.
       *
       *  If @p rebalancing is @c true or @c rebalance, the new tree is a balanced
       *  copy of a the @p other tree, resulting in @c log(n) order of complexity
       *  on most search functions.
       */
      Kdtree(const Self& other, bool balancing = false)
	: Base(other), m_count(0)
      {
	if (!other.empty())
	  {
	    if (balancing) { copy_rebalance(other); }
	    else { copy_structure(other); }
	  }
      }

      /**
       *  @brief  Assignment of @c other into the tree, with deep copy.
       *
       *  The copy preserve the structure of the tree @c other. Therefore, all
       *  operations should behave similarly to both trees after the copy.
       *
       *  @note  The allocator of the tree is not modified by the assignment.
       */
      Self&
      operator=(const Self& other)
      {
	if (&other != this)
	  {
	    clear();
	    Base::operator=(other);
	    if (!other.empty()) { copy_structure(other); }
	  }
	return *this;
      }

    public:
      /**
       *  @brief  Returns the number of elements in the K-d tree.
       */
      size_type
      size() const
      { return m_count; }

      /**
       *  @brief  Returns the number of elements in the K-d tree. Same as size().
       *  @see size()
       */
      size_type
      count() const
      { return size(); }

      /**
       *  @brief  Erase all elements in the K-d tree.
       */
      void
      clear()
      {
	Base::clear();
	m_count = 0;
      }

      /**
       *  @brief  Rebalance the K-d tree near-optimally, resulting in @c log(n)
       *  order of complexity on most search functions.
       *
       *  This function is time & memory consuming. Internally, it creates a
       *  vector of pointer to the node, and thus requires a substantial amount of
       *  memory for a large K-d tree. Ideally, this function should be called
       *  only once, when all the elements you will be working on have been
       *  inserted in the tree.
       *
       *  If you need to insert and erase multiple elements continuously, consider
       *  using a different balancing policy instead.
       *
       *  @see Relaxed_kdtree
       */
      void rebalance();

    public:
      /**
       *  @brief  Swap the K-d tree content with other.
       */
      void swap(Kdtree& other)
      {
	Base::swap(*static_cast<Base*>(&other));
	std::swap(m_count, other.m_count);
      }

      /**
       *  @brief  Insert a single key @c key in the tree.
       */
      iterator
      insert(const key_type& key)
      {
	Link_type tmp = Base::create_node(key); // may throw
	return insert_node(tmp);
      }

      /**
       *  @brief  Insert a serie of values in the tree at once.
       */
      template<typename InputIterator>
      void
      insert(InputIterator first, InputIterator last)
      { for (; first != last; ++first) { insert(*first); } }

      /**
       *  @brief  Deletes the node pointed to by the iterator.
       *
       *  The iterator must be pointing to an existing node belonging to the
       *  related tree, or dire things may happen.
       */
      void
      erase(const_iterator pointer);

      /**
       *  @brief  Deletes all nodes that match key @c value.
       *  @see    find
       *  @param  value that will be compared with the tree nodes.
       *
       *  The type @c key_type must be equally comparable.
       */
      size_type
      erase(const key_type& value);

      /**
       *  @brief  Deletes any node that matches one of the keys in the sequence
       *  covered by the iterators.
       */
      template<typename InputIterator>
      void
      erase(InputIterator first, InputIterator last)
      { for (; first != last; ++first) { erase(*first); } }
    };

    /**
     *  @brief  Swap the content of the tree @p left and @p right.
     */
    template <typename Rank, typename Key, typename Compare,
	      typename Alloc, bool ConstantIterator>
    inline void swap
    (Kdtree<Rank, Key, Compare, Alloc, ConstantIterator>& left,
     Kdtree<Rank, Key, Compare, Alloc, ConstantIterator>& right)
    { left.swap(right); }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_KDTREE_HPP
