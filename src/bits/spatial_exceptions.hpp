// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
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
#include <limits>

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

  /**
   *  Thrown to report that an negative distance has been passed as a parameter
   *  while distances are expected to be positive.
   *
   *  \see check_addition(), check_multiplication()
   */
  struct negative_distance : std::logic_error
  {
    explicit negative_distance(const std::string& arg)
      : std::logic_error(arg) { }
  };

  /**
   *  Thrown to report that an arithmetic error has occured during a
   *  calculation. It could be an overflow, or another kind of error.
   *
   *  \see check_addition(), check_multiplication()
   */
  struct arithmetic_error : std::logic_error
  {
    explicit arithmetic_error(const std::string& arg)
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
     *  Check that the distance given in \x has a positive value.
     *  \throws negative_distance
     */
    template<typename Tp>
    inline void check_positive_distance(const Tp& x)
    {
      if (!::std::numeric_limits<Tp>::is_signed()) return;
      if (x < -x) // Use inversion as we don't know how to represent '0' in Tp.
        throw negative_distance("distance is negative");
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
          ("iterator is points to null or header node");
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

    /**
     *  Check that the absolute of an element has not led to an error such as
     *  an overflow, by forcing the error itself.
     *
     *  This check is not the best check for arithmetic errors. There are ways
     *  to make it better, but it's hard to make it more portable.
     *
     *  In particular, if \c Tp is not a base type, the author of the type must
     *  define the numeric limits \c ::std::numeric_limits<Tp>::max() for that
     *  type.
     */
    template <typename Tp>
    inline Tp check_abs(const Tp& x)
    {
      if (!::std::numeric_limits<Tp>::is_signed()) return x;
      Tp _x = -x; _x = x > _x ? x : _x;
      if (x - x > _x)
        throw arithmetic_error
          ("Absolute of an element has resulted in an arithmetic error");
      return _x;
    }

    /**
     *  Check that the addtion of 2 elements of positive value has not led to an
     *  error such as an overflow, by forcing the error itself.
     *
     *  This check will only work for 2 positive element x and y. This check is
     *  not the best check for arithmetic errors. There are ways to make it
     *  better, but it's hard to make it more portable.
     *
     *  In particular, if \c Tp is not a base type, the author of the type must
     *  define the numeric limits \c ::std::numeric_limits<Tp>::max() for that
     *  type.
     */
    template <typename Tp>
    inline Tp check_positive_add(const Tp& x, const Tp& y)
    {
      if ((::std::numeric_limits<Tp>::max() - x) < y)
        throw arithmetic_error
          ("Addition of two elements has resulted in an arithmetic error");
      return x + y;
    }

    /**
     *  Check that the computation of the square of an element has not
     *  overflown.
     *
     *  This check is not the best check for arithmetic errors. There are ways
     *  to make it better, but it's hard to make it more portable.
     *
     *  In particular, if \c Tp is not a base type, the author of the type must
     *  define the numeric limits \c ::std::numeric_limits<Tp>::max() for that
     *  type.
     */
    template <typename Tp>
    inline Tp check_square(const Tp& x)
    {
      Tp _x = check_abs(x);
      if ((::std::numeric_limits<Tp>::max() / _x) < _x)
        throw arithmetic_error
          ("Square value of element has resulted in an arithmetic error");
      return x * x;
    }

    /**
     *  Check that the multiplication of 2 positive elements has not resulted
     *  into an arithmetic error such as overflown.
     *
     *  This check will only work for 2 positive element x and y. This check is
     *  not the best check for arithmetic errors. There are ways to make it
     *  better, but it's hard to make it more portable.
     *
     *  In particular, if \c Tp is not a base type, the author of the type must
     *  define the numeric limits \c ::std::numeric_limits<Tp>::max() for that
     *  type.
     */
    template <typename Tp>
    inline Tp check_positive_mul(const Tp& x, const Tp& y)
    {
      if ((::std::numeric_limits<Tp>::max() / y) < x)
        throw arithmetic_error
          ("Multiplication of two elements has resulted in an arithmetic error");
      return x * y;
    }
  } // namespace except

} // namespace spatial

#endif // SPATIAL_EXCEPTIONS_HPP
