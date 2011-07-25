// -*- C++ -*-

/**
 *  @file   spatial_exceptions.hpp
 *  @brief  Defines exception that are being thrown by the library
 *
 *  Change Log:
 *
 *  - 2009-12-14 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_EXCEPTIONS_HPP
#define SPATIAL_EXCEPTIONS_HPP

#include <stdexcept>

namespace spatial
{

  /**
   *  @brief  Thrown to report that an invalid rank was passed as an argument.
   *  Generally thrown because 0 was passed as an argument for the rank.
   */
  struct invalid_rank_argument : std::logic_error
  {
    explicit invalid_rank_argument(const std::string& arg)
      : logic_error(arg) { }
  };

  /**
   *  @brief  Thrown to report that an invalid dimension was passed as an
   *  argument.
   */
  struct invalid_dimension_argument : std::logic_error
  {
    explicit invalid_dimension_argument(const std::string& arg)
      : logic_error(arg) { }
  };

  /**
   *  @brief  Thrown to report that an invalid node was passed as an argument.
   */
  struct invalid_node_argument : std::logic_error
  {
    explicit invalid_node_argument(const std::string& arg)
      : logic_error(arg) { }
  };

  /**
   *  @brief  Thrown to report that an invalid iterator was passed as an argument.
   */
  struct invalid_iterator_argument : std::logic_error
  {
    explicit invalid_iterator_argument(const std::string& arg)
      : logic_error(arg) { }
  };

  /**
   *  @brief  Thrown to report that an empty container was passed as an argument,
   *  while the function does not accept an empty container.
   */
  struct invalid_empty_container_argument : std::logic_error
  {
    explicit invalid_empty_container_argument(const std::string& arg)
      : logic_error(arg) { }
  };

  /**
   *  @brief  Thrown to report that an invalid range bound has been given as
   *  argument.
   *  @see check_invalid_range_bounds()
   *  @see range_bounds
   *
   *  Generally, this means that the lower bound has a value that overlaps with
   *  the upper bound over one dimension, at least.
   */
  struct invalid_range_bounds : std::logic_error
  {
    explicit invalid_range_bounds(const std::string& arg)
      : logic_error(arg) { }
  };

  /**
   *  Thrown to report that a box has incorrect coordinates with regards to
   *  its layout.
   *  @see check_invalid_box()
   */
  struct invalid_box_argument : std::logic_error
  {
    explicit invalid_box_argument(const std::string& arg)
      : logic_error(arg) { }
  };

  namespace except
  {
    /**
     *  Checks that @c rank is not null.
     *  @exception invalid_rank_argument is thrown if checks fails.
     */
    inline void check_rank_argument(dimension_type rank)
    { if (0 == rank) throw invalid_rank_argument("rank is null"); }

    /**
     *  Checks that @c dimension is not greater or equal to @c rank.
     *  @exception invalid_dimension_argument is thrown if checks fails.
     */
    inline void check_dimension_argument
    (dimension_type rank, dimension_type dimension)
    {
      if (dimension >= rank)
        throw invalid_dimension_argument("dimension is out of range");
    }

    /**
     *  Checks that the node's pointer given as an argument to a function is not
     *  null or does not point to a header node.
     *  @exception invalid_node_argument is thrown if checks fails.
     *  @param node the node pointer to check.
     */
    template<typename Node>
    inline void check_node_argument(Node* node)
    {
      if (node == 0 || node->left == node)
        throw invalid_node_argument
          ("node points to null or header node");
    }

    /**
     *  Checks that the node pointed to by an iterator and given as an argument
     *  to a function is not null or does not point to a header node.
     *  @exception invalid_iterator_argument is thrown if checks fails.
     */
    template<typename Node>
    inline void check_node_iterator_argument(Node* node)
    {
      if (node == 0 || node->left == node)
        throw invalid_iterator_argument
          ("iterator is points to null or header node");
    }

    /**
     *  Checks if two iterators are of equal values, if not raises the @ref
     *  invalid_iterator exception.
     *  @exception invalid_iterator is raised when the nodes specified are not
     *  matching.
     *
     *  For this test to be useful, one of the iterator must be sure to belong
     *  to a container.
     */
    template <typename Ptr1, typename Ptr2>
    inline void check_iterator_argument(Ptr1 ptr1, Ptr2 ptr2)
    {
      if (ptr1 != ptr2)
        throw invalid_iterator_argument
          ("iterator is invalid or does not belong to the container used");
    }

    /**
     *  Checks that the container given as an argument to a function is not
     *  empty.
     *  @exception invalid_empty_container_argument is thrown if checks fails.
     *  @param cont the container that must not be empty.
     */
    template<typename Tp>
    inline void check_empty_container_argument(const Tp& cont)
    {
      if (cont.empty())
        throw invalid_empty_container_argument("container is empty");
    }

    /**
     *  Checks if all coordinates of @c lower are strictly less than these of
     *  @c higher along the same dimensions. The number of dimensions is limited
     *  by the rank of @c container.
     *  @exception invalid_range_bounds is thrown if the check fails.
     *  @param container providing type information, comparison and rank.
     *  @param lower the lower bound of the interval considered.
     *  @param upper the upper bound of the interval considered.
     *
     *  This check is performed mainly upon creation of a @ref open_range_bounds
     *  predicate.
     */
    template<typename Tp>
    inline void check_open_range_bounds
    (const Tp& container,
     const typename container_traits<Tp>::key_type& lower,
     const typename container_traits<Tp>::key_type& upper)
    {
      for (dimension_type dim = 0; dim < container.dimension(); ++dim)
        if (!container.key_comp()(dim, lower, upper))
          throw invalid_range_bounds
            ("lower is greater or equal to upper over one dimension at least");
    }

    /**
     *  Checks if all coordinates of @c lower are strictly less than these of
     *  @c higher along the same dimensions. The number of dimensions is limited
     *  by the rank of @c container.
     *  @exception invalid_range_bounds is thrown if the check fails.
     *
     *  This check is performed mainly upon creation of a @ref range_bounds
     *  predicate.
     */
    template<typename Tp>
    inline void check_range_bounds
    (const Tp& container,
     const typename container_traits<Tp>::key_type& lower,
     const typename container_traits<Tp>::key_type& upper)
    {
        return check_open_range_bounds(container, lower, upper);
    }

    /**
     *  Checks if all coordinates of @c lower are less or equal to these of
     *  @c higher along the same dimensions. The number of dimensions is limited
     *  by the rank of @c container.
     *  @exception invalid_closed_range_bounds is thrown if the check fails.
     *
     *  This check is performed mainly upon creation of a @ref
     *  closed_range_bounds predicate.
     */
    template<typename Tp>
    inline void check_closed_range_bounds
    (const Tp& container,
     const typename container_traits<Tp>::key_type& lower,
     const typename container_traits<Tp>::key_type& upper)
    {
      for (dimension_type dim = 0; dim < container.dimension(); ++dim)
        if (container.key_comp()(dim, upper, lower))
          throw invalid_range_bounds
            ("upper is stricly less than lower over one dimension at least");
    }

    //@{
    /**
     *  Checks that all coordinates of a box are matching with the layout
     *  specified or else, raise an @ref invalid_box exception.
     *  @exception invalid_box is raised when one of @p{box}'s lower coordinates
     *  have a greater values than their corresponding higher coordinates, it
     *  could mean that box is invalid or does not match the specified layout.
     *  @param container that could be used to store the box
     *  @param box the box that must be checked.
     */
    template <typename Tp>
    inline void check_box_argument
    (const Tp& container,
     const typename container_traits<Tp>::key_type& box,
     llhh_layout_tag)
    {
      dimension_type rank = container.dimension() >> 1;
      for (dimension_type i = 0; i < rank; ++i)
        if (container.key_comp()(i + rank, box, i, box))
          throw invalid_box_argument
            ("box does not follow specified layout or coordinates are invalid");
    }

    template <typename Tp>
    inline void check_box_argument
    (const Tp& container,
     const typename container_traits<Tp>::key_type& box,
     hhll_layout_tag)
    {
      dimension_type rank = container.dimension() >> 1;
      for (dimension_type i = 0; i < rank; ++i)
        if (container.key_comp()(i, box, i + rank, box))
          throw invalid_box_argument
            ("box does not follow specified layout or coordinates are invalid");
    }

    template <typename Tp>
    inline void check_box_argument
    (const Tp& container,
     const typename container_traits<Tp>::key_type& box,
     lhlh_layout_tag)
    {
      for (dimension_type i = 0; i < container.dimension(); i += 2)
        if (container.key_comp()(i + 1, box, i, box))
          throw invalid_box_argument
            ("box does not follow specified layout or coordinates are invalid");
    }

    template <typename Tp>
    inline void check_box_argument
    (const Tp& container,
     const typename container_traits<Tp>::key_type& box,
     hlhl_layout_tag)
    {
      for (dimension_type i = 0; i < container.dimension(); i += 2)
        if (container.key_comp()(i, box, i + 1, box))
          throw invalid_box_argument
            ("box does not follow specified layout or coordinates are invalid");
    }
    //@}

  } // namespace except

} // namespace spatial

#endif // SPATIAL_EXCEPTIONS_HPP
