// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_mapping.hpp
 *  @brief  Contains the definition of the mapping iterators. These iterators
 *  walk through all items in the container in order from the lowest to the
 *  highest value along a particular dimension. The @key_comp comparator of the
 *  container is used for comparision.
 *
 *  @see mapping
 */

#ifndef SPATIAL_MAPPING_HPP
#define SPATIAL_MAPPING_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#include <utility> // provides ::std::pair<> and ::std::make_pair()

namespace spatial
{
  namespace details
  {
    /**
     *  Extra information needed by the iterator to perform its work. This
     *  information is copied to each iterator from a given container.
     *
     *  Although it may be possible to modify this information directly from
     *  it's members, it may be unwise to do so, as it could invalidate the
     *  iterator and cause the program to behave unexpectedly. If any of this
     *  information needs to be modified, it is probably recommended to create a
     *  new iterator altogether.
     *
     *  \tparam Ct The container to which these iterator relate to.
     */
    template <typename Ct>
    struct Mapping_data : container_traits<Ct>::key_compare
    {
      //! Build an uninitialized mapping data object.
      Mapping_data() { }

      /**
       *  Builds required mapping data from the given container, dimension and
       *  mapping dimension.
       *
       *  \param c The container being iterated.
       *  \param m The mapping dimension used in the iteration.
       */
      Mapping_data
      (const Ct& c, dimension_type m)
        : container_traits<Ct>::key_compare(c.key_comp()), mapping_dim(m)
      { }

      /**
       *  The current dimension of iteration.
       *
       *  You can modify this key if you suddenly want the iterator to change
       *  dimension of iteration. However this field must always satisfy:
       *  \f[
       *  mapping_dim() < Rank()
       *  \f]
       *  Rank being the template rank provider for the iterator.
       *
       *  \Warning If you modify this value directly, no safety check will be
       *  performed.
       */
      dimension_type mapping_dim;
    };
  }

  /**
   *  All elements returned by this iterator are ordered from the smallest to
   *  the largest value of their key's coordinate along a single dimension,
   *  called the mapping dimension.
   *
   *  In effect, that makes any container of the library behave as a std::set
   *  or std::map. Through this iterator, a spatial container with 3
   *  dimensions can provide the same features as 3 std::set(s) or std::map(s)
   *  containing the same elements and ordered on each of these dimensions.
   *  Beware that iteration through the tree is very efficient when the
   *  dimension K-d tree is very small by comparison to the number of objects,
   *  but pretty inefficient otherwise, by comparison to a std::set.
   *
   *  \attention This iterator impose constness constraints on its \c
   *  value_type if the container's is a set and not a map. Iterators on sets
   *  prevent modification of the \c value_type because modifying the key may
   *  result in invalidation of the tree. If the container is a map, only the
   *  \c mapped_type can be modified (the \c second element).
   */
  template<typename Ct>
  struct mapping_iterator
    : details::Bidirectional_iterator
      <typename container_traits<Ct>::mode_type,
       typename container_traits<Ct>::rank_type,
       mapping_iterator<Ct> >
  {
  private:
    typedef details::Bidirectional_iterator
    <typename container_traits<Ct>::mode_type,
     typename container_traits<Ct>::rank_type,
     mapping_iterator<Ct> > Base;

  public:
    typedef typename container_traits<Ct>::key_compare key_compare;

    //! Uninitialized iterator.
    mapping_iterator() { }

    /**
     *  The standard way to build this iterator: specify a mapping dimension, an
     *  iterator on a container, and that container.
     *
     *  \param container   The container to iterate.
     *
     *  \param mapping_dim The dimension used to order all nodes during the
     *                     iteration.
     *
     *  \param node_dim    The dimension of the node pointed to by iterator.
     *
     *  \param iter        Use the value of \iter as the start point for the
     *                     iteration.
     */
    mapping_iterator(Ct& container, dimension_type mapping_dim,
                     const typename container_traits<Ct>::iterator& iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        data(container, mapping_dim)
    { except::check_dimension(container.dimension(), mapping_dim); }

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
     *  \param mapping_dim The dimension used to order all nodes during the
     *                     iteration.
     *
     *  \param dim The dimension of the node pointed to by iterator.
     *
     *  \param ptr Use the value of node as the start point for the
     *             iteration.
     */
    mapping_iterator(Ct& container, dimension_type mapping_dim,
                     dimension_type dim,
                     typename container_traits<Ct>::mode_type::node_ptr ptr)
      : Base(container.rank(), ptr, dim), data(container, mapping_dim)
    { except::check_dimension(container.dimension(), mapping_dim); }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    mapping_iterator<Ct>& operator++()
    { return increment_mapping(*this); }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    mapping_iterator<Ct> operator++(int)
    {
      mapping_iterator<Ct> x(*this);
      increment_mapping(*this);
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    mapping_iterator<Ct>& operator--()
    { return decrement_mapping(*this); }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    mapping_iterator<Ct> operator--(int)
    {
      mapping_iterator<Ct> x(*this);
      decrement_mapping(*this);
      return x;
    }

    //! Return the key_comparator used by the iterator
    const key_compare&
    key_comp() { return static_cast<const key_compare&>(data); }

    //! The related data for the iterator.
    details::Mapping_data<Ct> data;
  };

  /**
   *  All elements returned by this iterator are ordered from the smallest to
   *  the largest value of their key's coordinate along a single dimension,
   *  called the mapping dimension.
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
  struct mapping_iterator<const Ct>
    : details::Const_bidirectional_iterator
      <typename container_traits<Ct>::mode_type,
       typename container_traits<Ct>::rank_type,
       mapping_iterator<const Ct> >
  {
  private:
    typedef details::Const_bidirectional_iterator
    <typename container_traits<Ct>::mode_type,
     typename container_traits<Ct>::rank_type,
     mapping_iterator<const Ct> > Base;

  public:
    //! Alias for the key_compare type used by the iterator.
    typedef typename container_traits<Ct>::key_compare key_compare;

    //! Build an uninitialized iterator.
    mapping_iterator() { }

    /**
     *  The standard way to build this iterator: specify a mapping dimension,
     *  an iterator on a container, and that container.
     *
     *  \param container The container to iterate.
     *  \param mapping_dim The dimension used to order all nodes during the
     *  iteration.
     *  \param node_dim The dimension of the node pointed to by iterator.
     *  \param iter Use the value of \iter as the start point for the
     *  iteration.
     */
    mapping_iterator(const Ct& container, dimension_type mapping_dim,
                     const typename container_traits<Ct>::const_iterator& iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        data(container, mapping_dim)
    { except::check_dimension(container.dimension(), mapping_dim); }

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
     *  \param mapping_dim The dimension used to order all nodes during the
     *                     iteration.
     *
     *  \param dim The dimension of the node pointed to by iterator.
     *
     *  \param ptr Use the value of \c node as the start point for the
     *             iteration.
     */
    mapping_iterator(const Ct& container,
                     dimension_type mapping_dim, dimension_type dim,
                     typename container_traits<Ct>::mode_type::const_node_ptr
                     ptr)
      : Base(container.rank(), ptr, dim), data(container, mapping_dim)
    { except::check_dimension(container.dimension(), mapping_dim); }

    //! Convertion of mutable iterator into a constant iterator is permitted.
    mapping_iterator(const mapping_iterator<Ct>& iter)
      : Base(iter.rank(), iter.node, iter.node_dim), data(iter.data) { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    mapping_iterator<const Ct>& operator++()
    { return increment_mapping(*this); }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    mapping_iterator<const Ct> operator++(int)
    {
      mapping_iterator<const Ct> x(*this);
      increment_mapping(*this);
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    mapping_iterator<const Ct>& operator--()
    { return decrement_mapping(*this); }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    mapping_iterator<const Ct> operator--(int)
    {
      mapping_iterator<const Ct> x(*this);
      decrement_mapping(*this);
      return x;
    }

    //! Return the key_comparator used by the iterator
    const key_compare&
    key_comp() { return static_cast<const key_compare&>(data); }

    //! The related data for the iterator.
    details::Mapping_data<Ct> data;
  };

  //! Return true if 2 iterators are equal
  template<typename Ct>
  bool operator==(const mapping_iterator<Ct>& a,
                  const mapping_iterator<Ct>& b)
  { return a.node == b.node; }

  //! Return true if 2 iterators are equal
  template<typename Ct>
  bool operator==(const mapping_iterator<const Ct>& a,
                  const mapping_iterator<const Ct>& b)
  { return a.node == b.node; }

  //! Return true if 2 iterators are equal
  template<typename Ct>
  bool operator==(const mapping_iterator<Ct>& a,
                  const mapping_iterator<const Ct>& b)
  { return a.node == b.node; }

  //! Return true if 2 iterators are equal
  template<typename Ct>
  bool operator==(const mapping_iterator<const Ct>& a,
                  const mapping_iterator<Ct>& b)
  { return a.node == b.node; }

  //! Return true if 2 iterator are different
  template<typename Ct>
  bool operator!=(const mapping_iterator<Ct>& a,
                  const mapping_iterator<Ct>& b)
  { return !(a == b); }

  //! Return true if 2 iterator are different
  template<typename Ct>
  bool operator!=(const mapping_iterator<const Ct>& a,
                  const mapping_iterator<const Ct>& b)
  { return !(a == b); }

  //! Return true if 2 iterator are different
  template<typename Ct>
  bool operator!=(const mapping_iterator<Ct>& a,
                  const mapping_iterator<const Ct>& b)
  { return !(a == b); }

  //! Return true if 2 iterator are different
  template<typename Ct>
  bool operator!=(const mapping_iterator<const Ct>& a,
                  const mapping_iterator<Ct>& b)
  { return !(a == b); }

  namespace details
  {
    /**
     *  Move the pointer given in parameter to the next element in the ordered
     *  iteration of values along the mapping dimension.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  mapping<>::iterator(). In any case, use it cautiously, as this function
     *  does not perform any sanity checks on the iterator given in parameter.
     *
     *  \bestlogtime \worstdfractime Since Container is based on \kdtree and
     *  \kdtree exhibit good locality of reference (for arranging values in
     *  space, not for values location in memory), the function will run with
     *  time complexity close to \Olog in practice.
     */
    template <typename Container>
    mapping_iterator<Container>&
    increment_mapping(mapping_iterator<Container>& iter);

    /**
     *  Move the pointer given in parameter to the previous element in the
     *  ordered iteration of values along the mapping dimension.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  mapping<>::const_iterator. In any case, use it cautiously, as this
     *  function does not perform any sanity checks on the iterator given in
     *  parameter.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter The reference iterator that points to the current node.
     *  \return An iterator pointing to the value with the smallest coordinate
     *  along \c iter's \c mapping_dim, and among the children of the node
     *  pointed to by \c iter.
     *
     *  \bestlogtime \worstdfractime Since Container is based on \kdtree and
     *  \kdtree exhibit good locality of reference (for arranging values in
     *  space, not for values location in memory), the function will run with
     *  time complexity close to \Olog in practice.
     */
    template <typename Container>
    mapping_iterator<Container>&
    decrement_mapping(mapping_iterator<Container>& iter);

    /**
     *  Move the iterator given in parameter to the minimum value along the
     *  iterator's mapping dimension but only in the sub-tree composed of the
     *  node pointed to by the iterator and its children.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  mapping_begin(). In any case, use it cautiously, as this function does
     *  not perform any sanity checks on the iterator given in parameter.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter An iterator that points to the root node of the search.
     *  \return The iterator given in parameter is moved to the value with the
     *  smallest coordinate along \c iter's \c mapping_dim, and among the
     *  children of the node pointed to by \c iter.
     *
     *  \dfractime
     */
    template <typename Container>
    mapping_iterator<Container>&
    minimum_mapping(mapping_iterator<Container>& iter);

    /**
     *  Move the iterator given in parameter to the maximum value along the
     *  iterator's mapping dimension but only in the sub-tree composed of the
     *  node pointed to by the iterator and its children.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  mapping_begin(). In any case, use it cautiously, as this function does
     *  not perform any sanity checks on the iterator given in parameter.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter An iterator that points to the root node of the search.
     *  \return The iterator given in parameter is moved to the value with the
     *  largest coordinate along \c iter's \c mapping_dim, among the children of
     *  the node pointed to by \c iter.
     *
     *  \dfractime
     */
    template <typename Container>
    mapping_iterator<Container>&
    maximum_mapping(mapping_iterator<Container>& iter);

    /**
     *  Move the iterator given in parameter to the value with the smallest
     *  coordinate greater or equal to \c bound along the mapping dimension of
     *  \c iter, but only in the sub-tree composed of the node pointed to by the
     *  iterator and its children. If no such value exists, then move the
     *  iterator to the parent of the value currently pointed to.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  mapping_begin(). In any case, use it cautiously, as this function does
     *  not perform any sanity checks on the iterator given in parameter.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter An iterator that points to the root node of the search.
     *  \param bound The lower bound to the iterator position.
     *  \return An iterator pointing to the value with the smallest coordinate
     *  greater or equal to \c bound along \c iter's \c mapping_dim, or to the
     *  parent of the value pointed to.
     *
     *  \dfractime
     */
    template <typename Container>
    mapping_iterator<Container>&
    lower_bound_mapping
    (mapping_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound);

    /**
     *  Move the iterator given in parameter to the value with the largest
     *  coordinate strictly lower than \c bound along the mapping dimension of
     *  \c iter, but only in the sub-tree composed of the node pointed to by the
     *  iterator and its children. If no such value exists, then move the
     *  iterator to the parent of the value currently pointed to.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  mapping_begin(). In any case, use it cautiously, as this function does
     *  not perform any sanity checks on the iterator given in parameter.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter An iterator that points to the root node of the search.
     *  \param bound The upper bound to the iterator position.
     *  \return \c iter moved to the value with the largest coordinate strictly
     *  less than \c bound along \c iter's \c mapping_dim, or to the
     *  parent of the value pointed to.
     *
     *  \dfractime
     */
    template <typename Container>
    mapping_iterator<Container>&
    upper_bound_mapping
    (mapping_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound);

  } // namespace details

  /**
   *  This structure defines a pair of mutable mapping iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \see mapping_iterator
   */
  template <typename Ct>
  struct mapping_iterator_pair
        : std::pair<mapping_iterator<Ct>, mapping_iterator<Ct> >
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef std::pair<mapping_iterator<Ct>, mapping_iterator<Ct> > Base;

        //! Empty constructor.
        mapping_iterator_pair() { }

        //! Regular constructor that builds a mapping_iterator_pair out of 2
        //! mapping_iterators.
        mapping_iterator_pair(const mapping_iterator<Ct>& a,
                                  const mapping_iterator<Ct>& b) : Base(a, b) { }
  };

  /**
   *  This structure defines a pair of constant mapping iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \see mapping_iterator
   */
  template <typename Ct>
  struct mapping_iterator_pair<const Ct>
        : std::pair<mapping_iterator<const Ct>, mapping_iterator<const Ct> >
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef std::pair<mapping_iterator<const Ct>, mapping_iterator<const Ct> >
          Base;

        //! Empty constructor.
        mapping_iterator_pair() { }

        //! Regular constructor that builds a mapping_iterator_pair out of 2
        //! mapping_iterators.
        mapping_iterator_pair(const mapping_iterator<const Ct>& a,
                                  const mapping_iterator<const Ct>& b) : Base(a, b)
        { }

        //! Convert a mutable mapping iterator pair into a const mapping iterator
        //!pair.
        mapping_iterator_pair(const mapping_iterator_pair<Ct>& p)
          : Base(p.first, p.second) { }
  };

  /**
   *  Return the mapping dimension of the iterator.
   *  \param it the iterator where the mapping dimension is retreived.
   */
  template <typename Container>
  inline dimension_type
  mapping_dimension(const mapping_iterator<Container>& it)
  { return it.data.mapping_dim; }

  /**
   *  Sets the mapping dimension of the iterator.
   *  \param it The iterator where the mapping dimension is set.
   *  \param mapping_dim The new mapping dimension to use.
   */
  template <typename Container>
  inline void
  mapping_dimension(mapping_iterator<Container>& it,
                    dimension_type mapping_dim)
  {
    except::check_dimension(it.dimension(), mapping_dim);
    it.data.mapping_dim = mapping_dim;
  }

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
   *  \param mapping_dim The dimension that is the reference for the iteration:
   *  all iterated values will be ordered along this dimension, from smallest to
   *  largest.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the \ref Rank "rank" of the container.
   *  \return An iterator pointing to the past-the-end position in the
   *  container.
   *
   *  \consttime
   *  \see mapping
   */
  template <typename Container>
  inline mapping_iterator<Container>
  mapping_end(Container& container, dimension_type mapping_dim)
  {
    except::check_dimension(container.dimension(), mapping_dim);
    return mapping_iterator<Container>
      (container, mapping_dim, container.dimension() - 1,
       container.end().node); // At header (dim = rank - 1)
  }

  //@{
  /**
   *  Finds the past-the-end position in \c container for this constant
   *  iterator.
   *
   *  \tparam Container The type of container to iterate.
   *  \param mapping_dim The dimension that is the reference for the iteration:
   *  all iterated values will be ordered along this dimension, from smallest to
   *  largest.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the \ref Rank "rank" of the container.
   *  \return An iterator pointing to the past-the-end position in the
   *  container.
   *
   *  \consttime
   *  \see mapping
   */
  template <typename Container>
  inline mapping_iterator<const Container>
  mapping_end(const Container& container, dimension_type mapping_dim)
  {
    except::check_dimension(container.dimension(), mapping_dim);
    return mapping_iterator<const Container>
      (container, mapping_dim, container.dimension() - 1,
       container.end().node); // At header (dim = rank - 1)
  }

  template <typename Container>
  inline mapping_iterator<const Container>
  mapping_cend(const Container& container, dimension_type mapping_dim)
  { return mapping_end(container, mapping_dim); }
  //@}

  /**
   *  Finds the value in \c container for which its key has the smallest
   *  coordinate over the dimension \c mapping_dim.
   *
   *  \attention This iterator impose constness constraints on its \c value_type
   *  if the container's is a set and not a map. Iterators on sets prevent
   *  modification of the \c value_type because modifying the key may result in
   *  invalidation of the tree. If the container is a map, only the \c
   *  mapped_type can be modified (the \c second element).
   *
   *  \tparam Container The type of container to iterate.
   *  \param mapping_dim The dimension that is the reference for the iteration:
   *  all iterated values will be ordered along this dimension, from smallest to
   *  largest.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the \ref Rank "rank" of the container.
   *
   *  \dfractime
   *  \see mapping
   */
  template <typename Container>
  inline mapping_iterator<Container>
  mapping_begin(Container& container, dimension_type mapping_dim)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    mapping_iterator<Container> it(container, mapping_dim, 0,
                                   container.end().node->parent);
    return details::minimum_mapping(it);
  }

  //@{
  /**
   *  Finds the value in \c container for which its key has the smallest
   *  coordinate over the dimension \c mapping_dim.
   *
   *  \tparam Container The type of container to iterate.
   *  \param mapping_dim The dimension that is the reference for the iteration:
   *  all iterated values will be ordered along this dimension, from smallest to
   *  largest.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the \ref Rank "rank" of the container.
   *
   *  \dfractime
   *  \see mapping
   */
  template <typename Container>
  inline mapping_iterator<const Container>
  mapping_begin(const Container& container, dimension_type mapping_dim)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    mapping_iterator<const Container>
      it(container, mapping_dim, 0, container.end().node->parent);
    return details::minimum_mapping(it);
  }

  template <typename Container>
  inline mapping_iterator<const Container>
  mapping_cbegin(const Container& container, dimension_type mapping_dim)
  { return mapping_begin(container, mapping_dim); }
  //@}

  /**
   *  Returns a pair of iterator on the first and the last value in the range
   *  that can be iterated. This function is convenient to use with
   *  \tc{std::tie}, and is equivalent to calling \ref mapping_begin() and \ref
   *  mapping_end() on both iterators.
   *
   *  To combine it with \tc{std::tie}, use it this way:
   *  \code
   *  mapping<pointset<2, int> >::iterator it, end;
   *  std::tie(it, end) = mapping_range(0, my_points);
   *  for(; it != end; ++it)
   *  {
   *    // ...
   *  }
   *  \endcode
   *  Notice that an \c iterator type is declared, and not an \c iterator_pair
   *  type.
   *
   *  \attention This iterator impose constness constraints on its \c value_type
   *  if the container's is a set and not a map. Iterators on sets prevent
   *  modification of the \c value_type because modifying the key may result in
   *  invalidation of the tree. If the container is a map, only the \c
   *  mapped_type can be modified (the \c second element).
   *
   *  \tparam Container The type of container to iterate.
   *  \param mapping_dim The dimension that is the reference for the iteration:
   *  all iterated values will be ordered along this dimension, from smallest to
   *  largest.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the \ref Rank "rank" of the container.
   *  \return An iterator pointing to the past-the-end position in the
   *  container.
   *
   *  \dfractime
   *  \see mapping
   */
  template <typename Container>
  inline mapping_iterator_pair<Container>
  mapping_range(Container& container, dimension_type mapping_dim)
  {
    return mapping_iterator_pair<Container>
          (mapping_begin(container, mapping_dim),
           mapping_end(container, mapping_dim));
  }

  //@{
  /**
   *  Returns a pair of constant iterator on the first and the last value in the
   *  range that can be iterated. This function is convenient to use with
   *  \tc{std::tie}, and is equivalent to calling \ref mapping_begin() and \ref
   *  mapping_end() on both iterators.
   *
   *  To combine it with \tc{std::tie}, use it this way:
   *  \code
   *  mapping<pointset<2, int> >::iterator it, end;
   *  std::tie(it, end) = mapping_range(0, my_points);
   *  for(; it != end; ++it)
   *  {
   *    // ...
   *  }
   *  \endcode
   *  Notice that an \c iterator type is declared, and not an \c iterator_pair
   *  type.
   *
   *  \tparam Container The type of container to iterate.
   *  \param mapping_dim The dimension that is the reference for the iteration:
   *  all iterated values will be ordered along this dimension, from smallest to
   *  largest.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the \ref Rank "rank" of the container.
   *  \return An iterator pointing to the past-the-end position in the
   *  container.
   *
   *  \dfractime
   *  \see mapping
   */
  template <typename Container>
  inline mapping_iterator_pair<const Container>
  mapping_range(const Container& container, dimension_type mapping_dim)
  {
    return mapping_iterator_pair<const Container>
      (mapping_begin(container, mapping_dim),
       mapping_end(container, mapping_dim));
  }

  template <typename Container>
  inline mapping_iterator_pair<const Container>
  mapping_crange(const Container& container, dimension_type mapping_dim)
  {
    return mapping_iterator_pair<const Container>
      (mapping_begin(container, mapping_dim),
       mapping_end(container, mapping_dim));
  }
  //@}

  /**
   *  Finds the value with the smallest coordinate along the mapping dimension
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
   *  \param mapping_dim The dimension that is the reference for the iteration:
   *  all iterated values will be ordered along this dimension, from smallest to
   *  largest.
   *  \param bound The lowest bound to the iterator position.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the \ref Rank "rank" of the container.
   *  \return An iterator pointing to the value with the smallest coordinate
   *  greater or equal to \c bound along \c mapping_dim.
   *
   *  \dfractime
   *  \see mapping
   */
  template <typename Container>
  inline mapping_iterator<Container>
  mapping_lower_bound(Container& container, dimension_type mapping_dim,
                      const typename container_traits<Container>::key_type&
                      bound)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    mapping_iterator<Container> it
      (container, mapping_dim, 0, container.end().node->parent);
    return details::lower_bound_mapping(it, bound);
  }

  //@{
  /**
   *  Finds the value with the smallest coordinate along the mapping dimension
   *  that is greater or equal to \c bound, and return a constant iterator to
   *  this value.
   *
   *  \tparam Container The type of container to iterate.
   *  \param mapping_dim The dimension that is the reference for the iteration:
   *  all iterated values will be ordered along this dimension, from smallest to
   *  largest.
   *  \param bound The lowest bound to the iterator position.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the \ref Rank "rank" of the container.
   *  \return An iterator pointing to the value with the smallest coordinate
   *  greater or equal to \c bound along \c mapping_dim.
   *
   *  \dfractime
   *  \see mapping
   */
  template <typename Container>
  inline mapping_iterator<const Container>
  mapping_lower_bound
  (const Container& container, dimension_type mapping_dim,
   const typename container_traits<Container>::key_type& bound)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    mapping_iterator<const Container> it
      (container, mapping_dim, 0, container.end().node->parent);
    return details::lower_bound_mapping(it, bound);
  }

  template <typename Container>
  inline mapping_iterator<const Container>
  mapping_clower_bound
  (const Container& container, dimension_type mapping_dim,
   const typename container_traits<Container>::key_type& bound)
  { return mapping_lower_bound(container, mapping_dim, bound); }
  //@}

  /**
   *  Finds the value with the largest coordinate along the mapping dimension
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
   *  \param mapping_dim The dimension that is the reference for the iteration:
   *  all iterated values will be ordered along this dimension, from smallest to
   *  largest.
   *  \param bound The lowest bound to the iterator position.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the \ref Rank "rank" of the container.
   *  \return An iterator pointing to the value with the smallest coordinate
   *  greater or equal to \c bound along \c mapping_dim.
   *
   *  \dfractime
   *  \see mapping
   */
  template <typename Container>
  inline mapping_iterator<Container>
  mapping_upper_bound
  (Container& container, dimension_type mapping_dim,
   const typename container_traits<Container>::key_type& bound)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    mapping_iterator<Container> it // At root (dim = 0)
      (container, mapping_dim, 0, container.end().node->parent);
    return details::upper_bound_mapping(it, bound);
  }

  //@{
  /**
   *  Finds the value with the largest coordinate along the mapping dimension
   *  that is stricly less than \c bound, and return an iterator pointing to
   *  this value.
   *
   *  \tparam Container The type of container to iterate.
   *  \param mapping_dim The dimension that is the reference for the iteration:
   *  all iterated values will be ordered along this dimension, from smallest to
   *  largest.
   *  \param bound The lowest bound to the iterator position.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the \ref Rank "rank" of the container.
   *  \return An iterator pointing to the value with the smallest coordinate
   *  greater or equal to \c bound along \c mapping_dim.
   *
   *  \dfractime
   *  \see mapping
   */
  template <typename Container>
  inline mapping_iterator<const Container>
  mapping_upper_bound
  (const Container& container, dimension_type mapping_dim,
   const typename container_traits<Container>::key_type& bound)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    mapping_iterator<const Container> it  // At root (dim = 0)
      (container, mapping_dim, 0, container.end().node->parent);
    return details::upper_bound_mapping(it, bound);
  }

  template <typename Container>
  inline mapping_iterator<const Container>
  mapping_cupper_bound
  (const Container& container, dimension_type mapping_dim,
   const typename container_traits<Container>::key_type& bound)
  { return mapping_upper_bound(container, mapping_dim, bound); }
  //@}

} // namespace spatial

#include "spatial_mapping.tpp"

#endif // SPATIAL_MAPPING_HPP
