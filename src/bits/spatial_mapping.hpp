// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_mapping.hpp
 *  @brief  Contains the definition of the mapping iterators. These iterators
 *  walk through all nodes in the tree in order from the lowest to the highest
 *  value along a particular dimension. The @key_comp comparator of the
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
  // Forward declaration...
  template <typename Container>
  struct mapping;

  /**
   *  Returns the first value in \c container for mapping over the dimension \c dim.
   *
   *
   *  \see mapping
   */
  template <typename Container>
  typename mapping<Container>::iterator
  begin_mapping(dimension_type dim, Container& container);

  //@{
  /**
   *
   */
  template <typename Container>
  inline typename mapping<Container>::const_iterator
  begin_mapping(dimension_type dim, const Container& container)
  { return begin_mapping(dim, *const_cast<Container*>(&container)); }

  template <typename Container>
  inline typename mapping<Container>::const_iterator
  const_begin_mapping(dimension_type dim, const Container& container)
  { return begin_mapping(dim, *const_cast<Container*>(&container)); }
  //@}

  /**
   *
   */
  template <typename Container>
  typename mapping<Container>::iterator
  end_mapping(dimension_type dim, Container& container);

  //@{
  /**
   *
   */
  template <typename Container>
  inline typename mapping<Container>::const_iterator
  end_mapping(dimension_type dim, const Container& container)
  { return end_mapping(dim, *const_cast<Container*>(&container)); }

  template <typename Container>
  inline typename mapping<Container>::const_iterator
  const_end_mapping(dimension_type dim, const Container& container);
  { return end_mapping(dim, *const_cast<Container*>(&container)); }
  //@}

  /**
   *
   */
  template <typename Container>
  inline typename mapping<Container>::iterator_pair
  make_mapping(dimension_type dim, Container& container)
  {
    return ::std::make_pair(begin_mapping(dim, container),
                            end_mapping(dim, container));
  }

  //@{
  /**
   *
   */
  template <typename Container>
  inline typename mapping<Container>::const_iterator_pair
  make_mapping(dimension_type dim, const Container& container)
  {
    return ::std::make_pair(begin_mapping(dim, container),
                            end_mapping(dim, container));
  }

  template <typename Container>
  inline typename mapping<Container>::const_iterator_pair
  const_make_mapping(dimension_type dim, const Container& container)
  {
    return ::std::make_pair(begin_mapping(dim, container),
                            end_mapping(dim, container));
  }
  //@}

  /**
   *
   */
  template <typename Container>
  typename mapping<Container>::iterator
  lower_mapping(dimension_type dim,
                const typename Container::key_type& bound,
                Container& container);

  //@{
  /**
   *
   */
  template <typename Container>
  inline typename mapping<Container>::const_iterator
  lower_mapping(dimension_type dim,
                const typename Container::key_type& bound,
                const Container& container);
  { return lower_mapping(dim, bound, *const_cast<Container*>(&container)); }

  template <typename Container>
  inline typename mapping<Container>::const_iterator
  clower_mapping(dimension_type dim,
                 const typename Container::key_type& bound,
                 const Container& container);
  { return lower_mapping(dim, bound, *const_cast<Container*>(&container)); }
  //@}

  /**
   *
   */
  template <typename Container>
  typename mapping<Container>::iterator
  upper_mapping(dimension_type dim,
                const typename Container::key_type& bound,
                Container& container);

  //@{
  /**
   *
   */
  template <typename Container>
  inline typename mapping<Container>::const_iterator
  upper_mapping(dimension_type dim,
                const typename Container::key_type& bound,
                const Container& containe);

  template <typename Container>
  inline typename mapping<Container>::const_iterator
  cupper_mapping(dimension_type dim,
                 const typename Container::key_type& bound,
                 const Container& container);
  //@}

  namespace details
  {
    /**
     *
     */
    template <typename Container>
    typename mapping<Container>::iterator&
    increment(typename mapping<Container>::iterator&);

    /**
     *
     */
    template <typename Container>
    typename mapping<Container>::iterator&
    decrement(typename mapping<Container>::iterator&);

    /**
     *
     */
    template <typename Container>
    struct Mapping_data
      : typename Container::rank_type
    {
      //! Build an uninitialized mapping data object.
      Mapping_data()
        : mapping_dim_(), node_dim_() { }

      //! Build required mapping data from the given container.
      Mapping_data
      (dimension_type mapping_dim, dimension_type node_dim,
       const Container& container)
        : Container::rank_type(container.rank()),
          mapping_dim_(Container::key_compare(container.key_comp()),
                       mapping_dim),
          node_dim_(node_dim)
      { }

      /**
       *  The current dimension of iteration.
       *
       *  You can modify this key if you suddenly want the iterator to change
       *  dimension of iteration. However this field must always satisfy:
       *
       *     mapping_dim() < Rank()
       *
       *  Rank being the template rank provider for the iterator.
       */
      Compress<typename Container::key_comp, dimension_type> mapping_dim;

      /**
       *  The dimension for node.
       *
       *  Modifing this attribute can potientially invalidate the iterator. Do
       *  not modify this attribute unless you know what you're doing.
       */
      dimension_type node_dim;
    };
  } // namespace details

  /**
   *
   */
  template <typename Container>
  struct mapping
  {
    /**
     *
     */
    struct iterator : ::spatial::details::bidirectional_iterator
    <typename Container::mode_type, iterator, false>
    {
      using ::spatial::details::Mapping_data;
      using ::spatial::details::bidirectional_iterator
      <typename Container::mode_type, iterator, false>::node;

      //! Uninitialized iterator.
      iterator() { }

      //! Build a mapping iterator from a Container and a node.
      //!
      //! Striclty for use by the containers.
      iterator(dimension_type dim, dimension_type node_dim,
               typename Container::mode_type::link_ptr link,
               Container& container)
        : node(Container::mode_type::node(link)),
          data(dim, node_dim, container) { }

      //! Build a mapping iterator from a container iterator
      iterator(dimension_type dim,
               typename Container::const_iterator iterator,
               Container& container)
        : node(Container::mode_type::node(link)),
          data(dim, modulo(iterator.node, container.rank()), container) { }

      //@{
      /**
       *  This iterator can be casted silently into a container iterator. You can
       *  therefore use this iterator as an argument to the erase function of
       *  the container, for example.
       *
       *  \warning When using this iterator as an argument to the erase function
       *  of the container, this iterator will get invalidated after erase.
       */
      operator Container::iterator()
      { return Container::iterator(link(node)); }

      operator Container::const_iterator()
      { return Container::const_iterator(link(node)); }
      //@}

      //! The related data for the iterator.
      Mapping_data<Container> data;
    };

    /**
     *  @brief  A heavy iterator implementation meant to optimize transversal of
     *  all nodes in the tree when projected on a single dimension.
     *
     *  With this iterator, a n-dimensional K-d tree behaves like n different sets
     *  with similar data. Iteration through the tree is very efficient when the
     *  dimension K-d tree is very small by comparison to the number of objects,
     *  but pretty inefficient otherwise, by comparison to a set.
     */
    struct const_iterator : ::spatial::details::bidirectional_iterator
    <Container::mode_type, const_iterator, true>
    {
      using ::spatial::details::Mapping_data;
      using ::spatial::details::bidirectional_iterator
      <Container::mode_type, iterator, false>::node;

      //! Uninitialized constant iterator.
      const_iterator() { }

      //! Build a constant mapping iterator from a Container and a node, for use
      //! by the containers.
      const_iterator(dimension_type dim, dimension_type node_dim,
                     typename Container::mode_type::const_link_ptr link,
                     const Container& container)
        : node(Container::mode_type::node(link)),
          data(dim, node_dim, container) { }

      //! Build a constant mapping iterator from a container iterator
      const_iterator(dimension_type dim,
                     typename Container::const_iterator iterator,
                     const Container& container)
        : node(Container::mode_type::node(link)),
          data(dim, modulo(iterator.node, container.rank()), container) { }

      //! Allows to build a const_iterator from an iterator.
      const_iterator(const iterator& iter)
        : data(iter.data), node(iter.node) { }

      //@{
      /**
       *  This iterator can be casted silently into a container iterator. You can
       *  therefore use this iterator as an argument to the erase function of
       *  the container, for example.
       *
       *  \warning When using this iterator as an argument to the erase function
       *  of the container, this iterator will get invalidated after erase.
       */
      operator Container::const_iterator()
      { return Container::const_iterator(link(node)); }

      //! The related data for the iterator.
      Mapping_data<Container, true> data;
    };

    /**
     *  A pair or iterator that represents a stride.
     */
    typedef ::std::pair<iterator, iterator>             iterator_pair;

    /**
     *  A pair or constant iterator used to represent a range, that is used by
     *  several range function.
     */
    typedef ::std::pair<const_iterator, const_iterator> const_iterator_pair;
  };

  namespace details
  {
      template <typename Container>
      inline typename iterator<Container>::type
      end(Container& container, dimension_type mapping_dim)
      {
        except::check_dimension_argument(container.dimension(), mapping_dim);
        return typename iterator<Container>::type
          (container.rank(), container.key_comp(),
           mapping_dim, container.dimension() - 1,
           get_end(container));
      }

      template <typename Container>
      inline typename const_iterator<Container>::type
      const_end
      (const Container& container, dimension_type mapping_dim)
      {
        return end(*const_cast<Container*>(&container), mapping_dim);
      }

      template <typename Container>
      inline typename iterator<Container>::type
      begin(Container& container, dimension_type mapping_dim)
      {
        // Guarentees begin(n) == end(n) if tree is empty
        if (container.empty())
          { return end(container, mapping_dim); }
        else
          {
            except::check_dimension_argument
              (container.dimension(), mapping_dim);
            return iterator<Container>::type::minimum
              (container.rank(), container.key_comp(),
               mapping_dim, 0, container.end().node->parent);
          }
      }

      template <typename Container>
      inline typename const_iterator<Container>::type
      const_begin
      (const Container& container, dimension_type mapping_dim)
      {
        return begin(*const_cast<Container*>(&container), mapping_dim);
      }

      template <typename Container>
      inline typename iterator<Container>::type
      lower_bound
      (Container& container, dimension_type mapping_dim,
       const typename container_traits<Container>::key_type& key)
      {
        if (container.empty())
          { return end(container, mapping_dim); }
        else
          {
            except::check_dimension_argument
              (container.dimension(), mapping_dim);
            return iterator<Container>::type::lower_bound
              (container.rank(), container.key_comp(),
               mapping_dim, 0, container.end().node->parent, key);
          }
      }

      template <typename Container>
      inline typename const_iterator<Container>::type
      const_lower_bound
      (const Container& container, dimension_type mapping_dim,
       const typename container_traits<Container>::key_type& key)
      {
        return lower_bound
          (*const_cast<Container*>(&container), mapping_dim, key);
      }

      template <typename Container>
      inline typename iterator<Container>::type
      upper_bound
      (Container& container, dimension_type mapping_dim,
       const typename container_traits<Container>::key_type& key)
      {
        if (container.empty())
          { return end(container, mapping_dim); }
        else
          {
            except::check_dimension_argument
              (container.dimension(), mapping_dim);
            return iterator<Container>::type::upper_bound
              (container.rank(), container.key_comp(),
               mapping_dim, 0, container.end().node->parent, key);
          }
      }

      template <typename Container>
      inline typename const_iterator<Container>::type
      const_upper_bound
      (const Container& container, dimension_type mapping_dim,
       const typename container_traits<Container>::key_type& key)
      {
        return upper_bound
          (*const_cast<Container*>(&container), mapping_dim, key);
      }

    } // namespace mapping
  } // namespace details

} // namespace spatial

#include "spatial_mapping.tpp"

#endif // SPATIAL_MAPPING_HPP
