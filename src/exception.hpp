// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   exception.hpp
 *  \brief  Defines exceptions that are being thrown by the library.
 */

#ifndef SPATIAL_EXCEPTION_HPP
#define SPATIAL_EXCEPTION_HPP

#include <stdexcept>
#include "bits/spatial.hpp"
#include "bits/spatial_traits.hpp"

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
} // namespace spatial

#endif // SPATIAL_EXCEPTION_HPP
