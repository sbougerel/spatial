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
    struct Mapping_data
      : ::spatial::container_traits<Ct>::rank_type
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
        : ::spatial::container_traits<Ct>::rank_type(c.rank()),
          mapping_dim(typename ::spatial::container_traits<Ct>
                      ::key_compare(c.key_comp()), m)
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
      Compress<typename ::spatial::container_traits<Ct>::key_compare,
               dimension_type> mapping_dim;
    };

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
    struct Iterator_mapping : ::spatial::details::bidirectional_iterator
    <typename container_traits<Ct>::mode_type, Iterator_mapping<Ct>, false>
    {
    private:
      typedef typename ::spatial::details::bidirectional_iterator
      <typename container_traits<Ct>::mode_type, Iterator_mapping<Ct>, false>
      Base;

      template<typename Iterator> struct Rebind
      {
        typedef typename ::spatial::details::bidirectional_iterator
        <typename container_traits<Ct>::mode_type, Iterator, false> type;
      };

    public:
      //! Uninitialized iterator.
      Iterator_mapping() { }

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
      Iterator_mapping(Ct& container, dimension_type mapping_dim,
                       typename container_traits<Ct>::iterator iter)
        : Base(iter.node, modulo(iter.node, container.rank())),
          data(container, mapping_dim) { }

      /**
       *  Build this iterator from another bidirectional iterator.
       *
       *  \param container The container to iterate.
       *  \param mapping_dim The dimension used to order all nodes during the
       *  iteration.
       *  \param node_dim The dimension of the node pointed to by iterator.
       *  \param iter Use the node currenly pointed to by iter as a staring
       *  point of the iteration.
       */
      template<typename Iterator>
      Iterator_mapping(Ct& container, dimension_type mapping_dim,
                       const typename Rebind<Iterator>::type& iter)
        : Base(iter.node, iter.node_dim), data(container, mapping_dim) { }

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
       *  \param mapping_dim The dimension used to order all nodes during the
       *  iteration.
       *  \param dim The dimension of the node pointed to by iterator.
       *  \param ptr Use the value of node as the start point for the
       *  iteration.
       */
      Iterator_mapping(Ct& container, dimension_type mapping_dim,
                       dimension_type dim,
                       typename container_traits<Ct>::mode_type::node_ptr ptr)
        : Base(ptr, dim), data(container, mapping_dim) { }

      //@{
      /**
       *  This iterator can be casted silently into a container iterator. You can
       *  therefore use this iterator as an argument to the erase function of
       *  the container, for example.
       *
       *  \warning When using this iterator as an argument to the erase function
       *  of the container, this iterator will get invalidated after erase.
       */
      operator typename container_traits<Ct>::iterator() const
      { return container_traits<Ct>::iterator(Base::node); }

      operator typename container_traits<Ct>::const_iterator() const
      { return container_traits<Ct>::const_iterator(Base::node); }
      //@}

      //! Increments the iterator and returns the incremented value. Prefer to
      //! use this form in \c for loops.
      Iterator_mapping<Ct>& operator++()
      { return increment_mapping(*this); }

      //! Increments the iterator but returns the value of the iterator before
      //! the increment. Prefer to use the other form in \c for loops.
      Iterator_mapping<Ct> operator++(int)
      {
        Iterator_mapping<Ct> x(*this);
        increment_mapping(*this);
        return x;
      }

      //! Decrements the iterator and returns the decremented value. Prefer to
      //! use this form in \c for loops.
      Iterator_mapping<Ct>& operator--()
      { return decrement_mapping(*this); }

      //! Decrements the iterator but returns the value of the iterator before
      //! the decrement. Prefer to use the other form in \c for loops.
      Iterator_mapping<Ct> operator--(int)
      {
        Iterator_mapping<Ct>& x(*this);
        decrement_mapping(*this);
        return x;
      }

      //! The related data for the iterator.
      ::spatial::details::Mapping_data<Ct> data;
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
    struct Iterator_mapping<const Ct>
      : ::spatial::details::bidirectional_iterator
    <typename container_traits<Ct>::mode_type, Iterator_mapping<Ct>, true>
    {
    private:
      typedef typename ::spatial::details::bidirectional_iterator
      <typename container_traits<Ct>::mode_type, Iterator_mapping<Ct>, true>
      Base;

      template<typename Iterator> struct Rebind
      {
        typedef typename ::spatial::details::bidirectional_iterator
        <typename container_traits<Ct>::mode_type, Iterator, true> type;
      };

    public:
      //! Build an uninitialized iterator.
      Iterator_mapping() { }

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
      Iterator_mapping(const Ct& container, dimension_type dim,
                       typename container_traits<const Ct>::iterator iter)
        : Base(iter.node, modulo(iter.node, container.rank())),
          data(dim, container) { }

      /**
       *  Build this iterator from another bidirectional iterator.
       *
       *  \param container The container to iterate.
       *  \param mapping_dim The dimension used to order all nodes during the
       *  iteration.
       *  \param node_dim The dimension of the node pointed to by iterator.
       *  \param iter Use the node currenly pointed to by iter as a staring
       *  point of the iteration.
       */
      template<typename Iterator>
      Iterator_mapping(const Ct& container, dimension_type mapping_dim,
                       const typename Rebind<Iterator>::type& iter)
        : Base(iter.node, iter.node_dim),
          data(container, mapping_dim) { }

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
       *  \param mapping_dim The dimension used to order all nodes during the
       *  iteration.
       *  \param node_dim The dimension of the node pointed to by iterator.
       *  \param node Use the value of \c node as the start point for the
       *  iteration.
       */
      Iterator_mapping(const Ct& container,
                       dimension_type mapping_dim, dimension_type node_dim,
                       typename container_traits<Ct>::mode_type::node_ptr node)
        : Base(node, node_dim), data(container, mapping_dim) { }

      //! Convertion of mutable iterator into a constant iterator is permitted.
      Iterator_mapping(const Iterator_mapping<Ct>& iter)
        : Base(iter.node, iter.node_dim), data(iter.data) { }

      /**
       *  This iterator can be casted silently into a container iterator. You can
       *  therefore use this iterator as an argument to the erase function of
       *  the container, for example.
       *
       *  \warning When using this iterator as an argument to the erase function
       *  of the container, this iterator will get invalidated after erase.
       */
      operator typename container_traits<Ct>::const_iterator() const
      { return container_traits<Ct>::const_iterator(Base::node); }

      //! Increments the iterator and returns the incremented value. Prefer to
      //! use this form in \c for loops.
      Iterator_mapping<const Ct>& operator++()
      { return increment_mapping(*this); }

      //! Increments the iterator but returns the value of the iterator before
      //! the increment. Prefer to use the other form in \c for loops.
      Iterator_mapping<const Ct> operator++(int)
      {
        Iterator_mapping x(*this);
        increment_mapping(*this);
        return x;
      }

      //! Decrements the iterator and returns the decremented value. Prefer to
      //! use this form in \c for loops.
      Iterator_mapping<const Ct>& operator--()
      { return decrement_mapping(*this); }

      //! Decrements the iterator but returns the value of the iterator before
      //! the decrement. Prefer to use the other form in \c for loops.
      Iterator_mapping<const Ct> operator--(int)
      {
        Iterator_mapping& x(*this);
        decrement_mapping(*this);
        return x;
      }

      //! The related data for the iterator.
      ::spatial::details::Mapping_data<Ct> data;
    };

    //! Return true if 2 iterators are equal
    template<typename Ct>
    bool operator==(const Iterator_mapping<const Ct>& a,
                    const Iterator_mapping<const Ct>& b)
    { return a.node == b.node; }

    //! Return true if 2 iterators are equal
    template<typename Ct>
    bool operator==(const Iterator_mapping<Ct>& a,
                    const Iterator_mapping<const Ct>& b)
    { return a.node == b.node; }

    //! Return true if 2 iterator are different
    template<typename Ct>
    bool operator!=(const Iterator_mapping<const Ct>& a,
                    const Iterator_mapping<const Ct>& b)
    { return !(a == b); }

    //! Return true if 2 iterator are different
    template<typename Ct>
    bool operator!=(const Iterator_mapping<Ct>& a,
                    const Iterator_mapping<const Ct>& b)
    { return !(a == b); }

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
    Iterator_mapping<Container>&
    increment_mapping(Iterator_mapping<Container>& iter);

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
    Iterator_mapping<Container>&
    decrement_mapping(Iterator_mapping<Container>& iter);

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
    Iterator_mapping<Container>&
    minimum_mapping(Iterator_mapping<Container>& iter);

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
    Iterator_mapping<Container>&
    maximum_mapping(Iterator_mapping<Container>& iter);

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
    Iterator_mapping<Container>&
    lower_bound_mapping
    (Iterator_mapping<Container>& iter,
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
    Iterator_mapping<Container>&
    upper_bound_mapping
    (Iterator_mapping<Container>& iter,
     const typename container_traits<Container>::key_type& bound);

  } // namespace details

  /**
   *  This structure defines the mapping mutable iterator type as well as a pair
   *  of each iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \see mapping<>::iterator
   *  \see mapping<>::const_iterator
   */
  template <typename Ct>
  struct mapping_iterator
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef details::Iterator_mapping
    <typename details::condition
     <std::tr1::is_same<typename container_traits<Ct>::key_type,
                        typename container_traits<Ct>::value_type>::value,
      const Ct, Ct>::type> type;
  };

  /**
   *  This structure defines the mapping mutable iterator type as well as a pair
   *  of each iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \see mapping<>::iterator
   *  \see mapping<>::const_iterator
   */
  template <typename Ct>
  struct mapping_iterator<const Ct>
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef details::Iterator_mapping<const Ct> type;
  };

  /**
   *  This structure defines the mapping mutable iterator type as well as a pair
   *  of each iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \see mapping<>::iterator
   *  \see mapping<>::const_iterator
   */
  template <typename Ct>
  struct mapping_iterator_pair
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef ::std::pair<typename mapping_iterator<Ct>::type,
                        typename mapping_iterator<Ct>::type > type;
  };

  /**
   *  Return the mapping dimension of the iterator, and allow to modify it.
   *  \param it the iterator where the mapping dimension is retreived.
   */
  //@{
  template <typename Container>
  inline dimension_type
  mapping_dimension(const details::Iterator_mapping<Container>& it)
  { return it.data.mapping_dim(); }

  template <typename Container>
  inline dimension_type&
  mapping_dimension(details::Iterator_mapping<Container>& it)
  { return it.data.mapping_dim(); }
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
  inline typename mapping_iterator<Container>::type
  mapping_begin(Container& container, dimension_type mapping_dim)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    typename mapping_iterator<Container>::type
      it(container, mapping_dim, 0, container.top()); // At root (dim = 0)
    return ::spatial::details::minimum_mapping(it);
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
  inline typename mapping_iterator<const Container>::type
  mapping_begin(const Container& container, dimension_type mapping_dim)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    typename mapping_iterator<const Container>::type
      it(container, mapping_dim, 0, container.top()); // At root (dim = 0)
    return ::spatial::details::minimum_mapping(it);
  }

  template <typename Container>
  inline typename mapping_iterator<const Container>::type
  mapping_cbegin(const Container& container, dimension_type mapping_dim)
  { return mapping_begin(container, mapping_dim); }
  //@}

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
  inline typename mapping_iterator<Container>::type
  mapping_end(Container& container, dimension_type mapping_dim)
  {
    except::check_dimension(container.dimension(), mapping_dim);
    return mapping_iterator<Container>::type
      (container, mapping_dim, container.dimension() - 1,
       container.end()); // At header (dim = rank - 1)
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
  inline typename mapping_iterator<const Container>::type
  mapping_end(const Container& container, dimension_type mapping_dim)
  {
    except::check_dimension(container.dimension(), mapping_dim);
    return mapping_iterator<const Container>::type
      (container, mapping_dim, container.dimension() - 1,
       container.end()); // At header (dim = rank - 1)
  }

  template <typename Container>
  inline typename mapping_iterator<Container>::type
  mapping_cend(const Container& container, dimension_type mapping_dim)
  { return mapping_end(container, mapping_dim); }
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
  inline typename mapping_iterator_pair<Container>::type
  mapping_range(Container& container, dimension_type mapping_dim)
  {
    return ::std::make_pair(mapping_begin(container, mapping_dim),
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
  inline typename mapping_iterator_pair<const Container>::type
  mapping_range(const Container& container, dimension_type dim)
  {
    return ::std::make_pair(mapping_begin(container, dim),
                            mapping_end(container, dim));
  }

  template <typename Container>
  inline typename mapping_iterator_pair<const Container>::type
  mapping_crange(const Container& container, dimension_type dim)
  {
    return ::std::make_pair(mapping_begin(container, dim),
                            mapping_end(container, dim));
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
  inline typename mapping_iterator<Container>::type
  mapping_lower_bound(Container& container, dimension_type mapping_dim,
                      const typename container_traits<Container>::key_type&
                      bound)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    typename mapping_iterator<Container>::type it
      (container, mapping_dim, 0, container.top()); // At root (dim = 0)
    return ::spatial::details::lower_bound_mapping(it, bound);
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
  inline typename mapping_iterator<const Container>::type
  mapping_lower_bound
  (const Container& container, dimension_type mapping_dim,
   const typename container_traits<Container>::key_type& bound)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    typename mapping_iterator<const Container>::type it
      (container, mapping_dim, 0, container.top()); // At root (dim = 0)
    return ::spatial::details::lower_bound_mapping(it, bound);
  }

  template <typename Container>
  inline typename mapping_iterator<const Container>::type
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
  inline typename mapping_iterator<Container>::type
  mapping_upper_bound
  (Container& container, dimension_type mapping_dim,
   const typename container_traits<Container>::key_type& bound)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    typename mapping_iterator<Container>::type it
      (container, mapping_dim, 0, container.top()); // At root (dim = 0)
    return ::spatial::details::upper_bound_mapping(it, bound);
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
  inline typename mapping_iterator<const Container>::type
  mapping_upper_bound
  (const Container& container, dimension_type mapping_dim,
   const typename container_traits<Container>::key_type& bound)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    typename mapping_iterator<const Container>::type it
      (container, mapping_dim, 0, container.top()); // At root (dim = 0)
    return ::spatial::details::upper_bound_mapping(it, bound);
  }

  template <typename Container>
  inline typename mapping_iterator<Container>::type
  mapping_cupper_bound
  (const Container& container, dimension_type mapping_dim,
   const typename container_traits<Container>::key_type& bound)
  {
    return mapping_upper_bound(container, mapping_dim, bound);
  }
  //@}

} // namespace spatial

#include "spatial_mapping.tpp"

#endif // SPATIAL_MAPPING_HPP
