// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_ordered.hpp
 *  @brief  Contains the definition of the ordered iterators. These iterators
 *  walk through all items in the container in order from the lowest to the
 *  highest value along a particular dimension. The @key_comp comparator of the
 *  container is used for comparision.
 *
 *  @see ordered
 */

#ifndef SPATIAL_ORDERED_HPP
#define SPATIAL_ORDERED_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#include <utility> // provides ::std::pair<> and ::std::make_pair()

namespace spatial
{
  /**
   *  All elements returned by this iterator are ordered from the smallest to
   *  the largest value of their key's coordinate along a single dimension.
   */
  template<typename Ct>
  struct ordered_iterator
    : details::Bidirectional_iterator
      <typename container_traits<Ct>::mode_type,
       typename container_traits<Ct>::rank_type>
  {
  private:
    typedef details::Bidirectional_iterator
    <typename container_traits<Ct>::mode_type,
     typename container_traits<Ct>::rank_type> Base;

  public:
    typedef typename container_traits<Ct>::key_compare key_compare;

    //! Uninitialized iterator.
    ordered_iterator() { }

    /**
     *  The standard way to build this iterator: specify a ordered dimension, an
     *  iterator on a container, and that container.
     *
     *  \param container   The container to iterate.
     *
     *  \param node_dim    The dimension of the node pointed to by iterator.
     *
     *  \param iter        Use the value of \iter as the start point for the
     *                     iteration.
     */
    ordered_iterator(Ct& container,
                     const typename container_traits<Ct>::iterator& iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        _cmp(container.key_comp())
    { }

    /**
     *  When the information of the dimension for the current node being
     *  pointed to by the iterator is known, this function saves some CPU
     *  cycle, by comparison to the other.
     *
     *  In order to iterate through nodes in the \kdtree built in the
     *  container, the algorithm must know at each node which dimension is
     *  used to partition the space. Some algorithms will provide this
     *  dimension, such as the function \ref modulo().
     *
     *  \attention Specifying the incorrect dimension value for the node will
     *  result in unknown behavior. It is recommended that you do not use this
     *  constructor if you are not sure about this dimension, and use the
     *  other constructors instead.
     *
     *  \param container The container to iterate.
     *
     *  \param dim The dimension of the node pointed to by iterator.
     *
     *  \param ptr Use the value of node as the start point for the
     *             iteration.
     */
    ordered_iterator(Ct& container, dimension_type dim,
                     typename container_traits<Ct>::mode_type::node_ptr ptr)
      : Base(container.rank(), ptr, dim), _cmp(container.key_comp())
    { except::check_dimension(container.dimension(), dim); }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    ordered_iterator<Ct>& operator++()
    { return increment_ordered(*this); }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    ordered_iterator<Ct> operator++(int)
    {
      ordered_iterator<Ct> x(*this);
      increment_ordered(*this);
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    ordered_iterator<Ct>& operator--()
    { return decrement_ordered(*this); }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    ordered_iterator<Ct> operator--(int)
    {
      ordered_iterator<Ct> x(*this);
      decrement_ordered(*this);
      return x;
    }

    //! Return the key_comparator used by the iterator
    const key_compare&
    key_comp() const { return _cmp; }

  private:
    //! The related data for the iterator.
    key_compare _cmp;
  };

  /**
   *  All elements returned by this iterator are ordered from the smallest to
   *  the largest value of their key's coordinate along a single dimension,
   *  called the ordered dimension.
   *
   *  In effect, that makes any container of the library behave as a std::set
   *  or std::map. Through this iterator, a spatial container with 3
   *  dimensions can provide the same features as 3 std::set(s) or std::map(s)
   *  containing the same elements and ordered on each of these
   *  dimensions. Beware that iteration through the tree is very efficient
   *  when the dimension K-d tree is very small by comparison to the number of
   *  objects, but pretty inefficient otherwise, by comparison to a std::set.
   *
   *  Object deferenced by this iterator are always constant.
   */
  template<typename Ct>
  struct ordered_iterator<const Ct>
    : details::Const_bidirectional_iterator
      <typename container_traits<Ct>::mode_type,
       typename container_traits<Ct>::rank_type>
  {
  private:
    typedef details::Const_bidirectional_iterator
    <typename container_traits<Ct>::mode_type,
     typename container_traits<Ct>::rank_type> Base;

  public:
    //! Alias for the key_compare type used by the iterator.
    typedef typename container_traits<Ct>::key_compare key_compare;

    //! Build an uninitialized iterator.
    ordered_iterator() { }

    /**
     *  The standard way to build this iterator: specify a ordered dimension,
     *  an iterator on a container, and that container.
     *
     *  \param container The container to iterate.
     *  \param node_dim The dimension of the node pointed to by iterator.
     *  \param iter Use the value of \iter as the start point for the
     *  iteration.
     */
    ordered_iterator(const Ct& container,
                     const typename container_traits<Ct>::const_iterator& iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        _cmp(container.key_comp())
    { }

    /**
     *  When the information of the dimension for the current node being
     *  pointed to by the iterator is known, this function saves some CPU
     *  cycle, by comparison to the other.
     *
     *  In order to iterate through nodes in the \kdtree built in the
     *  container, the algorithm must know at each node which dimension is
     *  used to partition the space. Some algorithms will provide this
     *  dimension, such as the function \ref modulo().
     *
     *  \attention Specifying the incorrect dimension value for the node will
     *  result in unknown behavior. It is recommended that you do not use this
     *  constructor if you are not sure about this dimension, and use the
     *  other constructors instead.
     *
     *  \param container The container to iterate.
     *
     *  \param ordered_dim The dimension used to order all nodes during the
     *                     iteration.
     *
     *  \param dim The dimension of the node pointed to by iterator.
     *
     *  \param ptr Use the value of \c node as the start point for the
     *             iteration.
     */
    ordered_iterator(const Ct& container, dimension_type dim,
                     typename container_traits<Ct>::mode_type::const_node_ptr
                     ptr)
      : Base(container.rank(), ptr, dim), _cmp(container.key_comp())
    { except::check_dimension(container.dimension(), dim); }

    //! Convertion of mutable iterator into a constant iterator is permitted.
    ordered_iterator(const ordered_iterator<Ct>& iter)
      : Base(iter.rank(), iter.node, iter.node_dim), _cmp(iter.key_comp())
    { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    ordered_iterator<const Ct>& operator++()
    { return increment_ordered(*this); }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    ordered_iterator<const Ct> operator++(int)
    {
      ordered_iterator<const Ct> x(*this);
      increment_ordered(*this);
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    ordered_iterator<const Ct>& operator--()
    { return decrement_ordered(*this); }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    ordered_iterator<const Ct> operator--(int)
    {
      ordered_iterator<const Ct> x(*this);
      decrement_ordered(*this);
      return x;
    }

    //! Return the key_comparator used by the iterator
    const key_compare&
    key_comp() const { return _cmp; }

  private:
    //! The related data for the iterator.
    key_compare _cmp;
  };

  //! Return true if 2 iterators are equal
  template<typename Ct>
  bool operator==(const ordered_iterator<Ct>& a,
                  const ordered_iterator<Ct>& b)
  { return a.node == b.node; }

  //! Return true if 2 iterators are equal
  template<typename Ct>
  bool operator==(const ordered_iterator<const Ct>& a,
                  const ordered_iterator<const Ct>& b)
  { return a.node == b.node; }

  //! Return true if 2 iterators are equal
  template<typename Ct>
  bool operator==(const ordered_iterator<Ct>& a,
                  const ordered_iterator<const Ct>& b)
  { return a.node == b.node; }

  //! Return true if 2 iterators are equal
  template<typename Ct>
  bool operator==(const ordered_iterator<const Ct>& a,
                  const ordered_iterator<Ct>& b)
  { return a.node == b.node; }

  //! Return true if 2 iterator are different
  template<typename Ct>
  bool operator!=(const ordered_iterator<Ct>& a,
                  const ordered_iterator<Ct>& b)
  { return !(a == b); }

  //! Return true if 2 iterator are different
  template<typename Ct>
  bool operator!=(const ordered_iterator<const Ct>& a,
                  const ordered_iterator<const Ct>& b)
  { return !(a == b); }

  //! Return true if 2 iterator are different
  template<typename Ct>
  bool operator!=(const ordered_iterator<Ct>& a,
                  const ordered_iterator<const Ct>& b)
  { return !(a == b); }

  //! Return true if 2 iterator are different
  template<typename Ct>
  bool operator!=(const ordered_iterator<const Ct>& a,
                  const ordered_iterator<Ct>& b)
  { return !(a == b); }

  namespace details
  {
    /**
     *  Move the pointer given in parameter to the next element in the ordered
     *  iteration of values along the ordered dimension.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  ordered<>::iterator(). In any case, use it cautiously, as this function
     *  does not perform any sanity checks on the iterator given in parameter.
     *
     *  \bestlogtime \worstdfractime Since Container is based on \kdtree and
     *  \kdtree exhibit good locality of reference (for arranging values in
     *  space, not for values location in memory), the function will run with
     *  time complexity close to \Olog in practice.
     */
    template <typename Container>
    ordered_iterator<Container>&
    increment_ordered(ordered_iterator<Container>& iter);

    /**
     *  Move the pointer given in parameter to the previous element in the
     *  ordered iteration of values along the ordered dimension.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  ordered<>::const_iterator. In any case, use it cautiously, as this
     *  function does not perform any sanity checks on the iterator given in
     *  parameter.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter The reference iterator that points to the current node.
     *  \return An iterator pointing to the value with the smallest coordinate
     *  along \c iter's \c ordered_dim, and among the children of the node
     *  pointed to by \c iter.
     *
     *  \bestlogtime \worstdfractime Since Container is based on \kdtree and
     *  \kdtree exhibit good locality of reference (for arranging values in
     *  space, not for values location in memory), the function will run with
     *  time complexity close to \Olog in practice.
     */
    template <typename Container>
    ordered_iterator<Container>&
    decrement_ordered(ordered_iterator<Container>& iter);

    /**
     *  Move the iterator given in parameter to the minimum value along the
     *  iterator's ordered dimension but only in the sub-tree composed of the
     *  node pointed to by the iterator and its children.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  ordered_begin(). In any case, use it cautiously, as this function does
     *  not perform any sanity checks on the iterator given in parameter.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter An iterator that points to the root node of the search.
     *  \return The iterator given in parameter is moved to the value with the
     *  smallest coordinate along \c iter's \c ordered_dim, and among the
     *  children of the node pointed to by \c iter.
     *
     *  \dfractime
     */
    template <typename Container>
    ordered_iterator<Container>&
    minimum_ordered(ordered_iterator<Container>& iter);

    /**
     *  Move the iterator given in parameter to the maximum value along the
     *  iterator's ordered dimension but only in the sub-tree composed of the
     *  node pointed to by the iterator and its children.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  ordered_begin(). In any case, use it cautiously, as this function does
     *  not perform any sanity checks on the iterator given in parameter.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter An iterator that points to the root node of the search.
     *  \return The iterator given in parameter is moved to the value with the
     *  largest coordinate along \c iter's \c ordered_dim, among the children of
     *  the node pointed to by \c iter.
     *
     *  \dfractime
     */
    template <typename Container>
    ordered_iterator<Container>&
    maximum_ordered(ordered_iterator<Container>& iter);

    /**
     *  Move the iterator given in parameter to the value with the smallest
     *  coordinate greater or equal to \c bound along the ordered dimension of
     *  \c iter, but only in the sub-tree composed of the node pointed to by the
     *  iterator and its children. If no such value exists, then move the
     *  iterator to the parent of the value currently pointed to.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  ordered_begin(). In any case, use it cautiously, as this function does
     *  not perform any sanity checks on the iterator given in parameter.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter An iterator that points to the root node of the search.
     *  \param bound The lower bound to the iterator position.
     *  \return An iterator pointing to the value with the smallest coordinate
     *  greater or equal to \c bound along \c iter's \c ordered_dim, or to the
     *  parent of the value pointed to.
     *
     *  \dfractime
     */
    template <typename Container>
    ordered_iterator<Container>&
    lower_bound_ordered
    (ordered_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound);

    /**
     *  Move the iterator given in parameter to the value with the largest
     *  coordinate strictly lower than \c bound along the ordered dimension of
     *  \c iter, but only in the sub-tree composed of the node pointed to by the
     *  iterator and its children. If no such value exists, then move the
     *  iterator to the parent of the value currently pointed to.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  ordered_begin(). In any case, use it cautiously, as this function does
     *  not perform any sanity checks on the iterator given in parameter.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter An iterator that points to the root node of the search.
     *  \param bound The upper bound to the iterator position.
     *  \return \c iter moved to the value with the largest coordinate strictly
     *  less than \c bound along \c iter's \c ordered_dim, or to the
     *  parent of the value pointed to.
     *
     *  \dfractime
     */
    template <typename Container>
    ordered_iterator<Container>&
    upper_bound_ordered
    (ordered_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound);

  } // namespace details

  /**
   *  This structure defines a pair of mutable ordered iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \see ordered_iterator
   */
  template <typename Ct>
  struct ordered_iterator_pair
        : std::pair<ordered_iterator<Ct>, ordered_iterator<Ct> >
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef std::pair<ordered_iterator<Ct>, ordered_iterator<Ct> > Base;

    //! Empty constructor.
    ordered_iterator_pair() { }

    //! Regular constructor that builds a ordered_iterator_pair out of 2
    //! ordered_iterators.
    ordered_iterator_pair(const ordered_iterator<Ct>& a,
                          const ordered_iterator<Ct>& b) : Base(a, b) { }
  };

  /**
   *  This structure defines a pair of constant ordered iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \see ordered_iterator
   */
  template <typename Ct>
  struct ordered_iterator_pair<const Ct>
        : std::pair<ordered_iterator<const Ct>, ordered_iterator<const Ct> >
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef std::pair<ordered_iterator<const Ct>, ordered_iterator<const Ct> >
          Base;

    //! Empty constructor.
    ordered_iterator_pair() { }

    //! Regular constructor that builds a ordered_iterator_pair out of 2
    //! ordered_iterators.
    ordered_iterator_pair(const ordered_iterator<const Ct>& a,
                          const ordered_iterator<const Ct>& b) : Base(a, b)
    { }

    //! Convert a mutable ordered iterator pair into a const ordered iterator
    //!pair.
    ordered_iterator_pair(const ordered_iterator_pair<Ct>& p)
      : Base(p.first, p.second) { }
  };

  /**
   *  Finds the past-the-end position in \c container for this constant
   *  iterator.
   *
   *  \attention This iterator impose constness constraints on its \c value_type
   *  if the container's is a set and not a map. Iterators on sets prevent
   *  modification of the \c value_type because modifying the key may result in
   *  invalidation of the tree. If the container is a map, only the \c
   *  mapped_type can be modified (the \c second element).
   *
   *  \tparam Container The type of container to iterate.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the \ref Rank "rank" of the container.
   *  \return An iterator pointing to the past-the-end position in the
   *  container.
   *
   *  \consttime
   *  \see ordered
   */
  template <typename Container>
  inline ordered_iterator<Container>
  ordered_end(Container& container)
  {
    return ordered_iterator<Container>
      // At header (dim = rank - 1)
      (container, container.dimension() - 1, container.end().node);
  }

  //@{
  /**
   *  Finds the past-the-end position in \c container for this constant
   *  iterator.
   *
   *  \tparam Container The type of container to iterate.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the \ref Rank "rank" of the container.
   *  \return An iterator pointing to the past-the-end position in the
   *  container.
   *
   *  \consttime
   *  \see ordered
   */
  template <typename Container>
  inline ordered_iterator<const Container>
  ordered_end(const Container& container)
  {
    return ordered_iterator<const Container>
      // At header (dim = rank - 1)
      (container, container.dimension() - 1, container.end().node);
  }

  template <typename Container>
  inline ordered_iterator<const Container>
  ordered_cend(const Container& container)
  { return ordered_end(container); }
  //@}

  /**
   *  Finds the value in \c container for which its key has the smallest
   *  coordinate over the dimension \c ordered_dim.
   *
   *  \attention This iterator impose constness constraints on its \c value_type
   *  if the container's is a set and not a map. Iterators on sets prevent
   *  modification of the \c value_type because modifying the key may result in
   *  invalidation of the tree. If the container is a map, only the \c
   *  mapped_type can be modified (the \c second element).
   *
   *  \tparam Container The type of container to iterate.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the \ref Rank "rank" of the container.
   *
   *  \dfractime
   *  \see ordered
   */
  template <typename Container>
  inline ordered_iterator<Container>
  ordered_begin(Container& container)
  {
    if (container.empty()) return ordered_end(container);
    ordered_iterator<Container>
      it(container, 0, container.end().node->parent);
    return details::minimum_ordered(it);
  }

  //@{
  /**
   *  Finds the value in \c container for which its key has the smallest
   *  coordinate over the dimension \c ordered_dim.
   *
   *  \tparam Container The type of container to iterate.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the \ref Rank "rank" of the container.
   *
   *  \dfractime
   *  \see ordered
   */
  template <typename Container>
  inline ordered_iterator<const Container>
  ordered_begin(const Container& container)
  {
    if (container.empty()) return ordered_end(container);
    ordered_iterator<const Container>
      it(container, 0, container.end().node->parent);
    return details::minimum_ordered(it);
  }

  template <typename Container>
  inline ordered_iterator<const Container>
  ordered_cbegin(const Container& container)
  { return ordered_begin(container); }
  //@}

  /**
   *  Returns a pair of iterator on the first and the last value in the range
   *  that can be iterated. This function is convenient to use with
   *  \tc{std::tie}, and is equivalent to calling \ref ordered_begin() and \ref
   *  ordered_end() on both iterators.
   *
   *  \attention This iterator impose constness constraints on its \c value_type
   *  if the container's is a set and not a map. Iterators on sets prevent
   *  modification of the \c value_type because modifying the key may result in
   *  invalidation of the tree. If the container is a map, only the \c
   *  mapped_type can be modified (the \c second element).
   *
   *  \tparam Container The type of container to iterate.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the \ref Rank "rank" of the container.
   *  \return An iterator pointing to the past-the-end position in the
   *  container.
   *
   *  \dfractime
   *  \see ordered
   */
  template <typename Container>
  inline ordered_iterator_pair<Container>
  ordered_range(Container& container)
  {
    return ordered_iterator_pair<Container>(ordered_begin(container),
                                            ordered_end(container));
  }

  //@{
  /**
   *  Returns a pair of constant iterator on the first and the last value in the
   *  range that can be iterated. This function is convenient to use with
   *  \tc{std::tie}, and is equivalent to calling \ref ordered_begin() and \ref
   *  ordered_end() on both iterators.
   *
   *  \tparam Container The type of container to iterate.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the \ref Rank "rank" of the container.
   *  \return An iterator pointing to the past-the-end position in the
   *  container.
   *
   *  \dfractime
   *  \see ordered
   */
  template <typename Container>
  inline ordered_iterator_pair<const Container>
  ordered_range(const Container& container)
  {
    return ordered_iterator_pair<const Container>(ordered_begin(container),
                                                  ordered_end(container));
  }

  template <typename Container>
  inline ordered_iterator_pair<const Container>
  ordered_crange(const Container& container)
  {
    return ordered_iterator_pair<const Container>(ordered_begin(container),
                                                  ordered_end(container));
  }
  //@}

  /**
   *  Finds the value with the smallest coordinate along the ordered dimension
   *  that is greater or equal to \c bound, and return an iterator pointing to
   *  this value.
   *
   *  \attention This iterator impose constness constraints on its \c value_type
   *  if the container's is a set and not a map. Iterators on sets prevent
   *  modification of the \c value_type because modifying the key may result in
   *  invalidation of the tree. If the container is a map, only the \c
   *  mapped_type can be modified (the \c second element).
   *
   *  \tparam Container The type of container to iterate.
   *  \param bound The lowest bound to the iterator position.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the \ref Rank "rank" of the container.
   *  \return An iterator pointing to the value with the smallest coordinate
   *  greater or equal to \c bound along \c ordered_dim.
   *
   *  \dfractime
   *  \see ordered
   */
  template <typename Container>
  inline ordered_iterator<Container>
  ordered_lower_bound(Container& container,
                      const typename container_traits<Container>::key_type&
                      bound)
  {
    if (container.empty()) return ordered_end(container);
    ordered_iterator<Container> it
      (container, 0, container.end().node->parent);
    return details::lower_bound_ordered(it, bound);
  }

  //@{
  /**
   *  Finds the value with the smallest coordinate along the ordered dimension
   *  that is greater or equal to \c bound, and return a constant iterator to
   *  this value.
   *
   *  \tparam Container The type of container to iterate.
   *  \param bound The lowest bound to the iterator position.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the \ref Rank "rank" of the container.
   *  \return An iterator pointing to the value with the smallest coordinate
   *  greater or equal to \c bound along \c ordered_dim.
   *
   *  \dfractime
   *  \see ordered
   */
  template <typename Container>
  inline ordered_iterator<const Container>
  ordered_lower_bound
  (const Container& container,
   const typename container_traits<Container>::key_type& bound)
  {
    if (container.empty()) return ordered_end(container);
    ordered_iterator<const Container> it
      (container, 0, container.end().node->parent);
    return details::lower_bound_ordered(it, bound);
  }

  template <typename Container>
  inline ordered_iterator<const Container>
  ordered_clower_bound
  (const Container& container,
   const typename container_traits<Container>::key_type& bound)
  { return ordered_lower_bound(container, bound); }
  //@}

  /**
   *  Finds the value with the largest coordinate along the ordered dimension
   *  that is stricly less than \c bound, and return an iterator pointing to
   *  this value.
   *
   *  \attention This iterator impose constness constraints on its \c value_type
   *  if the container's is a set and not a map. Iterators on sets prevent
   *  modification of the \c value_type because modifying the key may result in
   *  invalidation of the tree. If the container is a map, only the \c
   *  mapped_type can be modified (the \c second element).
   *
   *  \tparam Container The type of container to iterate.
   *  \param bound The lowest bound to the iterator position.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the \ref Rank "rank" of the container.
   *  \return An iterator pointing to the value with the smallest coordinate
   *  greater or equal to \c bound along \c ordered_dim.
   *
   *  \dfractime
   *  \see ordered
   */
  template <typename Container>
  inline ordered_iterator<Container>
  ordered_upper_bound
  (Container& container,
   const typename container_traits<Container>::key_type& bound)
  {
    if (container.empty()) return ordered_end(container);
    ordered_iterator<Container> it // At root (dim = 0)
      (container, 0, container.end().node->parent);
    return details::upper_bound_ordered(it, bound);
  }

  //@{
  /**
   *  Finds the value with the largest coordinate along the ordered dimension
   *  that is stricly less than \c bound, and return an iterator pointing to
   *  this value.
   *
   *  \tparam Container The type of container to iterate.
   *  \param bound The lowest bound to the iterator position.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the \ref Rank "rank" of the container.
   *  \return An iterator pointing to the value with the smallest coordinate
   *  greater or equal to \c bound along \c ordered_dim.
   *
   *  \dfractime
   *  \see ordered
   */
  template <typename Container>
  inline ordered_iterator<const Container>
  ordered_upper_bound
  (const Container& container,
   const typename container_traits<Container>::key_type& bound)
  {
    if (container.empty()) return ordered_end(container);
    ordered_iterator<const Container> it  // At root (dim = 0)
      (container, 0, container.end().node->parent);
    return details::upper_bound_ordered(it, bound);
  }

  template <typename Container>
  inline ordered_iterator<const Container>
  ordered_cupper_bound
  (const Container& container,
   const typename container_traits<Container>::key_type& bound)
  { return ordered_upper_bound(container, bound); }
  //@}

} // namespace spatial

#include "spatial_ordered.tpp"

#endif // SPATIAL_ORDERED_HPP
