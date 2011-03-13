// -*- C++ -*-

/**
 *  @file   spatial.hpp
 *  @brief  Type defines and base operations
 *
 *  Change Log:
 *
 *  - 2009-09-03 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_HPP
#define SPATIAL_HPP

// For compatibility with older environments
#ifdef SPATIAL_MISSING_OR_OBSOLETE_CSTDDEF
#include <stddef.h>
namespace std
{
  using ::size_t;
  using ::ptrdiff_t;
}
#else
#include <cstddef>
#endif

namespace spatial
{

  /**
   *  @brief  Defines a positive integral type for counting objects or storing
   *  absolute values.
   */
  typedef std::size_t size_type;

  /**
   *  @brief  Defines the type for the dimension as being a size.
   */
  typedef std::size_t dimension_type;

  /**
   *  @brief  Defines weight as being a size.
   */
  typedef std::size_t weight_type;

  /**
   *  @brief  Defines values for relative ordering.
   */
  typedef
  enum { below = -1, matching = 0, above = 1 }
    relative_order;
}

#endif // SPATIAL_HPP
