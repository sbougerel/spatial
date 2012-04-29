// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   boxset.hpp
 *  @brief  Contains the definition of the @ref boxset and @ref
 *  runtime_boxset containers. These containers are not mapped containers and
 *  store values in space that can be represented as boxes.
 *
 *  Iterating these containers always yield a constant value iterator. That is
 *  because modifying the value stored in the container may compromise the
 *  ordering in the container. One way around this issue is to use a @ref boxmap
 *  container or to @c const_cast the value dereferenced from the iterator.
 *
 *  @see boxmap
 *  @see runtime_boxmap
 *  @see boxset
 *  @see runtime_boxset
 */

#ifndef SPATIAL_BOXSET_HPP
#define SPATIAL_BOXSET_HPP

#include "bits/spatial.hpp"
#include "bits/spatial_relaxed_kdtree.hpp"

namespace spatial
{

  template<dimension_type Rank, typename Key,
           typename Compare = bracket_less<Key>,
           typename BalancingPolicy = loose_balancing,
           typename Alloc = std::allocator<Key> >
  struct boxset
    : details::Relaxed_kdtree<details::Static_rank<Rank << 1>, Key, Key,
                              Compare, BalancingPolicy, Alloc>
  {
  private:
    typedef details::Relaxed_kdtree
    <details::Static_rank<Rank << 1>, Key, Key, Compare,
                          BalancingPolicy, Alloc>               base_type;
    typedef boxset<Rank, Key, Compare, BalancingPolicy, Alloc>  Self;

  public:
    boxset() { }

    explicit boxset(const Compare& compare)
      : base_type(details::Static_rank<Rank << 1>())
    { }

    boxset(const Compare& compare, const BalancingPolicy& balancing)
      : base_type(details::Static_rank<Rank << 1>(), compare, balancing)
    { }

    boxset(const Compare& compare, const BalancingPolicy& balancing,
             const Alloc& alloc)
      : base_type(details::Static_rank<Rank << 1>(), compare, balancing, alloc)
    { }

    boxset(const boxset& other)
      : base_type(other)
    { }

    boxset&
    operator=(const boxset& other)
    { return static_cast<Self&>(base_type::operator=(other)); }
  };

  /**
   *  Specialization for @ref boxset with runtime rank support. The
   *  rank of the @ref boxset can be determined at run time and does not need
   *  to be fixed at compile time. Using:
   *  @code
   *    struct box { ... };
   *    boxset<0, box> my_set;
   *  @endcode
   *  ...is therefore completely equivalent to:
   *  @code
   *    struct box { ... };
   *    runtime_boxset<box> my_set;
   *  @endcode
   *
   *  @see runtime_frozen_boxset for more information about how to use this
   *  container.
   */
  template<typename Key,
           typename Compare,
           typename BalancingPolicy,
           typename Alloc>
  struct boxset<0, Key, Compare, BalancingPolicy, Alloc>
    : details::Relaxed_kdtree<details::Dynamic_rank, Key, Key, Compare,
                              BalancingPolicy, Alloc>
  {
  private:
    typedef details::Relaxed_kdtree<details::Dynamic_rank, Key, Key, Compare,
                                    BalancingPolicy, Alloc> base_type;
    typedef boxset<0, Key, Compare, BalancingPolicy, Alloc> Self;

  public:
    boxset() : base_type(details::Dynamic_rank(2)) { }

    explicit boxset(dimension_type dim)
      : base_type(details::Dynamic_rank(dim << 1))
    { except::check_rank(dim); }

    boxset(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim << 1), compare)
    { except::check_rank(dim); }

    boxset(dimension_type dim, const Compare& compare,
           const BalancingPolicy& policy)
      : base_type(details::Dynamic_rank(dim << 1), compare, policy)
    { except::check_rank(dim); }

    boxset(dimension_type dim, const Compare& compare,
           const BalancingPolicy& policy, const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim << 1), compare, policy, alloc)
    { except::check_rank(dim); }

    explicit boxset(const Compare& compare)
      : base_type(details::Dynamic_rank(2), compare)
    { }

    boxset(const Compare& compare, const BalancingPolicy& policy)
      : base_type(details::Dynamic_rank(2), compare, policy)
    { }

    boxset(const Compare& compare, const BalancingPolicy& policy,
           const Alloc& alloc)
      : base_type(details::Dynamic_rank(2), compare, policy, alloc)
    { }

    boxset(const boxset& other)
      : base_type(other)
    { }

    boxset&
    operator=(const boxset& other)
    { return static_cast<Self&>(base_type::operator=(other)); }
  };

  /**
   *  @brief  boxset with runtime rank support. The rank of the boxset can
   *  be determined at run time and does not need to be fixed at compile time.
   */
  template<typename Key,
           typename Compare = bracket_less<Key>,
           typename BalancingPolicy = loose_balancing,
           typename Alloc = std::allocator<Key> >
  struct runtime_boxset
    : details::Relaxed_kdtree<details::Dynamic_rank, Key, Key, Compare,
                              BalancingPolicy, Alloc>
  {
  private:
    typedef details::Relaxed_kdtree<details::Dynamic_rank, Key, Key, Compare,
                                    BalancingPolicy, Alloc>      base_type;
    typedef runtime_boxset<Key, Compare, BalancingPolicy, Alloc> Self;

  public:
      runtime_boxset() : base_type(details::Dynamic_rank(2)) { }

    explicit runtime_boxset(dimension_type dim)
      : base_type(details::Dynamic_rank(dim << 1))
    { except::check_rank(dim); }

    runtime_boxset(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim << 1), compare)
    { except::check_rank(dim); }

    runtime_boxset(dimension_type dim, const Compare& compare,
                   const BalancingPolicy& policy)
      : base_type(details::Dynamic_rank(dim << 1), compare, policy)
    { except::check_rank(dim); }

    runtime_boxset(dimension_type dim, const Compare& compare,
                   const BalancingPolicy& policy, const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim << 1), compare, policy, alloc)
    { except::check_rank(dim); }

    explicit runtime_boxset(const Compare& compare)
      : base_type(details::Dynamic_rank(2), compare)
    { }

    runtime_boxset(const Compare& compare, const BalancingPolicy& policy)
      : base_type(details::Dynamic_rank(2), compare, policy)
    { }

    runtime_boxset(const Compare& compare, const BalancingPolicy& policy,
                   const Alloc& alloc)
      : base_type(details::Dynamic_rank(2), compare, policy, alloc)
    { }

    runtime_boxset(const runtime_boxset& other)
      : base_type(other)
    { }

    runtime_boxset&
    operator=(const runtime_boxset& other)
    { return static_cast<Self&>(base_type::operator=(other)); }
  };

}

#endif // SPATIAL_BOXSET_HPP
