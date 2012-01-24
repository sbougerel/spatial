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
       *  \param n The node dimension for the node pointed to by the iterator.
       */
      Mapping_data
      (const Ct& c, dimension_type m, dimension_type n)
        : ::spatial::container_traits<Ct>::rank_type(c.rank()),
          mapping_dim(::spatial::container_traits<Ct>
                      ::key_compare(c.key_comp()), m), node_dim(n)
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

      /**
       *  The dimension for node.
       *
       *  \Warning Modifing this attribute can potientially invalidate the
       *  iterator. Do not modify this attribute unless you know what you're
       *  doing.
       */
      dimension_type node_dim;
    };

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
  struct mapping
  {
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
    struct iterator : ::spatial::details::bidirectional_iterator
    <typename container_traits<Ct>::mode_type, iterator,
     ::std::tr1::is_same<typename container_traits<Ct>::key_type,
                         typename container_traits<Ct>::value_type>::value>
    {
    private:
      typedef typename ::spatial::details::bidirectional_iterator
      <typename container_traits<Ct>::mode_type, iterator,
       ::std::tr1::is_same<typename container_traits<Ct>::key_type,
                           typename container_traits<Ct>::value_type>::value>
      Base;

    public:
      //! Uninitialized iterator.
      iterator() { }

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
      iterator(Ct& container, dimension_type mapping_dim,
               typename container_traits<Ct>::iterator iter)
        : Base(iter.node),
          data(container, mapping_dim, modulo(iter.node, container.rank())) { }

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
       *  \param node Use the value of node as the start point for the
       *  iteration.
       */
      iterator(Ct& container, dimension_type mapping_dim,
	       dimension_type node_dim,
	       typename container_traits<Ct>::mode_type::node_ptr node)
        : Base(node), data(container, mapping_dim, node_dim) { }

      //@{
      /**
       *  This iterator can be casted silently into a container iterator. You can
       *  therefore use this iterator as an argument to the erase function of
       *  the container, for example.
       *
       *  \warning When using this iterator as an argument to the erase function
       *  of the container, this iterator will get invalidated after erase.
       */
      operator typename container_traits<Ct>::iterator()
      { return container_traits<Ct>::iterator(Base::node); }

      operator typename container_traits<Ct>::const_iterator()
      { return container_traits<Ct>::const_iterator(Base::node); }
      //@}

      //! The related data for the iterator.
      ::spatial::details::Mapping_data<Ct> data;
    };

    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef ::std::pair<iterator, iterator>             iterator_pair;
  };

  /**
   *  This specialization defines the constant mapping iterator type, as well as
   *  a pair of each iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \see mapping<>::iterator
   *  \see mapping<>::const_iterator
   */
  template <typename Ct>
  struct mapping<const Ct>
  {
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
    struct iterator : ::spatial::details::bidirectional_iterator
    <typename container_traits<Ct>::mode_type, iterator, true>
    {
    private:
      typedef typename ::spatial::details::bidirectional_iterator
      <typename container_traits<Ct>::mode_type, iterator, true> Base;

    public:
      //! Build an uninitialized iterator.
      iterator() { }

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
      iterator(const Ct& container, dimension_type dim,
               typename container_traits<const Ct>::iterator iter)
        : Base(iter.node),
          data(dim, modulo(iter.node, container.rank()), container) { }

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
      iterator(const Ct& container,
	       dimension_type mapping_dim, dimension_type node_dim,
	       typename container_traits<Ct>::mode_type::node_ptr node)
        : Base(node), data(container, mapping_dim, node_dim) { }

      //! Convertion of mutable iterator into a constant iterator is permitted.
      iterator(const typename mapping<Ct>::iterator& iter)
        : Base(iter.node), data(iter.data) { }

      /**
       *  This iterator can be casted silently into a container iterator. You can
       *  therefore use this iterator as an argument to the erase function of
       *  the container, for example.
       *
       *  \warning When using this iterator as an argument to the erase function
       *  of the container, this iterator will get invalidated after erase.
       */
      operator typename container_traits<Ct>::const_iterator()
      { return container_traits<Ct>::const_iterator(Base::node); }

      //! The related data for the iterator.
      ::spatial::details::Mapping_data<Ct> data;
    };

    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef ::std::pair<iterator, iterator>             iterator_pair;
  };

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
    typename mapping<Container>::iterator&
    increment(typename mapping<Container>::iterator& iter);

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
    typename mapping<Container>::iterator&
    decrement(typename mapping<Container>::iterator& iter);

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
    typename mapping<Container>::iterator&
    minimum(typename mapping<Container>::iterator& iter);

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
    typename mapping<Container>::iterator&
    maximum(typename mapping<Container>::iterator& iter);

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
    typename mapping<Container>::iterator&
    lower_bound(typename mapping<Container>::iterator& iter,
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
    typename mapping<Container>::iterator&
    upper_bound(typename mapping<Container>::iterator& iter,
                const typename container_traits<Container>::key_type& bound);

  } // namespace details

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
  inline typename mapping<Container>::iterator
  mapping_begin(Container& container, dimension_type mapping_dim)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    typename mapping<Container>::iterator
      it(container, mapping_dim, 0, container.top()); // At root (dim = 0)
    return ::spatial::details::minimum(it);
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
  inline typename mapping<const Container>::iterator
  mapping_begin(const Container& container, dimension_type mapping_dim)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    typename mapping<const Container>::iterator
      it(container, mapping_dim, 0, container.top()); // At root (dim = 0)
    return ::spatial::details::minimum(it);
  }

  template <typename Container>
  inline typename mapping<const Container>::iterator
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
  inline typename mapping<Container>::iterator
  mapping_end(Container& container, dimension_type mapping_dim)
  {
    except::check_dimension(container.dimension(), mapping_dim);
    return mapping<Container>::iterator
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
  inline typename mapping<const Container>::iterator
  mapping_end(const Container& container, dimension_type mapping_dim)
  {
    except::check_dimension(container.dimension(), mapping_dim);
    return mapping<const Container>::iterator
      (container, mapping_dim, container.dimension() - 1,
       container.end()); // At header (dim = rank - 1)
  }

  template <typename Container>
  inline typename mapping<Container>::iterator
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
  inline typename mapping<Container>::iterator_pair
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
  inline typename mapping<const Container>::iterator_pair
  mapping_range(const Container& container, dimension_type dim)
  {
    return ::std::make_pair(mapping_begin(container, dim),
                            mapping_end(container, dim));
  }

  template <typename Container>
  inline typename mapping<const Container>::iterator_pair
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
  inline typename mapping<Container>::iterator
  mapping_lower_bound(Container& container, dimension_type mapping_dim,
                      const typename container_traits<Container>::key_type&
		      bound)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    typename mapping<Container>::iterator it
      (container, mapping_dim, 0, container.top()); // At root (dim = 0)
    return ::spatial::details::lower_bound(it, bound);
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
  inline typename mapping<const Container>::iterator
  mapping_lower_bound(const Container& container, dimension_type dim,
                const typename container_traits<Container>::key_type& bound)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    typename mapping<const Container>::iterator it
      (container, mapping_dim, 0, container.top()); // At root (dim = 0)
    return ::spatial::details::lower_bound(it, bound);
  }

  template <typename Container>
  inline typename mapping<const Container>::iterator
  mapping_clower_bound(const Container& container, dimension_type dim,
                       const typename container_traits<Container>::key_type&
		       bound)
  {
    return mapping_lower_bound(container, dim, bound);
  }
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
  inline typename mapping<Container>::iterator
  mapping_upper_bound(Container& container, dimension_type mapping_dim,
                      const typename container_traits<Container>::key_type&
		      bound)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    typename mapping<Container>::iterator it
      (container, mapping_dim, 0, container.top()); // At root (dim = 0)
    return ::spatial::details::upper_bound(it, bound);
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
  inline typename mapping<const Container>::iterator
  mapping_upper_bound(const Container& container, dimension_type mapping_dim,
                      const typename container_traits<Container>::key_type&
		      bound)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    typename mapping<const Container>::iterator it
      (container, mapping_dim, 0, container.top()); // At root (dim = 0)
    return ::spatial::details::upper_bound(it, bound);
  }

  template <typename Container>
  inline typename mapping<Container>::iterator
  mapping_cupper_bound(const Container& container, dimension_type mapping_dim,
                 const typename container_traits<Container>::key_type& bound)
  {
    return mapping_upper_bound(container, mapping_dim, bound);
  }
  //@}

} // namespace spatial

#include "spatial_mapping.tpp"

#endif // SPATIAL_MAPPING_HPP
