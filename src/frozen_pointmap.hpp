// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   frozen_pointmap.hpp
 *  @brief  Contains the definition of the @ref frozen_pointmap and @ref
 *  runtime_frozen_pointmap containers. These containers are mapped containers
 *  and store values in space that can be represented as points.
 *
 *  @see frozen_pointmap
 *  @see runtime_frozen_pointmap
 */

#ifndef SPATIAL_FROZEN_POINTMAP_HPP
#define SPATIAL_FROZEN_POINTMAP_HPP

#include "bits/spatial.hpp"
#include "bits/spatial_kdtree.hpp"

namespace spatial
{

  template<dimension_type Rank, typename Key, typename Mapped,
           typename Compare = bracket_less<Key>,
           typename Alloc = std::allocator<std::pair<const Key, Mapped> > >
  struct frozen_pointmap
    : details::Kdtree<details::Static_rank<Rank>, Key,
                      std::pair<const Key, Mapped>, Compare, Alloc>
  {
  private:
    typedef details::Kdtree<details::Static_rank<Rank>, Key,
                            std::pair<const Key, Mapped>, Compare,
                            Alloc>            base_type;
    typedef frozen_pointmap<Rank, Key, Mapped,
                            Compare, Alloc>   Self;

  public:
    typedef Mapped                            mapped_type;

    frozen_pointmap() { }

    explicit frozen_pointmap(const Compare& compare)
      : base_type(details::Static_rank<Rank>())
    { }

    frozen_pointmap(const Compare& compare, const Alloc& alloc)
      : base_type(details::Static_rank<Rank>(), compare, alloc)
    { }

    frozen_pointmap(const frozen_pointmap& other, bool balancing = false)
      : base_type(other, balancing)
    { }

    frozen_pointmap&
    operator=(const frozen_pointmap& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

  /**
   *  Specialization for @ref frozen_pointmap with runtime rank support. The
   *  rank of the @ref frozen_pointmap can be determined at run time and does
   *  not need to be fixed at compile time. Using:
   *  @code
   *    struct point { ... };
   *    struct mapped { ... };
   *    frozen_pointmap<0, point, mapped> my_set;
   *  @endcode
   *  ...is therefore completely equivalent to:
   *  @code
   *    struct point { ... };
   *    struct mapped { ... };
   *    runtime_frozen_pointmap<point, mapped> my_set;
   *  @endcode
   *
   *  @see runtime_frozen_pointmap for more information about how to use this
   *  container.
   */
  template<typename Key, typename Mapped, typename Compare, typename Alloc>
  struct frozen_pointmap<0, Key, Mapped, Compare, Alloc>
    : details::Kdtree<details::Dynamic_rank, Key, std::pair<const Key, Mapped>,
                      Compare, Alloc>
  {
  private:
    typedef details::Kdtree<details::Dynamic_rank, Key,
                            std::pair<const Key, Mapped>,
                            Compare,Alloc>    base_type;
    typedef frozen_pointmap<0, Key, Mapped,
                            Compare, Alloc>   Self;

  public:
    typedef Mapped                            mapped_type;

    frozen_pointmap() { }

    explicit frozen_pointmap(dimension_type dim)
      : base_type(details::Dynamic_rank(dim))
    { except::check_rank(dim); }

    frozen_pointmap(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim), compare)
    { except::check_rank(dim); }

    explicit frozen_pointmap(const Compare& compare)
      : base_type(compare)
    { }

    frozen_pointmap(dimension_type dim, const Compare& compare,
                    const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim), compare, alloc)
    { except::check_rank(dim); }

    frozen_pointmap(const Compare& compare, const Alloc& alloc)
      : base_type(details::Dynamic_rank(), compare, alloc)
    { }

    frozen_pointmap(const frozen_pointmap& other, bool balancing = false)
      : base_type(other, balancing)
    { }

    frozen_pointmap&
    operator=(const frozen_pointmap& other)
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
  struct runtime_frozen_pointmap
    : details::Kdtree<details::Dynamic_rank, Key, std::pair<const Key, Mapped>,
                      Compare, Alloc>
  {
  private:
    typedef details::Kdtree<details::Dynamic_rank, Key,
                            std::pair<const Key, Mapped>,
                            Compare, Alloc>   base_type;
    typedef runtime_frozen_pointmap<Key, Mapped, Compare,
                                    Alloc>    Self;

  public:
    typedef Mapped                            mapped_type;

    runtime_frozen_pointmap() { }

    explicit runtime_frozen_pointmap(dimension_type dim)
      : base_type(details::Dynamic_rank(dim))
    { except::check_rank(dim); }

    runtime_frozen_pointmap(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim), compare)
    { except::check_rank(dim); }

    explicit runtime_frozen_pointmap(const Compare& compare)
      : base_type(compare)
    { }

    runtime_frozen_pointmap(dimension_type dim, const Compare& compare,
                            const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim), compare, alloc)
    { except::check_rank(dim); }

    runtime_frozen_pointmap(const Compare& compare, const Alloc& alloc)
      : base_type(details::Dynamic_rank(), compare, alloc)
    { }

    runtime_frozen_pointmap(const runtime_frozen_pointmap& other,
                            bool balancing = false)
      : base_type(other, balancing)
    { }

    runtime_frozen_pointmap&
    operator=(const runtime_frozen_pointmap& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

}

#endif // SPATIAL_FROZEN_POINTMAP_HPP
