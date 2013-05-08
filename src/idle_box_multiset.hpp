// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   idle_box_multiset.hpp
 *  @brief  Contains the definition of the @ref idle_box_multiset and @ref
 *  runtime_idle_box_multiset containers. These containers are not mapped containers
 *  and store values in space that can be represented as boxes.
 *
 *  Iterating these containers always yield a constant value iterator. That is
 *  because modifying the value stored in the container may compromise the
 *  ordering in the container. One way around this issue is to use a @ref
 *  idle_boxmap container or to @c const_cast the value dereferenced from the
 *  iterator.
 *
 *  @see idle_box_multiset
 */

#ifndef SPATIAL_IDLE_BOX_MULTISET_HPP
#define SPATIAL_IDLE_BOX_MULTISET_HPP

#include <memory>  // std::allocator
#include "bits/spatial_kdtree.hpp"
#include "function.hpp"

namespace spatial
{

  template<dimension_type Rank, typename Key,
           typename Compare = bracket_less<Key>,
           typename Alloc = std::allocator<Key> >
  struct idle_box_multiset
    : details::Kdtree<details::Static_rank<Rank << 1>, const Key, const Key,
                      Compare, Alloc>
  {
  private:
    typedef details::Kdtree<details::Static_rank<Rank << 1>, const Key,
                            const Key, Compare, Alloc>  base_type;
    typedef idle_box_multiset<Rank, Key, Compare, Alloc>      Self;

  public:
    idle_box_multiset() { }

    explicit idle_box_multiset(const Compare& compare)
      : base_type(details::Static_rank<Rank << 1>())
    { }

    idle_box_multiset(const Compare& compare, const Alloc& alloc)
      : base_type(details::Static_rank<Rank << 1>(), compare, alloc)
    { }

    idle_box_multiset(const idle_box_multiset& other, bool balancing = false)
      : base_type(other, balancing)
    { }

    idle_box_multiset&
    operator=(const idle_box_multiset& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

  /**
   *  Specialization for @ref idle_box_multiset with runtime rank support. The
   *  rank of the @ref idle_box_multiset can be determined at run time and does not
   *  need to be fixed at compile time. Using:
   *  @code
   *    struct box { ... };
   *    idle_box_multiset<0, box> my_set;
   *  @endcode
   *
   *  @see runtime_idle_box_multiset for more information about how to use this
   *  container.
   */
  template<typename Key, typename Compare, typename Alloc>
  struct idle_box_multiset<0, Key, Compare, Alloc>
    : details::Kdtree<details::Dynamic_rank, const Key, const Key,
                      Compare, Alloc>
  {
  private:
    typedef details::Kdtree<details::Dynamic_rank, const Key,
                            const Key, Compare, Alloc>  base_type;
    typedef idle_box_multiset<0, Key, Compare, Alloc>         Self;

  public:
    idle_box_multiset() : base_type(details::Dynamic_rank(2)) { }

    explicit idle_box_multiset(dimension_type dim)
      : base_type(details::Dynamic_rank(dim << 1))
    { except::check_rank(dim); }

    idle_box_multiset(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim << 1), compare)
    { except::check_rank(dim); }

    explicit idle_box_multiset(const Compare& compare)
      : base_type(details::Dynamic_rank(2), compare)
    { }

    idle_box_multiset(dimension_type dim, const Compare& compare,
                  const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim << 1), compare, alloc)
    { except::check_rank(dim); }

    idle_box_multiset(const Compare& compare, const Alloc& alloc)
      : base_type(details::Dynamic_rank(2), compare, alloc)
    { }

    idle_box_multiset(const idle_box_multiset& other, bool balancing = false)
      : base_type(other, balancing)
    { }

    idle_box_multiset&
    operator=(const idle_box_multiset& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

} // namespace spatial

#endif // SPATIAL_IDLE_BOX_MULTISET_HPP
