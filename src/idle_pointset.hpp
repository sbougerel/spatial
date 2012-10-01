// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   idle_pointset.hpp
 *  @brief  Contains the definition of the @ref idle_pointset and @ref
 *  runtime_idle_pointset containers. These containers are not mapped
 *  containers and store values in space that can be represented as points.
 *
 *  Iterating these containers always yield a constant value iterator. That is
 *  because modifying the value stored in the container may compromise the
 *  ordering in the container. One way around this issue is to use a @ref
 *  idle_pointmap container or to @c const_cast the value dereferenced from
 *  the iterator.
 *
 *  @see idle_pointmap
 *  @see runtime_idle_pointmap
 *  @see idle_pointset
 *  @see runtime_idle_pointset
 */

#ifndef SPATIAL_IDLE_POINTSET_HPP
#define SPATIAL_IDLE_POINTSET_HPP

#include "bits/spatial.hpp"
#include "bits/spatial_kdtree.hpp"

namespace spatial
{

  template<dimension_type Rank, typename Key,
           typename Compare = bracket_less<Key>,
           typename Alloc = std::allocator<Key> >
  struct idle_pointset
    : details::Kdtree<details::Static_rank<Rank>,
                      const Key, const Key, Compare, Alloc>
  {
  private:
    typedef details::Kdtree<details::Static_rank<Rank>, const Key, const Key,
                            Compare, Alloc>          base_type;
    typedef idle_pointset<Rank, Key, Compare, Alloc> Self;

  public:
    idle_pointset() { }

    explicit idle_pointset(const Compare& compare)
      : base_type(details::Static_rank<Rank>())
    { }

    idle_pointset(const Compare& compare, const Alloc& alloc)
      : base_type(details::Static_rank<Rank>(), compare, alloc)
    { }

    idle_pointset(const idle_pointset& other, bool balancing = false)
      : base_type(other, balancing)
    { }

    idle_pointset&
    operator=(const idle_pointset& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

  /**
   *  Specialization for @ref idle_pointset with runtime rank support. The
   *  rank of the @ref idle_pointset can be determined at run time and does
   *  not need to be fixed at compile time. Using:
   *  @code
   *    struct point { ... };
   *    idle_pointset<0, point> my_set;
   *  @endcode
   *  ...is therefore completely equivalent to:
   *  @code
   *    struct point { ... };
   *    runtime_idle_pointset<point> my_set;
   *  @endcode
   *
   *  @see runtime_idle_pointset for more information about how to use this
   *  container.
   */
  template<typename Key, typename Compare, typename Alloc>
  struct idle_pointset<0, Key, Compare, Alloc>
    : details::Kdtree<details::Dynamic_rank, const Key, const Key,
                      Compare, Alloc>
  {
  private:
    typedef details::Kdtree<details::Dynamic_rank, const Key, const Key,
                            Compare, Alloc>          base_type;
    typedef idle_pointset<0, Key, Compare, Alloc>    Self;

  public:
    idle_pointset() { }

    explicit idle_pointset(dimension_type dim)
      : base_type(details::Dynamic_rank(dim))
    { except::check_rank(dim); }

    idle_pointset(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim), compare)
    { except::check_rank(dim); }

    explicit idle_pointset(const Compare& compare)
      : base_type(compare)
    { }

    idle_pointset(dimension_type dim, const Compare& compare,
                    const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim), compare, alloc)
    { except::check_rank(dim); }

    idle_pointset(const Compare& compare, const Alloc& alloc)
      : base_type(details::Dynamic_rank(), compare, alloc)
    { }

    idle_pointset(const idle_pointset& other, bool balancing = false)
      : base_type(other, balancing)
    { }

    idle_pointset&
    operator=(const idle_pointset& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

  /**
   *  @brief  pointset with dynamic rank support. The rank of the pointset can
   *  be determined at run time and does not need to be fixed at compile time.
   */
  template<typename Key,
           typename Compare = bracket_less<Key>,
           typename Alloc = std::allocator<Key> >
  struct runtime_idle_pointset
    : details::Kdtree<details::Dynamic_rank, const Key, const Key,
                      Compare, Alloc>
  {
  private:
    typedef details::Kdtree<details::Dynamic_rank, const Key, const Key,
                            Compare, Alloc>              base_type;
    typedef runtime_idle_pointset<Key, Compare, Alloc>   Self;

  public:
    runtime_idle_pointset() { }

    explicit runtime_idle_pointset(dimension_type dim)
      : base_type(details::Dynamic_rank(dim))
    { except::check_rank(dim); }

    runtime_idle_pointset(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim), compare)
    { except::check_rank(dim); }

    explicit runtime_idle_pointset(const Compare& compare)
      : base_type(compare)
    { }

    runtime_idle_pointset(dimension_type dim, const Compare& compare,
                            const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim), compare, alloc)
    { except::check_rank(dim); }

    runtime_idle_pointset(const Compare& compare, const Alloc& alloc)
      : base_type(details::Dynamic_rank(), compare, alloc)
    { }

    runtime_idle_pointset(const runtime_idle_pointset& other,
                            bool balancing = false)
      : base_type(other, balancing)
    { }

    runtime_idle_pointset&
    operator=(const runtime_idle_pointset& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

}

#endif // SPATIAL_IDLE_POINTSET_HPP
