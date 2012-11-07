// -*- C++ -*-

/**
 *  @file   spatial_ordered.tpp
 *  @brief
 *
 *  Change Log:
 *
 *  - 2009-02-27 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_ORDERED_TPP
#define SPATIAL_ORDERED_TPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

namespace spatial
{
  namespace details
  {
    //! Specialization for iterators pointing to node using the relaxed
    //! invariant.
    //! \see increment<Container>(typename ordered<Container>::iterator&)
    template <typename Container>
    inline ordered_iterator<Container>&
    increment_ordered(ordered_iterator<Container>& iter,
                      details::relaxed_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      // Walk the tree in both directions: one step left, one step right, etc..
      node_ptr l_node, r_node;
      dimension_type l_node_dim, r_node_dim;
      l_node = r_node = iter.node;
      l_node_dim = r_node_dim = iter.node_dim;
      node_ptr best = 0;
      dimension_type best_dim = 0;
      while(true)
        {
        step_left:
          if (header(l_node)) { goto step_right; }
          if (l_node->left // less on node...
              ) { l_node = r_node; }
        step_right:
          if (header(r_node)) { if (header(l_node)) break; else continue; }
          if (r_node->right // less on node...
              ) { l_node = r_node; }
        }
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    //! Specialization for iterators pointing to node using the strict
    //! invariant.
    //! \see increment<Container>(typename ordered<Container>::iterator&)
    template <typename Container>
    inline ordered_iterator<Container>&
    increment_ordered(ordered_iterator<Container>& iter,
                      details::strict_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());


      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    //! Specialization for iterators pointed to node using the relaxed
    //! invariant.
    //! \see maximum<Container>(typename ordered<Container>::iterator&)
    template<typename Container>
    inline ordered_iterator<Container>&
    maximum_ordered(ordered_iterator<Container>& iter,
                    details::relaxed_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));


      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      return iter;
    }

    //! Specialization for iterators pointed to node using the strict
    //! invariant.
    //! \see maximum<Container>(typename ordered_iterator<Container>&)
    template<typename Container>
    inline ordered_iterator<Container>&
    maximum_ordered(ordered_iterator<Container>& iter,
                    details::strict_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));


      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      return iter;
    }

    //! Specialization for iterators pointed to node using the relaxed
    //! invariant.
    template<typename Container>
    inline ordered_iterator<Container>&
    lower_bound_ordered
    (ordered_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound,
     details::relaxed_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));


      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    //! Specialization for iterators pointed to node using the strict
    //! invariant.
    template<typename Container>
    inline ordered_iterator<Container>&
    lower_bound_ordered
    (ordered_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound,
     details::strict_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));


      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container>
    inline ordered_iterator<Container>&
    increment_ordered(ordered_iterator<Container>& iter)
    {
      return ::spatial::details::increment_ordered
        (iter, typename container_traits<Container>::mode_type
         ::invariant_category());
    }

    // The next largest key on the ordered dimension is likely to be found in the
    // children of the current best, so, descend into the children of node first.
    template <typename Container>
    inline ordered_iterator<Container>&
    decrement_ordered(ordered_iterator<Container>& iter)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());


      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    // Find the minimum from node and stop when reaching the parent. Iterate in
    // left-first fashion.
    template <typename Container>
    inline ordered_iterator<Container>&
    minimum_ordered(ordered_iterator<Container>& iter)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      node_ptr best = 0;
      dimension_type best_dim = 0;


      SPATIAL_ASSERT_CHECK(best_dim < rank());
      SPATIAL_ASSERT_CHECK(best != 0);
      SPATIAL_ASSERT_CHECK(!header(best));
      iter.node = best; iter.node_dim = best_dim;
      return iter;
    }

    template <typename Container>
    inline ordered_iterator<Container>&
    maximum_ordered(ordered_iterator<Container>& iter)
    {
      return ::spatial::details::maximum_ordered
        (iter, typename container_traits<Container>::mode_type
         ::invariant_category());
    }

    template <typename Container>
    inline ordered_iterator<Container>&
    lower_bound_ordered(ordered_iterator<Container>& iter,
                        const typename container_traits<Container>::key_type&
                        bound)
    {
      return ::spatial::details::lower_bound_ordered
        (iter, bound,
         typename container_traits<Container>::mode_type
         ::invariant_category());
    }

    // Walk tree nodes in right-first fashion, bouncing off values that are
    // higher than key.
    template <typename Container>
    inline ordered_iterator<Container>&
    upper_bound_ordered(ordered_iterator<Container>& iter,
                        const typename container_traits<Container>::key_type&
                        bound)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type&
        rank = iter.rank();
      const typename container_traits<Container>::key_compare&
        cmp = iter.key_comp();
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));


      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_ORDERED_TPP
