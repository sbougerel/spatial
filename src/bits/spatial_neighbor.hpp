// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_neighbor.hpp
 *  @brief  Contains the definition of the neighbor iterators. These iterators
 *  walk through all items in the container in order from the closest to the
 *  furthest away from a given key.
 *
 *  \see neighbor
 */

#ifndef SPATIAL_NEIGHBOR_HPP
#define SPATIAL_NEIGHBOR_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#include "spatial_geometry.hpp"

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
     *  \tparam Geometry The type of geometry applied to the iterator.
     */
    template<typename Ct, typename Geometry>
    struct Neighbor_data
    {
      //! Build an unintialized neighbor data object.
      Neighbor_data() { }

      /**
       *  Builds required neighbor data from the given container, geometry and
       *  dimension.
       *
       *  \param c The container being iterated.
       *  \param g The geometry to apply during iteration
       *  \param n The node dimension for the node pointed to by the iterator.
       */
      Neighbor_data
      (const Ct& c, const Geometry& g,
       const typename container_traits<Ct>::key_type& k)
        : compare(c.key_comp()), distance(g), target(k)
      { }

      /**
       *  The comparison functor for keys, compacted with Geometry.
       */
      typename container_traits<Ct>::key_compare compare;

      /**
       *  The last valid computed value of the distance. The value stored is
       *  only valid if the iterator is not past-the-end.
       */
      Compress<Geometry, typename Geometry::distance_type> distance;

      /**
       *  The target of the iteration; element of the container are iterate
       *  from the closest to the element furthest away from the target.
       */
      typename container_traits<Ct>::key_type target;
    };

  } // namespace details

  /**
   *  An iterator implementation meant to find nearest and furthest neighbors
   *  in a \kdtree where distances are applied in an arbitrary geometric space.
   */
  template<typename Ct, typename Geometry>
  struct neighbor
  {
    /**
     *  A spatial iterator for a container \c Ct that goes through the nearest
     *  to the furthest element from a target key, with distances applied
     *  according to a user-defined geometric space of type \c Geometry.
     *
     *  The Geometry type is a complex type that must be a model of \ref
     *  Geometry:
     *
     *  \code
     *  struct Geometry
     *  {
     *    typedef my_distance_type distance_type;
     *
     *    distance_type
     *    distance_to_key(dimension_type rank,
     *                    const Key& origin, const Key& key) const;
     *
     *    distance_type
     *    distance_to_plane(dimension_type rank, dimension_type dim,
     *                      const Key& origin, const Key& key) const;
     *  };
     *  \endcode
     *
     *  The details of the \c Geometry type are explained in \ref Geometry.
     *  Geometries are generally not defined by the user of the library, given
     *  their complexity. Rather, the user of the library uses ready-made
     *  models of \ref Geometry such as \ref euclid and \ref manhattan. If
     *  more geometries needs to be defined, see the explanation in \ref
     *  Geometry.
     */
    struct iterator : details::Bidirectional_iterator
                      <typename container_traits<Ct>::mode_type,
                       typename container_traits<Ct>::rank_type>
    {
    private:
      typedef typename details::Bidirectional_iterator
        <typename container_traits<Ct>::mode_type,
         typename container_traits<Ct>::rank_type> Base;

      template<typename Iterator> struct Rebind
      {
        typedef typename ::spatial::details::Bidirectional_iterator
        <typename container_traits<Ct>::mode_type,
         typename container_traits<Ct>::rank_type> type;
      };

    public:
      //! Uninitialized iterator.
      iterator() { }

      /**
       *  The standard way to build this iterator: specify a geometry to apply,
       *  an iterator on a container, and that container.
       *
       *  \param container The container to iterate.
       *  \param geometry The geometry applied during the iteration.
       *  \param target The target of the neighbor iteration.
       *  \param it An iterator on container.
       */
      iterator(Ct& container, const Geometry& geometry,
               const typename container_traits<Ct>::key_type& target,
               typename container_traits<Ct>::iterator it)
        : Base(container.rank(), it.node, modulo(it.node, container.rank())),
          data(container, geometry, target) { }

      /**
       *  Create a neighbor iterator from a container, a geometry and another
       *  bidirectional iterator on that container.
       *
       *  \param container The container to iterate.
       *  \param geometry The geometry applied during the iteration.
       *  \param target The target of the neighbor iteration.
       *  \param it An iterator on container.
       */
      template<typename Iterator>
      iterator(Ct& container, const Geometry& geometry,
               const typename container_traits<Ct>::key_type& target,
               const typename Rebind<Iterator>::type& it)
        : Base(container.rank(), it.node, it.node_dim),
          data(container, geometry, target) { }

      /**
       *  When the information of the dimension for the current node being
       *  pointed to by the iterator is known, this constructor saves some CPU
       *  cycle, by comparison to the other constructor.
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
       *  \param geometry The geometry applied during the iteration.
       *  \param target The target of the neighbor iteration.
       *  \param node_dim The dimension of the node pointed to by iterator.
       *  \param node Use the value of node as the start point for the
       *  iteration.
       */
      iterator(Ct& container, const Geometry& geometry,
               const typename container_traits<Ct>::key_type& target,
               dimension_type node_dim,
               typename container_traits<Ct>::mode_type::node_ptr node)
        : Base(container.rank(), node, node_dim),
          data(container, geometry, target) { }

      //@{
      /**
       *  This iterator can be casted silently into a container iterator. You
       *  can therefore use this iterator as an argument to the erase function
       *  of the container, for example.
       *
       *  \warning When using this iterator as an argument to the erase
       *  function of the container, this iterator will get invalidated after
       *  erase.
       */
      operator typename container_traits<Ct>::iterator()
      { return container_traits<Ct>::iterator(Base::node); }

      operator typename container_traits<Ct>::const_iterator()
      { return container_traits<Ct>::const_iterator(Base::node); }
      //@}

      //! The related data for the iterator.
      ::spatial::details::Neighbor_data<Ct, Geometry> data;
    };

    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef ::std::pair<iterator, iterator> iterator_pair;
  };

  /**
   *  An iterator implementation meant to find nearest and furthest neighbors
   *  in a \kdtree where distances are applied in an arbitrary geometric space.
   *  This iterator is specialized to work on constant containers, where
   *  elements returned by the iterator are always constant.
   */
  template<typename Ct, typename Geometry>
  struct neighbor<const Ct, Geometry>
  {
    /**
     *  A spatial iterator for a container \c Ct that goes through the nearest
     *  to the furthest element from a target key, with distances applied
     *  according to a user-defined geometric space of type \c Geometry.
     *
     *  The Geometry type is a complex type that must be a model of \ref
     *  Geometry:
     *
     *  \code
     *  struct Geometry
     *  {
     *    typedef my_distance_type distance_type;
     *
     *    distance_type
     *    distance_to_key(dimension_type rank,
     *                    const Key& origin, const Key& key) const;
     *
     *    distance_type
     *    distance_to_plane(dimension_type rank, dimension_type dim,
     *                      const Key& origin, const Key& key) const;
     *  };
     *  \endcode
     *
     *  The details of the \c Geometry type are explained in \ref Geometry.
     *  Geometries are generally not defined by the user of the library, given
     *  their complexity. Rather, the user of the library uses ready-made
     *  models of \ref Geometry such as \ref euclid and \ref manhattan. If
     *  more geometries needs to be defined, see the explanation in \ref
     *  Geometry.
     *
     *  This iterator only returns constant objects.
     */
    struct iterator
      : details::Const_bidirectional_iterator
        <typename container_traits<Ct>::mode_type,
         typename container_traits<Ct>::rank_type>
    {
    private:
      typedef typename details::Const_bidirectional_iterator
        <typename container_traits<Ct>::mode_type,
         typename container_traits<Ct>::rank_type> Base;

      template<typename Iterator> struct Rebind
      {
        typedef typename ::spatial::details::Const_bidirectional_iterator
        <typename container_traits<Ct>::mode_type,
         typename container_traits<Ct>::rank_type> type;
      };

    public:
      //! Uninitialized iterator.
      iterator() { }

      /**
       *  The standard way to build this iterator: specify a geometry to apply,
       *  an iterator on a container, and that container.
       *
       *  \param container The container to iterate.
       *  \param geometry The geometry applied during the iteration.
       *  \param target The target of the neighbor iteration.
       *  \param iter An iterator on \c container.
       */
      iterator(const Ct& container, const Geometry& geometry,
               const typename container_traits<Ct>::key_type& target,
               typename container_traits<Ct>::iterator iter)
        : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
          data(container, geometry, target) { }

      /**
       *  Create a neighbor iterator from a container, a geometry and another
       *  bidirectional iterator on that container.
       *
       *  \param container The container to iterate.
       *  \param geometry The geometry applied during the iteration.
       *  \param target The target of the neighbor iteration.
       *  \param iter An iterator on \c container.
       */
      template<typename Iterator>
      iterator(const Ct& container, const Geometry& geometry,
               const typename container_traits<Ct>::key_type& target,
               const typename Rebind<Iterator>::type& iter)
        : Base(container.rank(), iter.node, iter.node_dim),
          data(container, geometry, target) { }

      /**
       *  When the information of the dimension for the current node being
       *  pointed to by the iterator is known, this constructor saves some CPU
       *  cycle, by comparison to the other constructor.
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
       *  \param geometry The geometry applied during the iteration.
       *  \param target The target of the neighbor iteration.
       *  \param node_dim The dimension of the node pointed to by iterator.
       *  \param node Use the value of node as the start point for the
       *  iteration.
       */
      iterator(const Ct& container, const Geometry& geometry,
               const typename container_traits<Ct>::key_type& target,
               dimension_type node_dim,
               typename container_traits<Ct>::mode_type::node_ptr node)
        : Base(container.rank(), node, node_dim),
          data(container, geometry, target) { }

      //! Covertion of mutable iterator into a constant iterator is permitted.
      iterator(const typename neighbor<Ct, Geometry>::iterator& iter)
        : Base(iter.rank(), iter.node, iter.node_dim), data(iter.data) { }

      /**
       *  This iterator can be casted silently into a container iterator. You
       *  can therefore use this iterator as an argument to the erase function
       *  of the container, for example.
       *
       *  \warning When using this iterator as an argument to the erase
       *  function of the container, this iterator will get invalidated after
       *  erase.
       */
      operator typename container_traits<Ct>::const_iterator()
      { return container_traits<Ct>::const_iterator(Base::node); }

      //! The related data for the iterator.
      ::spatial::details::Neighbor_data<Ct, Geometry> data;
    };

    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef ::std::pair<iterator, iterator> iterator_pair;
  };

  namespace details
  {
    template <typename Container, typename Geometry>
    typename neighbor<Container, Geometry>::iterator&
    increment(typename neighbor<Container, Geometry>::iterator& iter);

    template <typename Container, typename Geometry>
    typename neighbor<Container, Geometry>::iterator&
    decrement(typename neighbor<Container, Geometry>::iterator& iter);

    template <typename Container, typename Geometry>
    typename neighbor<Container, Geometry>::iterator&
    minimum(typename neighbor<Container, Geometry>::iterator& iter);

    template <typename Container, typename Geometry>
    typename neighbor<Container, Geometry>::iterator&
    maximum(typename neighbor<Container, Geometry>::iterator& iter);

    template <typename Container, typename Geometry>
    typename neighbor<Container, Geometry>::iterator&
    lower_bound(typename neighbor<Container, Geometry>::iterator& iter,
                const typename Geometry::distance_type bound);

    template <typename Container, typename Geometry>
    typename neighbor<Container, Geometry>::iterator&
    upper_bound(typename neighbor<Container, Geometry>::iterator& iter,
                const typename Geometry::distance_type& bound);

  } // namespace details

  /**
   *  A quick accessor for neighbor iterators that retrieve the last valid
   *  calculated distance from the target. The distance read is only relevant
   *  if the iterator does not point past-the-end.
   */
  template <typename Ct, typename Geometry>
  inline const typename Geometry::distance&
  distance(const typename neighbor<Ct, Geometry>::iterator& iter)
  { return iter.data.distance(); }

  /**
   *  A quick accessor for neighbor iterators that retrive the key that is the
   *  target for the nearest neighbor iteration.
   */
  template <typename Ct, typename Geometry>
  inline const typename ::spatial::container_traits<Ct>::key_type&
  target(const typename neighbor<Ct, Geometry>::iterator& iter)
  { return iter.data.target; }

  template <typename Ct, typename Geometry>
  inline typename neighbor<Ct, Geometry>::iterator
  neighbor_end(Ct& container, const Geometry& geometry,
               const typename container_traits<Ct>::key_type& target)
  {
    return neighbor<Ct, Geometry>::iterator
      (container, geometry, target, container.dimension() - 1,
       container.end().node);
  }

  template <typename Ct, typename Geometry>
  inline typename neighbor<const Ct, Geometry>::iterator
  neighbor_end(const Ct& container, const Geometry& geometry,
               const typename container_traits<Ct>::key_type& target)
  {
    return neighbor<const Ct, Geometry>::iterator
      (container, geometry, target, container.dimension() - 1,
       container.end().node);
  }

  template <typename Ct, typename Geometry>
  inline typename neighbor<const Ct, Geometry>::iterator
  neighbor_cend(const Ct& container, const Geometry& geometry,
                const typename container_traits<Ct>::key_type& target)
  { return neighbor_end(container, geometry, target); }

  template <typename Ct, typename Geometry>
  inline typename neighbor<Ct, Geometry>::iterator
  neighbor_begin(Ct& container, const Geometry& geometry,
                 const typename container_traits<Ct>::key_type& target)
  {
    if (container.empty()) return neighbor_end(container, geometry, target);
    typename neighbor<Ct, Geometry>::iterator
      it(container, geometry, target, 0, container.top()); // At root dim = 0
    return ::spatial::details::minimum(it);
  }

  template <typename Ct, typename Geometry>
  inline typename neighbor<const Ct, Geometry>::iterator
  neighbor_begin(const Ct& container, const Geometry& geometry,
                 const typename container_traits<Ct>::key_type& target)
  {
    if (container.empty()) return mapping_end(container, geometry, target);
    typename neighbor<const Ct, Geometry>::iterator
      it(container, geometry, target, 0, container.top()); // At root dim = 0
    return ::spatial::details::minimum(it);
  }

  template <typename Ct, typename Geometry>
  inline typename neighbor<const Ct, Geometry>::iterator
  neighbor_cbegin(const Ct& container, const Geometry& geometry,
                  const typename container_traits<Ct>::key_type& target)
  { return neighbor_begin(container, geometry, target); }

  template <typename Ct, typename Geometry>
  inline typename neighbor<Ct, Geometry>::iterator
  neighbor_lower_bound(Ct& container, const Geometry& geometry,
                       const typename container_traits<Ct>::key_type& target,
                       const typename Geometry::distance_type& bound)
  {
    if (container.empty()) return mapping_end(container, geometry, target);
    ::spatial::except::check_positive_distance(bound);
    typename neighbor<Ct, Geometry>::iterator
      it(container, geometry, target, 0, container.top()); // At root dim = 0
    return ::spatial::details::lower_bound(it, bound);
  }

  template <typename Ct, typename Geometry>
  inline typename neighbor<const Ct, Geometry>::iterator
  neighbor_lower_bound(const Ct& container, const Geometry& geometry,
                       const typename container_traits<Ct>::key_type& target,
                       const typename Geometry::distance_type& bound)
  {
    if (container.empty()) return mapping_end(container, geometry, target);
    ::spatial::except::check_positive_distance(bound);
    typename neighbor<const Ct, Geometry>::iterator
      it(container, geometry, target, 0, container.top()); // At root dim = 0
    return ::spatial::details::lower_bound(it, bound);
  }

  template <typename Ct, typename Geometry>
  inline typename neighbor<const Ct, Geometry>::iterator
  neighbor_clower_bound(const Ct& container, const Geometry& geometry,
                        const typename container_traits<Ct>::key_type& target,
                        const typename Geometry::distance_type& bound)
  { return neighbor_lower_bound(container, geometry, target, bound); }

  template <typename Ct, typename Geometry>
  inline typename neighbor<Ct, Geometry>::iterator
  neighbor_upper_bound(Ct& container, const Geometry& geometry,
                       const typename container_traits<Ct>::key_type& target,
                       const typename Geometry::distance_type& bound)
  {
    if (container.empty()) return mapping_end(container, geometry, target);
    ::spatial::except::check_positive_distance(bound);
    typename neighbor<Ct, Geometry>::iterator
      it(container, geometry, target, 0, container.top()); // At root dim = 0
    return ::spatial::details::upper_bound(it, bound);
  }

  template <typename Ct, typename Geometry>
  inline typename neighbor<const Ct, Geometry>::iterator
  neighbor_upper_bound(const Ct& container, const Geometry& geometry,
                       const typename container_traits<Ct>::key_type& target,
                       const typename Geometry::distance_type& bound)
  {
    if (container.empty()) return mapping_end(container, geometry, target);
    ::spatial::except::check_positive_distance(bound);
    typename neighbor<const Ct, Geometry>::iterator
      it(container, geometry, target, 0, container.top()); // At root dim = 0
    return ::spatial::details::upper_bound(it, bound);
  }

  template <typename Ct, typename Geometry>
  inline typename neighbor<const Ct, Geometry>::iterator
  neighbor_cupper_bound(const Ct& container, const Geometry& geometry,
                        const typename container_traits<Ct>::key_type& target,
                        const typename Geometry::distance_type& bound)
  { return neighbor_upper_bound(container, geometry, target); }



} // namespace spatial

#include "spatial_neighbor.tpp"

#endif // SPATIAL_NEIGHBOR_HPP
