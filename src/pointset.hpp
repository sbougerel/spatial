// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   pointset.hpp
 *  @brief  Contains the definition of the @ref pointset and @ref
 *  runtime_pointset containers. These containers are not mapped containers and
 *  store values in space that can be represented as points.
 *
 *  Iterating these containers always yield a constant value iterator. That is
 *  because modifying the value stored in the container may compromise the
 *  ordering in the container. One way around this issue is to use a @ref
 *  pointmap container or to @c const_cast the value dereferenced from the
 *  iterator.
 *
 *  @see pointmap
 *  @see runtime_pointmap
 *  @see pointset
 *  @see runtime_pointset
 */

#ifndef SPATIAL_POINTSET_HPP
#define SPATIAL_POINTSET_HPP

#include "bits/spatial.hpp"
#include "bits/spatial_relaxed_kdtree.hpp"

namespace spatial
{

  template<dimension_type Rank, typename Key,
           typename Compare = bracket_less<Key>,
           typename BalancingPolicy = loose_balancing,
           typename Alloc = std::allocator<Key> >
  struct pointset
    : details::Relaxed_kdtree<details::Static_rank<Rank>, Key, Key, Compare,
                              BalancingPolicy, Alloc>
  {
  private:
    typedef
    details::Relaxed_kdtree<details::Static_rank<Rank>, Key, Key, Compare,
                            BalancingPolicy, Alloc>               base_type;
    typedef pointset<Rank, Key, Compare, BalancingPolicy, Alloc>  Self;

  public:
    pointset() { }

    explicit pointset(const Compare& compare)
      : base_type(details::Static_rank<Rank>())
    { }

    pointset(const Compare& compare, const BalancingPolicy& balancing)
      : base_type(details::Static_rank<Rank>(), compare, balancing)
    { }

    pointset(const Compare& compare, const BalancingPolicy& balancing,
             const Alloc& alloc)
      : base_type(details::Static_rank<Rank>(), compare, balancing, alloc)
    { }

    pointset(const pointset& other)
      : base_type(other)
    { }

    pointset&
    operator=(const pointset& other)
    { return static_cast<Self&>(base_type::operator=(other)); }
  };

  /**
   *  Specialization for @ref pointset with runtime rank support. The rank of
   *  the @ref pointset can be determined at run time and does not need to be
   *  fixed at compile time. Using:
   *  @code
   *    struct point { ... };
   *    pointset<0, point> my_set;
   *  @endcode
   *  ...is therefore completely equivalent to:
   *  @code
   *    struct point { ... };
   *    runtime_pointset<point> my_set;
   *  @endcode
   *
   *  @see runtime_pointset for more information about how to use this
   *  container.
   */
  template<typename Key, typename Compare, typename BalancingPolicy,
           typename Alloc>
  struct pointset<0, Key, Compare, BalancingPolicy, Alloc>
    : details::Relaxed_kdtree<details::Dynamic_rank, Key, Key, Compare,
                              BalancingPolicy, Alloc>
  {
  private:
    typedef details::Relaxed_kdtree<details::Dynamic_rank, Key, Key, Compare,
                                    BalancingPolicy, Alloc>    base_type;
    typedef pointset<0, Key, Compare, BalancingPolicy, Alloc>  Self;

  public:
    pointset() { }

    explicit pointset(dimension_type dim)
      : base_type(details::Dynamic_rank(dim))
    { except::check_rank_argument(dim); }

    pointset(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim), compare)
    { except::check_rank_argument(dim); }

    pointset(dimension_type dim, const Compare& compare,
                     const BalancingPolicy& policy)
      : base_type(details::Dynamic_rank(dim), compare, policy)
    { except::check_rank_argument(dim); }

    pointset(dimension_type dim, const Compare& compare,
                     const BalancingPolicy& policy, const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim), compare, policy, alloc)
    { except::check_rank_argument(dim); }

    explicit pointset(const Compare& compare)
      : base_type(details::Dynamic_rank(), compare)
    { }

    pointset(const Compare& compare, const BalancingPolicy& policy)
      : base_type(details::Dynamic_rank(), compare, policy)
    { }

    pointset(const Compare& compare, const BalancingPolicy& policy,
                     const Alloc& alloc)
      : base_type(details::Dynamic_rank(), compare, policy, alloc)
    { }

    pointset(const pointset& other)
      : base_type(other)
    { }

    pointset&
    operator=(const pointset& other)
    { return static_cast<Self&>(base_type::operator=(other)); }
  };

  /**
   *  @brief  pointset with runtime rank support. The rank of the pointset can
   *  be determined at run time and does not need to be fixed at compile time.
   */
  template<typename Key,
           typename Compare = bracket_less<Key>,
           typename BalancingPolicy = loose_balancing,
           typename Alloc = std::allocator<Key> >
  struct runtime_pointset
    : details::Relaxed_kdtree<details::Dynamic_rank, Key, Key, Compare,
                              BalancingPolicy, Alloc>
  {
  private:
    typedef details::Relaxed_kdtree<details::Dynamic_rank, Key, Key, Compare,
                                    BalancingPolicy, Alloc>        base_type;
    typedef runtime_pointset<Key, Compare, BalancingPolicy, Alloc> Self;

  public:
    runtime_pointset() { }

    explicit runtime_pointset(dimension_type dim)
      : base_type(details::Dynamic_rank(dim))
    { except::check_rank_argument(dim); }

    runtime_pointset(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim), compare)
    { except::check_rank_argument(dim); }

    runtime_pointset(dimension_type dim, const Compare& compare,
                     const BalancingPolicy& policy)
      : base_type(details::Dynamic_rank(dim), compare, policy)
    { except::check_rank_argument(dim); }

    runtime_pointset(dimension_type dim, const Compare& compare,
                     const BalancingPolicy& policy, const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim), compare, policy, alloc)
    { except::check_rank_argument(dim); }

    explicit runtime_pointset(const Compare& compare)
      : base_type(details::Dynamic_rank(), compare)
    { }

    runtime_pointset(const Compare& compare, const BalancingPolicy& policy)
      : base_type(details::Dynamic_rank(), compare, policy)
    { }

    runtime_pointset(const Compare& compare, const BalancingPolicy& policy,
                     const Alloc& alloc)
      : base_type(details::Dynamic_rank(), compare, policy, alloc)
    { }

    runtime_pointset(const runtime_pointset& other)
      : base_type(other)
    { }

    runtime_pointset&
    operator=(const runtime_pointset& other)
    { return static_cast<Self&>(base_type::operator=(other)); }
  };

}

#endif // SPATIAL_POINTSET_HPP
