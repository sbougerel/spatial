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
  /**
   *  Restrains the template Unit to a type that is a primitive floating point
   *  type.
   */
  //@{
  template<typename Unit, typename Enable = void>
  struct floating_point_type { };
  template<typename Unit> struct floating_point_type
  <Unit, typename enable_if<std::tr1::is_floating_point<Unit> >::type> { };
  //@}

  /**
   *  Restrains the template Unit to a type that is a primitive arithmetic type.
   */
  //@{
  template<typename Unit, typename Enable = void> struct arithmetic_type { };
  template<typename Unit> struct arithmetic_type
  <Unit, typename enable_if<std::tr1::is_arithmetic<Unit> >::type> { };
  //@}

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
    struct Neighbor_data : container_traits<Ct>::key_compare
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
        : container_traits<Ct>::key_compare(c.key_comp()), target(g, k)
      { }

      /**
       *  The target of the iteration; element of the container are iterate
       *  from the closest to the element furthest away from the target.
       */
      Compress<Geometry, typename container_traits<Ct>::key_type> target;

      /**
       *  The last valid computed value of the distance. The value stored is
       *  only valid if the iterator is not past-the-end.
       */
      typename Geometry::distance_type distance;
    };

  } // namespace details

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
  template<typename Ct, typename Geometry>
  struct neighbor_iterator
    : details::Bidirectional_iterator
  <typename container_traits<Ct>::mode_type,
   typename container_traits<Ct>::rank_type>
  {
  private:
    typedef typename details::Bidirectional_iterator
    <typename container_traits<Ct>::mode_type,
     typename container_traits<Ct>::rank_type> Base;

  public:
    //! Key comparator type transferred from the container
    typedef typename container_traits<Ct>::key_compare key_compare;

    //! The geometry type used by the iterator
    typedef Geometry geometry_type;

    //! The distance type that is read from geometry_type
    typedef typename Geometry::distance_type distance_type;

    //! The key type that is used as a target for the nearest neighbor search
    typedef typename container_traits<Ct>::key_type key_type;

    //! Uninitialized iterator.
    neighbor_iterator() { }

    /**
     *  The standard way to build this iterator: specify a geometry to apply, an
     *  iterator on a container, and that container.
     *
     *  \param container The container to iterate.
     *  \param geometry The geometry applied during the iteration.
     *  \param target The target of the neighbor iteration.
     *  \param it An iterator on container.
     */
    neighbor_iterator(Ct& container, const Geometry& geo,
                      const typename container_traits<Ct>::key_type& target,
                      const typename container_traits<Ct>::iterator& iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        data_(container, geo, target) { }

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
    neighbor_iterator(Ct& container, const Geometry& geo,
                      const typename container_traits<Ct>::key_type& target,
                      dimension_type node_dim,
                      typename container_traits<Ct>::mode_type::node_ptr node)
      : Base(container.rank(), node, node_dim),
        data_(container, geo, target) { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    neighbor_iterator<Ct, Geometry>& operator++()
    { return increment_neighbor(*this); }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    neighbor_iterator<Ct, Geometry> operator++(int)
    {
      neighbor_iterator<Ct, Geometry> x(*this);
      increment_neighbor(*this);
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    neighbor_iterator<Ct, Geometry>& operator--()
    { return decrement_neighbor(*this); }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    neighbor_iterator<Ct, Geometry> operator--(int)
    {
      neighbor_iterator<Ct, Geometry> x(*this);
      decrement_neighbor(*this);
      return x;
    }

    //! Return the key_comparator used by the iterator
    const key_compare&
    key_comp() const { return static_cast<const key_compare&>(data_); }

    //! Return the geometry used by the iterator
    const geometry_type&
    geometry() const { return data_.target.base(); }

    //! Accessor to the last valid distance of the iterator
    const distance_type&
    distance() const { return data_.distance; }

    //! Read-only accessor to the target of the iterator
    const key_type&
    target_key() const { return data_.target(); }

    //! Read/write accessor to the target of the iterator
    key_type&
    target_key() { return data_.target(); }

  private:
    //! The related data for the iterator.
    details::Neighbor_data<Ct, Geometry> data_;
  };

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
  template<typename Ct, typename Geometry>
  struct neighbor_iterator<const Ct, Geometry>
    : details::Const_bidirectional_iterator
      <typename container_traits<Ct>::mode_type,
       typename container_traits<Ct>::rank_type>
  {
    private:
      typedef typename details::Const_bidirectional_iterator
        <typename container_traits<Ct>::mode_type,
         typename container_traits<Ct>::rank_type> Base;

  public:
    //! Key comparator type transferred from the container
    typedef typename container_traits<Ct>::key_compare key_compare;

    //! The geometry type used by the iterator
    typedef Geometry geometry_type;

    //! The distance type that is read from geometry_type
    typedef typename Geometry::distance_type distance_type;

    //! The key type that is used as a target for the nearest neighbor search
    typedef typename container_traits<Ct>::key_type key_type;

    //! Uninitialized iterator.
    neighbor_iterator() { }

    /**
     *  The standard way to build this iterator: specify a geometry to apply,
     *  an iterator on a container, and that container.
     *
     *  \param container The container to iterate.
     *  \param geo The geometry applied during the iteration.
     *  \param target The target of the neighbor iteration.
     *  \param iter An iterator on \c container.
     */
    neighbor_iterator(const Ct& container, const Geometry& geo,
                      const typename container_traits<Ct>::key_type& target,
                      typename container_traits<Ct>::iterator iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        data_(container, geo, target) { }

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
     *  \param geo The geometry applied during the iteration.
     *  \param target The target of the neighbor iteration.
     *  \param node_dim The dimension of the node pointed to by iterator.
     *  \param node Use the value of node as the start point for the
     *  iteration.
     */
    neighbor_iterator(const Ct& container, const Geometry& geo,
                      const typename container_traits<Ct>::key_type& target,
                      dimension_type node_dim,
                      typename container_traits<Ct>::mode_type::node_ptr node)
      : Base(container.rank(), node, node_dim),
        data_(container, geo, target) { }

    //! Covertion of mutable iterator into a constant iterator is permitted.
    neighbor_iterator(const neighbor_iterator<Ct, Geometry>& iter)
      : Base(iter.rank(), iter.node, iter.node_dim),
        data_(iter.key_comp(), iter.geometry(), iter.target_key()) { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    neighbor_iterator<Ct, Geometry>& operator++()
    { return increment_neighbor(*this); }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    neighbor_iterator<Ct, Geometry> operator++(int)
    {
      neighbor_iterator<Ct, Geometry> x(*this);
      increment_neighbor(*this);
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    neighbor_iterator<Ct, Geometry>& operator--()
    { return decrement_neighbor(*this); }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    neighbor_iterator<Ct, Geometry> operator--(int)
    {
      neighbor_iterator<Ct, Geometry> x(*this);
      decrement_neighbor(*this);
      return x;
    }

    //! Return the key_comparator used by the iterator
    const key_compare&
    key_comp() const { return static_cast<const key_compare&>(data_); }

    //! Return the geometry used by the iterator
    const geometry_type&
    geometry() const { return data_.target.base(); }

    //! Accessor to the last valid distance of the iterator
    const distance_type&
    distance() const { return data_.distance; }

    //! Read-only accessor to the target of the iterator
    const key_type&
    target_key() const { return data_.target(); }

    //! Read/write accessor to the target of the iterator
    key_type&
    target_key() { return data_.target(); }

  private:
    //! The related data for the iterator.
    details::Neighbor_data<Ct, Geometry> data_;
  };

  //! Return true if 2 iterators are equal
  template<typename Ct, typename Geom>
  bool operator==(const neighbor_iterator<Ct, Geom>& a,
                  const neighbor_iterator<Ct, Geom>& b)
  { return a.node == b.node; }

  //! Return true if 2 iterators are equal
  template<typename Ct, typename Geom>
  bool operator==(const neighbor_iterator<const Ct, Geom>& a,
                  const neighbor_iterator<const Ct, Geom>& b)
  { return a.node == b.node; }

  //! Return true if 2 iterators are equal
  template<typename Ct, typename Geom>
  bool operator==(const neighbor_iterator<Ct, Geom>& a,
                  const neighbor_iterator<const Ct, Geom>& b)
  { return a.node == b.node; }

  //! Return true if 2 iterators are equal
  template<typename Ct, typename Geom>
  bool operator==(const neighbor_iterator<const Ct, Geom>& a,
                  const neighbor_iterator<Ct, Geom>& b)
  { return a.node == b.node; }

  //! Return true if 2 iterator are different
  template<typename Ct, typename Geom>
  bool operator!=(const neighbor_iterator<Ct, Geom>& a,
                  const neighbor_iterator<Ct, Geom>& b)
  { return !(a == b); }

  //! Return true if 2 iterator are different
  template<typename Ct, typename Geom>
  bool operator!=(const neighbor_iterator<const Ct, Geom>& a,
                  const neighbor_iterator<const Ct, Geom>& b)
  { return !(a == b); }

  //! Return true if 2 iterator are different
  template<typename Ct, typename Geom>
  bool operator!=(const neighbor_iterator<Ct, Geom>& a,
                  const neighbor_iterator<const Ct, Geom>& b)
  { return !(a == b); }

  //! Return true if 2 iterator are different
  template<typename Ct, typename Geom>
  bool operator!=(const neighbor_iterator<const Ct, Geom>& a,
                  const neighbor_iterator<Ct, Geom>& b)
  { return !(a == b); }

  namespace details
  {
    template <typename Container, typename Geometry>
    neighbor_iterator<Container, Geometry>&
    increment_neighbor(neighbor_iterator<Container, Geometry>& iter);

    template <typename Container, typename Geometry>
    neighbor_iterator<Container, Geometry>&
    decrement_neighbor(neighbor_iterator<Container, Geometry>& iter);

    template <typename Container, typename Geometry>
    neighbor_iterator<Container, Geometry>&
    minimum_neighbor(neighbor_iterator<Container, Geometry>& iter);

    template <typename Container, typename Geometry>
    neighbor_iterator<Container, Geometry>&
    maximum_neighbor(neighbor_iterator<Container, Geometry>& iter);

    template <typename Container, typename Geometry>
    neighbor_iterator<Container, Geometry>&
    lower_bound_neighbor(neighbor_iterator<Container, Geometry>& iter,
                         const typename Geometry::distance_type bound);

    template <typename Container, typename Geometry>
    neighbor_iterator<Container, Geometry>&
    upper_bound_neighbor(neighbor_iterator<Container, Geometry>& iter,
                         const typename Geometry::distance_type& bound);

  } // namespace details

  /**
   *  A quick accessor for neighbor iterators that retrieve the last valid
   *  calculated distance from the target. The distance read is only relevant if
   *  the iterator does not point past-the-end.
   */
  template <typename Ct, typename Geometry>
  inline const typename Geometry::distance&
  distance(const neighbor_iterator<Ct, Geometry>& iter)
  { return iter.distance(); }

  /**
   *  A quick accessor for neighbor iterators that retrive the key that is the
   *  target for the nearest neighbor iteration.
   */
  template <typename Ct, typename Geometry>
  inline const typename container_traits<Ct>::key_type&
  target_key(const neighbor_iterator<Ct, Geometry>& iter)
  { return iter.target_key(); }

  template <typename Ct, typename Geometry>
  inline neighbor_iterator<Ct, Geometry>
  neighbor_end(Ct& container, const Geometry& geometry,
               const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_iterator<Ct, Geometry>
      (container, geometry, target, container.dimension() - 1,
       container.end().node);
  }

  // template <typename Ct, typename Unit, typename Diff>
  // inline euclidian_iterator<Ct, Unit, Diff>
  // euclid_neighbor_end(Ct& container, floating_point<Unit>, const Diff& diff,
  //                     const typename container_traits<Ct>::key_type& target)
  // {
  //   return neighbor_end
  //     (container, euclid_geometry<Ct, Unit, Diff>(diff), target);
  // }

  // template <typename Ct, typename Unit>
  // inline typename enable_if<details::is_compare_builtin<Ct>,
  //                           euclid_neighbor_iterator<Ct, Unit> >::type
  // euclid_neighbor_end(Ct& container, floating_point_type<Unit>,
  //                     const typename container_traits<Ct>::key_type& target)
  // {
  //   return neighbor_end
  //     (container, euclid_geometry
  //      <Ct, Unit, typename details::auto_difference
  //       <typename container_traits<Ct>::key_compare, Unit>::type>
  //      (auto_difference_cast<typename container_traits<Ct>::key_compare,
  //                            Unit>(container.key_comp())), target);
  // }

  template <typename Ct, typename Geometry>
  inline neighbor_iterator<const Ct, Geometry>
  neighbor_end(const Ct& container, const Geometry& geometry,
               const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_iterator<const Ct, Geometry>
      (container, geometry, target, container.dimension() - 1,
       container.end().node);
  }

  template <typename Ct, typename Geometry>
  inline neighbor_iterator<const Ct, Geometry>
  neighbor_cend(const Ct& container, const Geometry& geometry,
                const typename container_traits<Ct>::key_type& target)
  { return neighbor_end(container, geometry, target); }

  template <typename Ct, typename Geometry>
  inline neighbor_iterator<Ct, Geometry>
  neighbor_begin(Ct& container, const Geometry& geometry,
                 const typename container_traits<Ct>::key_type& target)
  {
    if (container.empty()) return neighbor_end(container, geometry, target);
    neighbor_iterator<Ct, Geometry>
      it(container, geometry, target, 0, container.top()); // At root dim = 0
    return details::minimum_neighbor(it);
  }

  template <typename Ct, typename Geometry>
  inline neighbor_iterator<const Ct, Geometry>
  neighbor_begin(const Ct& container, const Geometry& geometry,
                 const typename container_traits<Ct>::key_type& target)
  {
    if (container.empty()) return mapping_end(container, geometry, target);
    neighbor_iterator<const Ct, Geometry>
      it(container, geometry, target, 0, container.top()); // At root dim = 0
    return details::minimum_neighbor(it);
  }

  template <typename Ct, typename Geometry>
  inline neighbor_iterator<const Ct, Geometry>
  neighbor_cbegin(const Ct& container, const Geometry& geometry,
                  const typename container_traits<Ct>::key_type& target)
  { return neighbor_begin(container, geometry, target); }

  template <typename Ct, typename Geometry>
  inline neighbor_iterator<Ct, Geometry>
  neighbor_lower_bound(Ct& container, const Geometry& geometry,
                       const typename container_traits<Ct>::key_type& target,
                       const typename Geometry::distance_type& bound)
  {
    if (container.empty()) return mapping_end(container, geometry, target);
    except::check_positive_distance(bound);
    neighbor_iterator<Ct, Geometry>
      it(container, geometry, target, 0, container.top()); // At root dim = 0
    return details::lower_bound_neighbor(it, bound);
  }

  template <typename Ct, typename Geometry>
  inline neighbor_iterator<const Ct, Geometry>
  neighbor_lower_bound(const Ct& container, const Geometry& geometry,
                       const typename container_traits<Ct>::key_type& target,
                       const typename Geometry::distance_type& bound)
  {
    if (container.empty()) return mapping_end(container, geometry, target);
    except::check_positive_distance(bound);
    neighbor_iterator<const Ct, Geometry>
      it(container, geometry, target, 0, container.top()); // At root dim = 0
    return details::lower_bound_neighbor(it, bound);
  }

  template <typename Ct, typename Geometry>
  inline neighbor_iterator<const Ct, Geometry>
  neighbor_clower_bound(const Ct& container, const Geometry& geometry,
                        const typename container_traits<Ct>::key_type& target,
                        const typename Geometry::distance_type& bound)
  { return neighbor_lower_bound(container, geometry, target, bound); }

  template <typename Ct, typename Geometry>
  inline neighbor_iterator<Ct, Geometry>
  neighbor_upper_bound(Ct& container, const Geometry& geometry,
                       const typename container_traits<Ct>::key_type& target,
                       const typename Geometry::distance_type& bound)
  {
    if (container.empty()) return mapping_end(container, geometry, target);
    except::check_positive_distance(bound);
    neighbor_iterator<Ct, Geometry>
      it(container, geometry, target, 0, container.top()); // At root dim = 0
    return details::upper_bound_neighbor(it, bound);
  }

  template <typename Ct, typename Geometry>
  inline neighbor_iterator<const Ct, Geometry>
  neighbor_upper_bound(const Ct& container, const Geometry& geometry,
                       const typename container_traits<Ct>::key_type& target,
                       const typename Geometry::distance_type& bound)
  {
    if (container.empty()) return mapping_end(container, geometry, target);
    except::check_positive_distance(bound);
    neighbor_iterator<const Ct, Geometry>
      it(container, geometry, target, 0, container.top()); // At root dim = 0
    return details::upper_bound_neighbor(it, bound);
  }

  template <typename Ct, typename Geometry>
  inline neighbor_iterator<const Ct, Geometry>
  neighbor_cupper_bound(const Ct& container, const Geometry& geometry,
                        const typename container_traits<Ct>::key_type& target,
                        const typename Geometry::distance_type& bound)
  { return neighbor_upper_bound(container, geometry, target); }

} // namespace spatial

#include "spatial_neighbor.tpp"

#endif // SPATIAL_NEIGHBOR_HPP
