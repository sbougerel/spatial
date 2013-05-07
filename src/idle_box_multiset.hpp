// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   idle_boxset.hpp
 *  @brief  Contains the definition of the @ref idle_boxset and @ref
 *  runtime_idle_boxset containers. These containers are not mapped containers
 *  and store values in space that can be represented as boxes.
 *
 *  Iterating these containers always yield a constant value iterator. That is
 *  because modifying the value stored in the container may compromise the
 *  ordering in the container. One way around this issue is to use a @ref
 *  idle_boxmap container or to @c const_cast the value dereferenced from the
 *  iterator.
 *
 *  @see idle_boxmap
 *  @see runtime_idle_boxmap
 *  @see idle_boxset
 *  @see runtime_idle_boxset
 */

#ifndef SPATIAL_IDLE_BOXSET_HPP
#define SPATIAL_IDLE_BOXSET_HPP

#include "bits/spatial.hpp"
#include "bits/spatial_kdtree.hpp"

namespace spatial
{

  template<dimension_type Rank, typename Key,
           typename Compare = bracket_less<Key>,
           typename Alloc = std::allocator<Key> >
  struct idle_boxset
    : details::Kdtree<details::Static_rank<Rank << 1>, const Key, const Key,
                      Compare, Alloc>
  {
  private:
    typedef details::Kdtree<details::Static_rank<Rank << 1>, const Key,
                            const Key, Compare, Alloc>  base_type;
    typedef idle_boxset<Rank, Key, Compare, Alloc>      Self;

  public:
    idle_boxset() { }

    explicit idle_boxset(const Compare& compare)
      : base_type(details::Static_rank<Rank << 1>())
    { }

    idle_boxset(const Compare& compare, const Alloc& alloc)
      : base_type(details::Static_rank<Rank << 1>(), compare, alloc)
    { }

    idle_boxset(const idle_boxset& other, bool balancing = false)
      : base_type(other, balancing)
    { }

    idle_boxset&
    operator=(const idle_boxset& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

  /**
   *  Specialization for @ref idle_boxset with runtime rank support. The
   *  rank of the @ref idle_boxset can be determined at run time and does not
   *  need to be fixed at compile time. Using:
   *  @code
   *    struct box { ... };
   *    idle_boxset<0, box> my_set;
   *  @endcode
   *  ...is therefore completely equivalent to:
   *  @code
   *    struct box { ... };
   *    runtime_idle_boxset<box> my_set;
   *  @endcode
   *
   *  @see runtime_idle_boxset for more information about how to use this
   *  container.
   */
  template<typename Key, typename Compare, typename Alloc>
  struct idle_boxset<0, Key, Compare, Alloc>
    : details::Kdtree<details::Dynamic_rank, const Key, const Key,
                      Compare, Alloc>
  {
  private:
    typedef details::Kdtree<details::Dynamic_rank, const Key,
                            const Key, Compare, Alloc>  base_type;
    typedef idle_boxset<0, Key, Compare, Alloc>         Self;

  public:
    idle_boxset() : base_type(details::Dynamic_rank(2)) { }

    explicit idle_boxset(dimension_type dim)
      : base_type(details::Dynamic_rank(dim << 1))
    { except::check_rank(dim); }

    idle_boxset(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim << 1), compare)
    { except::check_rank(dim); }

    explicit idle_boxset(const Compare& compare)
      : base_type(details::Dynamic_rank(2), compare)
    { }

    idle_boxset(dimension_type dim, const Compare& compare,
                  const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim << 1), compare, alloc)
    { except::check_rank(dim); }

    idle_boxset(const Compare& compare, const Alloc& alloc)
      : base_type(details::Dynamic_rank(2), compare, alloc)
    { }

    idle_boxset(const idle_boxset& other, bool balancing = false)
      : base_type(other, balancing)
    { }

    idle_boxset&
    operator=(const idle_boxset& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

  /**
   *  @brief  boxset with dynamic rank support. The rank of the boxset can
   *  be determined at run time and does not need to be fixed at compile time.
   */
  template<typename Key,
           typename Compare = bracket_less<Key>,
           typename Alloc = std::allocator<Key> >
  struct runtime_idle_boxset
    : details::Kdtree<details::Dynamic_rank, const Key, const Key,
                      Compare, Alloc>
  {
  private:
    typedef details::Kdtree<details::Dynamic_rank, const Key, const Key,
                            Compare, Alloc>              base_type;
    typedef runtime_idle_boxset<Key, Compare, Alloc>   Self;

  public:
    runtime_idle_boxset() : base_type(details::Dynamic_rank(2)) { }

    explicit runtime_idle_boxset(dimension_type dim)
      : base_type(details::Dynamic_rank(dim << 1))
    { except::check_rank(dim); }

    runtime_idle_boxset(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim << 1), compare)
    { except::check_rank(dim); }

    explicit runtime_idle_boxset(const Compare& compare)
      : base_type(details::Dynamic_rank(2), compare)
    { }

    runtime_idle_boxset(dimension_type dim, const Compare& compare,
                          const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim << 1), compare, alloc)
    { except::check_rank(dim); }

    runtime_idle_boxset(const Compare& compare, const Alloc& alloc)
      : base_type(details::Dynamic_rank(2), compare, alloc)
    { }

    runtime_idle_boxset(const runtime_idle_boxset& other,
                          bool balancing = false)
      : base_type(other, balancing)
    { }

    runtime_idle_boxset&
    operator=(const runtime_idle_boxset& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

} // namespace spatial

#endif // SPATIAL_IDLE_BOXSET_HPP
