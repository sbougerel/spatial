// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   idle_pointmap.hpp
 *  @brief  Contains the definition of the @ref idle_pointmap and @ref
 *  runtime_idle_pointmap containers. These containers are mapped containers
 *  and store values in space that can be represented as points.
 *
 *  @see idle_pointmap
 *  @see runtime_idle_pointmap
 */

#ifndef SPATIAL_IDLE_POINTMAP_HPP
#define SPATIAL_IDLE_POINTMAP_HPP

#include "bits/spatial.hpp"
#include "bits/spatial_kdtree.hpp"

namespace spatial
{

  template<dimension_type Rank, typename Key, typename Mapped,
           typename Compare = bracket_less<Key>,
           typename Alloc = std::allocator<std::pair<const Key, Mapped> > >
  struct idle_pointmap
    : details::Kdtree<details::Static_rank<Rank>, const Key,
                      std::pair<const Key, Mapped>, Compare, Alloc>
  {
  private:
    typedef details::Kdtree<details::Static_rank<Rank>, const Key,
                            std::pair<const Key, Mapped>, Compare,
                            Alloc>            base_type;
    typedef idle_pointmap<Rank, Key, Mapped,
                          Compare, Alloc>     Self;

  public:
    typedef Mapped                            mapped_type;

    idle_pointmap() { }

    explicit idle_pointmap(const Compare& compare)
      : base_type(details::Static_rank<Rank>())
    { }

    idle_pointmap(const Compare& compare, const Alloc& alloc)
      : base_type(details::Static_rank<Rank>(), compare, alloc)
    { }

    idle_pointmap(const idle_pointmap& other, bool balancing = false)
      : base_type(other, balancing)
    { }

    idle_pointmap&
    operator=(const idle_pointmap& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

  /**
   *  Specialization for @ref idle_pointmap with runtime rank support. The
   *  rank of the @ref idle_pointmap can be determined at run time and does
   *  not need to be fixed at compile time. Using:
   *  @code
   *    struct point { ... };
   *    struct mapped { ... };
   *    idle_pointmap<0, point, mapped> my_set;
   *  @endcode
   *  ...is therefore completely equivalent to:
   *  @code
   *    struct point { ... };
   *    struct mapped { ... };
   *    runtime_idle_pointmap<point, mapped> my_set;
   *  @endcode
   *
   *  @see runtime_idle_pointmap for more information about how to use this
   *  container.
   */
  template<typename Key, typename Mapped, typename Compare, typename Alloc>
  struct idle_pointmap<0, Key, Mapped, Compare, Alloc>
    : details::Kdtree<details::Dynamic_rank, const Key,
                      std::pair<const Key, Mapped>, Compare, Alloc>
  {
  private:
    typedef details::Kdtree<details::Dynamic_rank, const Key,
                            std::pair<const Key, Mapped>,
                            Compare,Alloc>    base_type;
    typedef idle_pointmap<0, Key, Mapped,
                          Compare, Alloc>     Self;

  public:
    typedef Mapped                            mapped_type;

    idle_pointmap() { }

    explicit idle_pointmap(dimension_type dim)
      : base_type(details::Dynamic_rank(dim))
    { except::check_rank(dim); }

    idle_pointmap(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim), compare)
    { except::check_rank(dim); }

    explicit idle_pointmap(const Compare& compare)
      : base_type(compare)
    { }

    idle_pointmap(dimension_type dim, const Compare& compare,
                    const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim), compare, alloc)
    { except::check_rank(dim); }

    idle_pointmap(const Compare& compare, const Alloc& alloc)
      : base_type(details::Dynamic_rank(), compare, alloc)
    { }

    idle_pointmap(const idle_pointmap& other, bool balancing = false)
      : base_type(other, balancing)
    { }

    idle_pointmap&
    operator=(const idle_pointmap& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

  /**
   *  @brief  pointset with dynamic rank support. The rank of the pointset can
   *  be determined at run time and does not need to be fixed at compile time.
   */
  template<typename Key, typename Mapped,
           typename Compare = bracket_less<Key>,
           typename Alloc = std::allocator<std::pair<const Key, Mapped> > >
  struct runtime_idle_pointmap
    : details::Kdtree<details::Dynamic_rank, const Key,
                      std::pair<const Key, Mapped>, Compare, Alloc>
  {
  private:
    typedef details::Kdtree<details::Dynamic_rank, const Key,
                            std::pair<const Key, Mapped>,
                            Compare, Alloc>   base_type;
    typedef runtime_idle_pointmap<Key, Mapped, Compare,
                                  Alloc>      Self;

  public:
    typedef Mapped                            mapped_type;

    runtime_idle_pointmap() { }

    explicit runtime_idle_pointmap(dimension_type dim)
      : base_type(details::Dynamic_rank(dim))
    { except::check_rank(dim); }

    runtime_idle_pointmap(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim), compare)
    { except::check_rank(dim); }

    explicit runtime_idle_pointmap(const Compare& compare)
      : base_type(compare)
    { }

    runtime_idle_pointmap(dimension_type dim, const Compare& compare,
                            const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim), compare, alloc)
    { except::check_rank(dim); }

    runtime_idle_pointmap(const Compare& compare, const Alloc& alloc)
      : base_type(details::Dynamic_rank(), compare, alloc)
    { }

    runtime_idle_pointmap(const runtime_idle_pointmap& other,
                            bool balancing = false)
      : base_type(other, balancing)
    { }

    runtime_idle_pointmap&
    operator=(const runtime_idle_pointmap& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

}

#endif // SPATIAL_IDLE_POINTMAP_HPP
