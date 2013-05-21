// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_rank.hpp
 *  @brief  Defines spatial::details::Static_rank and
 *  spatial::details::Dynamic_rank as well as associated functions on rank.
 */

#ifndef SPATIAL_RANK_HPP
#define SPATIAL_RANK_HPP

#include "../spatial.hpp"

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

    /**
     *  Increment dimension \c node_dim, given \c rank.
     *  \tparam Rank The model of \ref Rank used in this function.
     *  \param rank An object that is a model of \ref Rank.
     *  \param node_dim The value of the dimension for the node.
     */
    template<typename Rank>
    inline dimension_type
    incr_dim(const Rank& rank, dimension_type node_dim)
    { return (node_dim + 1) % rank(); }

    /**
     *  Decrement dimension \c node_dim, given \c rank.
     *  \tparam Rank The model of \ref Rank used in this function.
     *  \param rank An object that is a model of \ref Rank.
     *  \param node_dim The value of the dimension for the node.
     */
    template<typename Rank>
    inline dimension_type
    decr_dim(const Rank& rank, dimension_type node_dim)
    { return node_dim ? node_dim - 1 : rank() - 1; }

    /**
     *  Return true if \c x coordinate is less than \c y coordinate over
     *  dimension \c node_dim, given \c compare. If both coordinate are equal,
     *  then return true if address of \c x is less than address of \c y.
     *
     *  This operator always discriminate \c x and \c y unless they are
     *  precisely the same object.
     *  \tparam Key The key object to use in the comparison.
     *  \tparam Compare The comparison object that is a model of
     *  \ref TrivialComparison.
     */
    template <typename Key, typename Compare>
    inline bool
    less_by_ref(const Compare& compare, dimension_type node_dim,
                const Key& x, const Key& y)
    {
      return (compare(node_dim, x, y) || (&x < &y && !compare(node_dim, y, x)));
    }

    /**
     *  Return a boolean indicating whether all of \c key's coordinates are
     *  within range or not.
     *
     *  The key is simply tested across all dimesions over the predicate.
     *  \tparam Rank A type that is a model of \ref Rank.
     *  \tparam Key The key type that is used in the comparison.
     *  \tparam Predicate A type that is a model of \ref RangePredicate.
     *  \param rank The object of type \c Rank.
     *  \param key The key whose coordinates are verified to be within the
     *  range.
     *  \param predicate The \ref RangePredicate object used to represent the
     *  range.
     */
    template <typename Rank, typename Key, typename Predicate>
    inline bool
    match_all(const Rank& rank, const Key& key, const Predicate& predicate)
    {
      for (dimension_type i = 0; i < rank(); ++i)
        { if (predicate(i, rank(), key) != matching) { return false; } }
      return true;
    }

    /**
     *  Return a boolean indicating whether one of \c key's coordinates is
     *  within range or not.
     *
     *  The key is simply tested across all dimesions over the predicate.
     *  \tparam Rank A type that is a model of \ref Rank.
     *  \tparam Key The key type that is used in the comparison.
     *  \tparam Predicate A type that is a model of \ref RangePredicate.
     *  \param rank The object of type \c Rank.
     *  \param key The key whose coordinates are verified to be within the
     *  range.
     *  \param predicate The \ref RangePredicate object used to represent the
     *  range.
     */
    template <typename Rank, typename Key, typename Predicate>
    inline bool
    match_any(const Rank& rank, const Key& key, const Predicate& predicate)
    {
      for (dimension_type i = 0; i < rank(); ++i)
        { if (predicate(i, rank(), key) == matching) { return true; } }
      return false;
    }

    /**
     *  Return a boolean indicating whether all of \c key's coordinates are
     *  within range or not, except for \c exlude_dim which is left out of the
     *  test.
     *
     *  The key is simply tested across all dimesions over the predicate.
     *  \tparam Rank A type that is a model of \ref Rank.
     *  \tparam Key The key type that is used in the comparison.
     *  \tparam Predicate A type that is a model of \ref RangePredicate.
     *  \param rank The object of type \c Rank.
     *  \param exclude_dim The dimension that will be skipped during the test.
     *  \param key The key whose coordinates are verified to be within the
     *  range.
     *  \param predicate The \ref RangePredicate object used to represent the
     *  range.
     */
    template <typename Rank, typename Key, typename Predicate>
    inline bool
    match_most(const Rank& rank, dimension_type exclude_dim,
               const Key& key, const Predicate& predicate)
    {
      for (dimension_type i = 0; i < rank(); ++i)
        {
          if (i != exclude_dim && predicate(i, rank(), key) != matching)
            { return false; }
        }
      return true;
    }
  }
}

#endif // SPATIAL_RANK_HPP
