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
   *  @see range_bounds<Compare, Key>
   *
   *  Generally, this means that the lower bound has a greater value than the
   *  upper bound over one dimension, at least.
   */
  struct invalid_range_bounds : std::logic_error
  {
    explicit invalid_range_bounds(const std::string& arg)
      : logic_error(arg) { }
  };

  /**
   *  @brief  Thrown to report that an invalid range bound has been given as
   *  argument.
   *  @see check_invalid_range_bounds()
   *  @see range_bounds<Compare, Key>
   *
   *  Generally, this means that the lower bound has a greater value than the
   *  upper bound over one dimension, at least.
   */
  struct invalid_open_range_bounds : std::logic_error
  {
    explicit invalid_open_range_bounds(const std::string& arg)
      : logic_error(arg) { }
  };

  /**
   *  @brief  Thrown to report that an invalid range bound has been given as
   *  argument.
   *  @see check_invalid_range_bounds()
   *  @see range_bounds<Compare, Key>
   *
   *  Generally, this means that the lower bound has a greater value than the
   *  upper bound over one dimension, at least.
   */
  struct invalid_closed_range_bounds : std::logic_error
  {
    explicit invalid_closed_range_bounds(const std::string& arg)
      : logic_error(arg) { }
  };

  /**
   *  @brief  Thrown to report that the rank being used is too large for
   *  operation that is requested.
   *
   *  This is the case for iterations over nearest neighbor: very large number
   *  of dimension are not supported, and simply, one should use brute force
   *  instead as it would probably give better results.
   */
  struct invalid_rank_argument : std::logic_error
  {
    explicit invalid_rank_argument(const std::string& arg)
      : logic_error(arg) { }
  };

  namespace except
  {
    inline void check_dimension_argument
    (dimension_type rank, dimension_type dimension)
    {
      if (dimension >= rank)
	throw invalid_dimension_argument
	  ("dimension is out of range");
    }

    template<typename Node>
    inline void check_node_argument(Node* node)
    {
      if (node == 0 || node->left == node)
	throw invalid_node_argument
	  ("node points to null or header node");
    }

    template<typename Node>
    inline void check_iterator_argument(Node* node)
    {
      if (node == 0 || node->left == node)
	throw invalid_iterator_argument
	  ("iterator is points to null or header node");
    }

    template<typename Tp>
    inline void check_empty_container_argument(const Tp& cont)
    {
      if (cont.empty())
	throw invalid_empty_container_argument("container is empty");
    }

    template<typename Tp>
    inline void check_open_range_bounds
    (const Tp& container,
     const typename container_traits<Tp>::key_type& lower,
     const typename container_traits<Tp>::key_type& upper)
    {
      for (dimension_type dim = 0; dim < container.dimension(); ++dim)
	if (!container.compare()(dim, lower, upper))
	  throw invalid_open_range_bounds
	    ("lower is greater or equal to upper over one dimension at least");
    }

    template<typename Tp>
    inline void check_range_bounds
    (const Tp& tree,
     const typename container_traits<Tp>::key_type& lower,
     const typename container_traits<Tp>::key_type& upper)
    {
      for (dimension_type dim = 0; dim < tree.dimension(); ++dim)
	if (!tree.compare()(dim, lower, upper))
	  throw invalid_range_bounds
	    ("lower is greater or equal to upper over one dimension at least");
    }

    template<typename Tp>
    inline void check_closed_range_bounds
    (const Tp& container,
     const typename container_traits<Tp>::key_type& lower,
     const typename container_traits<Tp>::key_type& upper)
    {
      for (dimension_type dim = 0; dim < container.dimension(); ++dim)
	if (container.compare()(dim, upper, lower))
	  throw invalid_closed_range_bounds
	    ("upper is stricly less than lower over one dimension at least");
    }

    inline void check_rank_argument
    (dimension_type max_rank, dimension_type rank_arg)
    {
      if (rank_arg > max_rank)
	throw invalid_rank_argument
	  ("rank is too large, brute force would yield better results");
    }

  } // namespace except

} // namespace spatial

#endif // SPATIAL_EXCEPTIONS_HPP
