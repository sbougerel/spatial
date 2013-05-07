// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   idle_boxmap.hpp
 *  @brief  Contains the definition of the @ref idle_boxmap and @ref
 *  runtime_idle_boxmap containers. These containers are mapped containers and
 *  store values in space that can be represented as boxes.
 *
 *  @see idle_boxmap
 *  @see runtime_idle_boxmap
 */

#ifndef SPATIAL_IDLE_BOXMAP_HPP
#define SPATIAL_IDLE_BOXMAP_HPP

#include "bits/spatial.hpp"
#include "bits/spatial_kdtree.hpp"

namespace spatial
{

  template<dimension_type Rank, typename Key, typename Mapped,
           typename Compare = bracket_less<Key>,
           typename Alloc = std::allocator<std::pair<const Key, Mapped> > >
  struct idle_boxmap
    : details::Kdtree<details::Static_rank<Rank << 1>, const Key,
                      std::pair<const Key, Mapped>, Compare, Alloc>
  {
  private:
    typedef details::Kdtree
    <details::Static_rank<Rank << 1>, const Key, std::pair<const Key, Mapped>,
     Compare, Alloc>                                       base_type;
    typedef idle_boxmap<Rank, Key, Mapped, Compare, Alloc> Self;

  public:
    typedef Mapped                            mapped_type;

    idle_boxmap() { }

    explicit idle_boxmap(const Compare& compare)
      : base_type(details::Static_rank<Rank << 1>())
    { }

    idle_boxmap(const Compare& compare, const Alloc& alloc)
      : base_type(details::Static_rank<Rank << 1>(), compare, alloc)
    { }

    idle_boxmap(const idle_boxmap& other, bool balancing = false)
      : base_type(other, balancing)
    { }

    idle_boxmap&
    operator=(const idle_boxmap& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

  /**
   *  Specialization for @ref idle_boxmap with runtime rank support. The
   *  rank of the @ref idle_boxmap can be determined at run time and does not
   *  need to be fixed at compile time. Using:
   *  @code
   *    struct box { ... };
   *    struct mapped { ... };
   *    idle_boxmap<0, box, mapped> my_set;
   *  @endcode
   *  ...is therefore equivalent to:
   *  @code
   *    struct box { ... };
   *    struct mapped { ... };
   *    runtime_idle_boxmap<box, mapped> my_set;
   *  @endcode
   *
   *  @see runtime_idle_boxmap for more information about how to use this
   *  container.
   */
  template<typename Key, typename Mapped, typename Compare, typename Alloc>
  struct idle_boxmap<0, Key, Mapped, Compare, Alloc>
    : details::Kdtree<details::Dynamic_rank, const Key,
                      std::pair<const Key, Mapped>, Compare, Alloc>
  {
  private:
    typedef details::Kdtree<details::Dynamic_rank, const Key,
                            std::pair<const Key, Mapped>,
                            Compare, Alloc>              base_type;
    typedef idle_boxmap<0, Key, Mapped, Compare, Alloc>  Self;

  public:
    typedef Mapped                            mapped_type;

    idle_boxmap() : base_type(details::Dynamic_rank(2)) { }

    explicit idle_boxmap(dimension_type dim)
      : base_type(details::Dynamic_rank(dim << 1))
    { except::check_rank(dim); }

    idle_boxmap(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim << 1), compare)
    { except::check_rank(dim); }

    explicit idle_boxmap(const Compare& compare)
      : base_type(details::Dynamic_rank(2), compare)
    { }

    idle_boxmap(dimension_type dim, const Compare& compare,
                  const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim << 1), compare, alloc)
    { except::check_rank(dim); }

    idle_boxmap(const Compare& compare, const Alloc& alloc)
      : base_type(details::Dynamic_rank(2), compare, alloc)
    { }

    idle_boxmap(const idle_boxmap& other, bool balancing = false)
      : base_type(other, balancing)
    { }

    idle_boxmap&
    operator=(const idle_boxmap& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

  /**
   *  @brief  boxmap with dynamic rank support. The rank of the boxmap can
   *  be determined at run time and does not need to be fixed at compile time.
   */
  template<typename Key, typename Mapped,
           typename Compare = bracket_less<Key>,
           typename Alloc = std::allocator<std::pair<const Key, Mapped> > >
  struct runtime_idle_boxmap
    : details::Kdtree<details::Dynamic_rank, const Key,
                      std::pair<const Key, Mapped>, Compare, Alloc>
  {
  private:
    typedef details::Kdtree<details::Dynamic_rank, const Key,
                            std::pair<const Key, Mapped>,
                            Compare, Alloc>                  base_type;
    typedef runtime_idle_boxmap<Key, Mapped, Compare, Alloc> Self;

  public:
    typedef Mapped mapped_type;

    runtime_idle_boxmap() : base_type(details::Dynamic_rank(2)) { }

    explicit runtime_idle_boxmap(dimension_type dim)
      : base_type(details::Dynamic_rank(dim << 1))
    { except::check_rank(dim); }

    runtime_idle_boxmap(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim << 1), compare)
    { except::check_rank(dim); }

    explicit runtime_idle_boxmap(const Compare& compare)
      : base_type(details::Dynamic_rank(2), compare)
    { }

    runtime_idle_boxmap(dimension_type dim, const Compare& compare,
                          const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim << 1), compare, alloc)
    { except::check_rank(dim); }

    runtime_idle_boxmap(const Compare& compare, const Alloc& alloc)
      : base_type(details::Dynamic_rank(2), compare, alloc)
    { }

    runtime_idle_boxmap(const runtime_idle_boxmap& other,
                          bool balancing = false)
      : base_type(other, balancing)
    { }

    runtime_idle_boxmap&
    operator=(const runtime_idle_boxmap& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

} // namespace spatial

#endif // SPATIAL_IDLE_BOXMAP_HPP
