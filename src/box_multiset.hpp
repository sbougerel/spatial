// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   box_multiset.hpp
 *  @brief  Contains the definition of the @ref box_multiset and @ref
 *  runtime_box_multiset containers. These containers are not mapped containers and
 *  store values in space that can be represented as boxes.
 *
 *  Iterating these containers always yield a constant value iterator. That is
 *  because modifying the value stored in the container may compromise the
 *  ordering in the container. One way around this issue is to use a @ref
 *  box_multimap container or to @c const_cast the value dereferenced from the
 *  iterator.
 *
 *  @see box_multimap
 *  @see box_multiset
 */

#ifndef SPATIAL_BOX_MULTISET_HPP
#define SPATIAL_BOX_MULTISET_HPP

#include <memory>  // std::allocator
#include "bits/spatial_relaxed_kdtree.hpp"
#include "function.hpp"

namespace spatial
{

  template<dimension_type Rank, typename Key,
           typename Compare = bracket_less<Key>,
           typename BalancingPolicy = loose_balancing,
           typename Alloc = std::allocator<Key> >
  struct box_multiset
    : details::Relaxed_kdtree<details::Static_rank<Rank << 1>, const Key,
                              const Key, Compare, BalancingPolicy, Alloc>
  {
  private:
    typedef details::Relaxed_kdtree
    <details::Static_rank<Rank << 1>, const Key, const Key, Compare,
                          BalancingPolicy, Alloc>               base_type;
    typedef box_multiset<Rank, Key, Compare, BalancingPolicy, Alloc>  Self;

  public:
    box_multiset() { }

    explicit box_multiset(const Compare& compare)
      : base_type(details::Static_rank<Rank << 1>())
    { }

    box_multiset(const Compare& compare, const BalancingPolicy& balancing)
      : base_type(details::Static_rank<Rank << 1>(), compare, balancing)
    { }

    box_multiset(const Compare& compare, const BalancingPolicy& balancing,
             const Alloc& alloc)
      : base_type(details::Static_rank<Rank << 1>(), compare, balancing, alloc)
    { }

    box_multiset(const box_multiset& other)
      : base_type(other)
    { }

    box_multiset&
    operator=(const box_multiset& other)
    { return static_cast<Self&>(base_type::operator=(other)); }
  };

  /**
   *  Specialization for @ref box_multiset with runtime rank support. The rank
   *  of the @ref box_multiset can be determined at run time and does not need
   *  to be fixed at compile time. Using:
   *
   *  @code
   *    struct box { ... };
   *    box_multiset<0, box> my_set(2);
   *  @endcode
   */
  template<typename Key,
           typename Compare,
           typename BalancingPolicy,
           typename Alloc>
  struct box_multiset<0, Key, Compare, BalancingPolicy, Alloc>
    : details::Relaxed_kdtree<details::Dynamic_rank, const Key, const Key,
                              Compare, BalancingPolicy, Alloc>
  {
  private:
    typedef details::Relaxed_kdtree<details::Dynamic_rank,
                                    const Key, const Key, Compare,
                                    BalancingPolicy, Alloc> base_type;
    typedef box_multiset<0, Key, Compare, BalancingPolicy, Alloc> Self;

  public:
    box_multiset() : base_type(details::Dynamic_rank(2)) { }

    explicit box_multiset(dimension_type dim)
      : base_type(details::Dynamic_rank(dim << 1))
    { except::check_rank(dim); }

    box_multiset(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim << 1), compare)
    { except::check_rank(dim); }

    box_multiset(dimension_type dim, const Compare& compare,
           const BalancingPolicy& policy)
      : base_type(details::Dynamic_rank(dim << 1), compare, policy)
    { except::check_rank(dim); }

    box_multiset(dimension_type dim, const Compare& compare,
           const BalancingPolicy& policy, const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim << 1), compare, policy, alloc)
    { except::check_rank(dim); }

    explicit box_multiset(const Compare& compare)
      : base_type(details::Dynamic_rank(2), compare)
    { }

    box_multiset(const Compare& compare, const BalancingPolicy& policy)
      : base_type(details::Dynamic_rank(2), compare, policy)
    { }

    box_multiset(const Compare& compare, const BalancingPolicy& policy,
           const Alloc& alloc)
      : base_type(details::Dynamic_rank(2), compare, policy, alloc)
    { }

    box_multiset(const box_multiset& other)
      : base_type(other)
    { }

    box_multiset&
    operator=(const box_multiset& other)
    { return static_cast<Self&>(base_type::operator=(other)); }
  };

}

#endif // SPATIAL_BOX_MULTISET_HPP
