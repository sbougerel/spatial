// -*- C++ -*-

/**
 *  @file   spatial_rank.hpp
 *  @brief  Defines spatial::details::Static_rank and
 *  spatial::details::Dynamic_rank
 */

#ifndef SPATIAL_RANK_HPP
#define SPATIAL_RANK_HPP

#include "spatial.hpp"

namespace spatial
{
  namespace details
  {
    /**
     *  The dimension value is set by a template value, thus consuming
     *  no memory.
     *
     *  \tparam The dimension of the rank.
     */
    template <dimension_type Value>
    struct Static_rank
    {
      //! Returns the dimension for the rank specified in the template parameter
      //! \c Value.
      dimension_type operator()() const
      { return Value; }
    };

    /**
     *  The dimension value is stored by a member of the object, but can be
     *  modified at run time.
     */
    struct Dynamic_rank
    {
      //! Returns the dimension for the rank stored in \c _rank
      dimension_type operator()() const
      { return _rank; }

      //! Build a rank with a default dimension of 1.
      //! \param rank The specified rank dimension.
      explicit
      Dynamic_rank(dimension_type rank = 1)
        : _rank(rank)
      { }

    private:
      //! The value that stores the rank dimension.
      dimension_type _rank;
    };
  }
}

#endif // SPATIAL_RANK_HPP
