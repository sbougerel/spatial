// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_exceptions.hpp
 *  \brief  Defines exception that are being thrown by the library
 */

#ifndef SPATIAL_EXCEPTIONS_HPP
#define SPATIAL_EXCEPTIONS_HPP

#include <stdexcept>
#include "spatial.hpp"
#include "spatial_traits.hpp"

namespace spatial
{
  /**
   *  \brief  Thrown to report that an invalid rank was passed as an argument.
   *  Generally thrown because 0 was passed as an argument for the rank.
   */
  struct invalid_rank : std::logic_error
  {
    explicit invalid_rank(const std::string& arg)
      : std::logic_error(arg) { }
  };

  /**
   *  \brief  Thrown to report that an invalid dimension was passed as an
   *  argument.
   */
  struct invalid_dimension : std::logic_error
  {
    explicit invalid_dimension(const std::string& arg)
      : std::logic_error(arg) { }
  };

  /**
   *  \brief  Thrown to report that an invalid node was passed as an argument.
   */
  struct invalid_node : std::logic_error
  {
    explicit invalid_node(const std::string& arg)
      : std::logic_error(arg) { }
  };

  /**
   *  \brief  Thrown to report that an invalid iterator was passed as an argument.
   */
  struct invalid_iterator : std::logic_error
  {
    explicit invalid_iterator(const std::string& arg)
      : std::logic_error(arg) { }
  };

  /**
   *  \brief  Thrown to report that an empty container was passed as an argument,
   *  while the function does not accept an empty container.
   */
  struct invalid_empty_container : std::logic_error
  {
    explicit invalid_empty_container(const std::string& arg)
      : std::logic_error(arg) { }
  };

  /**
   *  \brief  Thrown to report that an invalid range bound has been given as
   *  argument.
   *  \see check_invalid_bounds()
   *  \see bounds
   *
   *  Generally, this means that the lower bound has a value that overlaps with
   *  the upper bound over one dimension, at least.
   */
  struct invalid_bounds : std::logic_error
  {
    explicit invalid_bounds(const std::string& arg)
      : std::logic_error(arg) { }
  };

  /**
   *  Thrown to report that a box has incorrect coordinates with regards to
   *  its layout.
   *  \see check_invalid_box()
   */
  struct invalid_box : std::logic_error
  {
    explicit invalid_box(const std::string& arg)
      : std::logic_error(arg) { }
  };

  namespace except
  {
    /**
     *  Checks that \c rank is not null.
     *  \throws invalid_rank is thrown if checks fails.
     */
    inline void check_rank(dimension_type rank)
    { if (0 == rank) throw invalid_rank("rank is null"); }

    /**
     *  Checks that \c dimension is not greater or equal to \c rank.
     *  \throws invalid_dimension is thrown if checks fails.
     */
    inline void check_dimension
    (dimension_type rank, dimension_type dimension)
    {
      if (dimension >= rank)
        throw invalid_dimension("dimension is out of range");
    }

    /**
     *  Checks that the node's pointer given as an argument to a function is not
     *  null or does not point to a header node.
     *  \exception invalid_node is thrown if checks fails.
     *  \param node the node pointer to check.
     */
    template<typename Node>
    inline void check_node(Node* node)
    {
      if (node == 0 || node->left == node)
        throw invalid_node
          ("node points to null or header node");
    }

    /**
     *  Checks that the node pointed to by an iterator and given as an argument
     *  to a function is not null or does not point to a header node.
     *  \exception invalid_iterator is thrown if checks fails.
     */
    template<typename Node>
    inline void check_node_iterator(Node* node)
    {
      if (node == 0 || node->left == node)
        throw invalid_iterator
          ("iterator points to null or header node");
    }

    /**
     *  Checks if two iterators are of equal values, if not raises the \ref
     *  invalid_iterator exception.
     *  \exception invalid_iterator is raised when the nodes specified are not
     *  matching.
     *
     *  For this test to be useful, one of the iterator must be sure to belong
     *  to a container.
     */
    template <typename Ptr1, typename Ptr2>
    inline void check_iterator(Ptr1 ptr1, Ptr2 ptr2)
    {
      if (ptr1 != ptr2)
        throw invalid_iterator
          ("iterator is invalid or does not belong to the container used");
    }

    /**
     *  Checks that the container given as an argument to a function is not
     *  empty.
     *  \exception invalid_empty_container is thrown if checks fails.
     *  \param cont the container that must not be empty.
     */
    template<typename Tp>
    inline void check_empty_container(const Tp& cont)
    {
      if (cont.empty())
        throw invalid_empty_container("container is empty");
    }

    /**
     *  Checks if all coordinates of \c lower are strictly less than these of
     *  \c higher along the same dimensions. The number of dimensions is limited
     *  by the rank of \c container.
     *  \exception invalid_bounds is thrown if the check fails.
     *  \param container providing type information, comparison and rank.
     *  \param lower the lower bound of the interval considered.
     *  \param upper the upper bound of the interval considered.
     *
     *  This check is performed mainly upon creation of a \ref open_bounds
     *  predicate.
     */
    template<typename Tp>
    inline void check_open_bounds
    (const Tp& container,
     const typename container_traits<Tp>::key_type& lower,
     const typename container_traits<Tp>::key_type& upper)
    {
      for (dimension_type dim = 0; dim < container.dimension(); ++dim)
        if (!container.key_comp()(dim, lower, upper))
          throw invalid_bounds
            ("lower is greater or equal to upper over one dimension at least");
    }

    /**
     *  Checks if all coordinates of \c lower are strictly less than these of
     *  \c higher along the same dimensions. The number of dimensions is limited
     *  by the rank of \c container.
     *  \exception invalid_bounds is thrown if the check fails.
     *
     *  This check is performed mainly upon creation of a \ref bounds
     *  predicate.
     */
    template<typename Tp>
    inline void check_bounds
    (const Tp& container,
     const typename container_traits<Tp>::key_type& lower,
     const typename container_traits<Tp>::key_type& upper)
    {
        return check_open_bounds(container, lower, upper);
    }

    /**
     *  Checks if all coordinates of \c lower are less or equal to these of
     *  \c higher along the same dimensions. The number of dimensions is limited
     *  by the rank of \c container.
     *  \throw invalid_closed_bounds is thrown if the check fails.
     *
     *  This check is performed mainly upon creation of a \ref
     *  closed_bounds predicate.
     */
    template<typename Tp>
    inline void check_closed_bounds
    (const Tp& container,
     const typename container_traits<Tp>::key_type& lower,
     const typename container_traits<Tp>::key_type& upper)
    {
      for (dimension_type dim = 0; dim < container.dimension(); ++dim)
        if (container.key_comp()(dim, upper, lower))
          throw invalid_bounds
            ("upper is stricly less than lower over one dimension at least");
    }

    //@{
    /**
     *  Checks that all coordinates of a box are matching with the layout
     *  specified or else, raise an \ref invalid_box exception.
     *  \exception invalid_box is raised when one of \c box's lower coordinates
     *  have a greater values than their corresponding higher coordinates, it
     *  could mean that box is invalid or does not match the specified layout.
     *  \param container that could be used to store the box
     *  \param box the box that must be checked.
     */
    template <typename Tp>
    inline void check_box
    (const Tp& container,
     const typename container_traits<Tp>::key_type& box,
     llhh_layout_tag)
    {
      dimension_type rank = container.dimension() >> 1;
      for (dimension_type i = 0; i < rank; ++i)
        if (container.key_comp()(i + rank, box, i, box))
          throw invalid_box
            ("box does not follow specified layout or coordinates are invalid");
    }

    template <typename Tp>
    inline void check_box
    (const Tp& container,
     const typename container_traits<Tp>::key_type& box,
     hhll_layout_tag)
    {
      dimension_type rank = container.dimension() >> 1;
      for (dimension_type i = 0; i < rank; ++i)
        if (container.key_comp()(i, box, i + rank, box))
          throw invalid_box
            ("box does not follow specified layout or coordinates are invalid");
    }

    template <typename Tp>
    inline void check_box
    (const Tp& container,
     const typename container_traits<Tp>::key_type& box,
     lhlh_layout_tag)
    {
      for (dimension_type i = 0; i < container.dimension(); i += 2)
        if (container.key_comp()(i + 1, box, i, box))
          throw invalid_box
            ("box does not follow specified layout or coordinates are invalid");
    }

    template <typename Tp>
    inline void check_box
    (const Tp& container,
     const typename container_traits<Tp>::key_type& box,
     hlhl_layout_tag)
    {
      for (dimension_type i = 0; i < container.dimension(); i += 2)
        if (container.key_comp()(i, box, i + 1, box))
          throw invalid_box
            ("box does not follow specified layout or coordinates are invalid");
    }
    //@}

  } // namespace except

} // namespace spatial

#endif // SPATIAL_EXCEPTIONS_HPP
