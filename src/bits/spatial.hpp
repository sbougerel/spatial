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
#ifdef SPATIAL_BAD_CSTDDEF
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

  /**
   *  Represents a coordinate layout for the box. llhh stands for low, low,
   *  high, high. This layout means that the lower coordinates of the box are
   *  expressed first, and the higher coordinates are expressed after.
   *
   *  For a box of dimension 2, the llhh layout means that for each box,
   *  delmited by 2 points x and y, the coordinate are ordered as follow:
   *
   *      B = { x0, x1, y0, y1 }
   *
   *  With <tt>x0 <= y0</tt> and <tt>x1 <= y1</tt>.
   */
  struct llhh_layout_tag { };

  /**
   *  Represents a coordinate layout for the box. lhlh stands for low, high,
   *  low, high. This layout means that the lower coordinates of the box are
   *  expressed first, and the higher coordinates are expressed second,
   *  alternatively for each dimension.
   *
   *  For a box of dimension 2, the lhlh layout means that for each box,
   *  delmited by 2 points x and y, the coordinate are ordered as follow:
   *
   *      B = { x0, y0, x1, y1 }
   *
   *  With <tt>x0 <= y0</tt> and <tt>x1 <= y1</tt>.
   */
  struct lhlh_layout_tag { };

  /**
   *  Represents a coordinate layout for the box. hhll stands for high, high,
   *  low, low. This layout means that the upper coordinates of the box are
   *  expressed first, and the lower coordinates are expressed after.
   *
   *  For a box of dimension 2, the hhll layout means that for each box,
   *  delmited by 2 points x and y, the coordinate are ordered as follow:
   *
   *      B = { x0, x1, y0, y1 }
   *
   *  With <tt>x0 >= y0</tt> and <tt>x1 >= y1</tt>.
   */
  struct hhll_layout_tag { };

  /**
   *  Represents a coordinate layout for the box. lhlh stands for high, low
   *  high, low. This layout means that the upper coordinates of the box are
   *  expressed first, and the lower coordinates are expressed second,
   *  alternatively for each dimension.
   *
   *  For a box of dimension 2, the hlhl layout means that for each box,
   *  delmited by 2 points x and y, the coordinate are ordered as follow:
   *
   *      B = { x0, y0, x1, y1 }
   *
   *  With <tt>x0 >= y0</tt> and <tt>x1 >= y1</tt>.
   */
  struct hlhl_layout_tag { };

}

#endif // SPATIAL_HPP
