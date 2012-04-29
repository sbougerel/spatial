// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   boxmap.hpp
 *  @brief  Contains the definition of the @ref boxmap and @ref
 *  runtime_boxmap containers. These containers are mapped containers and
 *  store values in space that can be represented as boxes.
 *
 *  @see boxmap
 *  @see runtime_boxmap
 */

#ifndef SPATIAL_BOXMAP_HPP
#define SPATIAL_BOXMAP_HPP

#include "bits/spatial.hpp"
#include "bits/spatial_relaxed_kdtree.hpp"

namespace spatial
{

  template<dimension_type Rank, typename Key, typename Mapped,
           typename Compare = bracket_less<Key>,
           typename BalancingPolicy = loose_balancing,
           typename Alloc = std::allocator<std::pair<const Key, Mapped> > >
  struct boxmap
    : details::Relaxed_kdtree<details::Static_rank<Rank << 1>, Key,
                              std::pair<const Key, Mapped>,
                              Compare, BalancingPolicy, Alloc>
  {
  private:
    typedef details::Relaxed_kdtree
    <details::Static_rank<Rank << 1>, Key, std::pair<const Key, Mapped>, Compare,
     BalancingPolicy, Alloc>                  base_type;
    typedef boxmap<Rank, Key, Mapped, Compare, BalancingPolicy,
                   Alloc>                     Self;

  public:
    typedef Mapped                            mapped_type;

    boxmap() { }

    explicit boxmap(const Compare& compare)
      : base_type(details::Static_rank<Rank << 1>())
    { }

    boxmap(const Compare& compare, const BalancingPolicy& balancing)
      : base_type(details::Static_rank<Rank << 1>(), compare, balancing)
    { }

    boxmap(const Compare& compare, const BalancingPolicy& balancing,
             const Alloc& alloc)
      : base_type(details::Static_rank<Rank << 1>(), compare, balancing, alloc)
    { }

    boxmap(const boxmap& other)
      : base_type(other)
    { }

    boxmap&
    operator=(const boxmap& other)
    { return static_cast<Self&>(base_type::operator=(other)); }
  };

  /**
   *  Specialization for @ref boxmap with runtime rank support. The
   *  rank of the @ref boxmap can be determined at run time and does not need
   *  to be fixed at compile time. Using:
   *  @code
   *    struct box { ... };
   *    struct mapped { ... };
   *    boxmap<0, box, mapped> my_set;
   *  @endcode
   *  ...is therefore completely equivalent to:
   *  @code
   *    struct box { ... };
   *    struct mapped { ... };
   *    runtime_boxmap<box, mapped> my_set;
   *  @endcode
   *
   *  @see runtime_frozen_boxmap for more information about how to use this
   *  container.
   */
  template<typename Key, typename Mapped,
           typename Compare,
           typename BalancingPolicy,
           typename Alloc>
  struct boxmap<0, Key, Mapped, Compare, BalancingPolicy, Alloc>
    : details::Relaxed_kdtree<details::Dynamic_rank, Key,
                              std::pair<const Key, Mapped>, Compare,
                              BalancingPolicy, Alloc>
  {
  private:
    typedef details::Relaxed_kdtree
    <details::Dynamic_rank, Key, std::pair<const Key, Mapped>, Compare,
     BalancingPolicy, Alloc>                  base_type;
    typedef boxmap<0, Key, Mapped, Compare,
                   BalancingPolicy, Alloc>    Self;

  public:
    typedef Mapped                            mapped_type;

    boxmap() : base_type(details::Dynamic_rank(2)) { }

    explicit boxmap(dimension_type dim)
      : base_type(details::Dynamic_rank(dim << 1))
    { except::check_rank(dim); }

    boxmap(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim << 1), compare)
    { except::check_rank(dim); }

    boxmap(dimension_type dim, const Compare& compare,
           const BalancingPolicy& policy)
      : base_type(details::Dynamic_rank(dim << 1), compare, policy)
    { except::check_rank(dim); }

    boxmap(dimension_type dim, const Compare& compare,
           const BalancingPolicy& policy, const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim << 1), compare, policy, alloc)
    { except::check_rank(dim); }

    explicit boxmap(const Compare& compare)
      : base_type(details::Dynamic_rank(2), compare)
    { }

    boxmap(const Compare& compare, const BalancingPolicy& policy)
      : base_type(details::Dynamic_rank(2), compare, policy)
    { }

    boxmap(const Compare& compare, const BalancingPolicy& policy,
           const Alloc& alloc)
      : base_type(details::Dynamic_rank(2), compare, policy, alloc)
    { }

    boxmap(const boxmap& other)
      : base_type(other)
    { }

    boxmap&
    operator=(const boxmap& other)
    { return static_cast<Self&>(base_type::operator=(other)); }
  };

  /**
   *  @brief  boxmap with runtime rank support. The rank of the boxmap can
   *  be determined at run time and does not need to be fixed at compile time.
   */
  template<typename Key, typename Mapped,
           typename Compare = bracket_less<Key>,
           typename BalancingPolicy = loose_balancing,
           typename Alloc = std::allocator<std::pair<const Key, Mapped> > >
  struct runtime_boxmap
    : details::Relaxed_kdtree<details::Dynamic_rank, Key,
                              std::pair<const Key, Mapped>, Compare,
                              BalancingPolicy, Alloc>
  {
  private:
    typedef details::Relaxed_kdtree
    <details::Dynamic_rank, Key, std::pair<const Key, Mapped>, Compare,
     BalancingPolicy, Alloc>                  base_type;
    typedef runtime_boxmap<Key, Mapped, Compare, BalancingPolicy,
                           Alloc>             Self;

  public:
    typedef Mapped                            mapped_type;

    runtime_boxmap() : base_type(details::Dynamic_rank(2)) { }

    explicit runtime_boxmap(dimension_type dim)
      : base_type(details::Dynamic_rank(dim << 1))
    { except::check_rank(dim); }

    runtime_boxmap(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim << 1), compare)
    { except::check_rank(dim); }

    runtime_boxmap(dimension_type dim, const Compare& compare,
                   const BalancingPolicy& policy)
      : base_type(details::Dynamic_rank(dim << 1), compare, policy)
    { except::check_rank(dim); }

    runtime_boxmap(dimension_type dim, const Compare& compare,
                   const BalancingPolicy& policy, const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim << 1), compare, policy, alloc)
    { except::check_rank(dim); }

    explicit runtime_boxmap(const Compare& compare)
      : base_type(details::Dynamic_rank(2), compare)
    { }

    runtime_boxmap(const Compare& compare, const BalancingPolicy& policy)
      : base_type(details::Dynamic_rank(2), compare, policy)
    { }

    runtime_boxmap(const Compare& compare, const BalancingPolicy& policy,
                   const Alloc& alloc)
      : base_type(details::Dynamic_rank(2), compare, policy, alloc)
    { }

    runtime_boxmap(const runtime_boxmap& other)
      : base_type(other)
    { }

    runtime_boxmap&
    operator=(const runtime_boxmap& other)
    { return static_cast<Self&>(base_type::operator=(other)); }
  };

}

#endif // SPATIAL_BOXMAP_HPP
